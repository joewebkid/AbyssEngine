// A real EGL pbuffer and GLES readback; failure is never converted to success.
#pragma once
#include <EGL/egl.h>
#ifdef FindResource
#undef FindResource
#endif
#include <GLES2/gl2.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>

struct AtlasEglContext {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    AtlasEglContext() {
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        EGLint major, minor, count;
        assert(display != EGL_NO_DISPLAY && eglInitialize(display, &major, &minor));
        assert(eglBindAPI(EGL_OPENGL_ES_API));
        const EGLint configAttrs[] = {EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8, EGL_NONE};
        EGLConfig config;
        assert(eglChooseConfig(display, configAttrs, &config, 1, &count) && count == 1);
        const EGLint surfaceAttrs[] = {EGL_WIDTH, 16, EGL_HEIGHT, 16, EGL_NONE};
        surface = eglCreatePbufferSurface(display, config, surfaceAttrs);
        const EGLint contextAttrs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttrs);
        assert(surface != EGL_NO_SURFACE && context != EGL_NO_CONTEXT);
        assert(eglMakeCurrent(display, surface, surface, context));
        std::printf("EGL %d.%d vendor=%s; GLES vendor=%s renderer=%s version=%s\n",
            major, minor, eglQueryString(display, EGL_VENDOR), glGetString(GL_VENDOR),
            glGetString(GL_RENDERER), glGetString(GL_VERSION));
        assert(glGetError() == GL_NO_ERROR);
    }
    ~AtlasEglContext() {
        assert(eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
        assert(eglDestroyContext(display, context));
        assert(eglDestroySurface(display, surface));
        assert(eglTerminate(display));
    }
};

static void checkGpuPixels(GLuint texture, unsigned side, const unsigned char *expected) {
    assert(glIsTexture(texture));
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    std::vector<unsigned char> pixels(side * side * 4);
    glReadPixels(0, 0, side, side, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    assert(glGetError() == GL_NO_ERROR);
    assert(std::memcmp(pixels.data(), expected, pixels.size()) == 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    assert(glGetError() == GL_NO_ERROR);
    std::printf("GPU readback: %u RGBA bytes identical to original AEI payload\n", side * side * 4);
}
