"""Call target for the shared VectorNormalize return-by-value ABI."""
from pathlib import Path
import subprocess
import unittest

import delink

ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)
TARGET = '_ZN11AbyssEngine6AEMath15VectorNormalizeERKNS0_6VectorE@plt'


class VectorNormalizeCallAbiArm(unittest.TestCase):
    @unittest.skipUnless(SO.is_file(), 'original Android binary unavailable')
    def test_geometry_and_ship_callers_use_return_by_value_symbol(self):
        for start, stop, count in (
            (0xa49b8, 0xa4a40, 1),        # AEGeometry::moveForward
            (0xdfc88, 0xdfdc8, 2),       # PlayerFighter::initPush
            (0x176660, 0x176968, 1),     # PlayerGasCloud::explode
            (0x1769b4, 0x176e19, 1),     # PlayerGasCloud::update
        ):
            with self.subTest(address=hex(start)):
                asm = subprocess.check_output(delink.tool_argv(
                    str(ROOT / 'tools/verify/orbobjdump'), [
                        '-d', f'--start-address={hex(start)}',
                        f'--stop-address={hex(stop)}', str(SO)]), text=True)
                self.assertEqual(asm.count(TARGET), count)


if __name__ == '__main__':
    unittest.main()
