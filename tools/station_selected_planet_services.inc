static void stationSecondPlanetBoundary(StarSystem *star) {
    auto *planet = (*star->planetsArray)[2];
    auto *targets = static_cast<Array<KIPlayer *> *>(star->playerTargets);
    auto *player = static_cast<PlayerStatic *>((*targets)[1]);
    assert(planet && player && player->geometry == planet);
    assert((*star->stationIdxArray)[1] == 6);
    // Pan seed 2100: next accepted slot 17, size jitter 25, pitch jitter -1132.
    assert(AERandom::gRandom->seedLow == 0xe30be6c1U);
    assert(AERandom::gRandom->seedHigh == 0x3940U);
    assert(planet->getScaling().x == 0.0368499755859375f);
    assert(planet->getScaling().y == 0.0368499755859375f);
    assert(planet->getScaling().z == 0.0368499755859375f);
    assert(planet->rotation.x == -0.003953464329242706f);
    assert(planet->rotation.y == 0.27652597427368164f);
    assert(planet->rotation.z == 0.0f);
    Vector pos = planet->getPosition();
    Vector saved = (*star->positionsArray)[2];
    assert(saved.x == pos.x && saved.y == pos.y && saved.z == pos.z);
}

static void stationSelectedPlanetBoundary(StarSystem *star) {
    auto *planet = (*star->planetsArray)[3];
    auto *targets = static_cast<Array<KIPlayer *> *>(star->playerTargets);
    auto *player = static_cast<PlayerStatic *>((*targets)[2]);
    assert(planet && player && player->geometry == planet);
    assert((*star->stationIdxArray)[2] == 7 && Globals::status->getStation()->getIndex() == 7);
    assert(!Globals::status->inPlanetRingOrbit());
    assert(Globals::status->getStation()->getTextureIndex() == 0);
    // The selected planet inherits slot 17, draws distance 23495, and draws no pitch.
    assert(AERandom::gRandom->seedLow == 0x754ea638U);
    assert(AERandom::gRandom->seedHigh == 0xcf76U);
    assert(star->planetScale == 23.4950008392334f);
    assert(planet->getScaling().x == star->planetScale);
    assert(planet->getScaling().y == star->planetScale);
    assert(planet->getScaling().z == star->planetScale);
    assert(planet->rotation.x == 0.0f);
    assert(planet->rotation.y == 0.16208505630493164f);
    assert(planet->rotation.z == 0.0f);
    Vector pos = planet->getPosition();
    Vector saved = (*star->positionsArray)[3];
    assert(saved.x == pos.x && saved.y == pos.y && saved.z == pos.z);
    assert(pos.z < -19000.0f && pos.z > -20000.0f);
    std::puts("Second orbit and selected station planet reached; Stage 0 incomplete");
    throw FlareImageBoundary{};
}
