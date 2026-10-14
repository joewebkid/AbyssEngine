"""Exercise the production PlayerEgo cloak entry and update lifecycle."""

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
    percent_begin = source.index("float PlayerEgo::getCloakingPercentage() {")
    percent_end = source.index("int PlayerEgo::getBoostSpeed()", percent_begin)
    begin = source.index("int PlayerEgo::toggleCloaking() {")
    end = source.index("// Static data members present in the original binary", begin)
    update_begin = source.index("void PlayerEgo::update(")
    charge_begin = source.index("    if (this->cloaked != 0) {", update_begin)
    charge_end = source.index("    if (this->tractorBeam != nullptr)", charge_begin)
    fixture = (ROOT / "tools/player_ego_cloak_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_PERCENTAGE", source[percent_begin:percent_end])
    fixture = fixture.replace("// PRODUCTION_METHOD", source[begin:end])
    fixture = fixture.replace("// PRODUCTION_UPDATE_FRAGMENT", source[charge_begin:charge_end])
    with tempfile.TemporaryDirectory(prefix="ego-cloak-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "cloak.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "cloak.exe"
        subprocess.run([compiler, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
