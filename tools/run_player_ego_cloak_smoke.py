"""Exercise the production PlayerEgo cloak entry and activation paths."""

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
    begin = source.index("int PlayerEgo::toggleCloaking() {")
    end = source.index("// Static data members present in the original binary", begin)
    fixture = (ROOT / "tools/player_ego_cloak_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="ego-cloak-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "cloak.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "cloak.exe"
        subprocess.run([compiler, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
