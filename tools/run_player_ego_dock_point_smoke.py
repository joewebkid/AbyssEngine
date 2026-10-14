"""Run the production PlayerEgo::dockToDockingPoint body against fixtures."""

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
    begin = source.index("void PlayerEgo::dockToDockingPoint(")
    end = source.index("void PlayerEgo::draw(", begin)
    fixture = (ROOT / "tools/player_ego_dock_point_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="dock-point-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "dock_point.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "dock_point.exe"
        subprocess.run([compiler, "-std=gnu++17", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
