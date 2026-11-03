"""Check ownership using the production campaign setter body."""

import argparse
from pathlib import Path
import subprocess

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--cxx", required=True)
    args = parser.parse_args()
    out = args.out_dir.resolve()
    out.mkdir(parents=True, exist_ok=True)

    source = (ROOT / "src/game/mission/Status.cpp").read_text()
    (out / "status_campaign_sentinel_under_test.inc").write_text(
        function(source, "void Status::setCampaignMission(Mission *m) {")
    )
    for opt in ("-O0", "-O2"):
        exe = out / f"status_campaign_sentinel_{opt[1:]}.exe"
        subprocess.run([
            args.cxx, "-std=c++17", opt, "-I", str(out),
            "tools/status_campaign_sentinel_smoke.cpp", "-o", str(exe),
        ], cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
