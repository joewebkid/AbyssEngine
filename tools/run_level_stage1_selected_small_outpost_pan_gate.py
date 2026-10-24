"""Select the faction-one small outpost through the original mission predicate."""

import os

import run_station_binary_asset_gate as runner
import run_level_stage1_small_outpost_pan_gate as prior
from run_gles_atlas_asset_gate import SOURCES
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    seed = int(os.environ.get("GOF2_STAGE1_CLOCK_SEED", "1792800112"))
    variants = {1792800112: (22, 9), 1792800340: (13, 41), 1792800107: (17, 39)}
    if seed not in variants:
        raise ValueError("GOF2_STAGE1_CLOCK_SEED must name a verified fixture seed")
    small_roll, first_type = variants[seed]
    prior.prepare(out, cxx, root, reuse)

    path = out / "level_init_under_test.inc"
    level = path.read_text()
    level = replace_once(level,
        "    Globals::status->system->faction = 1; // Pan fixture changes faction only at this test boundary.",
        "    Globals::status->system->faction = 1; // Test-only boundary after Pan stage zero.\n"
        "    Globals::status->currentCampaignMission = 141;")
    level = replace_once(level,
        "        field_164 = 1; // Test variant: keep the RNG draw, force one pre-dead ship.\n", "")
    level = replace_once(level,
        "        field_168 = 1; // Test variant: keep the RNG draw, force one freighter.\n", "")
    level = replace_once(level,
        "    smallOutpost = true; largeOutpost = alternateLarge = false; // Forced after faction-one selection draws.",
        "    assert(smallOutpost && !largeOutpost && !alternateLarge);")
    path.write_text(level)

    path = out / "sun_host.inc"
    host = path.read_text()
    assert host.count("id == 24077") == 2
    host = host.replace("id == 24077",
                        "id == 24077 || id == 32539 || id == 32541 || id == 32239 || id == 32241")
    host = replace_once(host,
        "id == 20052 || id == 0)));",
        "id == 20052 || id == 0 || id == 32239 || id == 32241))); // Missing DLC material is a production no-op.")
    path.write_text(host)

    path = out / "station_binary_chain.inc"
    chain = path.read_text()
    chain = replace_once(chain,
        "    assert(resource && resource->kind == 6 && resId >= 0x7dc8 && resId <= 0x7dec);",
        "    assert((resource && resource->kind == 6 && resId >= 0x7dc8 && resId <= 0x7dec)\n"
        "           || (!resource && (resId == 32239 || resId == 32241)));")
    path.write_text(chain)

    path = out / "station_binary_gate_under_test.inc"
    gate = path.read_text()
    gate = replace_once(gate, "Status *Globals::status;",
        """// The original AERandom::reset still runs; the tested seeds yield
// one dead ship, one class-one slot, no ninjas, and a small outpost.
extern "C" time_t time(time_t *storage) {
    constexpr time_t sample = 1792800112;
    if (storage) *storage = sample;
    return sample;
}
Status *Globals::status;""")
    gate = replace_once(gate, "constexpr time_t sample = 1792800112;",
                        f"constexpr time_t sample = {seed};")
    gate = replace_once(gate,
        "        f164 = 1; // Match the test-only one-dead-ship variant.\n", "")
    gate = replace_once(gate,
        "        f168 = 1; // Match the test-only one-freighter variant.\n", "")
    gate = replace_once(gate,
        "        int friendX = expected.nextInt(20000) - 10000;",
        "        assert(expected.nextInt(100) >= 19); // No ninjas at campaign 141.\n"
        "        int friendX = expected.nextInt(20000) - 10000;")
    gate = replace_once(gate,
        "            (void) expected.nextInt(100); // Campaign 50 cannot select small.",
        f"            assert(expected.nextInt(100) == {small_roll} && status->getCurrentCampaignMission() > 140);")
    gate = replace_once(gate,
        "        int smallTurrets = 5; // Match the forced faction-one small outpost.",
        "        int smallTurrets = 5; // The asserted original selection predicate succeeded.")
    gate = replace_once(gate,
        "        unsigned firstType = 9; // Faction one selects this fighter without a draw.",
        "        int fighterRoll = expected.nextInt(100);\n"
        "        unsigned firstType = fighterRoll < 60 ? 9 : fighterRoll < 85 ? 41 : 39;\n"
        f"        assert(firstType == {first_type});")
    gate = replace_once(gate,
        "    bool countsMatchReset = false;",
        """    const int levelCap = status->getLevel() > 20 ? 20 : status->getLevel();
    const int healthBase = (status->gameWon() ? 180 : 4 * 141) + 14 * levelCap + 20;
    const int expectedFriendHp = int(float(healthBase) + (difficulty - 0.5f) * float(healthBase));
    bool countsMatchReset = false;""")
    gate = replace_once(gate, "firstFriendPlayer->hitpoints == 405",
                        "firstFriendPlayer->hitpoints == expectedFriendHp")
    gate = replace_once(gate, "firstFriendPlayer->maxHitpoints == 405",
                        "firstFriendPlayer->maxHitpoints == expectedFriendHp")
    assert gate.count("second->type < 37") == 2
    gate = gate.replace("second->type < 37",
        "(second->type < 37 || second->type == 39 || second->type == 41)")
    gate = replace_once(gate, "ship->type < 37",
        "(ship->type < 37 || ship->type == 39 || ship->type == 41)")
    path.write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
