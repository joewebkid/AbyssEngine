"""Optional read-only original ARM contract checks for texture String ownership."""
from pathlib import Path
import re
import subprocess
import unittest

import delink

ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)


@unittest.skipUnless(SO.is_file(), 'original Android binary unavailable')
class ResourceTextureStringArm(unittest.TestCase):
    def disassemble(self, start, end):
        return subprocess.check_output(delink.tool_argv(str(ROOT / 'tools/verify/orbobjdump'), [
            '-d', f'--start-address={start:#x}', f'--stop-address={end:#x}', str(SO)]), text=True)

    def test_saved_length_copy_and_temporary_ownership(self):
        asm = self.disassemble(0x11b6d2, 0x11b710)
        calls = re.findall(r'\bblx?\s+[0-9a-f]+\s+<([^>]+)>', asm)
        self.assertEqual(calls, [
            '_ZNK11AbyssEngine6String9GetAECharEv@plt', '_Znaj@plt',
            '__aeabi_memcpy@plt', '_ZdaPv@plt',
        ])
        for instruction in (
            r'ldr\s+r0, \[r5, #8\]', r'adds\s+r5, r0, #1', r'mov\s+r2, r5',
            r'str\s+r0, \[r6, #0\]', r'str\.w\s+r8, \[r6, #4\]',
        ):
            self.assertRegex(asm, instruction)

    def test_conversion_includes_stored_terminal_low_byte(self):
        asm = self.disassemble(0x72752, 0x72784)
        for instruction in (
            r'ldr\s+r0, \[r0, #8\]', r'cbz\s+r0, 72778', r'adds\s+r5, r0, #1',
            r'ldr\s+r2, \[r4, #4\]', r'ldrb\.w\s+r2, \[r2, r1, lsl #1\]',
            r'strb\s+r2, \[r0, r1\]', r'cmp\s+r5, r1', r'bne\.n\s+72768',
            r'movs\s+r0, #1', r'movs\s+r1, #0', r'strb\s+r1, \[r0, #0\]',
        ):
            self.assertRegex(asm, instruction)

    def test_destructor_deletes_owned_buffer_then_clears_pointer(self):
        asm = self.disassemble(0x77ebc, 0x77ed0)
        self.assertRegex(asm, r'ldr\s+r0, \[r0, #0\]')
        self.assertIn('_ZdaPv@plt', asm)
        self.assertRegex(asm, r'movs\s+r0, #0')
        self.assertRegex(asm, r'str\s+r0, \[r4, #0\]')


if __name__ == '__main__':
    unittest.main()
