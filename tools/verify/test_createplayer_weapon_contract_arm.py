"""Primary Android ARM anchors for createPlayer's immediate weapon consumers."""
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
class CreatePlayerWeaponsArm(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.elf = Elf32(SO)
        cls.asm = cls.disassemble(0xaf1bc, 0xaf6c8)

    @staticmethod
    def disassemble(start, end):
        return subprocess.check_output(delink.tool_argv(str(ROOT / 'tools/verify/orbobjdump'), [
            '-d', f'--start-address={start:#x}', f'--stop-address={end:#x}', str(SO)]), text=True)

    def test_loader_lifetime_and_engine_pairs(self):
        functions = self.elf.functions()
        for symbol, address, size in (
            ('_ZN5Level12createPlayerEv', 0xaf1bc, 0x53c),
            ('_ZN3Gun9setOffsetEPN11AbyssEngine6AEMath6VectorE', 0x1522e4, 0x44),
            ('_ZN9PlayerEgo17setTurretPositionEN11AbyssEngine6AEMath6VectorE', 0x9b2bc, 0x30),
        ):
            self.assertEqual((functions[symbol]['value'], functions[symbol]['size']), (address, size))
        asm = self.asm
        self.assertRegex(asm, r'af360:.*<_ZN8FileRead19loadWeaponPositionsEi@plt>')
        self.assertRegex(asm, r'af368:.*<_ZN8FileReadD1Ev@plt>')
        self.assertRegex(asm, r'af36c:.*<_ZdlPv@plt>')
        for pattern in (
            r'af372:.*ldr\s+r0, \[r0, #12\]',  # outer slot 3
            r'af3b8:.*ldr.w\s+r0, \[r1, r5, lsl #1\]',  # geometry k/2
            r'af3bc:.*ldr.w\s+r1, \[r2, r5, lsl #2\]',  # position k
            r'af3c0:.*<_ZN10AEGeometry11setPositionERKN11AbyssEngine6AEMath6VectorE@plt>',
            r'af3d8:.*ldr\s+r1, \[r1, #4\]',  # scaling k+1
            r'af3da:.*<_ZN10AEGeometry10setScalingERKN11AbyssEngine6AEMath6VectorE@plt>',
            r'af3de:.*adds\s+r5, #2',
        ):
            self.assertRegex(asm, pattern)

    def test_turret_uses_first_vector_and_other_guns_select_mount(self):
        turret = self.asm[self.asm.index('   af5c8:'):self.asm.index('   af5f2:')]
        for pattern in (
            r'af5d0:.*ldr\s+r0, \[r0, #8\]',  # bucket 2
            r'af5da:.*str.w\s+r6, \[r0, r1, lsl #2\]',
            r'af5e0:.*ldr\s+r0, \[r0, #8\]',  # position slot 2
            r'af5e4:.*ldr\s+r3, \[r0, #0\]',  # ALWAYS vector 0
            r'af5ea:.*ldmia\s+r3, \{r1, r2, r3\}',
            r'af5ec:.*<_ZN9PlayerEgo17setTurretPositionEN11AbyssEngine6AEMath6VectorE@plt>',
        ):
            self.assertRegex(turret, pattern)
        self.assertNotIn('getSlotPos', turret)
        for pattern in (
            r'af5ae:.*subs\s+r1, #1', r'af5d4:.*subs\s+r1, #1', r'af5fc:.*subs\s+r1, #1',
            r'af616:.*<_ZN4Ship10getSlotPosEP4Item@plt>',
            r'af61e:.*ldr.w\s+r1, \[r1, r0, lsl #2\]',
            r'af624:.*<_ZN3Gun9setOffsetEPN11AbyssEngine6AEMath6VectorE@plt>',
        ):
            self.assertRegex(self.asm, pattern)

    def test_cleanup_precedes_gun_publication(self):
        cleanup = self.asm[self.asm.index('   af634:'):self.asm.index('   af680:')]
        calls = re.findall(r'\bblx?\s+[0-9a-f]+\s+<([^>]+)>', cleanup)
        self.assertEqual(calls, [
            '_Z19ArrayReleaseClassesIPN11AbyssEngine6AEMath6VectorEEvR5ArrayIT_E@plt',
            '_ZN5ArrayIPN11AbyssEngine6AEMath6VectorEED2Ev@plt', '_ZdlPv@plt',
            '_Z19ArrayReleaseClassesIP5ArrayIPN11AbyssEngine6AEMath6VectorEEEvRS0_IT_E@plt',
            '_ZN5ArrayIPS_IPN11AbyssEngine6AEMath6VectorEEED2Ev@plt', '_ZdlPv@plt'])
        self.assertRegex(cleanup, r'af662:.*str.w\s+r8, \[r0\]')  # clear inner pointer
        tail = self.asm[self.asm.index('   af680:'):]
        self.assertRegex(tail, r'af698:.*<_ZN9PlayerEgo6addGunEP5ArrayIP3GunEi@plt>')
        self.assertRegex(tail, r'af6a8:.*<_ZdaPv@plt>')  # slot counters, NOT gun buckets
        self.assertNotIn('_ZdlPv@plt', tail)
        # Normal addGun path copies Gun pointers into a new Array. The delete
        # at 0xa397c belongs to constructor exception cleanup, outside this span.
        publish = self.disassemble(0xa3910, 0xa3978)
        self.assertRegex(publish, r'a3930:.*<_ZN5ArrayIP3GunEC2Ev@plt>')
        self.assertRegex(publish, r'a3946:.*ldr.w\s+r0, \[r1, r5, lsl #2\]')
        self.assertRegex(publish, r'a394e:.*<_Z8ArrayAddIP3GunEvT_R5ArrayIS2_E@plt>')
        self.assertNotIn('_ZdlPv@plt', publish)
        wrapper = self.disassemble(0x9b78e, 0x9b7a4)
        self.assertRegex(wrapper, r'9b796:.*<_ZN6Player6addGunEP5ArrayIP3GunEi@plt>')

    def test_value_copy_receivers_and_z_bias(self):
        gun = self.disassemble(0x1522e4, 0x152328)
        self.assertRegex(gun, r'1522ea:.*adds\s+r0, #124')
        self.assertRegex(gun, r'1522f8:.*vldr\s+s2, \[r1, #8\]')
        self.assertRegex(gun, r'152300:.*vadd.f32\s+s0, s2, s0')
        self.assertEqual(struct.unpack('<f', self.elf.bytes_at(0x152320, 4))[0], 100.0)
        self.assertIn('_ZN11AbyssEngine6AEMath6VectoraSERKS1_@plt', gun)
        ego = self.disassemble(0x9b2bc, 0x9b2ec)
        self.assertRegex(ego, r'9b2c2:.*add.w\s+r0, r0, #548')
        self.assertRegex(ego, r'9b2c6:.*strd\s+r1, r2, \[sp\]')
        self.assertRegex(ego, r'9b2d0:.*str\s+r3, \[sp, #8\]')
        self.assertIn('_ZN11AbyssEngine6AEMath6VectoraSERKS1_@plt', ego)


if __name__ == '__main__':
    unittest.main()
