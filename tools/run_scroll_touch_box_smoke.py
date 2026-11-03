"""Exercise the production ScrollTouchBox constructor, text layout, and draw bodies."""

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

    source = (ROOT / "src/game/ui/ScrollTouchBox.cpp").read_text()
    fixture = (ROOT / "tools/scroll_touch_box_smoke.cpp").read_text()
    for marker, signature in (
        ("PRODUCTION_CONSTRUCTOR", "ScrollTouchBox::ScrollTouchBox(int x"),
        ("PRODUCTION_SET_TEXT", "void ScrollTouchBox::setText(AbyssEngine::String text, int font)"),
        ("PRODUCTION_DRAW", "void ScrollTouchBox::draw()"),
    ):
        fixture = fixture.replace("// " + marker, function(source, signature))
    with tempfile.TemporaryDirectory(prefix="scroll-touch-box-smoke-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "scroll_touch_box_smoke.cpp"
        generated.write_text(fixture)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / f"scroll_touch_box_{level[1:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++14", level, str(generated), "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
