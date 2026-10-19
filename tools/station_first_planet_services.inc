static void stationFirstPlanetBoundary(StarSystem *star) {
    auto *planet = (*star->planetsArray)[1];
    auto *targets = static_cast<Array<KIPlayer *> *>(star->playerTargets);
    auto *player = static_cast<PlayerStatic *>((*targets)[0]);
    assert(planet && player && player->geometry == planet);
    assert((*star->stationIdxArray)[0] == 5 && Globals::status->getStation()->getIndex() == 7);
    // Pan seed 2100: after sun draws, slot 15, size jitter 31, pitch jitter -1744.
    assert(AERandom::gRandom->seedLow == 0xd4756df4U);
    assert(AERandom::gRandom->seedHigh == 0x1301U);
    assert(planet->getScaling().x == 0.0389556884765625f);
    assert(planet->getScaling().y == 0.0389556884765625f);
    assert(planet->getScaling().z == 0.0389556884765625f);
    assert(planet->rotation.x == -0.0060908496379852295f);
    assert(planet->rotation.y == 0.2574571371078491f);
    assert(planet->rotation.z == 0.0f);
    Vector pos = planet->getPosition();
    Vector saved = (*star->positionsArray)[1];
    assert(saved.x == pos.x && saved.y == pos.y && saved.z == pos.z);
    assert(pos.z < -19000.0f && pos.z > -20000.0f);
    std::puts("First non-selected planet slot, size, pose and position reached; Stage 0 incomplete");
    throw FlareImageBoundary{};
}
