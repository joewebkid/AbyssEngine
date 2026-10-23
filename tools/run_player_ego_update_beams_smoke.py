"""Exercise the production PlayerEgo::update beam-dispatch fragment."""

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
    begin = source.index("    if (this->tractorBeam != nullptr)",
                         source.index("void PlayerEgo::update("))
    end = source.index("    // Android Thumb 0x9da58-0x9db60:", begin)
    fixture = (ROOT / "tools/player_ego_update_beams_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_FRAGMENT", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="ego-beams-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "update_beams.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "update_beams.exe"
        subprocess.run([compiler, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
