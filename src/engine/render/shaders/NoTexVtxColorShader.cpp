#include "engine/render/shaders/NoTexVtxColorShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#if defined(__ANDROID__) && __SIZEOF_POINTER__ == 8
#include "engine/render/shaders/EmbeddedNoTexShaders.h"
#endif
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int NoTexVtxColorShader::ShaderIndex;

    void NoTexVtxColorShader::SetInActive() {
        glDisableVertexAttribArray(aPosition);
        glDisableVertexAttribArray(aColor);
    }

    void NoTexVtxColorShader::Init(Engine *) {
#if defined(__ANDROID__) && __SIZEOF_POINTER__ == 8
        this->program = this->ES2LoadProgram(EmbeddedNoTexVtxColorShader::vertex,
                                             EmbeddedNoTexVtxColorShader::fragment);
#else
        this->program = this->ES2LoadProgram("NoTexVtxColorShader.vsh", "NoTexVtxColorShader.fsh");
#endif

        aPosition = glGetAttribLocation(this->program, "a_position");
#if defined(__ANDROID__) && __SIZEOF_POINTER__ == 8
        aColor = glGetAttribLocation(this->program, "a_VertexColor");
        uMvpMatrix = glGetUniformLocation(this->program, "u_WorldMatrix");
        uColor = glGetUniformLocation(this->program, "glColor");
#else
        aColor = glGetAttribLocation(this->program, "a_color");

        uMvpMatrix = glGetUniformLocation(this->program, "u_mvp");
        uColor = glGetUniformLocation(this->program, "u_color");
#endif

        glUseProgram(this->program);
    }

    NoTexVtxColorShader::NoTexVtxColorShader() {
        NoTexVtxColorShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("NoTexVtxColorShader");
        this->name = shaderName;
    }

    void NoTexVtxColorShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        glUniformMatrix4fv(uMvpMatrix, 1, 0, engine->worldViewProjMatrix);
        if (this->dirty != 0) {
            glUniform4fv(uColor, 1, engine->glColor);
            this->dirty = 0;
        }
        glEnableVertexAttribArray(aPosition);
        glEnableVertexAttribArray(aColor);

        int index = aPosition;
        int size;
        const void *ptr;
        int stride = 0;
        if (mesh == 0) {
            ptr = *(void **) &engine->field_0x348;
            size = 2;
        } else {
            glVertexAttribPointer(index, 3, 0x1406, 0, stride, mesh->positions);
            ptr = mesh->colors;
            index = aColor;
            size = 4;
        }
        glVertexAttribPointer(index, size, 0x1406, 0, stride, ptr);
    }
}
