"""Exercise production asteroid docking, cargo, shield and repair branches."""

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

    source = (ROOT / "src/game/ship/PlayerEgo.cpp").read_text()
    fixture = (ROOT / "tools/player_ego_dock_asteroid_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHOD",
                              function(source, "void PlayerEgo::dockToAsteroid("))
    start = source.index("    if (((char &) this->dockingState) != 0) {",
                         source.index("void PlayerEgo::update("))
    end = source.index("    if (this->turretActive != 0", start)
    docking = source[start:end]
    fixture = fixture.replace("// PRODUCTION_UPDATE_DOCKING",
                              "void PlayerEgo::updateDocking(Radar *radar, Hud *hud, Radio *radio) {\n" +
                              docking + "}\n")
    start = source.index("            Status *status = PE_status();",
                         source.index("approachDockingPoint(hud, 0, radar)"))
    end = source.index("            this->dockTransferTimer += this->shakeIntensity;", start)
    end = source.index("\n", end) + 1
    fixture = fixture.replace("// PRODUCTION_UPDATE_DOCK_FINISH",
                              "void PlayerEgo::updateDockFinish(Hud *hud) {\n" +
                              "    KIPlayer *target = (KIPlayer *) this->asteroidTargetPtr;\n" +
                              source[start:end] + "}\n")
    start = source.index("            int ev = 0;",
                         source.index("int PlayerEgo::approachDockingPoint("))
    end = source.index("                this->switchToFreeLookCam = 1;", start)
    end = source.index("\n", end) + 1
    fixture = fixture.replace("// PRODUCTION_DOCK_TRANSFER_INIT",
                              "void PlayerEgo::initializeDockTransfer(Hud *hud) {\n" +
                              source[start:end] + "}\n")
    start = source.index("    KIPlayer *target = (KIPlayer *)(void *&) this->asteroidTarget;",
                         source.index("int PlayerEgo::approachDockingPoint("))
    end = source.index("    if (state == 0) {", start)
    fixture = fixture.replace("// PRODUCTION_DOCK_STATE_GATE",
                              "int PlayerEgo::dockStateGate() {\n" +
                              source[start:end] + "    return -1;\n}\n")
    fixture = fixture.replace("// PRODUCTION_APPROACH_DOCKING_POINT",
                              function(source, "static inline void adp_clearDockVector(") +
                              function(source, "int PlayerEgo::approachDockingPoint("))
    start = source.index("    {\n        Ship *ship = PE_status()->getShip();",
                         source.index("void PlayerEgo::update("))
    end = source.index("    if (this->boostingFlag != 0) {", start)
    cargo = source[start:end]
    fixture = fixture.replace("// PRODUCTION_UPDATE_CARGO",
                              "void PlayerEgo::updateCargoState() {\n" + cargo + "}\n")
    start = source.index("    int boostTime = this->boostTimer;",
                         source.index("void PlayerEgo::update("))
    end = source.index("    Player *player = (Player *) this->player;", start)
    fixture = fixture.replace("// PRODUCTION_UPDATE_BOOST_TIMER",
                              "void PlayerEgo::updateBoostTimer(int dt) {\n" +
                              source[start:end] + "}\n")
    start = source.index("    if (this->boostingFlag != 0) {",
                         source.index("void PlayerEgo::update("))
    end = source.index("    if (((char &) this->dockingState) != 0) {", start)
    fixture = fixture.replace("// PRODUCTION_UPDATE_BOOST",
                              "void PlayerEgo::updateBoost(int dt) {\n" +
                              "    Player *player = this->player;\n" + source[start:end] + "}\n")
    start = source.index("    if (this->emergencySystemTimer > 0) {",
                         source.index("void PlayerEgo::update("))
    end = source.index("    {\n        Ship *ship = PE_status()->getShip();", start)
    fixture = fixture.replace("// PRODUCTION_UPDATE_SUBSYSTEMS",
                              "void PlayerEgo::updateSubsystems(int dt, Hud *hud) {\n" +
                              source[start:end] + "}\n")
    start = source.index("    Player *player = (Player *) this->player;",
                         source.index("void PlayerEgo::update("))
    end = source.index("    if (this->emergencySystemTimer > 0) {", start)
    fixture = fixture.replace("// PRODUCTION_UPDATE_SHIELD",
                              "void PlayerEgo::updateShield(int dt, Hud *hud) {\n" +
                              source[start:end] + "}\n")
    end = source.index("    player->update(dt, !isDead() && this->field_0x1c != -1);",
                       source.index("void PlayerEgo::update("))
    start = source.rindex("    Ship *ship = PE_status()->getShip();", 0, end)
    fixture = fixture.replace("// PRODUCTION_UPDATE_RECOVERY",
                              "void PlayerEgo::updateRecovery(int dt) {\n" +
                              "    Player *player = this->player;\n" + source[start:end] + "}\n")
    start = end
    end = source.index("    Matrix *local = static_cast<Matrix *>", start)
    fixture = fixture.replace("// PRODUCTION_UPDATE_PLAYER_SOUND",
                              "void PlayerEgo::updatePlayerSound(int dt) {\n" +
                              "    Player *player = this->player;\n" + source[start:end] + "}\n")
    start = end
    end = source.index("    if (this->route != nullptr) {", start)
    fixture = fixture.replace("// PRODUCTION_UPDATE_VISUAL_STATE",
                              "void PlayerEgo::updateVisualState() {\n" +
                              source[start:end] + "}\n")
    start = end
    end = source.index("    this->hudHitDirectionFlags = 0;", start)
    fixture = fixture.replace("// PRODUCTION_UPDATE_ROUTE",
                              "void PlayerEgo::updateRoute(Hud *hud) {\n" +
                              source[start:end] + "}\n")
    start = end
    end = source.index("    Vector cameraPosition = AbyssEngine::AEMath::MatrixGetPosition", start)
    hit_state = source[start:end]
    start = end
    end = source.index("    if (this->cursorStateDirty) {", start)
    crosshair = source[start:end]
    start = end
    end = source.index("    if (isDead() && this->explosion != nullptr) {", start)
    handoff = source[start:end]
    copy_start = handoff.index("    this->crosshairPosPrevious = crosshairPos;")
    previous_copy = handoff[copy_start:handoff.index("\n", copy_start) + 1]
    fixture = fixture.replace("// PRODUCTION_UPDATE_CROSSHAIR",
                              "void PlayerEgo::updateCrosshair(int dt, bool arg6, int arg7) {\n" +
                              "    Player *player = this->player;\n" + crosshair +
                              previous_copy + "}")
    start = end
    end = source.index("\n}\n\nvoid PlayerEgo::setTurretPosition", start)
    fixture = fixture.replace("// PRODUCTION_UPDATE_TAIL",
                              "void PlayerEgo::updateTail(Vector cameraPosition, Vector cameraDirection) {\n" +
                              "    Player *player = this->player;\n" + source[start:end] + "}\n")
    fixture = fixture.replace("// PRODUCTION_UPDATE_HIT_STATE",
                              "void PlayerEgo::updateHitState(Hud *hud) {\n" +
                              "    Player *player = this->player;\n" + hit_state + handoff + "}\n")
    start = source.index("        this->shieldRecoveryAmount = (float) ship->getMaxShieldHP()",
                         source.index("PlayerEgo::PlayerEgo("))
    end = source.index(";", start) + 1
    fixture = fixture.replace("// PRODUCTION_RECOVERY_AMOUNT",
                              "void PlayerEgo::initializeRecoveryAmount(Ship *ship) {\n" +
                              source[start:end] + "\n}\n")
    start = source.index("        this->handling = ship->getHandling();",
                         source.index("PlayerEgo::PlayerEgo("))
    end = source.index("        int agility = ship->getAgility();", start)
    fixture = fixture.replace("// PRODUCTION_ENGINE_SOUND_SELECTION",
                              "void PlayerEgo::initializeEngineSound(Ship *ship) {\n" +
                              source[start:end] + "}\n")

    with tempfile.TemporaryDirectory(prefix="dock-asteroid-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "player_ego_dock_asteroid_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "player_ego_dock_asteroid_smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
