"""Run the production PlayerEgo axis-ramp helpers against input modes."""

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
    begin = source.index("static inline __attribute__((always_inline)) float PE_pitchRampDelta(")
    end = source.index("void Mat_assign(", begin)
    fixture = (ROOT / "tools/player_ego_axis_ramp_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_HELPERS", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="axis-ramp-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "axis_ramp.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "axis_ramp.exe"
        subprocess.run([compiler, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
