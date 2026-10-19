"""Execute real String float bodies and the unchanged production digit helper."""
import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cxx', default=shutil.which('g++'))
    args = parser.parse_args()
    if not args.cxx:
        parser.error('specify --cxx')
    engine = (ROOT / 'src/engine/core/AbyssEngine.cpp').read_text()
    with tempfile.TemporaryDirectory(prefix='string-float-', dir=ROOT / '_work') as temp:
        out = Path(temp)
        (out / 'string_float_concat.inc').write_text(function(
            engine, 'String operator+(const String &a, const String &b)'))
        for opt in ('-O0', '-O2'):
            exe = out / f'string_float_{opt[1:]}.exe'
            subprocess.run([
                args.cxx, '-std=gnu++17', opt, '-g0', '-Isrc', '-I', str(out),
                'tools/string_float_smoke.cpp', 'src/game/core/String.cpp', '-o', str(exe),
            ], cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
