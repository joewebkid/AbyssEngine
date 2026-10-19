"""Optional read-only Android evidence gate; original bytes stay outside the checkout."""

from pathlib import Path
import re
import struct
import subprocess
import unittest

import delink

ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)


@unittest.skipUnless(SO.is_file(), 'original Android binary unavailable')
class LoadPortraitsArmEvidence(unittest.TestCase):
    def test_post_table_image_resource_contract(self):
        asm = subprocess.check_output(delink.tool_argv(str(ROOT / 'tools/verify/orbobjdump'), [
            '-d', '--start-address=0x10a1aa', '--stop-address=0x10a25a', str(SO)]), text=True)
        for instruction in (
            r'movw\s+fp, #10200', r'movw\s+r6, #5000', r'movs\s+r4, #3',
            r'mov\.w\s+r8, #4294967295', r'mov\.w\s+r9, #0',
            r'add\.w\s+r1, r9, fp', r'add\.w\s+r1, r9, r6',
            r'strh\s+r1, \[r0, #0\]', r'strh\s+r1, \[r5, #0\]',
            r'strd\s+r4, r8, \[r5, #4\]', r'str\s+r0, \[r5, #12\]',
            r'add\.w\s+r9, r9, #1', r'cmp\.w\s+r9, #152', r'bcc\.n\s+10a224',
        ):
            self.assertRegex(asm, instruction)
        self.assertIn('PaintCanvas11AddResource', asm)

    def test_got_suffixes_and_resource_records(self):
        data = SO.read_bytes()
        self.assertEqual(data[:6], b'\x7fELF\x01\x01')
        shoff = struct.unpack_from('<I', data, 0x20)[0]
        entsize, count, names_index = struct.unpack_from('<HHH', data, 0x2e)
        sections = [struct.unpack_from('<10I', data, shoff + i * entsize) for i in range(count)]
        names_section = sections[names_index]
        names = data[names_section[4]:names_section[4] + names_section[5]]

        def cstring(blob, offset):
            return blob[offset:blob.index(b'\0', offset)].decode()

        named = {cstring(names, s[0]): s for s in sections}

        def offset(address):
            for section in sections:
                if section[1] != 8 and section[3] <= address < section[3] + section[5]:
                    return section[4] + address - section[3]
            raise AssertionError(f'unmapped ELF address {address:#x}')

        def word(address):
            return struct.unpack_from('<I', data, offset(address))[0]

        dynstr = named['.dynstr']
        strings = data[dynstr[4]:dynstr[4] + dynstr[5]]
        dynsym = named['.dynsym']
        rel = named['.rel.dyn']
        relocations = {}
        for position in range(rel[4], rel[4] + rel[5], rel[9]):
            address, info = struct.unpack_from('<II', data, position)
            name = struct.unpack_from('<I', data, dynsym[4] + (info >> 8) * dynsym[9])[0]
            relocations[address] = (info & 255, cstring(strings, name))
        # (literal, ADD-PC instruction, GOT slot, symbol) from Android 2.0.16.
        for literal, pc, slot, symbol in (
            (0x112d90, 0x112d60, 0x2103f0, '_ZN7Globals9iPadLargeE'),
            (0x112d98, 0x112d7c, 0x21038c, '_ZN7Globals6iPadHDE'),
            (0x112dcc, 0x112da6, 0x2103e4, '_ZN7Globals13retinaDisplayE'),
            (0x112dd0, 0x112da8, 0x21032c, '_ZN7Globals4iPadE'),
            (0x112dd4, 0x112daa, 0x2103e8, '_ZN7Globals2n9E'),
        ):
            self.assertEqual(pc + 4 + word(literal), slot)
            self.assertEqual(relocations[slot], (21, symbol))  # R_ARM_GLOB_DAT
        for literal, pc, suffix in (
            (0x112d94, 0x112d76, '_ipad_large'),
            (0x112d9c, 0x112d86, '_ipad_1440'),
            (0x112dd8, 0x112dc0, '_ipad'),
            (0x112ddc, 0x112dc4, ''),
        ):
            self.assertEqual(cstring(data, offset(pc + 4 + word(literal))), suffix)

        asm = subprocess.check_output(delink.tool_argv(str(ROOT / 'tools/verify/orbobjdump'), [
            '-d', '--start-address=0x112d4c', '--stop-address=0x118186', str(SO)]), text=True)
        registers = {}
        paths = []
        for line in asm.splitlines():
            match = re.search(r'ldr\s+(r\d+), \[pc, #\d+\].*; \(([0-9a-f]+)', line)
            if match:
                registers[match[1]] = int(match[2], 16)
            match = re.search(r'^\s*([0-9a-f]+):.*\badd\s+(r\d+), pc', line)
            if match and match[2] in registers:
                address = int(match[1], 16) + 4 + word(registers[match[2]])
                path = cstring(data, offset(address))
                if path.startswith('data/textures/'):
                    paths.append(path)
        ids = [int(value) for value in re.findall(r'(?:movw|mov\.w)\s+r1, #(10\d+)', asm)]
        # Normal-path named calls prove allocation and temporary lifetimes; the
        # four destructor calls after each AddResource use an indirect BLX.
        calls = re.findall(r'\bblx?\s+[0-9a-f]+\s+<([^>]+)>', asm)
        add = '_ZN11AbyssEngine11PaintCanvas11AddResourceEPNS_8ResourceE@plt'
        normal = calls[:max(i for i, name in enumerate(calls) if name == add) + 1]
        constructor = '_ZN11AbyssEngine6StringC1EPKcb@plt'
        concat = '_ZN11AbyssEngineplERKNS_6StringES2_@plt'
        self.assertEqual(normal, [constructor] + [
            '_Znwj@plt', '_Znwj@plt', constructor, concat, constructor, concat,
            '_ZN11AbyssEngine15ResourceTextureC2ERKNS_6StringEf@plt', add,
        ] * 152)
        source = (ROOT / 'src/engine/render/ResourceMaterial.cpp').read_text()
        self.assertEqual(paths, re.findall(r'String\("(data/textures/[^"]+)"', source))
        self.assertEqual(ids, [int(value, 16) for value in re.findall(
            r'new AbyssEngine::Resource\{(0x[0-9a-f]+), 2, -1,', source)])
        self.assertEqual(ids, list(range(10200, 10352)))


if __name__ == '__main__':
    unittest.main()
