"""Exercise the production PlayerEgo::stopMining body with small service doubles."""

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

    source = (ROOT / "src/game/ship/PlayerEgo.cpp").read_text()
    status = (ROOT / "src/game/mission/Status.cpp").read_text()
    fixture = (ROOT / "tools/player_ego_stop_mining_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD",
                              function(status, "bool Status::hardCoreMode()") +
                              function(source, "void PlayerEgo::stopMining()"))

    with tempfile.TemporaryDirectory(prefix="stop-mining-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "player_ego_stop_mining_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "player_ego_stop_mining_smoke.exe"
        build = subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                               cwd=ROOT, capture_output=True, text=True)
        if build.returncode:
            print(build.stdout[-4000:])
            print(build.stderr[-4000:])
            build.check_returncode()
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
