"""Run the current system-specific station loader against synthetic file records."""
import argparse
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--compiler", default="g++")
parser.add_argument("--output-dir", type=Path, required=True)
args = parser.parse_args()

root = Path(__file__).resolve().parents[1]
source = (root / "src/engine/file/FileRead.cpp").read_text()
pattern = r"Array<Station \*> \*FileRead::loadStationsBinary\(SolarSystem \*system\) \{"
matches = list(re.finditer(pattern, source))
assert len(matches) == 1, "Expected one production loader"
start = matches[0].start()
end = matches[0].end()
depth = 1
while depth:
    assert end < len(source), "Unclosed loader body"
    depth += (source[end] == "{") - (source[end] == "}")
    end += 1
body = source[start:end]

output = args.output_dir.resolve()
output.mkdir(parents=True, exist_ok=True)
include = output / "station_loader_under_test.inc"
include.write_text(body + "\n")
assert body in include.read_text()
exe = output / "station_loader_smoke.exe"
subprocess.run([
    args.compiler, "-std=c++17", "-O0", "-I" + str(root / "src"),
    "-I" + str(output), str(root / "tools/station_loader_smoke.cpp"),
    "-o", str(exe),
], check=True)
subprocess.run([str(exe)], check=True)
