"""Exercise the production StatusWindow touch-end method with fixture services."""

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

    source = (ROOT / "src/game/ui/StatusWindow.cpp").read_text()
    move_start = source.index("int StatusWindow::OnTouchMove(int x, int y) {")
    move_end = source.index("\nfloat StatusWindow::getRelativeScrollStartPos(", move_start)
    start = source.index("int StatusWindow::OnTouchEnd(int x, int y) {")
    end = source.index("\nint StatusWindow::OnTouchBegin(", start)
    method = source[start:end]
    begin_start = source.index("int StatusWindow::OnTouchBegin(int x, int y) {")
    begin_end = source.index("\nfloat StatusWindow::getRelativeScrollHeight(", begin_start)
    fixture = (ROOT / "tools/status_touch_end_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_TOUCH_MOVE", source[move_start:move_end])
    fixture = fixture.replace("// PRODUCTION_TOUCH_END", method)
    fixture = fixture.replace("// PRODUCTION_TOUCH_BEGIN", source[begin_start:begin_end])

    with tempfile.TemporaryDirectory(prefix="status-touch-end-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "status_touch_end_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "status_touch_end_smoke.exe"
        env = os.environ.copy()
        env.update({"TEMP": temp, "TMP": temp, "TMPDIR": temp})
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, env=env, check=True)
        subprocess.run([str(exe)], cwd=ROOT, env=env, check=True)


if __name__ == "__main__":
    main()
