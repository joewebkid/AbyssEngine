"""Exercise production FogSetParameter in GL and shader modes."""

from pathlib import Path
import argparse
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/engine/render/PaintCanvas.cpp").read_text()
    body = function(source, "void PaintCanvas::FogSetParameter(")
    with tempfile.TemporaryDirectory(prefix="canvas-fog-", dir=ROOT / "_work") as temp:
        (Path(temp) / "canvas_fog_parameter_under_test.inc").write_text(body)
        for opt in ("-O0", "-O2"):
            exe = Path(temp) / f"canvas_fog_{opt[1:]}.exe"
            subprocess.run([
                args.cxx, "-std=gnu++17", opt, "-I", temp,
                "tools/canvas_fog_parameter_smoke.cpp", "-o", str(exe),
            ], cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)
    print("PaintCanvas fog parameter GL/shader -O0/-O2: OK")


if __name__ == "__main__":
    main()
