"""Read-only original ARM anchors for the four-slot weapon-position loader."""
from pathlib import Path
import re
import struct
import subprocess
import unittest

import delink
from elf import Elf32, SHT_DYNSYM

ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)


@unittest.skipUnless(SO.is_file(), 'original Android binary unavailable')
class WeaponPositionsArm(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.elf = Elf32(SO)
        cls.asm = subprocess.check_output(delink.tool_argv(str(ROOT / 'tools/verify/orbobjdump'), [
            '-d', '--start-address=0x1212e4', '--stop-address=0x1214d8', str(SO)]), text=True)

    def test_path_lifetime_four_slots_missing_return(self):
        symbol = self.elf.functions()['_ZN8FileRead19loadWeaponPositionsEi']
        self.assertEqual((symbol['value'], symbol['size']), (0x1212e4, 0x1f4))
        names = re.findall(r'\bblx?\s+[0-9a-f]+\s+<([^>]+)>', self.asm)
        self.assertEqual(names[:4], ['_ZN11AbyssEngine6StringC1EPKcb@plt',
            '_ZN6AEFile9FileExistERKN11AbyssEngine6StringE@plt',
            '_ZN11AbyssEngine6StringD1Ev@plt', '_ZN6AEFile8OpenReadEPKcPj@plt'])
        self.assertRegex(self.asm, r'12132e:.*movs\s+r0, #4')
        self.assertIn('ArraySetLength', self.asm)
        self.assertRegex(self.asm, r'12148a:.*mov\.w\s+fp, #0')
        self.assertIn('_ZN6AEFile5CloseEj@plt', self.asm)

    def test_raw_read_got_targets_and_persistent_float_slots(self):
        symbols = list(self.elf._symbols(SHT_DYNSYM))
        relocations = {}
        for section in self.elf.sections:
            if section['type'] != 9: # SHT_REL
                continue
            for offset in range(section['offset'], section['offset'] + section['size'], 8):
                address, info = struct.unpack_from('<II', self.elf.data, offset)
                relocations[address] = (info & 255, symbols[info >> 8]['name'])
        for literal, pc, expected in (
            (0x1214d0, 0x12136c, '_ZN6AEFile4ReadERsj'),
            (0x1214d4, 0x121374, '_ZN6AEFile4ReadERfj'),
        ):
            slot = pc + 4 + struct.unpack('<I', self.elf.bytes_at(literal, 4))[0]
            self.assertEqual(relocations[slot], (21, expected))
        self.assertRegex(self.asm, r'121368:.*str\s+r0, \[sp, #20\]')
        for address, offset in ((0x1213ba, 28), (0x1213c4, 24), (0x1213ca, 20)):
            self.assertRegex(self.asm, rf'{address:x}:.*add\s+r0, sp, #{offset}')
        loop = self.asm[self.asm.index('  12137a:'):self.asm.index('  121480:')]
        self.assertNotRegex(loop, r'\b(?:str|vstr)(?:\.w)?\s+[^\n]+\[sp, #(20|24|28)\]')
        wrapper = subprocess.check_output(delink.tool_argv(str(ROOT / 'tools/verify/orbobjdump'), [
            '-d', '--start-address=0x70c3a', '--stop-address=0x70c44', str(SO)]), text=True)
        self.assertRegex(wrapper, r'mov\s+r2, r1')
        self.assertRegex(wrapper, r'mov\s+r1, r0')
        self.assertRegex(wrapper, r'movs\s+r0, #4')
        self.assertNotRegex(wrapper, r'\b(?:str|vstr)') # typed wrapper does not initialize destination

    def test_coordinate_store_order_and_signed_count(self):
        for instruction in (
            r'121410:.*negs\s+r1, r1', r'12142e:.*vstr\s+s2, \[r0\]',
            r'121432:.*vstr\s+s0, \[r0, #4\]', r'121436:.*vstr\s+s4, \[r0, #8\]',
            r'121454:.*ldr\s+r2, \[sp, #28\]', r'121456:.*ldrd\s+r3, r1, \[sp, #20\]',
            r'12145a:.*strd\s+r2, r3, \[r0\]', r'12145e:.*str\s+r1, \[r0, #8\]',
            r'12146e:.*ldrsh\.w\s+r0, \[sp, #40\]',
        ):
            self.assertRegex(self.asm, instruction)


if __name__ == '__main__':
    unittest.main()
