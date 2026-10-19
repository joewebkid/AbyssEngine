static unsigned stationUploads;
static void checkStationMipPixels(GLuint texture, const unsigned char *base, unsigned baseSide = 64) {
    unsigned levels = 1;
    for (unsigned side = baseSide; side > 1; side /= 2) ++levels;
    auto pixels = readMipPixels(texture, baseSide, levels);
    assert(std::memcmp(pixels[0].data(), base, baseSide * baseSide * 4) == 0);
    // Generated mips have no original-file payload. This checks a 2x2 box
    // filter with accumulated nearest-byte rounding, not a GLES-mandated kernel.
    for (unsigned level = 1, side = baseSide / 2; level < levels; ++level, side /= 2) {
        unsigned span = 1u << level, area = span * span, maxError = 0;
        for (unsigned y = 0; y < side; ++y) for (unsigned x = 0; x < side; ++x)
            for (unsigned c = 0; c < 4; ++c) {
                unsigned sum = 0;
                for (unsigned yy = 0; yy < span; ++yy) for (unsigned xx = 0; xx < span; ++xx)
                    sum += base[((y * span + yy) * baseSide + x * span + xx) * 4 + c];
                int expected = int((sum + area / 2) / area);
                unsigned error = unsigned(std::abs(int(pixels[level][(y * side + x) * 4 + c]) - expected));
                maxError = std::max(maxError, error);
            }
        std::printf("Station generated mip %u: box-reference max error %u, bound %u\n", level, maxError, (level + 1) / 2);
        assert(maxError <= (level + 1) / 2);
    }
    std::printf("Station RGBA: %u base bytes exact; %u generated mips checked separately\n", baseSide * baseSide * 4, levels - 1);
}

int AbyssEngine::stationTextureUpload(Engine *engine, const char *path, ImageCallback callback,
                                     void *user, unsigned *out, float scale,
                                     AELoadedTexture *record, bool managed) {
    assert(!callback && !user && !record && managed && scale == 0 && *out == 0);
    assert(engine->boundTextures[0] == -1 && engine->boundTextures[1] == -1);
    assert(stationUploads++ == 0 && sunUploads == 1);
    auto &textures = engine->appManager->paintCanvas->cubeTextures;
    assert(textures.size() == 2);
    int result = FlareUpload::TextureCreateFromFileIntern(engine, path, callback, user, out, scale, record, managed);
    assert(result == 1 && *out == 2 && textures.size() == 3);
    auto *station = textures[2];
    assert(station && station->valid && !station->isCube && station->scale == 0);
    assert(station->glId != 0xffffffffU && glIsTexture(station->glId) && station->byteSize == 16384);
    assert(engine->textureByteCounter == 4194304 + 43704 + 16384 && Engine::ImageCount == 3);
    assert(input.size() == 16409 && cursor == 16407 && input[8] == 3);
    assert(input[9] == 64 && input[10] == 0 && input[11] == 64 && input[12] == 0);
    assert(input[13] == 1 && input[14] == 0 && glGetError() == GL_NO_ERROR);
    checkGpuPixels(station->glId, 64, input.data() + 23);
    checkStationMipPixels(station->glId, input.data() + 23);
    return result;
}

static void firstStationTextureReturned() {
    assert(stationUploads == 1 && systemTextureBoundaryCalls == 1);
    assert(starTextures->size() == 6 && (*starTextures)[0] == 1 && (*starTextures)[1] == 2);
    for (unsigned i = 2; i < 6; ++i) assert((*starTextures)[i] == 0);
    assert((*starIndices)[0] == 5); // The original ring query returned and the first iteration completed.
    for (unsigned i = 1; i < starIndices->size(); ++i) assert((*starIndices)[i] == 0);
    auto *resource = static_cast<Resource *>(Globals::Canvas->FindResource(10014));
    assert(resource && resource->handle == 2 && opened == closed && cursor == 16407);
    auto *next = static_cast<Resource *>(Globals::Canvas->FindResource(10021));
    assert(next && next->handle == -1);
    std::puts("Station TextureCreate(10014) returned, managed index 2 and station index 5 published; next boundary: TextureCreate(10021); Stage 0 incomplete");
    throw FlareImageBoundary{};
}
