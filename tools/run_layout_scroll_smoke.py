"""Exercise the production Layout scrollbar with fixture drawing services."""

import argparse
import os
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

    source = (ROOT / "src/game/ui/Layout.cpp").read_text()
    start = source.index("void Layout::drawScrollBar(int x, int y, int trackH, int pos, int range) {")
    end = source.index("\nuint8_t Layout::drawFade()", start)
    fixture = (ROOT / "tools/layout_scroll_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_SCROLLBAR", source[start:end])

    with tempfile.TemporaryDirectory(prefix="layout-scroll-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "layout_scroll_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "layout_scroll_smoke.exe"
        env = os.environ.copy()
        env.update({"TEMP": temp, "TMP": temp, "TMPDIR": temp})
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, env=env, check=True)
        subprocess.run([str(exe)], cwd=ROOT, env=env, check=True)


if __name__ == "__main__":
    main()
