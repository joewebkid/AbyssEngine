"""Compile the current MeshReadData body verbatim with bounded test boundaries.

The Windows linker retains unrelated dependencies of AbyssEngine.cpp, so this
test extracts only its exact function text. This is a reader-body smoke test,
not a whole-TU linkage or native ABI test. The native library build is separate.
"""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--compiler', default='g++')
parser.add_argument('--output-dir', type=Path, required=True)
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
source = (root / 'src/engine/core/AbyssEngine.cpp').read_text()
pattern = r'    int MeshReadData\(Engine \*engine, const unsigned int &handleRef, unsigned int flags, Mesh \*\*slot,\s+Material \*mat\) \{'
matches = list(re.finditer(pattern, source))
assert len(matches) == 1, 'Expected exactly one production definition'
match = matches[0]
depth = 1
end = match.end()
while depth:
    assert end < len(source), 'Unclosed function body'
    depth += (source[end] == '{') - (source[end] == '}')
    end += 1
body = source[match.start():end]
output = args.output_dir.resolve()
output.mkdir(parents=True, exist_ok=True)
include = output / 'mesh_read_under_test.inc'
include.write_text('namespace AbyssEngine {\n' + body + '\n}\n')
assert body in include.read_text(), 'Extracted function must remain verbatim'
exe = output / 'mesh_read_color_smoke.exe'
subprocess.run([args.compiler, '-std=c++14', '-I' + str(root / 'src'),
                '-I' + str(root / 'third_party/gl'), '-I' + str(output),
                str(root / 'tools/mesh_read_color_smoke.cpp'),
                str(root / 'src/engine/math/Vector.cpp'), '-o', str(exe)], check=True)
subprocess.run([str(exe)], check=True)
print('MeshReadData color/position/truncation smoke passed')
