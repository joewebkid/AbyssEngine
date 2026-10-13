"""Exercise the production Transform clone constructors and destructor."""

import argparse
from pathlib import Path
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

    source = (ROOT / "src/engine/math/Transform.cpp").read_text()
    fixture = (ROOT / "tools/transform_clone_smoke.cpp").read_text()
    bodies = ("Transform::Transform(Transform *other)", "Transform::Transform()",
              "Transform::~Transform()")
    fixture = fixture.replace("// PRODUCTION_BODIES", "\n".join(function(source, name) for name in bodies))
    with tempfile.TemporaryDirectory(prefix="transform-clone-smoke-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "transform_clone_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "transform_clone_smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", "-Isrc", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
