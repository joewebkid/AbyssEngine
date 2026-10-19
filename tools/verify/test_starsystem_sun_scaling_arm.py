"""StarSystem sun/streak scale branches against Android ARM literals."""
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest

import delink

ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)


class StarSystemSunScalingArm(unittest.TestCase):
    @unittest.skipUnless(SO.is_file(), 'original Android binary unavailable')
    def test_arm_mission_branch_and_literal_words(self):
        asm = subprocess.check_output(delink.tool_argv(str(ROOT / 'tools/verify/orbobjdump'), [
            '-d', '--start-address=0x13444a', '--stop-address=0x134512', str(SO)]), text=True)
        self.assertRegex(asm, r'13444e:.*getCurrentCampaignMissionEv@plt')
        self.assertRegex(asm, r'134458:.*cmp\s+r1, #106')
        self.assertRegex(asm, r'13445e:.*it[e]?\s+lt')
        self.assertRegex(asm, r'1344de:.*setScalingERK')
        self.assertRegex(asm, r'13450e:.*setScalingERK')
        words = subprocess.check_output(delink.tool_argv(str(ROOT / 'tools/verify/orbobjdump'), [
            '-s', '--start-address=0x134690', '--stop-address=0x1346b8', str(SO)]), text=True)
        for word in ('00606a3e', '0060ea3e', '00803b3d', '00e87d3f',
                     '00c8af3f', '00e8fd3f', '00204b3e'):
            self.assertIn(word, words)

    def test_production_expressions_cover_normal_and_supernova_scales(self):
        cxx = shutil.which('g++')
        if not cxx:
            self.skipTest('host C++ compiler unavailable')
        source = (ROOT / 'src/game/world/StarSystem.cpp').read_text()
        normal = source[source.index('    int sunSlot = 0;'):]
        sun = re.search(r'float sunScale = (.*?);', normal, re.S)
        streak = re.search(r'streak->setScaling\(Vector\{(.*?)\}\);', normal, re.S)
        self.assertIsNotNone(sun)
        self.assertIsNotNone(streak)
        expression = sun.group(1).replace('this->supernovaSystem', 'supernova').replace(
            'Globals::status->getCurrentCampaignMission()', 'mission')
        axes = streak.group(1).replace('this->supernovaSystem', 'supernova')
        fixture = '''#include <cassert>
void check(int supernova, int mission, float sunExpected,
           float xExpected, float yExpected, float zExpected) {
    float sunScale = SUN;
    float streak[3] = {AXES};
    assert(sunScale == sunExpected);
    assert(streak[0] == xExpected && streak[1] == yExpected && streak[2] == zExpected);
}
int main() {
    check(0, 50, 0.2288818359375f, 0.457763671875f, 0.0457763671875f, 0.2288818359375f);
    check(1, 105, 0.9918212890625f, 1.983642578125f, 0.1983642578125f, 0.9918212890625f);
    check(1, 106, 1.373291015625f, 1.983642578125f, 0.1983642578125f, 0.9918212890625f);
}
'''.replace('SUN', expression).replace('AXES', axes)
        with tempfile.TemporaryDirectory(prefix='starsystem-sun-scale-') as folder:
            path = Path(folder) / 'scale.cpp'
            path.write_text(fixture)
            for opt in ('-O0', '-O2'):
                exe = Path(folder) / (opt[1:] + '.exe')
                compiled = subprocess.run([cxx, '-std=gnu++17', opt, str(path), '-o', str(exe)],
                                          capture_output=True, text=True)
                self.assertEqual(compiled.returncode, 0, compiled.stderr)
                subprocess.run([str(exe)], check=True)


if __name__ == '__main__':
    unittest.main()
