#include "game/mission/GameRecord.h"

#include <cstdint>

namespace AbyssEngine {
String::String() : data(nullptr), length(0) {}
String::~String() {}
}

template <typename T>
T *marker(unsigned index) {
    return reinterpret_cast<T *>(static_cast<uintptr_t>(0x00007ff000000000ULL + index * 0x1000ULL));
}

#define SET_PTR(Type, Name, Index) record.set##Name(marker<Type>(Index))
#define CHECK_PTR(Type, Name, Index) if (record.get##Name() != marker<Type>(Index)) return Index

int main() {
    GameRecord record;
    if (record.getShip() != nullptr || record.getCampaignMission() != nullptr ||
        record.getArray184() != nullptr || record.getWanted() != nullptr) return 1;

    record.field_0x64 = 0x11223344;
    record.field_0x88 = 0x55667788;
    record.field_0x15c = 0xaabbccdd;
    record.shipId = 0x12345678;

    SET_PTR(Mission, FreelanceMission, 2);
    SET_PTR(Mission, CampaignMission, 3);
    SET_PTR(Array<Station *>, StationStack, 4);
    SET_PTR(int, Medals, 5);
    SET_PTR(Array<bool>, Flags68, 6);
    SET_PTR(Array<bool>, Flags6c, 7);
    SET_PTR(Array<int>, Array70, 8);
    SET_PTR(Array<bool>, Flags84, 9);
    SET_PTR(Array<bool>, Flags8c, 10);
    SET_PTR(Ship, Ship, 11);
    SET_PTR(Ship, PreviousShip, 12);
    SET_PTR(Station, Station, 13);
    SET_PTR(Standing, Standing, 14);
    SET_PTR(Array<BluePrint *>, BluePrints, 15);
    SET_PTR(Array<PendingProduct *>, PendingProducts, 16);
    SET_PTR(Array<Agent *>, Agents, 17);
    SET_PTR(Array<AbyssEngine::String *>, Wingmen, 18);
    SET_PTR(int, WingmenStats, 19);
    SET_PTR(Array<bool>, Array160, 20);
    SET_PTR(Array<int>, Array164, 21);
    SET_PTR(Array<int>, Array168, 22);
    SET_PTR(Array<int>, Array16c, 23);
    SET_PTR(Array<int>, Array170, 24);
    SET_PTR(Array<bool>, Array174, 25);
    SET_PTR(Array<bool>, Array178, 26);
    SET_PTR(Array<bool>, Array17c, 27);
    SET_PTR(Array<Item *>, Array180, 28);
    SET_PTR(Array<Ship *>, Array184, 29);
    SET_PTR(Array<Wanted *>, Wanted, 30);

    CHECK_PTR(Mission, FreelanceMission, 2);
    CHECK_PTR(Mission, CampaignMission, 3);
    CHECK_PTR(Array<Station *>, StationStack, 4);
    CHECK_PTR(int, Medals, 5);
    CHECK_PTR(Array<bool>, Flags68, 6);
    CHECK_PTR(Array<bool>, Flags6c, 7);
    CHECK_PTR(Array<int>, Array70, 8);
    CHECK_PTR(Array<bool>, Flags84, 9);
    CHECK_PTR(Array<bool>, Flags8c, 10);
    CHECK_PTR(Ship, Ship, 11);
    CHECK_PTR(Ship, PreviousShip, 12);
    CHECK_PTR(Station, Station, 13);
    CHECK_PTR(Standing, Standing, 14);
    CHECK_PTR(Array<BluePrint *>, BluePrints, 15);
    CHECK_PTR(Array<PendingProduct *>, PendingProducts, 16);
    CHECK_PTR(Array<Agent *>, Agents, 17);
    CHECK_PTR(Array<AbyssEngine::String *>, Wingmen, 18);
    CHECK_PTR(int, WingmenStats, 19);
    CHECK_PTR(Array<bool>, Array160, 20);
    CHECK_PTR(Array<int>, Array164, 21);
    CHECK_PTR(Array<int>, Array168, 22);
    CHECK_PTR(Array<int>, Array16c, 23);
    CHECK_PTR(Array<int>, Array170, 24);
    CHECK_PTR(Array<bool>, Array174, 25);
    CHECK_PTR(Array<bool>, Array178, 26);
    CHECK_PTR(Array<bool>, Array17c, 27);
    CHECK_PTR(Array<Item *>, Array180, 28);
    CHECK_PTR(Array<Ship *>, Array184, 29);
    CHECK_PTR(Array<Wanted *>, Wanted, 30);

    if (record.field_0x64 != 0x11223344 || record.field_0x88 != 0x55667788 ||
        record.field_0x15c != 0xaabbccdd || record.shipId != 0x12345678) return 31;
    delete[] static_cast<uint8_t *>(record.data);
    return 0;
}
