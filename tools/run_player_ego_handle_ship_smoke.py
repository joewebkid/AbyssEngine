"""Run the production PlayerEgo handleShip body against ship fixtures."""

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
    begin = source.index("void PlayerEgo::handleShip(int dt)")
    end = source.index("void PlayerEgo::stopBoost()", begin)
    fixture = (ROOT / "tools/player_ego_handle_ship_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="handle-ship-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "handle_ship.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "handle_ship.exe"
        subprocess.run([compiler, "-std=gnu++17", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
