"""Read-only primary ARM anchors for the bounded String float family."""
from pathlib import Path
import re
import struct
import subprocess
import unittest

import delink
from elf import Elf32

ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)


@unittest.skipUnless(SO.is_file(), 'original Android binary unavailable')
class StringFloatArm(unittest.TestCase):
    def disassemble(self, start, end):
        return subprocess.check_output(delink.tool_argv(str(ROOT / 'tools/verify/orbobjdump'), [
            '-d', f'--start-address={start:#x}', f'--stop-address={end:#x}', str(SO)]), text=True)

    def test_fixed_original_symbols_and_constructor_abi(self):
        functions = Elf32(SO).functions()
        self.assertNotIn('_ZN11AbyssEngine6StringC1Ef', functions)
        self.assertEqual(functions['_ZN11AbyssEngine6StringC2Ef']['value'], 0x729bc)
        self.assertEqual(functions['_ZN11AbyssEngine6StringC2Ef']['size'], 0x24)
        self.assertEqual(functions['_ZN11AbyssEngine6String3SetEf']['value'], 0x72cb0)
        self.assertEqual(functions['_ZN11AbyssEngine6String3SetEf']['size'], 0x1cc)
        asm = self.disassemble(0x729bc, 0x729e0)
        self.assertIn('_ZN11AbyssEngine6String3SetEf@plt', asm)
        self.assertRegex(asm, r'movs\s+r2, #0')
        self.assertRegex(asm, r'strd\s+r0, r2, \[r4\]') # vptr + null data, not length
        self.assertNotRegex(asm, r'\[r4, #8\]')

    def test_softfp_precision_and_mode_one_call(self):
        asm = self.disassemble(0x72cb0, 0x72cda)
        for instruction in (
            r'72cce:.*movs\s+r0, #1', r'72cd0:.*str\s+r0, \[sp, #0\]',
            r'72cd2:.*mov\s+r0, r1', r'72cd4:.*movs\s+r1, #10',
            r'add\s+r2, sp, #84', r'add\s+r3, sp, #80',
        ):
            self.assertRegex(asm, instruction)
        self.assertIn('_ZN11AbyssEngine18computeFloatStringEfiPiS0_i@plt', asm)

    def test_digit_ownership_and_final_copy_order(self):
        asm = self.disassemble(0x72d98, 0x72dac)
        calls = re.findall(r'\bblx?\s+[0-9a-f]+\s+<([^>]+)>', asm)
        self.assertEqual(calls, ['_ZdaPv@plt', '_ZN11AbyssEngine6String3SetEPKt@plt',
                                 '_ZN11AbyssEngine6StringD2Ev@plt'])
        self.assertRegex(asm, r'mov\s+r0, r9') # helper return saved in r9, delete[] first
        self.assertRegex(asm, r'ldr\s+r1, \[sp, #72\]') # result.data remains alive for Set
        elf = Elf32(SO)
        for literal, pc, expected in (
            (0x72e68, 0x72dc4, b'0.'), (0x72e6c, 0x72dd8, b'0'),
            (0x72e70, 0x72d1e, b'.'), (0x72e78, 0x72d70, b'-'),
        ):
            address = pc + 4 + struct.unpack('<I', elf.bytes_at(literal, 4))[0]
            self.assertEqual(elf.bytes_at(address, len(expected) + 1), expected + b'\0')


if __name__ == '__main__':
    unittest.main()
