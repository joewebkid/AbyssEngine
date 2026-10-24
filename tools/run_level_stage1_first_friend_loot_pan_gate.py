"""Exercise Fighter's real loot-selection dependency with original items.bin."""

import run_station_binary_asset_gate as runner
import run_level_stage1_first_friend_explosion_pan_gate as first
from run_gles_atlas_asset_gate import ROOT, SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once
from run_particle_control_smoke import function


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    source = (ROOT / "src/game/mission/Generator.cpp").read_text()
    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + function(source, "static const volatile int g_Generator_typeChances[5] = {") + ";\n"
    chain += extract("src/game/mission/Generator.cpp", [
        "Generator::Generator() {", "Generator::~Generator() {",
        "Array<int> *Generator::getLootList(int itemIndex, int amount) {"])
    chain += extract("src/engine/file/FileRead.cpp", [
        "Array<Item *> *FileRead::loadItemsBinary() {"])
    chain += extract("src/game/mission/Item.cpp", [
        "void Item::init() {", "Item::~Item() {",
        "IntArray *Item::getIngredients() {", "int Item::getOccurence() {",
        "int Item::getSinglePrice() {", "int Item::getTecLevel() {"])
    chain += extract("src/game/ship/Ship.cpp", ["unsigned int Ship::hasJumpDrive() {"])
    (out / "station_binary_chain.inc").write_text(chain)
    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, "TRAP(_ZN4ItemD1Ev)", "")
    (out / "station_binary_traps.inc").write_text(traps)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, '#include "engine/file/FileRead.h"',
        '#include "engine/file/FileRead.h"\n#include "game/mission/Generator.h"')
    gate = replace_once(gate,
        '    std::printf("stage 1 first-friend Explosion(0) and fire streaks: OK\\n");',
        '''    std::printf("stage 1 first-friend Explosion(0) and fire streaks: OK\\n");
    int filesBeforeItems = opened;
    FileRead itemReader;
    auto *items = itemReader.loadItemsBinary();
    assert(items && items->size() == 233 && opened == filesBeforeItems + 1
           && opened == closed);
    // The inherited Pan fixture has a minimal Item constructor; initialize
    // real catalog attributes with the production Item::init body here.
    for (unsigned i = 0; i < items->size(); ++i) {
        Item *item = (*items)[i];
        assert(item && item->attributes && item->attributes->size() >= 18);
        item->init();
        assert(item->index == int(i) && item->type >= 0 && item->type < 5);
    }
    assert((*items)[0]->minPrice == 400 && (*items)[232]->minPrice == 1550000);
    Globals::items = items;
    status->ship->hasJumpDriveFlag = 0;
    AERandom *panRandom = static_cast<AERandom *>(Globals::rnd);
    Generator lootGenerator;
    unsigned emptyLoot = 0, populatedLoot = 0;
    for (int seed = 0; seed < 20; ++seed) {
        AERandom trial(seed);
        Globals::rnd = &trial;
        auto *loot = lootGenerator.getLootList(-1, -1);
        if (!loot) {
            ++emptyLoot;
            continue;
        }
        ++populatedLoot;
        assert(loot->size() == 2 || loot->size() == 4);
        for (unsigned i = 0; i < loot->size(); i += 2) {
            assert((*loot)[i] >= 0 && (*loot)[i] < 233);
            assert((*loot)[i + 1] >= 1 && (*loot)[i + 1] <= 9);
        }
        delete loot;
    }
    Globals::rnd = panRandom;
    assert(emptyLoot && populatedLoot && opened == closed);
    Globals::items = nullptr;
    for (Item *item : *items) {
        delete item->ingredients;
        delete item->quantities;
        delete item->attributes;
        delete item;
    }
    delete items;
    std::printf("stage 1 first-friend loot dependency with original items: OK\\n");''')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
