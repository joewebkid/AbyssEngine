"""Exercise opening and equipment-replacement campaign steps from production source."""

import argparse
from pathlib import Path
import subprocess


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--cxx", required=True)
    args = parser.parse_args()
    out = args.out_dir.resolve()
    out.mkdir(parents=True, exist_ok=True)

    source = (ROOT / "src/game/mission/Status.cpp").read_text()
    start = source.index("static int g_ncmAchTable[3]")
    end = source.index("void Status::setStation(", start)
    (out / "status_campaign_opening_replacement_under_test.inc").write_text(
        source[start:end]
    )
    for opt in ("-O0", "-O2"):
        exe = out / f"status_campaign_opening_replacement_{opt[1:]}.exe"
        subprocess.run([
            args.cxx, "-std=c++17", opt, "-I", str(out),
            "tools/status_campaign_opening_replacement_smoke.cpp", "-o", str(exe),
        ], cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
