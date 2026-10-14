"""Run the production turret construction body against representative equipment."""

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
    begin = source.index("void PlayerEgo::checkForTurret() {")
    end = source.index("void PlayerEgo::pitchAllPrimaryGuns", begin)
    single_begin = source.index("void PlayerEgo::addGun(Gun *gun, int x) {")
    single_end = source.index("unsigned char PlayerEgo::aboutToReachAutoTarget", single_begin)
    array_begin = source.index("void PlayerEgo::addGun(Array<Gun *> *arr, int x) {")
    array_end = source.index("void PlayerEgo::render", array_begin)
    dock_begin = source.index("void PlayerEgo::dockToStream(bool param) {")
    dock_end = source.index("void PlayerEgo::hackingShuffle", dock_begin)
    fixture = (ROOT / "tools/player_ego_check_turret_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", source[begin:end])
    fixture = fixture.replace("// PRODUCTION_CALLERS", source[single_begin:single_end] + source[array_begin:array_end])
    fixture = fixture.replace("// PRODUCTION_DOCK", source[dock_begin:dock_end])
    with tempfile.TemporaryDirectory(prefix="check-turret-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "check_turret.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "check_turret.exe"
        subprocess.run([compiler, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
