// Included after the inherited station services and before its main fixture.
#include "engine/render/LensFlare.h"
#define GL_APICALL
#include <GLES/gl.h>
#include <GLES2/gl2.h>
using namespace AbyssEngine;

struct FlareImageBoundary {};
static unsigned flareUploadCalls, flareGlCalls;
unsigned char Globals::iPad, Globals::iPadHD, Globals::retinaDisplay;
unsigned char Globals::iPadLarge, Globals::iPadAssetsWithLowerRes;
bool Engine::clampTextures;
float Engine::AnisotropyValue;
int Engine::ImageCount;

extern "C" GLenum glGetError() {
    ++flareGlCalls;
    throw FlareImageBoundary{};
}
// All other upload calls fail closed, including any accidental reordered call.
#define GL_TRAP(name) extern "C" void trap_##name() asm(#name); \
    extern "C" void trap_##name() { unexpected(#name); }
GL_TRAP(glGenTextures)
GL_TRAP(glBindTexture)
GL_TRAP(glPixelStorei)
GL_TRAP(glTexEnvi)
GL_TRAP(glTexParameteri)
GL_TRAP(glTexParameterf)
GL_TRAP(glTexImage2D)
GL_TRAP(glCompressedTexImage2D)
GL_TRAP(glDeleteTextures)
GL_TRAP(glGenerateMipmap)
#undef GL_TRAP
unsigned int AELabelObject(unsigned int, unsigned int, const char *) { unexpected(__func__); return 0; }

#include "lensflare_cpu_under_test.inc"
#include "lensflare_platform_under_test.inc"

int AbyssEngine::flareTextureBoundary(Engine *engine, const char *path, ImageCallback callback,
                                     void *user, unsigned int *out, float scale,
                                     AELoadedTexture *record, bool managed) {
    assert(std::strcmp(path, "data/textures/gof2_interface.aei") == 0);
    assert(!callback && !user && !record && managed && scale == 0.0f && *out == 0);
    assert(engine->boundTextures[0] == -1 && engine->boundTextures[1] == -1);
    ++flareUploadCalls;
    return FlareUpload::TextureCreateFromFileIntern(engine, path, callback, user, out, scale, record, managed);
}

static void checkGlBoundary(PaintCanvas &canvas) {
    assert(flareUploadCalls == 1 && flareGlCalls == 1);
    auto *texture = static_cast<Resource *>(canvas.FindResource(10062));
    auto *image = static_cast<Resource *>(canvas.FindResource(1288));
    assert(texture && texture->kind == 2 && texture->handle == -1);
    assert(image && image->kind == 3 && image->handle == -1);
    assert(canvas.images.size() == 0); // No fake texture or image completion.
}
