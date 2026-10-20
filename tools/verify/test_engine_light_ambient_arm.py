"""The original ARM ambient-light setter stores float-one bits in its alpha slot."""
from pathlib import Path
import hashlib
import subprocess
import unittest

import delink

ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)
OBJDUMP = str(ROOT / "tools/verify/orbobjdump")


@unittest.skipUnless(SO.is_file(), "original Android binary unavailable")
class EngineLightAmbientArm(unittest.TestCase):
    def test_scene_ambient_alpha_is_float_one(self):
        self.assertEqual(hashlib.sha256(SO.read_bytes()).hexdigest(),
                         "6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58")
        asm = subprocess.check_output(delink.tool_argv(OBJDUMP, [
            "-d", "--start-address=0x85830", "--stop-address=0x85848", str(SO)]), text=True)
        self.assertRegex(asm, r"85836:.*mov\.w\s+ip, #1065353216.*0x3f800000")
        self.assertRegex(asm, r"85840:.*stmia\.w\s+lr, \{r1, r2, r3, ip\}")


if __name__ == "__main__":
    unittest.main()
