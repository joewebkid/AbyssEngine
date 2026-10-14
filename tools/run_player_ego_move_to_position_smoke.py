"""Run the recovered PlayerEgo navigation bodies against host fixtures."""

from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    compiler = shutil.which("g++")
    if compiler is None:
        raise SystemExit("g++ is required")
    source = (ROOT / "src/game/ship/PlayerEgo.cpp").read_text()
    start = source.index("float PlayerEgo::moveToPosition(")
    end = source.index("void PlayerEgo::resetGunDelay()", start)
    pilot_start = source.index("void PlayerEgo::setAutoPilot(")
    pilot_end = source.index("// IDA 0x9c1f8:", pilot_start)
    fixture = (ROOT / "tools/player_ego_move_to_position_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_MOVE", source[start:end])
    fixture = fixture.replace("// PRODUCTION_AUTOPILOT", source[pilot_start:pilot_end])
    with tempfile.TemporaryDirectory(prefix="move-to-position-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "move_to_position.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "move_to_position.exe"
        subprocess.run([compiler, "-std=gnu++17", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
