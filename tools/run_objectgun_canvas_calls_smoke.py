"""Exercise production ObjectGun canvas, animation, and render bindings."""

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

    source = (ROOT / "src/game/weapons/ObjectGun.cpp").read_text()
    fixture = (ROOT / "tools/objectgun_canvas_calls_smoke.cpp").read_text()
    body = function(source, "void ObjectGun::replaceGun(unsigned int mesh, int /*unused*/)")
    inactive = source.split("    if (gun->delayActive == 0) {", 1)[1].split(
        "\n    {\n        Player *player", 1)[0]
    inactive = "    if (gun->delayActive == 0) {" + inactive
    inactive = inactive.replace("goto after_geometry;", "return;")
    inactive_body = (
        "void ObjectGun::applyInactive() {\n"
        "    PaintCanvas **canvas = &Globals::Canvas;\n"
        "    Gun *gun = this->gun;\n"
        f"{inactive}\n"
        "}\n"
    )
    render_scale = "                if (Globals::iPad != 0)" + source.split(
        "                if (Globals::iPad != 0)", 1)[1].split(
        "\n\n                if (this->gun->weaponType", 1)[0]
    render_scale_body = (
        "void ObjectGun::applyRenderScale() {\n"
        f"{render_scale}\n"
        "}\n"
    )
    ego_start = "                    scaleMatrix = AbyssEngine::AEMath::MatrixSetRotation(local, player->empPointsF"
    render_ego = ego_start + source.split(ego_start, 1)[1].split(
        "\n                    scaleMatrix = AbyssEngine::AEMath::MatrixSetTranslation", 1)[0]
    render_ego_body = (
        "void ObjectGun::applyEgoRotation(EgoPlayerView *playerView, Player *player, "
        "Matrix &local, Matrix &scaleMatrix) {\n"
        f"{render_ego}\n"
        "}\n"
    )
    render_final = "                PaintCanvas **canvas = &Globals::Canvas;" + source.split(
        "                PaintCanvas **canvas = &Globals::Canvas;", 1)[1].split(
        "\n            } else {\n                ++inactive;", 1)[0]
    render_final_body = (
        "void ObjectGun::renderFinal() {\n"
        f"{render_final}\n"
        "}\n"
    )
    with tempfile.TemporaryDirectory(prefix="objectgun-canvas-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "smoke.cpp"
        generated.write_text(fixture.replace(
            "// PRODUCTION_METHODS",
            "\n".join((body, inactive_body, render_scale_body, render_ego_body,
                       render_final_body))))
        for optimization in ("-O0", "-O2"):
            exe = Path(temp) / f"smoke-{optimization[2:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++17", optimization,
                            str(generated), "-o", str(exe)], cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
