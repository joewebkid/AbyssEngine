"""Run the production PlayerEgo::initManeuver body with fixture vectors."""

from pathlib import Path
import re
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    compiler = shutil.which("g++")
    if compiler is None:
        raise SystemExit("g++ is required")
    source = (ROOT / "src/game/ship/PlayerEgo.cpp").read_text()
    fixture = (ROOT / "tools/player_ego_maneuver_smoke.cpp").read_text()
    constants = "\n".join(re.findall(
        r"    static const float g_PE_(?:rollNudge|strafeDist) = [^;]+;",
        source,
    ))
    begin = source.index("void PlayerEgo::initManeuver(int type)")
    end = source.index("void PlayerEgo::refillGunDelay()", begin)
    fixture = fixture.replace("// PRODUCTION_METHOD", constants + "\n" + source[begin:end])
    with tempfile.TemporaryDirectory(prefix="maneuver-smoke-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "maneuver.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "maneuver.exe"
        subprocess.run([compiler, "-std=gnu++17", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
