#include "game/core/BumpShaderRefract.h"

#include "engine/render/Engine.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int BumpShaderRefract::ShaderIndex;

    void BumpShaderRefract::SetInActive() {
        if (aPosition >= 0)
            glDisableVertexAttribArray(aPosition);
        if (aTexCoord >= 0)
            glDisableVertexAttribArray(aTexCoord);
        if (uSampler0 >= 0)
            glDisableVertexAttribArray(uSampler0);
    }

    BumpShaderRefract::BumpShaderRefract() {
        ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"BumpShaderRefract";
    }

    void BumpShaderRefract::Init(::Engine *engine) {
        int program = this->ES2LoadProgram("BumpShaderRefract.vsh", "BumpShaderRefract.fsh");
        this->program = program;

        aPosition = glGetAttribLocation(program, "a0");
        aTexCoord = glGetAttribLocation(this->program, "a1");

        uSampler0 = glGetUniformLocation(this->program, "u0");
        uMvpMatrix = glGetUniformLocation(this->program, "u1");
        uSampler2 = glGetUniformLocation(this->program, "u2");
        uSampler3 = glGetUniformLocation(this->program, "u3");
        uPixelSize = glGetUniformLocation(this->program, "u4");
        uViewMatrix = glGetUniformLocation(this->program, "u5");
        uColorIndex = glGetUniformLocation(this->program, "u6");
        uColor = glGetUniformLocation(this->program, "u7");

        glActiveTexture(0x84c7);
        engine->ActivateRefractFBO();
        uRefractSampler = glGetUniformLocation(this->program, "u8");

        glUseProgram(this->program);

        int *samplers = &uSampler2;
        for (int i = 0; i != 2; i++) {
            int loc = samplers[i];
            if (loc >= 0)
                glUniform1i(loc, i);
        }
        glUniform1i(uRefractSampler, 7);
    }

    void BumpShaderRefract::UpdateMeshData(Mesh *mesh, ::Engine *engine) {
        char *e = (char *) engine;
        char *m = (char *) mesh;

        if (uMvpMatrix >= 0)
            glUniformMatrix4fv(uMvpMatrix, 1, 0, (float *) (e + 0x104));
        if (uViewMatrix >= 0)
            glUniformMatrix4fv(uViewMatrix, 1, 0, (float *) (e + 0x1c4));

        if (this->dirty != 0) {
            if (uColorIndex >= 0)
                glUniform1i(uColorIndex, *(uint8_t *) (m + 0x85));
            glUniform4fv(uColor, 1, (float *) (e + 0xd0));
            int loc = uPixelSize;
            if (loc >= 0) {
                float w = (float) engine->GetDisplayWidth();
                float h = (float) engine->GetDisplayHeight();
                glUniform2f(loc, 1.0f / w, 1.0f / h);
            }
            glActiveTexture(0x84c7);
            engine->ActivateRefractFBO();
            this->dirty = 0;
        }

        if (aPosition >= 0)
            glEnableVertexAttribArray(aPosition);
        if (aTexCoord >= 0)
            glEnableVertexAttribArray(aTexCoord);
        if (uSampler0 >= 0)
            glEnableVertexAttribArray(uSampler0);

        int loc0 = aPosition;
        const void *ptr;
        int last;
        if (*(uint8_t *) (m + 0x5c) == 0) {
            if (loc0 >= 0)
                glVertexAttribPointer(loc0, 3, 0x1406, 0, 0, *(void **) (m + 0x4));
            if (aTexCoord >= 0)
                glVertexAttribPointer(aTexCoord, 2, 0x1406, 0, 0, *(void **) (m + 0x8));
            last = uSampler0;
            if (last < 0)
                return;
            ptr = *(void **) (m + 0xc);
        } else {
            if (loc0 >= 0) {
                glBindBuffer(0x8892, *(uint32_t *) (m + 0x60));
                glVertexAttribPointer(aPosition, 3, 0x1406, 0, 0, 0);
            }
            if (aTexCoord >= 0) {
                glBindBuffer(0x8892, *(uint32_t *) (m + 0x68));
                glVertexAttribPointer(aTexCoord, 2, 0x1406, 0, 0, 0);
            }
            if (uSampler0 < 0)
                return;
            glBindBuffer(0x8892, *(uint32_t *) (m + 0x78));
            ptr = 0;
            last = uSampler0;
        }
        glVertexAttribPointer(last, 4, 0x1406, 0, 0, ptr);
    }
}
