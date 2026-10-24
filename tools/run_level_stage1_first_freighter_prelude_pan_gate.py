"""Reach Pan's first class-one freighter Player setup after ordinary friends."""

import run_station_binary_asset_gate as runner
import run_level_stage1_dead_friend_pan_gate as prior
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    prior.prepare(out, cxx, root, reuse)

    host_path = out / "sun_host.inc"
    host = host_path.read_text()
    host = replace_once(host, "struct FirstFriendCompleteBoundary {};",
        """struct FirstFriendCompleteBoundary {};
struct FirstFreighterPreludeBoundary {};
static uint32_t freighterSeedBeforeLow, freighterSeedBeforeHigh;
static uint32_t freighterSeedAfterLow, freighterSeedAfterHigh;
static int freighterRace, freighterClass, freighterType, freighterIndex;
static bool freighterHostile, freighterGroup;
static Waypoint *freighterWaypoint;
static Player *freighterPlayer;
static float freighterX, freighterY, freighterZ;""")
    host_path.write_text(host)

    mission = extract("src/game/world/Level.cpp", ["void Level::createMission() {"])
    start = mission.index("            bool foreign = static_cast<AERandom *>(Globals::rnd)->nextInt(100) <= 29;")
    end = mission.index("            static_cast<PlayerFixedObject *>((*enemies)[index])->setMoving(true);", start)
    freighter = mission[start:end]
    assert "createShip(shipRace, 1, shipRace == 1 ? 13 : 15, nullptr, true, false)" in freighter
    level_path = out / "level_init_under_test.inc"
    level = level_path.read_text()
    level = replace_once(level,
        "        field_168 = static_cast<AERandom *>(Globals::rnd)->nextInt(5);",
        "        field_168 = static_cast<AERandom *>(Globals::rnd)->nextInt(5);\n"
        "        field_168 = 1; // Test variant: keep the RNG draw, force one freighter.")
    level = replace_once(level,
        "    if (Globals::status->inSupernovaSystem()) {\n"
        "        friendCount = field_164 = field_168 = hostileCount = 0;",
        "    largeOutpost = smallOutpost = false; // Test variant after the original draws.\n"
        "    if (Globals::status->inSupernovaSystem()) {\n"
        "        friendCount = field_164 = field_168 = hostileCount = 0;")
    extension = '''    int foreignRace = race == 2 ? 0 : race;
    for (; index < friendCount + field_164 + field_168; ++index) {
        int start = friendCount + field_164;
        assert(index == start && !largeOutpost && !smallOutpost);
        freighterIndex = index;
        freighterSeedBeforeLow = static_cast<AERandom *>(Globals::rnd)->seedLow;
        freighterSeedBeforeHigh = static_cast<AERandom *>(Globals::rnd)->seedHigh;
''' + freighter + '''    }
'''
    level = replace_once(level, "    throw FirstFriendCompleteBoundary{};\n}",
        extension + "    throw FirstFriendCompleteBoundary{};\n}")
    level_path.write_text(level)

    chain_path = out / "station_binary_chain.inc"
    chain = chain_path.read_text()
    chain = replace_once(chain,
        "    assert(shipClass == 0); // This gate covers the Pan ordinary-fighter branch only.",
        """    if (shipClass == 1) {
        freighterRace = race; freighterClass = shipClass; freighterType = type;
        freighterWaypoint = wp; freighterHostile = hostile; freighterGroup = group;
        freighterPlayer = pl; freighterX = fx; freighterY = fy; freighterZ = fz;
        freighterSeedAfterLow = static_cast<AERandom *>(Globals::rnd)->seedLow;
        freighterSeedAfterHigh = static_cast<AERandom *>(Globals::rnd)->seedHigh;
        throw FirstFreighterPreludeBoundary{};
    }
    assert(shipClass == 0);""")
    chain_path.write_text(chain)

    gate_path = out / "station_binary_gate_under_test.inc"
    gate = gate_path.read_text()
    gate = replace_once(gate, "        int f168 = expected.nextInt(5);",
        "        int f168 = expected.nextInt(5);\n"
        "        f168 = 1; // Match the test-only one-freighter variant.")
    gate = replace_once(gate,
        "        assert(race == 0); // Pan's ordinary non-DLC faction.",
        "        largeTurrets = 0; // The test variant excludes outposts after their draws.\n"
        "        assert(race == 0); // Pan's ordinary non-DLC faction.")
    gate = replace_once(gate,
        "catch (const FirstFriendCompleteBoundary &) { reachedMissionDispatch = true; }",
        "catch (const FirstFreighterPreludeBoundary &) { reachedMissionDispatch = true; }")
    gate = replace_once(gate, "    assert(countsMatchReset);", """    assert(countsMatchReset);
    AERandom expectedFreighter(0);
    expectedFreighter.seedLow = freighterSeedBeforeLow;
    expectedFreighter.seedHigh = freighterSeedBeforeHigh;
    bool foreign = expectedFreighter.nextInt(100) <= 29;
    int expectedRace = foreign ? 2 : status->system->getRace();
    int jitterX = expectedFreighter.nextInt(40000);
    int jitterY = expectedFreighter.nextInt(40000);
    int jitterZ = expectedFreighter.nextInt(40000);
    int emp = (status->getLevel() > 20 ? 20 : status->getLevel()) * 5 + 40;
    assert(level->field_168 == 1 && freighterIndex ==
           level->friendCount + level->field_164
           && freighterRace == expectedRace && freighterClass == 1
           && freighterType == 15 && !freighterWaypoint
           && freighterHostile && !freighterGroup && freighterPlayer
           && freighterPlayer->radius == 1000
           && freighterPlayer->hitpoints == 2025
           && freighterPlayer->maxHitpoints == 2025
           && freighterPlayer->empPoints == emp * 3
           && freighterPlayer->maxEmpPoints == emp * 3
           && freighterPlayer->empData == 45000
           && freighterX == float(jitterX - 20000)
           && freighterY == float(jitterY - 20000)
           && freighterZ == float(jitterZ - 20000)
           && freighterSeedAfterLow == expectedFreighter.seedLow
           && freighterSeedAfterHigh == expectedFreighter.seedHigh);
    std::printf("Pan first class-one freighter Player prelude: OK\\n");""")
    gate_path.write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
