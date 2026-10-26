#include "engine/render/shaders/NoTexShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#if defined(__ANDROID__) && __SIZEOF_POINTER__ == 8
#include "engine/render/shaders/EmbeddedNoTexShaders.h"
#endif
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int NoTexShader::ShaderIndex;

    void NoTexShader::SetInActive() {
        glDisableVertexAttribArray(this->aPosition);
    }

    void NoTexShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        glUniformMatrix4fv(this->uMvpMatrix, 1, 0, engine->worldViewProjMatrix);
        if (this->dirty != 0) {
            glUniform4fv(this->uColor, 1, engine->glColor);
            this->dirty = 0;
        }
        glEnableVertexAttribArray(this->aPosition);

        int size;
        const void *ptr;
        if (mesh == 0) {
            ptr = *(void **) &engine->field_0x348;
            size = 2;
        } else {
            if (mesh->uploaded == 0) {
                ptr = mesh->positions;
            } else {
                glBindBuffer(0x8892, mesh->positionVBO);
                ptr = 0;
            }
            size = 3;
        }
        glVertexAttribPointer(this->aPosition, size, 0x1406, 0, 0, ptr);
    }

    void NoTexShader::Init(Engine *) {
#if defined(__ANDROID__) && __SIZEOF_POINTER__ == 8
        this->program = this->ES2LoadProgram(EmbeddedNoTexShader::vertex,
                                             EmbeddedNoTexShader::fragment);
#else
        this->program = this->ES2LoadProgram("NoTexShader.vsh", "NoTexShader.fsh");
#endif
        this->aPosition = glGetAttribLocation(this->program, "a_position");
#if defined(__ANDROID__) && __SIZEOF_POINTER__ == 8
        this->uMvpMatrix = glGetUniformLocation(this->program, "u_WorldMatrix");
        this->uColor = glGetUniformLocation(this->program, "glColor");
#else
        this->uMvpMatrix = glGetUniformLocation(this->program, "u_mvp");
        this->uColor = glGetUniformLocation(this->program, "u_color");
#endif
        glUseProgram(this->program);
    }

    NoTexShader::NoTexShader() {
        NoTexShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("NoTexShader");
        this->name = shaderName;
    }
}
