#include "engine/render/FBOContainer.h"
#include "engine/render/Engine.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    FBOContainer::FBOContainer(Engine *engine, String name) {
        this->created = 0;
        this->field_0x28 = 0;
        this->framebuffer = 0;
        this->texture = 0;
        this->renderbuffer = 0;
        this->width = 0;
        this->height = 0;
        this->name = name;
        this->engine = engine;
    }

    FBOContainer::~FBOContainer() {
        Release();
    }

    void FBOContainer::Create(int width, int height, bool a, bool linear) {
        this->width = width;
        this->height = height;
        glGenFramebuffers(1, &this->framebuffer);
        glBindFramebuffer(0x8d40, this->framebuffer);
        glGenTextures(1, &this->texture);
        glBindTexture(0xde1, this->texture);
        glPixelStorei(0xcf5, 1);
        glTexParameteri(0xde1, 0x2802, 0x812f);
        glTexParameteri(0xde1, 0x2803, 0x812f);
        int filter;
        if (linear) {
            glTexParameteri(0xde1, 0x2800, 0x2601);
            filter = 0x2601;
        } else {
            glTexParameteri(0xde1, 0x2800, 0x2600);
            filter = 0x2600;
        }
        glTexParameteri(0xde1, 0x2801, filter);
        glTexImage2D(0xde1, 0, 0x1908, this->width, this->height, 0, 0x1908, 0x1401, 0);
        glFramebufferTexture2D(0x8d40, 0x8ce0, 0xde1, this->texture, 0);
        glGenRenderbuffers(1, &this->renderbuffer);
        glBindRenderbuffer(0x8d41, this->renderbuffer);
        glRenderbufferStorage(0x8d41, 0x81a5, this->width, this->height);
        glFramebufferRenderbuffer(0x8d40, 0x8d00, 0x8d41, this->renderbuffer);
        glCheckFramebufferStatus(0x8d40);
        this->created = 1;
        glBindFramebuffer(0x8d40, this->engine->field_0x40c);
    }

    void FBOContainer::Release() {
        if (this->created == 0) {
            return;
        }
        glDeleteFramebuffers(1, &this->framebuffer);
        this->framebuffer = 0;
        glDeleteTextures(1, &this->texture);
        this->texture = 0;
        glDeleteRenderbuffers(1, &this->renderbuffer);
        this->renderbuffer = 0;
        this->created = 0;
        glDeleteRenderbuffers(1, &this->extraRenderbuffer1);
        glDeleteRenderbuffers(1, &this->extraRenderbuffer2);
        glBindFramebuffer(0x8d40, 0);
        glDeleteRenderbuffers(1, &this->extraRenderbuffer0);
    }

    void FBOContainer::Activate() {
        glBindTexture(0xde1, this->texture);
    }

    void FBOContainer::BeginCapture() {
        glBindFramebuffer(0x8d40, this->framebuffer);
        glViewport(0, 0, this->width, this->height);
    }

    void FBOContainer::EndCapture() {
        glBindFramebuffer(0x8d40, this->engine->field_0x40c);
    }
}
