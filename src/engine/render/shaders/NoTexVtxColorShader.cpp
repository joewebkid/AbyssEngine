#include "engine/render/shaders/NoTexVtxColorShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include "engine/render/shaders/EmbeddedNoTexShaders.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int NoTexVtxColorShader::ShaderIndex;

    void NoTexVtxColorShader::SetInActive() {
        glDisableVertexAttribArray(aPosition);
        glDisableVertexAttribArray(aColor);
    }

    void NoTexVtxColorShader::Init(Engine *) {
        this->program = this->ES2LoadProgram(EmbeddedNoTexVtxColorShader::vertex,
                                             EmbeddedNoTexVtxColorShader::fragment);

        aPosition = glGetAttribLocation(this->program, "a_position");
        aColor = glGetAttribLocation(this->program, "a_VertexColor");
        uMvpMatrix = glGetUniformLocation(this->program, "u_WorldMatrix");
        uColor = glGetUniformLocation(this->program, "glColor");

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
            ptr = engine->GetLineVertices2D();
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
