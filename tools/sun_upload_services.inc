#include <algorithm>
static unsigned sunUploads;
static void checkSunMipPixels(GLuint, const unsigned char *, unsigned);

int AbyssEngine::sunTextureUpload(Engine *engine, const char *path, ImageCallback callback,
                                 void *user, unsigned *out, float scale,
                                 AELoadedTexture *record, bool managed) {
    assert(!callback && !user && !record && managed && scale == 0 && *out == 0);
    assert(engine->boundTextures[0] == -1 && engine->boundTextures[1] == -1);
    assert(sunUploads++ == 0 && engine->appManager->paintCanvas->cubeTextures.size() == 1);
    GLint formatCount = 0;
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &formatCount);
    std::vector<GLint> formats(formatCount);
    glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, formats.data());
    std::printf("ETC1 advertised: %d; extension: %d\n",
        std::find(formats.begin(), formats.end(), 0x8d64) != formats.end(),
        std::strstr(reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)), "GL_OES_compressed_ETC1_RGB8_texture") != nullptr);
    int result = FlareUpload::TextureCreateFromFileIntern(engine, path, callback, user, out, scale, record, managed);
    auto &textures = engine->appManager->paintCanvas->cubeTextures;
    assert(result == 1 && *out == 1 && textures.size() == 2);
    auto *sun = textures[1];
    if (!sun || sun->glId == 0xffffffffU)
        std::fprintf(stderr, "Sun upload did not produce a valid GL record; refusing to cross boundary\n");
    assert(sun && sun->valid && !sun->isCube && sun->glId != 0xffffffffU && glIsTexture(sun->glId));
    assert(sun->byteSize == 43704 && sun->scale == 0);
    assert(engine->textureByteCounter == 4194304 + 43704 && Engine::ImageCount == 2);
    assert(cursor == 43731 && input.size() == 43733 && input[8] == 0x42);
    assert(input[9] == 0 && input[10] == 1 && input[11] == 0 && input[12] == 1);
    unsigned regions = unsigned(input[13]) | unsigned(input[14]) << 8;
    assert(regions == 1);
    unsigned lengthAt = 15 + regions * 8;
    uint32_t size;
    std::memcpy(&size, input.data() + lengthAt, 4);
    assert(size == 43704 && lengthAt + 4 + size + 2 == input.size());
    assert(glGetError() == GL_NO_ERROR);
    checkSunMipPixels(sun->glId, input.data() + lengthAt + 4, size);
    return result;
}
