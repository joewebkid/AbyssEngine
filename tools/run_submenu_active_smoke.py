"""Compile the production IsSubMenuActive body and exercise its menu states."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/engine/input/VirtualInput.cpp").read_text()
    fixture = (ROOT / "tools/submenu_active_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_FUNCTION", function(source, "int IsSubMenuActive(int player)"))
    with tempfile.TemporaryDirectory(prefix="submenu-active-smoke-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "submenu_active_smoke.cpp"
        generated.write_text(fixture)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / f"submenu_active_{level[1:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++14", level, str(generated), "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
