"""Exercise the production StatusWindow medal-hint body with fixture data."""

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
    start = source.index("String StatusWindow::getMedalHintText(int medalIndex) {")
    end = source.index("\nvoid *Achievements_get();", start)
    fixture = (ROOT / "tools/status_medal_hint_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_MEDAL_HINT", source[start:end])

    with tempfile.TemporaryDirectory(prefix="status-medal-hint-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "status_medal_hint_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "status_medal_hint_smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
