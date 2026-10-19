// Production loop extraction with observable calls; no GLES or game bootstrap.
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
static std::vector<std::string> calls;
struct Station {
    int index;
    int getIndex() { calls.push_back("index:" + std::to_string(index)); return index; }
    int getTextureIndex() { calls.push_back("textureIndex"); return 3; }
};
struct Status {
    Station current;
    int mission;
    Station *getStation() { calls.push_back("station"); return &current; }
    int getCurrentCampaignMission() { calls.push_back("mission"); return mission; }
    unsigned orbitHasPlanetRing(int index) { assert(index == 5); calls.push_back("ring"); return 0; }
};
struct Canvas {
    void TextureCreate(uint16_t id, uint32_t &out, bool flag) {
        assert(!flag && out == 0); calls.push_back("upload:" + std::to_string(id)); out = 42;
    }
};
namespace Globals { static Status *status; static Canvas *Canvas; }
struct AEGeometry { AEGeometry(uint16_t, Canvas *, bool) { assert(false); } };
static const uint32_t g_StarSystem_ctor_stationPlanetTextures[] = {0,0,0,10045};
static const uint32_t g_StarSystem_ctor_stationTextures[] = {0,0,0,10014};
struct StarSystem {
    std::vector<uint32_t> textures{0,0};
    std::vector<int> indices{0};
    std::vector<uint32_t> *texturesArray = &textures;
    std::vector<int> *stationIdxArray = &indices;
    unsigned selectedStationSlot = 99, planetRingTexture = 0;
    AEGeometry *planetRing = nullptr;
    int planetRingIndex = -1;
    void loop(int mode, int *stationIds, Station **loadedStations) {
#include "starsystem_texture_loop_under_test.inc"
    }
};
int main() {
    Canvas canvas;
    Globals::Canvas = &canvas;
    for (int current : {5,7}) for (int mode : {0,3}) for (int mission : {0,50}) {
        Status status{{current}, mission}; Globals::status = &status;
        Station station{5}; Station *loaded[] = {&station}; int ids[] = {5};
        StarSystem system;
        calls.clear(); system.loop(mode, ids, loaded);
        std::vector<std::string> expected = {"station", "index:" + std::to_string(current)};
        bool intro = current == 5 && mode == 3 && mission == 0;
        if (current == 5) expected.push_back("mission");
        if (!intro) expected.push_back("textureIndex");
        expected.push_back("upload:" + std::to_string(intro ? 10043 : current == 5 ? 10045 : 10014));
        if (current != 5) { expected.push_back("index:5"); expected.push_back("ring"); }
        expected.push_back("index:5");
        assert(calls == expected);
        assert(system.textures[1] == 42 && system.indices[0] == 5);
        assert(system.selectedStationSlot == (intro ? 1u : 99u));
    }
    std::puts("StarSystem texture loop: eight current/remote, mode and mission cases: OK");
}
