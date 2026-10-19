struct LoopTexture { unsigned id, side; float scale; const char *path; };
static const LoopTexture loopTextures[] = {
    {10014,64,0,"data/assets/main/3d/textures/low/etc/planets/planet_003_small.aei"},
    {10021,64,0,"data/assets/main/3d/textures/low/etc/planets/planet_010_small.aei"},
    {10056,1024,0,"data/assets/main/3d/textures/low/etc/planets/planet_014_big.aei"},
    {10024,64,0,"data/assets/main/3d/textures/low/etc/planets/planet_013_small.aei"},
    {10028,64,0,"data/assets/main/3d/textures/low/etc/planets/planet_017_small.aei"},
};
static unsigned loopUploads, loopPixelBytes, stationDestructors;

int AbyssEngine::loopTextureUpload(Engine *engine, const char *path, ImageCallback callback,
                                  void *user, unsigned *out, float scale,
                                  AELoadedTexture *record, bool managed) {
    assert(loopUploads < 5 && sunUploads == 1 && managed && !callback && !user && !record && *out == 0);
    const auto &expected = loopTextures[loopUploads];
    assert(std::strcmp(path, expected.path) == 0 && scale == expected.scale);
    auto *resource = static_cast<Resource *>(Globals::Canvas->FindResource(expected.id));
    assert(resource && resource->handle == -1);
    assert(engine->boundTextures[0] == -1 && engine->boundTextures[1] == -1);
    auto &textures = engine->appManager->paintCanvas->cubeTextures;
    assert(textures.size() == loopUploads + 2);
    int result = FlareUpload::TextureCreateFromFileIntern(engine, path, callback, user, out, scale, record, managed);
    assert(result == 1 && *out == loopUploads + 2 && textures.size() == loopUploads + 3);
    auto *texture = textures[*out];
    unsigned bytes = expected.side * expected.side * 4;
    assert(texture && texture->valid && !texture->isCube && texture->scale == scale);
    assert(texture->glId != 0xffffffffU && glIsTexture(texture->glId) && texture->byteSize == bytes);
    assert(input.size() == bytes + 25 && cursor == bytes + 23 && input[8] == 3);
    assert((unsigned(input[9]) | unsigned(input[10]) << 8) == expected.side);
    assert((unsigned(input[11]) | unsigned(input[12]) << 8) == expected.side);
    assert(input[13] == 1 && input[14] == 0 && opened == closed && glGetError() == GL_NO_ERROR);
    checkGpuPixels(texture->glId, expected.side, input.data() + 23);
    checkStationMipPixels(texture->glId, input.data() + 23, expected.side);
    ++loopUploads; loopPixelBytes += bytes;
    assert(engine->textureByteCounter == int(4194304 + 43704 + loopPixelBytes));
    assert(Engine::ImageCount == int(loopUploads + 2));
    std::printf("Orbit texture %u: real upload, managed index %u, %u base bytes\n", expected.id, *out, bytes);
    return result;
}

static void stationDestroySeen(Station *station) {
    assert(loopUploads == 5 && stationDestructors < 5);
    assert(station == loadedStationRecords->data()[stationDestructors]);
    assert(station->index == int(stationDestructors + 5));
    assert(!station->ships && !station->items && !station->agents);
    ++stationDestructors;
}

static void stationLoopBoundary(StarSystem *star) {
    assert(loopUploads == 5 && stationDestructors == 5 && loopPixelBytes == 4259840);
    assert(star->texturesArray == starTextures && star->stationIdxArray == starIndices);
    assert(starTextures->size() == 6 && (*starTextures)[0] == 1 && starIndices->size() == 5);
    for (unsigned i = 0; i < 5; ++i) {
        assert((*starTextures)[i + 1] == i + 2 && (*starIndices)[i] == int(i + 5));
        auto *res = static_cast<Resource *>(Globals::Canvas->FindResource(loopTextures[i].id));
        assert(res && res->handle == int(i + 2));
    }
    auto *targets = static_cast<Array<KIPlayer *> *>(star->playerTargets);
    assert(targets && targets->size() == 5 && targets->capacity_ == 5);
    for (auto *target : *targets) assert(!target);
    // Pointer-width storage check, without dereferencing these observation pointers.
    auto *marker = reinterpret_cast<KIPlayer *>(star);
    (*targets)[0] = marker; (*targets)[4] = marker;
    assert((*targets)[0] == marker && (*targets)[4] == marker && !(*targets)[3]);
    (*targets)[0] = nullptr; (*targets)[4] = nullptr;
    assert(star->planetsArray->size() == 6 && star->positionsArray->size() == 6);
    for (auto *planet : *star->planetsArray) assert(!planet);
    for (const auto &position : *star->positionsArray)
        assert(position.x == 0 && position.y == 0 && position.z == 0);
    assert(!star->planetRing && star->planetRingIndex == -1 && opened == closed);
}

static void stationSeedBoundary(StarSystem *star) {
    assert(star->planetsArray->size() == 6 && star->positionsArray->size() == 6);
    assert(Globals::status->getStation()->getIndex() == 7);
    assert(AERandom::gRandom->seedLow == (0xdeece66dU ^ 2100U));
    assert(AERandom::gRandom->seedHigh == 5U);
}

static void stationGeometryBoundary(StarSystem *star, AEGeometry *geom) {
    assert(geom && geom == (*star->planetsArray)[0]);
    for (unsigned i = 1; i < star->planetsArray->size(); ++i)
        assert(!(*star->planetsArray)[i]);
    assert(geom->mesh == 0x1a70 && geom->canvas == Globals::Canvas);
    assert(AERandom::gRandom->seedLow == (0xdeece66dU ^ 2100U));
    assert(AERandom::gRandom->seedHigh == 5U);
}

static void stationSunSetupBoundary(StarSystem *star, int sunSlot) {
    assert(!star->supernovaSystem && Globals::status->getCurrentCampaignMission() == 50);
    assert(Globals::status->getStation()->getTextureIndex() == 0);
    assert(sunSlot == 5); // ARM nextInt(14) returns 0 for seed 2100; no override for texture 0.
    assert(AERandom::gRandom->seedLow == 0xb11171f0U);
    assert(AERandom::gRandom->seedHigh == 0xe9a2U);
    auto *sun = (*star->planetsArray)[0];
    auto *streak = star->sunStreak;
    assert(sun && streak && sun != streak && sun->mesh == 0x1a70 && streak->mesh == 0x1a70);
    assert(sun->getScaling().x == 0.2288818359375f);
    assert(sun->getScaling().y == 0.2288818359375f);
    assert(sun->getScaling().z == 0.2288818359375f);
    assert(streak->getScaling().x == 0.457763671875f);
    assert(streak->getScaling().y == 0.0457763671875f);
    assert(streak->getScaling().z == 0.2288818359375f);
}

static void stationSunPoseBoundary(StarSystem *star) {
    auto *sun = (*star->planetsArray)[0];
    assert(sun && star->sunStreak && star->positionsArray->size() == 6);
    assert(AERandom::gRandom->seedLow == 0x3a0b233bU);
    assert(AERandom::gRandom->seedHigh == 0x3beeU);
    assert(sun->rotation.x == -0.0038067810237407684f);
    assert(sun->rotation.y == 0.047672074288129807f);
    assert(sun->rotation.z == 0.0f);
    Vector pos = sun->getPosition();
    assert(pos.x < -900.0f && pos.x > -1000.0f);
    assert(pos.y < -50.0f && pos.y > -100.0f);
    assert(pos.z < -19900.0f && pos.z > -20000.0f);
    Vector stored = (*star->positionsArray)[0];
    assert(stored.x == pos.x && stored.y == pos.y && stored.z == pos.z);
    Vector dir = sun->getDirection();
    assert(star->lightDirection.x == -dir.x && star->lightDirection.y == -dir.y &&
           star->lightDirection.z == -dir.z);
    std::puts("First sun rotation, -20000 forward placement and light direction reached; Stage 0 incomplete");
    throw FlareImageBoundary{};
}
