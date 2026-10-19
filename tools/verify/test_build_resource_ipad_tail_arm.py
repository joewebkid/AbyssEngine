"""Read-only Android 2.0.16 evidence for the ordinary iPad resource tail."""
from pathlib import Path
import re
import struct
import subprocess
import unittest

import delink
from elf import Elf32

ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)
# (ADD-PC instruction, literal, ID, filename); final ID is in the shared join.
RECORDS = (
    (0x10f16a, 0x10f560, 10089, 'gof2_interface3_ipad.aei'),
    (0x10f19c, 0x10f564, 10063, 'gof2_interface2_ipad.aei'),
    (0x10f1ce, 0x10f568, 10064, 'gof2_items_ipad.aei'),
    (0x10f200, 0x10f56c, 11632, 'gof2_items_ipad_2.aei'),
    (0x10f232, 0x10f570, 11630, 'gof2_dlc_interface_ipad.aei'),
    (0x10f264, 0x10f574, 27341, 'gof2_challenge_interface_ipad.aei'),
    (0x10f296, 0x10f578, 27342, 'gof2_campaign_select_ipad.aei'),
    (0x10f2c8, 0x10f57c, 27343, 'gof2_shop_button.aei'),
)


@unittest.skipUnless(SO.is_file(), 'original Android binary unavailable')
class BuildResourceIpadArmEvidence(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.elf = Elf32(SO)
        cls.asm = subprocess.check_output(delink.tool_argv(str(ROOT / 'tools/verify/orbobjdump'), [
            '-d', '--start-address=0x10a250', '--stop-address=0x10f2d4', str(SO)]), text=True)

    def word(self, address):
        return struct.unpack('<I', self.elf.bytes_at(address, 4))[0]

    def test_flag_relocations_and_register_setup(self):
        symbols = list(self.elf._symbols(11))
        relocations = {}
        for section in self.elf.sections:
            if section['type'] != 9:  # SHT_REL
                continue
            for pos in range(section['offset'], section['offset'] + section['size'], section['entsize']):
                address, info = struct.unpack_from('<II', self.elf.data, pos)
                relocations[address] = (info & 255, symbols[info >> 8]['name'])
        for pc, literal, got, name in (
            (0x10a25c, 0x10a270, 0x21038c, '_ZN7Globals6iPadHDE'),
            (0x10a4de, 0x10a6a0, 0x2103e4, '_ZN7Globals13retinaDisplayE'),
            (0x10a6c6, 0x10a9ec, 0x2103e4, '_ZN7Globals13retinaDisplayE'),
            (0x10effc, 0x10f144, 0x21032c, '_ZN7Globals4iPadE'),
        ):
            self.assertEqual(pc + 4 + self.word(literal), got)
            self.assertEqual(relocations[got], (21, name))  # R_ARM_GLOB_DAT
        self.assertRegex(self.asm, r'10a25e:.*ldr\.w\s+r8, \[r0\]')
        self.assertRegex(self.asm, r'10be22:.*ldrb\.w\s+r4, \[r8\]')

    def test_zero_branches_and_shared_shop_join(self):
        for address, opcode, operands in (
            ('10a266', 'cmp', r'r0, #0'),
            ('10a268', r'beq\.w', r'10a6c4'),
            ('10a6d4', 'cmp', r'r4, #0'),
            ('10a6d6', r'beq\.w', r'10ee40'),
            ('10f002', 'cmp', r'r0, #0'),
            ('10f004', r'bne\.w', r'10a4dc'),
            ('10a4e4', 'cmp', r'r0, #0'),
            ('10a4e6', r'beq\.w', r'10be20'),
            ('10be2c', 'cmp', r'r4, #0'),
            ('10be2e', r'beq\.w', r'10f160'),
            ('10f2d0', r'b\.w', r'10a662'),
            ('10a664', 'movw', r'r1, #27343'),
            ('10a668', r'b\.w', r'10bf32'),
        ):
            self.assertRegex(self.asm, address + r':.*\b' + opcode + r'\s+' + operands)

    def test_ordered_ids_paths_and_constructor_calls(self):
        tail = self.asm[self.asm.index('  10f160:'):]
        ids = [int(i) for i in re.findall(r'movw\s+r1, #(\d+)', tail)]
        self.assertEqual(ids, [row[2] for row in RECORDS[:-1]])
        self.assertEqual(len(re.findall(r'blx\s+65e68\b', tail)), 8)
        self.assertEqual(len(re.findall(r'blx\s+65ea4\b', tail)), 7)
        for pc, literal, _, filename in RECORDS:
            data = self.elf.bytes_at(pc + 4 + self.word(literal), 256)
            self.assertEqual(data.split(b'\0', 1)[0].decode(), 'data/textures/' + filename)
        source = (ROOT / 'src/engine/render/BuildResourceList.cpp').read_text()
        tail_source = source[source.index('// ARM 0x10f160-0x10f2d0:'):]
        tail_source = tail_source[:tail_source.index('\n        }')]
        pairs = [(int(i), name) for i, name in re.findall(
            r'(?:makeRes|REGISTER_RES)\((\d+), 2, new AbyssEngine::ResourceTexture\("data/textures/([^"]+)"', tail_source)]
        self.assertEqual(pairs, [(row[2], row[3]) for row in RECORDS])


if __name__ == '__main__':
    unittest.main()
