"""Exercise the production PlayerEgo::explode body against small fixtures."""

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
    source = (ROOT / "src/game/ship/PlayerEgo.cpp").read_text()
    begin = source.index("void PlayerEgo::explode()")
    end = source.index("unsigned char PlayerEgo::isDockingToStream()", begin)
    end_begin = source.index("void PlayerEgo::endExplosion()")
    end_end = source.index("bool PlayerEgo::isLandingOrTakingOff()", end_begin)
    fixture = (ROOT / "tools/player_ego_explode_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", source[begin:end])
    fixture = fixture.replace("// PRODUCTION_END_METHOD", source[end_begin:end_end])
    with tempfile.TemporaryDirectory(prefix="ego-explode-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "explode.cpp"
        generated.write_text(fixture)
        executable = Path(temp) / "explode.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(executable)],
                       cwd=ROOT, check=True)
        subprocess.run([str(executable)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
