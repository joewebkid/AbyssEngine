"""StarSystem seed width and sign extension against the original ARM body."""
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest

import delink

ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)


class StarSystemSeedArm(unittest.TestCase):
    @unittest.skipUnless(SO.is_file(), 'original Android binary unavailable')
    def test_arm_multiplies_at_32_bits_then_sign_extends(self):
        asm = subprocess.check_output(delink.tool_argv(str(ROOT / 'tools/verify/orbobjdump'), [
            '-d', '--start-address=0x1340a4', '--stop-address=0x1340bc', str(SO)]), text=True)
        for pattern in (
            r'1340ac:.*mov.w\s+r1, #300',
            r'1340b0:.*mul.w\s+r2, r0, r1',
            r'1340b6:.*asrs\s+r3, r2, #31',
            r'1340b8:.*<_ZN11AbyssEngine8AERandom7setSeedEx@plt>',
        ):
            self.assertRegex(asm, pattern)

    def test_production_expression_preserves_wrap_for_edge_indices(self):
        cxx = shutil.which('g++')
        if not cxx:
            self.skipTest('host C++ compiler unavailable')
        source = (ROOT / 'src/game/world/StarSystem.cpp').read_text()
        calls = re.findall(r'AERandom::gRandom->setSeed\((.*?)\);', source, re.S)
        self.assertEqual(len(calls), 1)
        receiver = 'Globals::status->getStation()->getIndex()'
        self.assertEqual(calls[0].count(receiver), 1)
        current = calls[0].replace(receiver, 'index')
        baseline = '(long long) index * 300'

        with tempfile.TemporaryDirectory(prefix='starsystem-seed-') as temp:
            folder = Path(temp)
            for label, expression, expected_exit in (
                ('current', current, 0), ('old-negative-control', baseline, 1)):
                body = '''#include "engine/core/AERandom.h"
#include <cstdint>
#include <initializer_list>
#include <limits>
static long long armSeed(int index) {
    uint32_t product = uint32_t(index) * 300u;
    return product < 0x80000000u ? (long long)product : (long long)product - 0x100000000LL;
}
int main() {
    for (int index : {7, -1, 7158279, -7158279,
                      std::numeric_limits<int>::min(), std::numeric_limits<int>::max()}) {
        AbyssEngine::AERandom actual(0);
        actual.setSeed(EXPRESSION);
        AbyssEngine::AERandom expected(armSeed(index));
        if (actual.seedLow != expected.seedLow || actual.seedHigh != expected.seedHigh)
            return 1;
    }
    return 0;
}
'''.replace('EXPRESSION', expression)
                fixture = folder / (label + '.cpp')
                fixture.write_text(body)
                for optimization in ('-O0', '-O2'):
                    exe = folder / (label + optimization + '.exe')
                    compiled = subprocess.run([cxx, '-std=gnu++17', optimization,
                                               '-Isrc', str(fixture),
                                               'src/engine/core/AERandom.cpp', '-o', str(exe)],
                                              cwd=ROOT, capture_output=True, text=True)
                    self.assertEqual(compiled.returncode, 0, compiled.stderr)
                    result = subprocess.run([str(exe)], cwd=ROOT, check=False)
                    self.assertEqual(result.returncode, expected_exit,
                                     label + optimization)


if __name__ == '__main__':
    unittest.main()
