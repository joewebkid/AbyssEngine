"""Run the production PlayerEgo auto-turret body with target fixtures."""

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
    begin = source.index("void PlayerEgo::handleAutoTurret(int dt)")
    end = source.index("void PlayerEgo::dockToAsteroid(", begin)
    fixture = (ROOT / "tools/player_ego_auto_turret_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="auto-turret-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "auto_turret.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "auto_turret.exe"
        subprocess.run([compiler, "-std=gnu++17", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
