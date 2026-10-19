#include "lensflare_station_under_test.inc"

static void checkRegion(const Image2D &region, unsigned side, unsigned x, unsigned y) {
    assert(region.atlasW == side && region.atlasH == side);
    assert(region.offX == x && region.offY == y && region.sizeX == 64 && region.sizeY == 64);
    assert(region.restoreFlag == 0 && region.textureId == 73);
    auto *mesh = region.mesh;
    assert(mesh && mesh->vertexCount == 4 && mesh->indexCount == 6 && mesh->vertexFormat == 0x13);
    const float positions[] = {0, 0, 0, 64, 0, 0, 64, 64, 0, 0, 64, 0};
    const float uv[] = {float(x) / side, float(y) / side, float(x + 64) / side, float(y) / side,
                        float(x + 64) / side, float(y + 64) / side, float(x) / side, float(y + 64) / side};
    const uint16_t indices[] = {0, 2, 1, 0, 3, 2};
    assert(std::memcmp(mesh->positions, positions, sizeof(positions)) == 0);
    assert(std::memcmp(mesh->texCoords, uv, sizeof(uv)) == 0);
    assert(std::memcmp(mesh->indices, indices, sizeof(indices)) == 0);
}

static void checkCachedAtlas(unsigned variant) {
    Engine engine;
    engine.displayWidth = 1280;
    engine.displayHeight = 720;
    PaintCanvas canvas(&engine);
    Globals::Canvas = PaintCanvas::gCanvas = &canvas;
    Globals::retinaDisplay = variant == 1;
    Globals::iPadHD = variant == 2;
    registerResources(&canvas);
    registerFlarePlatform(&canvas);
    auto *resource = static_cast<Resource *>(canvas.FindResource(10062));
    assert(resource && resource->kind == 2 && resource->handle == -1);
    const char *paths[] = {"data/textures/gof2_interface.aei", "data/textures/gof2_interface_iphone4.aei",
                           "data/textures/gof2_interface_ipad_1440.aei"};
    auto *texture = static_cast<ResourceTexture *>(resource->payload);
    assert(std::strcmp(texture->name, paths[variant]) == 0 && texture->value == 0.0f);
    // Independently decoded original AEI header/regions, not copied from a mock.
    const unsigned xy[][6] = {{71, 934, 160, 902, 285, 601},
                              {561, 1265, 1811, 102, 1632, 855},
                              {366, 1684, 436, 1767, 1978, 191}};
    unsigned side = variant ? 2048 : 1024;
    int before = opened;
    Image *pixels = nullptr;
    assert(ImageCreateFromFile(&engine, texture->name, &pixels) == 1);
    assert(pixels && pixels->format == 3 && pixels->hasMipmaps == 0);
    assert(pixels->width == side && pixels->height == side && pixels->dataLen == side * side * 4);
    assert(pixels->data && opened == before + 1 && opened == closed);
    ImageRelease(&pixels);
    assert(!pixels);

    // Explicit pre-existing GPU handle for this CPU-only branch. Never claim
    // that the texture above was uploaded, or resume the continuous gate here.
    resource->handle = 73;
    engine.boundTextures[0] = engine.boundTextures[1] = 9;
    unsigned cached = 0xffffffffU;
    canvas.TextureCreate(10062, cached, false);
    assert(cached == 73 && engine.boundTextures[0] == -1 && engine.boundTextures[1] == -1);
    alignas(LensFlare) unsigned char storage[sizeof(LensFlare)];
    std::memset(storage, 0xa5, sizeof(storage));
    auto *flare = new (storage) LensFlare(&canvas);
    assert(flare->canvas == &canvas && flare->width == 1280 && flare->height == 720);
    assert(canvas.images.size() == 3 && opened == before + 4 && opened == closed);
    for (unsigned i = 0; i < 3; ++i) {
        assert(flare->images[i] == i);
        auto *image = static_cast<Resource *>(canvas.FindResource(1288 + i));
        assert(image->handle == int(i));
        checkRegion(*canvas.images[i], side, xy[variant][i * 2], xy[variant][i * 2 + 1]);
        assert(canvas.GetImage2DWidth(i) == 64 && canvas.GetImage2DHeight(i) == 64);
        unsigned out = 99;
        canvas.Image2DCreate(1288 + i, out);
        assert(out == i && canvas.images.size() == 3 && opened == before + 4);
    }
    assert(canvas.GetImage2DWidth(999) == 0 && canvas.GetImage2DHeight(999) == 0);
    flare->intensity = 90;
    flare->update(123);
    assert(flare->intensity == 0);
    flare->~LensFlare();
    assert(canvas.images.size() == 3); // LensFlare does not release canvas-owned images.

    unsigned untouched = 0x12345678;
    canvas.Image2DCreate(65534, untouched);
    assert(untouched == 0x12345678);
    auto *image = static_cast<Resource *>(canvas.FindResource(1288));
    auto *info = static_cast<uint16_t *>(image->payload);
    unsigned short saved = info[0];
    info[0] = 65534;
    canvas.Image2DCreate(1288, untouched);
    assert(untouched == 0x12345678); // Missing texture wins even over cached image.
    info[0] = saved;
    std::printf("CPU cached-atlas branch: %s; %ux%u RGBA; 3 original regions, cache/reject/lifetime OK\n",
                paths[variant], side, side);
}

int main(int argc, char **argv) {
    int result = stationEntryMain(argc, argv);
    assert(result == 0);
    for (unsigned variant = 0; variant < 3; ++variant) checkCachedAtlas(variant);
    assert(flareUploadCalls == 1 && flareGlCalls == 1);
    std::puts("No LensFlare GPU operation returned success; continuous stage 0 remains incomplete.");
}
