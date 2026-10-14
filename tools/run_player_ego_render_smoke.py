"""Exercise the production PlayerEgo::render body across its ARM branches."""

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
    begin = source.index("void PlayerEgo::render(bool allowHud) {")
    end = source.index("int PlayerEgo::toggleCloaking()", begin)
    fixture = (ROOT / "tools/player_ego_render_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD", source[begin:end])
    with tempfile.TemporaryDirectory(prefix="ego-render-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "render.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "render.exe"
        subprocess.run([compiler, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
