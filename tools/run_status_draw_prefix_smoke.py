"""Exercise the production StatusWindow draw setup with fixture services."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/game/ui/StatusWindow.cpp").read_text()
    start = source.index("void StatusWindow::draw() {")
    end = source.index("    String creditStr;", start)
    body = source[start:end] + "}\n"
    fixture = (ROOT / "tools/status_draw_prefix_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_DRAW_PREFIX", body)

    with tempfile.TemporaryDirectory(prefix="status-draw-prefix-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "status_draw_prefix_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "status_draw_prefix_smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
