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
    set_enemies = function(source, "void ObjectGun::setEnemies(Array<Player *> *enemies)")
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
    camera_start = "            dir = AbyssEngine::AEMath::MatrixGetDir(playerMatrix);"
    camera_direction = camera_start + source.split(camera_start, 1)[1].split(
        "\n            goto after_geometry;", 1)[0]
    camera_body = (
        "void ObjectGun::applyCameraDirection(Matrix &playerMatrix, "
        "Matrix &cameraMatrix) {\n"
        "    Vector dir, up;\n"
        f"{camera_direction}\n"
        "}\n"
    )
    turret_start = "        PlayerEgo *ego = (PlayerEgo *) player;"
    turret_matrix = turret_start + source.split(turret_start, 1)[1].split(
        "\n\n        gun = this->gun;", 1)[0]
    turret_body = (
        "void ObjectGun::applyTurretMatrix(PlayerEgo *player, Matrix &workMatrix) {\n"
        f"{turret_matrix}\n"
        "}\n"
    )
    explosion_tail = "after_geometry:\n" + source.split("after_geometry:\n", 1)[1].split(
        "\n}\n\nvoid ObjectGun::render()", 1)[0]
    explosion_body = (
        "void ObjectGun::applyExplosionUpdate(int dt) {\n"
        "    Vector zero = {0, 0, 0};\n"
        f"{explosion_tail}\n"
        "}\n"
    )
    render_start = "    Gun *gun = this->gun;\n    gun->render();"
    explosion_render = render_start + source.split(render_start, 1)[1].split(
        "\n\n    if (gun->active != 0)", 1)[0]
    explosion_render_body = (
        "void ObjectGun::applyExplosionRender() {\n"
        f"{explosion_render}\n"
        "}\n"
    )
    visible_start = "                    this->orientation.m[0] = cameraLocal.m[0];"
    visible_axes = visible_start + source.split(visible_start, 1)[1].split(
        "\n\n                    int scale = gun->lifetimes[i];", 1)[0]
    visible_body = (
        "void ObjectGun::applyVisibleOrientation(Matrix &cameraLocal, "
        "Vector &muzzle) {\n"
        f"{visible_axes}\n"
        "}\n"
    )
    renderable_condition = source.split("            if (gunPos->x != ", 1)[1].split(") {", 1)[0]
    renderable_body = (
        "bool ObjectGun::isRenderable(const Vector *gunPos) {\n"
        f"    return gunPos->x != {renderable_condition};\n"
        "}\n"
    )
    turret_spin_start = "                scaleMatrix = AbyssEngine::AEMath::MatrixSetRotation(rotate, "
    turret_spin = turret_spin_start + source.split(turret_spin_start, 1)[1].split(";", 1)[0] + ";"
    turret_spin_body = (
        "void ObjectGun::applyTurretSpin(Matrix &rotate, Matrix &scaleMatrix) {\n"
        f"{turret_spin}\n"
        "}\n"
    )
    lifetime_start = "                    int scale = gun->lifetimes[i];"
    lifetime_parts = source.split(lifetime_start)
    visible_lifetime = lifetime_start + lifetime_parts[1].split(
        "\n                } else if (this->useEgoOrientation", 1)[0]
    world_lifetime = lifetime_start + lifetime_parts[2].split(
        "\n                } else {", 1)[0]
    visible_lifetime_body = (
        "void ObjectGun::applyVisibleLifetimeScaling(unsigned i) {\n"
        "    Gun *gun = this->gun;\n"
        f"{visible_lifetime}\n"
        "}\n"
    )
    world_lifetime_body = (
        "void ObjectGun::applyWorldLifetimeScaling(unsigned i) {\n"
        "    Gun *gun = this->gun;\n"
        f"{world_lifetime}\n"
        "}\n"
    )
    basis_start = "                    side = AbyssEngine::AEMath::VectorCross(this->dir, this->side);"
    basis = basis_start + source.split(basis_start, 1)[1].split(
        "\n\n                    this->orientation.m[0]", 1)[0]
    basis_body = (
        "void ObjectGun::applyBasisUp() {\n"
        "    Vector side;\n"
        f"{basis}\n"
        "}\n"
    )
    wobble_start = "                        float step = (float) this->deltaTime * "
    wobble = wobble_start + source.split(wobble_start, 1)[1].split(
        "\n                    }", 1)[0]
    wobble_body = (
        "void ObjectGun::applyWobbleStep(Vector *spin) {\n"
        f"{wobble}\n"
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
            "\n".join((set_enemies, body, inactive_body, camera_body, turret_body,
                       explosion_body, explosion_render_body, visible_body,
                       renderable_body, turret_spin_body, visible_lifetime_body,
                       world_lifetime_body, basis_body, wobble_body,
                       render_scale_body, render_ego_body, render_final_body))))
        for optimization in ("-O0", "-O2"):
            exe = Path(temp) / f"smoke-{optimization[2:]}.exe"
            subprocess.run([args.cxx, "-std=gnu++17", optimization,
                            str(generated), "-o", str(exe)], cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
