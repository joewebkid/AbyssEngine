"""Run the production PlayerEgo turret-view body against camera fixtures."""

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
    begin = source.index("void PlayerEgo::handleTurretView(int dt)")
    end = source.index("void PlayerEgo::approachAsteroid(", begin)
    fixture = (ROOT / "tools/player_ego_turret_view_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="turret-view-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "turret_view.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "turret_view.exe"
        subprocess.run([compiler, "-std=gnu++17", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
