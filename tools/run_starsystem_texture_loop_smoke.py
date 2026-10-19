"""Compile the actual StarSystem texture loop against call-order assertions."""
from pathlib import Path
import subprocess
from run_particle_control_smoke import function

ROOT = Path(__file__).resolve().parents[1]
out = ROOT / "_work/gles-host"
source = (ROOT / "src/game/world/StarSystem.cpp").read_text()
loop = function(source, "for (uint32_t i = 1; i < this->texturesArray->size(); ++i)")
(out / "starsystem_texture_loop_under_test.inc").write_text(loop)
for opt in ("-O0", "-O2"):
    exe = out / f"station_texture_loop_{opt[1:]}.exe"
    subprocess.run(["C:/msys64/ucrt64/bin/g++.exe", "-std=c++17", opt,
                    "-I", str(out), "tools/starsystem_texture_loop_smoke.cpp", "-o", str(exe)], cwd=ROOT, check=True)
    subprocess.run([str(exe)], cwd=ROOT, check=True)
