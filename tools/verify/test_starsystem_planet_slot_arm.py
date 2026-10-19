"""ARM anchors for non-selected slot rejection and selected-planet scale."""
from pathlib import Path
import hashlib
import struct
import subprocess
import unittest

import delink

ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)


@unittest.skipUnless(SO.is_file(), 'original Android binary unavailable')
class StarSystemPlanetSlotArm(unittest.TestCase):
    def test_slot_loop_scale_and_rotation_anchors(self):
        data = SO.read_bytes()
        self.assertEqual(hashlib.sha256(data).hexdigest(),
                         '6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58')
        asm = subprocess.check_output(delink.tool_argv(str(ROOT / 'tools/verify/orbobjdump'), [
            '-d', '--start-address=0x13416c', '--stop-address=0x1343ac', str(SO)]), text=True)
        for pattern in (
            r'134196:.*cmp\s+r4, r5',
            r'1341ac:.*PlayerStaticC1EiP10AEGeometryfff@plt',
            r'1341ea:.*nextIntEi@plt',
            r'134262:.*nextIntEi@plt',
            r'134266:.*add\.w\s+fp, r0, #7',
            r'134274:.*cmp\s+r0, #3',
            r'134284:.*beq\.n\s+13425e',
            r'13428a:.*nextIntEi@plt',
            r'1342a6:.*vmla\.f64',
            r'1342aa:.*vmul\.f64',
            r'1342be:.*setScalingERK',
            r'134332:.*nextIntEi@plt',
            r'134350:.*vstr\s+s0, \[r8, #88\]',
            r'1343a4:.*setRotationERK',
        ):
            self.assertRegex(asm, pattern)
        self.assertEqual(struct.unpack_from('<d', data, 0x134418)[0], 0.01)
        self.assertEqual(struct.unpack_from('<d', data, 0x134420)[0], float(struct.unpack('<f', bytes.fromhex('cdcc4c3f'))[0]))
        self.assertEqual(struct.unpack_from('<d', data, 0x134428)[0], 0.03509521484375)
        self.assertEqual(struct.unpack_from('<I', data, 0x1346a4)[0], 0x21840)


if __name__ == '__main__':
    unittest.main()
