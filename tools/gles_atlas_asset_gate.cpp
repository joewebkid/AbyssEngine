#include "gles_atlas_file_service.h"
#include "gles_atlas_station_under_test.inc"

static void rejectDimensions(Image *image, void *user) {
    assert(user == nullptr);
    GLint maximum;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maximum);
    assert(maximum < 65535);
    image->width = 65535; // Real driver rejects this; no GL result is mocked.
}

static void checkUploadBranches() {
    const char *paths[] = {"data/textures/gof2_interface.aei",
        "data/textures/gof2_interface_iphone4.aei", "data/textures/gof2_interface_ipad_1440.aei"};
    for (unsigned variant = 0; variant < 3; ++variant) {
        Engine engine;
        PaintCanvas canvas(&engine);
        ApplicationManager manager(&engine);
        manager.paintCanvas = &canvas;
        engine.appManager = &manager;
        engine.linearFilterFlag = 1;
        engine.textureByteCounter = 0;
        Engine::ImageCount = 0;
        unsigned side = variant ? 2048 : 1024;
        unsigned id = 999;
        auto upload = [&](ImageCallback callback, AELoadedTexture *record, bool managed) {
            return FlareUpload::TextureCreateFromFileIntern(&engine, paths[variant], callback,
                nullptr, &id, 0.75f, record, managed);
        };
        // Fresh managed publication; the GL name and managed index are different domains.
        assert(upload(nullptr, nullptr, true) == 1 && id == 0 && canvas.cubeTextures.size() == 1);
        auto *record = canvas.cubeTextures[0];
        assert(record->glId && glIsTexture(record->glId));
        assert(record->valid == 1 && record->isCube == 0 && record->scale == 0.75f);
        assert(record->byteSize == side * side * 4 && Engine::ImageCount == 1);
        assert(engine.textureByteCounter == int(record->byteSize));
        size_t pixelOffset = 15 + 8 * (unsigned(input[13]) | unsigned(input[14]) << 8);
        checkGpuPixels(record->glId, side, input.data() + pixelOffset);
        glDeleteTextures(1, &record->glId);
        if (variant != 0) {
            delete record;
            std::printf("Real GLES managed atlas upload: %s OK\n", paths[variant]);
            continue;
        }

        // Existing managed record is updated in place and retains its name/scale.
        engine.textureByteCounter = 37;
        Engine::ImageCount = 9;
        assert(upload(nullptr, record, true) == 1 && id == record->glId);
        assert(canvas.cubeTextures.size() == 1 && Engine::ImageCount == 10);
        assert(record->scale == 0.75f && engine.textureByteCounter == int(37 + record->byteSize));
        glDeleteTextures(1, &record->glId);
        engine.textureByteCounter = 37;
        Engine::ImageCount = 9;
        assert(upload(rejectDimensions, record, true) == 1);
        assert(record->glId == 0xffffffffU && record->byteSize == 0 && record->valid == 1);
        assert(!glIsTexture(id) && canvas.cubeTextures.size() == 1);
        assert(engine.textureByteCounter == 37 && Engine::ImageCount == 9 && engine.lastGlError == 0);

        // Failed fresh managed upload still publishes the original sentinel record.
        assert(upload(rejectDimensions, nullptr, true) == 1 && id == 1);
        auto *failed = canvas.cubeTextures[1];
        assert(failed->glId == 0xffffffffU && failed->byteSize == 0 && failed->valid == 1);
        assert(engine.textureByteCounter == 37 && Engine::ImageCount == 9);

        // Unmanaged success returns a GL name. Failure deletes it and returns -4.
        assert(upload(nullptr, nullptr, false) == 1 && glIsTexture(id));
        assert(canvas.cubeTextures.size() == 2 && engine.lastGlError == 0);
        glDeleteTextures(1, &id);
        assert(upload(rejectDimensions, nullptr, false) == -4 && !glIsTexture(id));
        assert(engine.lastGlError == GL_INVALID_VALUE && canvas.cubeTextures.size() == 2);
        char *errorPath = engine.lastErrorPath.GetAEChar();
        assert(std::strcmp(errorPath, paths[variant]) == 0);
        delete[] errorPath;
        assert(engine.textureByteCounter == 37 && Engine::ImageCount == 9);
        assert(glGetError() == GL_NO_ERROR && opened == closed);
        delete record;
        delete failed;
        std::printf("Real GLES upload/restore/unmanaged/error ownership branches: %s OK\n", paths[variant]);
    }
}

int main(int argc, char **argv) {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    AtlasEglContext context;
    assert(stationEntryMain(argc, argv) == 0);
    checkUploadBranches();
    std::puts("Continuous Level/LensFlare atlas upload, managed publication and GPU readback passed; "
              "Level stage 0 remains incomplete at SolarSystem::getStations.");
}
