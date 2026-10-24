"""Run Pan's seven non-alternate large-outpost turret slots."""

import run_station_binary_asset_gate as runner
import run_level_stage1_large_outpost_ship_pan_gate as prior
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    prior.prepare(out, cxx, root, reuse)

    mission = extract("src/game/world/Level.cpp", ["void Level::createMission() {"])
    start = mission.index("    if (largeTurrets) {")
    end = mission.index("    if (smallOutpost) {", start)
    turret_loop = mission[start:end]
    level_path = out / "level_init_under_test.inc"
    level = level_path.read_text()
    level = replace_once(level, "    throw FirstFriendCompleteBoundary{};",
                         turret_loop + "    throw FirstFriendCompleteBoundary{};")
    level_path.write_text(level)

    host_path = out / "sun_host.inc"
    host = host_path.read_text()
    assert host.count("id == 27253") == 2
    host_path.write_text(host.replace("id == 27253", "id == 27253 || id == 24076"))

    static_object = extract("src/game/world/Level.cpp", [
        "intptr_t Level::createStaticObject(Waypoint *wp, int type, bool jitter) {"])
    prelude = static_object[:static_object.index("    KIPlayer *object = nullptr;")]
    branch_start = static_object.index("            auto *body = new Player(1000, 100, false, false, false);")
    branch_end = static_object.index("        } else if (type == 14243) {", branch_start)
    branch = static_object[branch_start:branch_end]
    # Only this original branch is reachable from the bounded Pan path.
    static_object = (prelude + "    assert(type == 6772);\n"
                     "    KIPlayer *object = nullptr;\n"
                     "    AEGeometry *geometry = new AEGeometry(Globals::Canvas);\n"
                     + branch + "    object->setLevel(this);\n"
                     "    return reinterpret_cast<intptr_t>(object);\n}\n")
    turret_source = (runner.ROOT / "src/game/ship/PlayerTurret.cpp").read_text()
    ctor_start = turret_source.index("PlayerTurret::PlayerTurret(int mesh, Player *player, AEGeometry *geometry, float x, float y, float z)")
    ctor_end = turret_source.index("\n}", turret_source.index("    this->particleSystemId = -1;", ctor_start)) + 2
    turret = turret_source[ctor_start:ctor_end] + "\n" + extract("src/game/ship/PlayerTurret.cpp", [
        "void PlayerTurret::setPosition(const Vector &position) {",
        "void PlayerTurret::setLevel(Level *level) {",
        "void PlayerTurret::setScaling(float scale) {"])
    turret += """
// Unreached virtual methods fail closed in this construction-only gate.
PlayerTurret::~PlayerTurret() { std::abort(); }
void PlayerTurret::render() { std::abort(); }
int PlayerTurret::collide(float, float, float) { std::abort(); }
int PlayerTurret::outerCollide(float, float, float) { std::abort(); }
void PlayerTurret::revive() { std::abort(); }
void PlayerTurret::update(int) { std::abort(); }
"""
    chain_path = out / "station_binary_chain.inc"
    chain = chain_path.read_text()
    chain = replace_once(chain,
        "V3 PlayerFixedObject::getPosition() { std::abort(); }",
        extract("src/game/ship/PlayerFixedObject.cpp", ["V3 PlayerFixedObject::getPosition() {"]))
    chain_path.write_text(chain + "\n" + static_object + "\n" + turret)

    gate_path = out / "station_binary_gate_under_test.inc"
    gate = gate_path.read_text()
    gate = replace_once(gate, '#include "sun_host.inc"',
                        '#include "game/ship/PlayerTurret.h"\n#include "engine/core/GameText.h"\n#include "sun_host.inc"')
    gate = replace_once(gate, "Status *Globals::status;",
        """Status *Globals::status;
GameText *Globals::gameText;
GameText::GameText() : textTable(nullptr), textCount(0) {}
AbyssEngine::String *GameText::getText(int key) {
    assert(key == 1666); // Localized text is outside this construction gate.
    static String label("Turret", false);
    return &label;
}""")
    gate = replace_once(gate, "    status->ship->hasJumpDriveFlag = 0;",
                        "    Globals::gameText = new GameText();\n    status->ship->hasJumpDriveFlag = 0;")
    gate = replace_once(gate,
        "freighterMoveAfterLow == expectedMove.seedLow\n"
        "           && freighterMoveAfterHigh == expectedMove.seedHigh\n"
        "           && static_cast<AERandom *>(Globals::rnd)->seedLow == expectedMove.seedLow\n"
        "           && static_cast<AERandom *>(Globals::rnd)->seedHigh == expectedMove.seedHigh",
        "freighterMoveAfterLow == expectedMove.seedLow\n"
        "           && freighterMoveAfterHigh == expectedMove.seedHigh")
    gate = replace_once(gate,
        "    for (unsigned i = level->friendCount + level->field_164 + 1;\n"
        "         i < level->enemies->size(); ++i)\n"
        "        assert((*level->enemies)[i] == nullptr);",
        """    const int turretStart = freighterIndex + level->field_168;
    assert(turretStart == level->friendCount + level->field_164 + level->field_168);
    static const float turretRecords[7][6] = {
        {5321.740234375f, 1893.0999755859375f, -15570.7001953125f, 0.0f, 0.0f, -1.5707963705062866f},
        {2355.669921875f, 1115.47998046875f, 7324.10009765625f, 0.0f, 0.0f, -1.5707963705062866f},
        {0.0f, -4261.27978515625f, 4876.27978515625f, 0.0f, 0.0f, 3.1415927410125732f},
        {-5321.740234375f, 1893.0999755859375f, -15570.7001953125f, 0.0f, 0.0f, 1.5707963705062866f},
        {0.0f, 5668.83984375f, -13871.7001953125f, 0.0f, 0.0f, 0.0f},
        {-2355.669921875f, 1115.47998046875f, 7324.10009765625f, 0.0f, 0.0f, 1.5707963705062866f},
        {0.00020068499725311995f, 2855.300048828125f, 4161.14990234375f, 0.0f, 0.0f, 0.0f}
    };
    for (int i = 0; i < 7; ++i) {
        auto *turret = dynamic_cast<PlayerTurret *>((*level->enemies)[turretStart + i]);
        assert(turret && turret->type == 6772 && turret->player
               && turret->player->kiPlayer == turret
               && turret->player->maxHitpoints == 1000
               && turret->level == level && turret->shipGroup == status->system->getRace()
               && turret->cargo == nullptr && turret->geometry
               && turret->baseGeometry && turret->baseGeometry->mesh == 6772
               && turret->turretGeometry && turret->turretGeometry->mesh == 6773
               && turret->helperGeometry);
        assert(turret->helperGeometry->scalingX == 6.0f
               && turret->helperGeometry->scalingY == 6.0f
               && turret->helperGeometry->scalingZ == 6.0f
               && turret->geometry->rotation.x == turretRecords[i][3]
               && turret->geometry->rotation.y == turretRecords[i][4]
               && turret->geometry->rotation.z == turretRecords[i][5]);
        auto turretPosition = turret->geometry->getPosition();
        assert(turret->posX == float(freighter->intPosX) + turretRecords[i][0]
               && turret->posY == float(freighter->intPosY) + turretRecords[i][1]
               && turret->posZ == float(freighter->intPosZ) + turretRecords[i][2]
               && turretPosition.x == turret->posX
               && turretPosition.y == turret->posY
               && turretPosition.z == turret->posZ);
        for (int j = 0; j < i; ++j)
            assert(turret != (*level->enemies)[turretStart + j]);
    }
    std::printf("Pan seven large-outpost turrets: OK\\n");""")
    gate_path.write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
