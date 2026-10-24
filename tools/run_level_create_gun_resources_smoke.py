"""Check the restored createGun tables against Android and execute key branches."""

import argparse
import hashlib
from pathlib import Path
import re
import struct
import subprocess
import tempfile

from run_level_init_asset_gate import extract


ROOT = Path(__file__).resolve().parents[1]
ORIGINAL_SHA256 = "6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original-so", type=Path, required=True)
    parser.add_argument("--cxx", default="C:/msys64/ucrt64/bin/g++.exe")
    args = parser.parse_args()

    original = args.original_so.read_bytes()
    assert hashlib.sha256(original).hexdigest() == ORIGINAL_SHA256
    source = (ROOT / "src/game/world/Level.cpp").read_text()
    tables = []
    for name, offset in (("mesh", 0x1FC54C), ("sound", 0x1FC8F0)):
        match = re.search(rf"static const int g_cg_{name}Table\[233\] = \{{([^}}]+)\}};", source)
        assert match, name
        values = tuple(int(x) for x in re.findall(r"-?\d+", match.group(1)))
        assert values == struct.unpack_from("<233i", original, offset), name
        tables.append(match.group(0))

    fixture = (ROOT / "tools/level_create_gun_resources_smoke.cpp").read_text()
    fixture = fixture.replace("// SOURCE_TABLES", "\n".join(tables))
    fixture = fixture.replace("// SOURCE_METHOD", extract("src/game/world/Level.cpp", [
        "Gun *Level::createGun(int idx, int owner, int kind, int hp, int dmg, int rate, int cool, int color) {"]))
    assert "// SOURCE_" not in fixture
    with tempfile.TemporaryDirectory(prefix="create-gun-", dir=ROOT / "_work") as directory:
        source_path = Path(directory) / "smoke.cpp"
        source_path.write_text(fixture)
        for opt in ("-O0", "-O2"):
            executable = Path(directory) / ("smoke" + opt[1:] + ".exe")
            subprocess.run([args.cxx, "-std=gnu++14", opt, str(source_path), "-o", str(executable)],
                           cwd=ROOT, check=True)
            subprocess.run([str(executable)], cwd=ROOT, check=True)
    print("Android createGun tables: 233/233 mesh and sound entries exact; O0/O2 branches pass")


if __name__ == "__main__":
    main()
