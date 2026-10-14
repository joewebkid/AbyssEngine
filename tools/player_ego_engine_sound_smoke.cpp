#include <cassert>

struct Vector {};
struct Item {};
struct Ship {
    Item item;
    bool gammaEquipment = false;
    Item *getFirstEquipmentOfSort(int sort) {
        assert(sort == 38);
        return gammaEquipment ? &item : nullptr;
    }
};
struct Station {
    int getIndex() { return 109; }
};
struct Status {
    Ship ship;
    Station station;
    bool alienOrbit = false;
    int gammaBits = 0;
    int mission = 106;
    Ship *getShip() { return &ship; }
    Station *getStation() { return &station; }
    bool inAlienOrbit() { return alienOrbit; }
    int getCurrentCampaignMission() { return mission; }
    int getGammaRayDamagePerSecond(int stationIndex, int campaignMission) {
        assert(stationIndex == 109 && campaignMission == mission);
        return gammaBits;
    }
};
struct FModSound {
    int playCount = 0;
    int lastId = -1;
    void play(int id, Vector *position, Vector *velocity, float gain) {
        assert(position == nullptr && velocity == nullptr && gain == 0.0f);
        lastId = id;
        ++playCount;
    }
};
namespace Globals {
    Status *status;
    FModSound *sound;
}
struct Player {
    int starts = 0;
    int stops = 0;
    int lastResource = -1;
    void PlayEngineSound(int resource, Vector *position) {
        assert(position == nullptr);
        lastResource = resource;
        ++starts;
    }
    void StopEngineSound() { ++stops; }
};
struct PlayerEgo {
    Player *player = nullptr;
    bool dockedFlag = false;
    int dockingPointIndex = 0;
    int engineSoundId = -1;
    int field_0x1c = -1;
    void PlayEngineSound();
    void StopEngineSound();
};

// PRODUCTION_PLAY_ENGINE_SOUND
// PRODUCTION_STOP_ENGINE_SOUND

int main() {
    Status status;
    FModSound sound;
    Player player;
    PlayerEgo ego;
    Globals::status = &status;
    Globals::sound = &sound;
    ego.player = &player;
    ego.engineSoundId = 2260;
    ego.field_0x1c = 1104;

    ego.PlayEngineSound();
    assert(sound.playCount == 0 && player.starts == 1 && player.lastResource == 1104);
    status.ship.gammaEquipment = true;
    status.alienOrbit = true;
    ego.PlayEngineSound();
    assert(sound.playCount == 0 && player.starts == 2);
    status.alienOrbit = false;
    ego.PlayEngineSound();
    assert(sound.playCount == 0 && player.starts == 3);
    status.gammaBits = 0x3f800000;
    ego.engineSoundId = -1;
    ego.PlayEngineSound();
    assert(sound.playCount == 0 && player.starts == 4);
    ego.engineSoundId = 2260;
    ego.PlayEngineSound();
    assert(sound.playCount == 1 && sound.lastId == 2260 && player.starts == 5);

    ego.dockedFlag = true;
    ego.dockingPointIndex = 1;
    ego.StopEngineSound();
    assert(sound.playCount == 1 && player.stops == 1);
    ego.dockingPointIndex = 0;
    ego.StopEngineSound();
    assert(sound.playCount == 2 && sound.lastId == 2260 && player.stops == 2);
    status.gammaBits = 0;
    ego.StopEngineSound();
    assert(sound.playCount == 2 && player.stops == 3);
}
