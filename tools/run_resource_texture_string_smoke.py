"""Link real production String/ResourceTexture TUs and check the ARM byte contract."""
import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cxx', default=shutil.which('g++'))
    args = parser.parse_args()
    if not args.cxx:
        parser.error('specify --cxx')
    with tempfile.TemporaryDirectory(prefix='texture-string-', dir=ROOT / '_work') as temp:
        for opt in ('-O0', '-O2'):
            exe = Path(temp) / f'texture_string_{opt[1:]}.exe'
            subprocess.run([
                args.cxx, '-std=gnu++17', opt, '-g0', '-ffunction-sections', '-fdata-sections',
                '-Isrc', 'tools/resource_texture_string_smoke.cpp', 'src/game/core/String.cpp',
                'src/engine/render/ResourceTexture.cpp', '-Wl,--gc-sections', '-o', str(exe),
            ], cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
