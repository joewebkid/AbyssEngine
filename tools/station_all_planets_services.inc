static void stationRemainingPlanetBoundary(StarSystem *star, unsigned i) {
    assert(i == 4 || i == 5);
    auto *planet = (*star->planetsArray)[i];
    auto *targets = static_cast<Array<KIPlayer *> *>(star->playerTargets);
    auto *player = static_cast<PlayerStatic *>((*targets)[i - 1]);
    assert(planet && player && player->geometry == planet);
    assert((*star->stationIdxArray)[i - 1] == int(i + 4));
    if (i == 4) {
        // Slot 17 is occupied: retry accepts 14, then size 30 and pitch -1412.
        assert(AERandom::gRandom->seedLow == 0x074c9ebcU);
        assert(AERandom::gRandom->seedHigh == 0x27cfU);
        assert(planet->getScaling().x == 0.038604736328125f);
        assert(planet->rotation.x == -0.004931353032588959f);
        assert(planet->rotation.y == 0.24792271852493286f);
    } else {
        // Slot 14 is occupied: retry accepts 10, then size 13 and pitch 1843.
        assert(AERandom::gRandom->seedLow == 0x0b131b80U);
        assert(AERandom::gRandom->seedHigh == 0xf336U);
        assert(planet->getScaling().x == 0.0326385498046875f);
        assert(planet->rotation.x == 0.006436603143811226f);
        assert(planet->rotation.y == 0.2097850739955902f);
    }
    assert(planet->getScaling().x == planet->getScaling().y &&
           planet->getScaling().x == planet->getScaling().z);
    assert(planet->rotation.z == 0.0f);
    Vector pos = planet->getPosition();
    Vector saved = (*star->positionsArray)[i];
    assert(saved.x == pos.x && saved.y == pos.y && saved.z == pos.z);
    if (i == 5) {
        assert(star->planetScale == 23.4950008392334f);
        for (unsigned j = 0; j < 5; ++j) {
            assert((*targets)[j] && (*star->planetsArray)[j + 1]);
            Vector current = (*star->planetsArray)[j + 1]->getPosition();
            Vector stored = (*star->positionsArray)[j + 1];
            assert(stored.x == current.x && stored.y == current.y && stored.z == current.z);
        }
        std::puts("All five orbit planets, including two rejected slots, reached; Stage 0 incomplete");
        throw FlareImageBoundary{};
    }
}
