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
    start = source.index("    {\n        Ship *ship = PE_status()->getShip();",
                         source.index("void PlayerEgo::update("))
    end = source.index("    PE_upd_boost(this, dt);", start)
    cargo = source[start:end]
    fixture = fixture.replace("// PRODUCTION_UPDATE_CARGO",
                              "void PlayerEgo::updateCargoState() {\n" + cargo + "}\n")
    start = source.index("    Player *player = (Player *) this->player;",
                         source.index("void PlayerEgo::update("))
    end = source.index("    PE_upd_subsystems(this, dt, radar, hud, radio, script);", start)
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
    end = source.index("    if (!Globals::mouseCursorActivated || arg6", start)
    hit_state = source[start:end]
    start = end
    end = source.index("    // shortcut: cursor-driven steering", start)
    fixture = fixture.replace("// PRODUCTION_UPDATE_CROSSHAIR",
                              "void PlayerEgo::updateCrosshair(bool arg6, int arg7) {\n" +
                              "    Player *player = this->player;\n" + source[start:end] + "}\n")
    start = end
    end = source.index("    PE_upd_post(this, dt, radar, hud, radio, arg5);", start)
    fixture = fixture.replace("// PRODUCTION_UPDATE_HIT_STATE",
                              "void PlayerEgo::updateHitState(Hud *hud) {\n" +
                              "    Player *player = this->player;\n" + hit_state +
                              source[start:end] + "}\n")
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
