"""Original ARM GOT target for StarSystem::initLight's repeated GetEngine calls."""
from pathlib import Path
import hashlib
import subprocess
import unittest

import delink

ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)
OBJDUMP = str(ROOT / 'tools/verify/orbobjdump')


@unittest.skipUnless(SO.is_file(), 'original Android binary unavailable')
class StarSystemLightGetterArm(unittest.TestCase):
    def test_indirect_target_is_application_manager_get_engine(self):
        self.assertEqual(hashlib.sha256(SO.read_bytes()).hexdigest(),
                         '6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58')
        reloc = subprocess.check_output(delink.tool_argv(OBJDUMP, ['-R', str(SO)]), text=True)
        self.assertRegex(reloc, r'00210374\s+R_ARM_GLOB_DAT\s+'
                         r'_ZN11AbyssEngine18ApplicationManager9GetEngineEv')
        asm = subprocess.check_output(delink.tool_argv(OBJDUMP, [
            '-d', '--start-address=0x1348b8', '--stop-address=0x1348fa', str(SO)]), text=True)
        for pattern in (
            r'1348b8:.*ldr\s+r1, \[pc, #824\]',
            r'1348be:.*add\s+r1, pc',
            r'1348c8:.*ldr\.w\s+sl, \[r1\]',
            r'1348d0:.*blx\s+sl',
            r'1348f6:.*LightSetGlobalSceneColorAmbientEfff@plt',
        ):
            self.assertRegex(asm, pattern)
        words = subprocess.check_output(delink.tool_argv(OBJDUMP, [
            '-s', '--start-address=0x134bf4', '--stop-address=0x134bf8', str(SO)]), text=True)
        self.assertIn('b2ba0d00', words)  # PC-relative displacement to GOT 0x210374.


if __name__ == '__main__':
    unittest.main()
