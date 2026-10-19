"""Run the production portrait registration body and String/texture code on a host."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--cxx', default=shutil.which('g++'))
    parser.add_argument('--assets', type=Path, help='optional original Android assets directory')
    args = parser.parse_args()
    if not args.cxx:
        parser.error('specify --cxx')
    material = (ROOT / 'src/engine/render/ResourceMaterial.cpp').read_text()
    string = (ROOT / 'src/game/core/String.cpp').read_text()
    engine = (ROOT / 'src/engine/core/AbyssEngine.cpp').read_text()
    texture = (ROOT / 'src/engine/render/ResourceTexture.cpp').read_text()
    bodies = '\n'.join(function(string, signature) for signature in (
        'String::~String()', 'char *String::GetAEChar() const',
        'String::String() :', 'String::String(const char *cstr, bool reverse)',
        'String::String(const String &other, bool reverse)',
        'void String::Set(const char *s)', 'void String::Set(const unsigned short *s)',
        'int String::GetStringLength(const char *s)',
        'String &String::operator+=(const String &other)'))
    bodies += '\nnamespace AbyssEngine {\n' + function(
        engine, 'String operator+(const String &a, const String &b)')
    bodies += function(texture, 'ResourceTexture::ResourceTexture(const String &name, float value)')
    bodies += function(texture, 'ResourceTexture::~ResourceTexture()') + '\n}\n'
    resource = 'namespace AbyssEngine {\n' + function(material, 'struct Resource {') + ';\n}\n'
    bodies += resource + function(material, 'void loadPortraits(AbyssEngine::Engine *engine)')
    fixture = (ROOT / 'tools/load_portraits_smoke.cpp').read_text().replace(
        '// PRODUCTION_BODIES', bodies)
    with tempfile.TemporaryDirectory(prefix='load-portraits-', dir=ROOT / '_work') as temp:
        generated = Path(temp) / 'load_portraits.cpp'
        generated.write_text(fixture)
        for opt in ('-O0', '-O2'):
            exe = Path(temp) / f'portraits_{opt[1:]}.exe'
            subprocess.run([args.cxx, '-std=gnu++17', opt, '-Isrc', str(generated),
                            '-o', str(exe)], cwd=ROOT, check=True)
            subprocess.run([str(exe), *([str(args.assets)] if args.assets else [])],
                           cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
