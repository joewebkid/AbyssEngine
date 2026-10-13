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
    start = source.index("int StatusWindow::OnTouchEnd(int x, int y) {")
    end = source.index("\nint StatusWindow::OnTouchBegin(", start)
    method = source[start:end]
    fixture = (ROOT / "tools/status_touch_end_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_TOUCH_END", method)

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
