"""Exercise the production emergency/cloak tail of PlayerEgo::setShip."""

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
    begin = source.index("void PlayerEgo::setShip(int race, int group) {")
    end = source.index("void PlayerEgo::addGun(Array<Gun *> *arr, int x)", begin)
    method = source[begin:end]
    emergency_begin = method.index("    if (((Ship *) (PE_status()->getShip()))->getFirstEquipmentOfSort(0x1b)")
    emergency_end = method.index("    if (PE_status()->inSupernovaSystem()", emergency_begin)
    cloak_begin = method.index("    if (this->cloak != 0) {", emergency_end)
    cloak_end = method.rindex("\n}")
    fragment = method[emergency_begin:emergency_end] + method[cloak_begin:cloak_end]
    fixture = (ROOT / "tools/player_ego_set_ship_tail_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_TAIL", fragment)
    with tempfile.TemporaryDirectory(prefix="set-ship-tail-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "set_ship_tail.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "set_ship_tail.exe"
        subprocess.run([compiler, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
