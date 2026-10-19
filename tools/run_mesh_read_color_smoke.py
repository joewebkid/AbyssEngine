"""Compile current MeshReadData, optional caller and vector math bodies verbatim.

The Windows linker retains unrelated dependencies of AbyssEngine.cpp, so this
test extracts their exact function text. This is a function-body smoke test,
not a whole-TU linkage or native ABI test. The native library build is separate.
"""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--compiler', default='g++')
parser.add_argument('--output-dir', type=Path, required=True)
parser.add_argument('--create-smoke', action='store_true', help='Also exercise the current caller with the real reader and enhanced parser')
parser.add_argument('--asset', type=Path, action='append', default=[], help='Original mesh file to parse with --create-smoke')
args = parser.parse_args()
if args.asset and not args.create_smoke:
    parser.error('--asset requires --create-smoke')
for asset in args.asset:
    if not asset.is_file():
        parser.error(f'asset does not exist: {asset}')
root = Path(__file__).resolve().parents[1]
def extract(source, pattern):
    matches = list(re.finditer(pattern, source))
    assert len(matches) == 1, 'Expected exactly one production definition'
    match = matches[0]
    depth, end = 1, match.end()
    while depth:
        assert end < len(source), 'Unclosed function body'
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[match.start():end]

source = (root / 'src/engine/core/AbyssEngine.cpp').read_text()
pattern = r'    int MeshReadData\(Engine \*engine, const unsigned int &handleRef, unsigned int flags, Mesh \*\*slot,\s+Material \*mat\) \{'
body = extract(source, pattern)
output = args.output_dir.resolve()
output.mkdir(parents=True, exist_ok=True)
include = output / 'mesh_read_under_test.inc'
include.write_text('namespace AbyssEngine {\n' + body + '\n}\n')
assert body in include.read_text(), 'Extracted function must remain verbatim'
math_source = (root / 'src/engine/math/AEMath.cpp').read_text()
signatures = [
    '__attribute__((noinline)) Vector &Vector::operator=(const Vector &other)',
    'Vector &Vector::operator+=(const Vector &other)',
    'Vector operator-(const Vector &lhs, const Vector &rhs)',
    'Vector operator*(const Vector &lhs, float rhs)',
    'float VectorDot(const Vector &lhs, const Vector &rhs)',
    'Vector VectorCross(const Vector &lhs, const Vector &rhs)',
    'Vector VectorNormalize(const Vector &value)',
    'float VectorLength(const Vector &value)',
]
math_bodies = [extract(math_source, re.escape(signature) + r' \{') for signature in signatures]
math_include = output / 'mesh_read_math_under_test.inc'
math_include.write_text('namespace AbyssEngine { namespace AEMath {\n' + '\n'.join(math_bodies) + '\n}}\n')
assert all(part in math_include.read_text() for part in math_bodies)
exe = output / 'mesh_read_color_smoke.exe'
subprocess.run([args.compiler, '-std=c++14', '-I' + str(root / 'src'),
                '-I' + str(root / 'third_party/gl'), '-I' + str(output),
                str(root / 'tools/mesh_read_color_smoke.cpp'),
                str(root / 'src/engine/math/Vector.cpp'), '-o', str(exe)], check=True)
subprocess.run([str(exe)], check=True)
print('MeshReadData color/position/tangent/truncation smoke passed')
if args.create_smoke:
    create_body = extract(source, r'    int MeshCreateFromFile\(Engine \*engine, const char \*path, Mesh \*\*out, Material \*mat\) \{')
    create_include = output / 'mesh_create_under_test.inc'
    create_include.write_text('namespace AbyssEngine {\n' + create_body + '\n}\n')
    assert create_body in create_include.read_text()
    bounds_source = (root / 'src/engine/math/BSphere.cpp').read_text()
    bounds_bodies = [extract(bounds_source, re.escape(signature) + r' \{') for signature in (
        '        BSphere &BSphere::operator=(const BSphere &other)',
        '        void BSphere::Merge(const BSphere &other)',
    )]
    bounds_include = output / 'mesh_read_bounds_under_test.inc'
    bounds_include.write_text('namespace AbyssEngine { namespace AEMath {\n' + '\n'.join(bounds_bodies) + '\n}}\n')
    assert all(part in bounds_include.read_text() for part in bounds_bodies)
    create_exe = output / 'mesh_create_smoke.exe'
    subprocess.run([args.compiler, '-std=c++14', '-ffunction-sections', '-fdata-sections',
                    '-I' + str(root / 'src'), '-I' + str(root / 'third_party/gl'), '-I' + str(output),
                    str(root / 'tools/mesh_create_smoke.cpp'),
                    str(root / 'src/engine/render/Mesh.cpp'), str(root / 'src/engine/math/Vector.cpp'),
                    '-Wl,--gc-sections', '-o', str(create_exe)], check=True)
    subprocess.run([str(create_exe), *(str(asset.resolve()) for asset in args.asset)], check=True)
    print(f'MeshCreateFromFile lifecycle and MeshReadData smoke passed; original assets: {len(args.asset)}')
