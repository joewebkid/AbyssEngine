"""Run the production killLiberator body against its ARM-confirmed contract."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/game/ship/PlayerEgo.cpp").read_text()
    fixture = (ROOT / "tools/player_ego_kill_liberator_smoke.cpp").read_text()
    assert fixture.count("// PRODUCTION_METHOD") == 1
    fixture = fixture.replace("// PRODUCTION_METHOD",
                              function(source, "int PlayerEgo::killLiberator("))
    with tempfile.TemporaryDirectory(prefix="gof2-kill-liberator-") as temp:
        generated = Path(temp) / "kill_liberator.cpp"
        generated.write_text(fixture)
        for opt in ("-O0", "-O2"):
            exe = Path(temp) / ("kill_liberator_" + opt[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", opt, str(generated), "-o", str(exe)],
                           check=True)
            subprocess.run([str(exe)], check=True)
    print("PlayerEgo::killLiberator source-extracted O0/O2 smoke: OK")


if __name__ == "__main__":
    main()
