"""Compile production PlayerEgo boost setup and activation in isolation."""

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
    constructor = source.index("PlayerEgo::PlayerEgo(")
    defaults_start = source.index("    this->boostTimer = 0;", constructor)
    defaults_end = source.index("    this->speedValue = 2.0f;", defaults_start)
    ship_start = source.index("        this->field_0xd0 = ship->getBoostDelay();", constructor)
    ship_end = source.index("        this->handling = ship->getHandling();", ship_start)
    fixture = (ROOT / "tools/player_ego_boost_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_BOOST_DEFAULTS", source[defaults_start:defaults_end])
    fixture = fixture.replace("// PRODUCTION_BOOST_SHIP", source[ship_start:ship_end])
    fixture = fixture.replace("// PRODUCTION_BOOST_METHOD", function(source, "void PlayerEgo::boost()"))
    with tempfile.TemporaryDirectory(prefix="player-boost-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "boost.cpp"
        generated.write_text(fixture)
        executable = Path(temp) / "boost.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(executable)],
                       cwd=ROOT, check=True)
        subprocess.run([str(executable)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
