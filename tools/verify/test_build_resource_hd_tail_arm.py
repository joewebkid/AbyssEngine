"""Bounded Android ARM oracle for platform and post-platform resource registration.

Executes only integer loads/stores, branches and the three identified PLT calls;
unknown instructions/calls fail closed. No game instructions are executed on host.
"""
from pathlib import Path
import hashlib
import re
import struct
import subprocess
import unittest
import delink
from elf import Elf32

ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)
FLAGS = {0x21038c: '_ZN7Globals6iPadHDE', 0x21032c: '_ZN7Globals4iPadE',
         0x2103e4: '_ZN7Globals13retinaDisplayE', 0x2103f0: '_ZN7Globals9iPadLargeE'}

class TailOracle:
    def __init__(self):
        self.elf = Elf32(SO)
        asm = subprocess.check_output(delink.tool_argv(str(ROOT / 'tools/verify/orbobjdump'), [
            '-d', '--start-address=0x10a25a', '--stop-address=0x10f2d4', str(SO)]), text=True)
        self.code = {}
        for line in asm.splitlines():
            m = re.match(r'\s*([0-9a-f]+):\s+((?:[0-9a-f]{4}\s+)+)\s*([a-z][a-z0-9.]*)\s*(.*)', line)
            if m:
                address, raw, op, args = m.groups()
                self.code[int(address, 16)] = (len(raw.split()) * 2, op, args.split(';')[0].strip())

    def word(self, address):
        data = self.elf.bytes_at(address, 4)
        assert len(data) == 4, hex(address)
        return struct.unpack('<I', data)[0]

    def run(self, hd, pad, retina, large=0, start=0x10a25a, stop=0x10edbe):
        regs = {'sl': 0x50000000, 'sp': 0x70000000}
        memory = {0x70000004: 0x71000000}  # saved stack-canary pointer; used only after stop
        textures = {}
        records = []
        for index, (got, value) in enumerate(zip(FLAGS, (hd, pad, retina, large))):
            pointer = 0x60000000 + index * 4
            memory[got] = pointer
            memory[pointer] = value
        pc, allocated, zero = start, 0x40000000, False
        def value(token):
            return int(token[1:]) if token.startswith('#') else regs[token]
        for _ in range(30000):
            if pc == stop:
                return records
            size, op, args = self.code[pc]
            next_pc = pc + size
            base = op.split('.')[0]
            if base in ('b', 'beq', 'bne'):
                if base == 'b' or (base == 'beq' and zero) or (base == 'bne' and not zero):
                    next_pc = int(args.split()[0], 16)
            elif base == 'blx':
                target = int(args.split()[0], 16)
                if target == 0x5eb24:
                    assert regs['r0'] in (4, 8, 16), (hex(pc), regs['r0'])
                    regs['r0'] = allocated
                    allocated += 32
                elif target == 0x65e68:
                    assert regs['r2'] == 0
                    name = self.elf.bytes_at(regs['r1'], 256).split(b'\0')[0].decode()
                    textures[regs['r0']] = name
                elif target == 0x65ea4:
                    assert regs['r0'] == regs['sl']
                    resource = regs['r1']
                    rid, kind, unused, payload = [memory[resource + offset] for offset in (0, 4, 8, 12)]
                    assert unused == 0xffffffff
                    if kind == 2:
                        data = textures[payload]
                    else:
                        assert kind == 3
                        data = memory[payload]
                    records.append((pc, rid, kind, data))
                else:
                    raise AssertionError(('unexpected call', hex(pc), hex(target)))
                for reg in ('r0', 'r1', 'r2', 'r3', 'ip'):
                    if reg != 'r0' or target != 0x5eb24:
                        regs.pop(reg, None)
            elif base in ('mov', 'movs', 'movw'):
                dest, src = args.split(', ')
                regs[dest] = value(src)
            elif base in ('add', 'adds'):
                parts = args.split(', ')
                dest = parts[0]
                left, right = parts[1:] if len(parts) == 3 else (dest, parts[1])
                regs[dest] = (value(left) + (pc + 4 if right == 'pc' else value(right))) & 0xffffffff
            elif base == 'cmp':
                left, right = args.split(', ')
                zero = value(left) == value(right)
            elif base in ('ldr', 'ldrb', 'str', 'strh', 'strd'):
                m = re.fullmatch(r'([^\[]+), \[(\w+)(?:, #(-?\d+))?\]', args)
                assert m, (hex(pc), op, args)
                operands, addr_reg, offset = m.groups()
                address = ((pc + 4) & ~3 if addr_reg == 'pc' else regs[addr_reg]) + int(offset or 0)
                operands = operands.split(', ')
                if base.startswith('ldr'):
                    assert address != 0 and (address in memory or address < 0x300000), (hex(pc), hex(address), regs)
                    loaded = memory[address] if address in memory else self.word(address)
                    regs[operands[0]] = loaded & (255 if base == 'ldrb' else 0xffffffff)
                else:
                    for i, operand in enumerate(operands):
                        memory[address + 4 * i] = value(operand) & (65535 if base == 'strh' else 0xffffffff)
            elif base == 'stmia':
                m = re.fullmatch(r'(\w+)!, \{(.+)\}', args)
                assert m
                dest, operands = m.groups()
                address = regs[dest]
                for operand in operands.split(', '):
                    memory[address] = regs[operand]
                    address += 4
                regs[dest] = address
            elif base != 'nop':
                raise AssertionError(('unexpected instruction', hex(pc), op, args))
            pc = next_pc
        raise AssertionError('bounded instruction limit exceeded')

@unittest.skipUnless(SO.is_file(), 'original Android binary unavailable')
class BuildResourceHdTailArmEvidence(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.oracle = TailOracle()

    def test_original_identity_and_got_symbols(self):
        oracle = self.oracle
        self.assertEqual(hashlib.sha256(oracle.elf.data).hexdigest(),
                         '6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58')
        symbols = list(oracle.elf._symbols(11))
        found = {}
        for section in oracle.elf.sections:
            if section['type'] == 9:
                for pos in range(section['offset'], section['offset'] + section['size'], section['entsize']):
                    address, info = struct.unpack_from('<II', oracle.elf.data, pos)
                    if address in FLAGS:
                        found[address] = (info & 255, symbols[info >> 8]['name'])
        self.assertEqual(found, {got: (21, name) for got, name in FLAGS.items()})
        self.assertEqual(0x10a896 + 4 + oracle.word(0x10aa14), 0x2103f0)
        self.assertEqual(oracle.code[0x10a89e][1], 'bne.w')
        self.assertTrue(oracle.code[0x10a89e][2].startswith('10a4ce '))

    def test_hd_five_and_shared_stream(self):
        rows = self.oracle.run(1, 1, 0)
        self.assertEqual([(rid, kind, path) for _, rid, kind, path in rows[12:17]], [
            (10089, 2, 'data/textures/gof2_interface3_1440.aei'),
            (10063, 2, 'data/textures/gof2_interface2_ipad_1440.aei'),
            (10064, 2, 'data/textures/gof2_items_ipad_1440.aei'),
            (11632, 2, 'data/textures/gof2_items_ipad_2_1440.aei'),
            (11630, 2, 'data/textures/gof2_dlc_interface_ipad_1440.aei')])
        self.assertEqual(rows[17], (0x10bf70, 3001, 3, 11630))
        self.assertEqual(rows[18], (0x10bf96, 3101, 3, 0x12d6e))
        self.assertEqual(rows[-1], (0x10edba, 9510, 3, 0x56acf))
        self.assertEqual(len([r for r in rows if 0x10bf46 <= r[0] < 0x10d8c0]), 136)
        self.assertEqual(len([r for r in rows if 0x10d8c0 <= r[0] < 0x10edbe]), 111)

    def test_all_flag_values_and_join_paths(self):
        counts = (234, 231, 264, 264, 235, 231, 271, 267,
                  234, 231, 264, 264, 228, 231, 264, 267)
        from itertools import product
        canonical = {}
        for flags in product((0, 1), repeat=4):
            rows = self.oracle.run(*flags)
            route = sum(value << bit for bit, value in enumerate(flags))
            self.assertEqual(len(rows), counts[route])
            canonical[flags] = rows
            self.assertEqual(len([r for r in rows if 0x10aa6e <= r[0] <= 0x10bdb6]),
                             0 if flags[1] else 106)
        for flags in product((0, 1, 2, 255), repeat=4):
            self.assertEqual(self.oracle.run(*flags), canonical[tuple(int(bool(v)) for v in flags)])

if __name__ == '__main__':
    unittest.main()
