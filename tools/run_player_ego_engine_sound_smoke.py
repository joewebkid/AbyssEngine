"""Compile and exercise the production PlayerEgo engine-sound methods."""

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
    fixture = (ROOT / "tools/player_ego_engine_sound_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_PLAY_ENGINE_SOUND",
                              function(source, "void PlayerEgo::PlayEngineSound("))
    fixture = fixture.replace("// PRODUCTION_STOP_ENGINE_SOUND",
                              function(source, "void PlayerEgo::StopEngineSound("))
    with tempfile.TemporaryDirectory(prefix="player-engine-sound-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "engine_sound.cpp"
        generated.write_text(fixture)
        executable = Path(temp) / "engine_sound.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(executable)],
                       cwd=ROOT, check=True)
        subprocess.run([str(executable)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
