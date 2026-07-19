#include "engine/render/shaders/TexOnlyShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int TexOnlyShader::ShaderIndex;

    TexOnlyShader::TexOnlyShader() {
        TexOnlyShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"TexOnlyShader";
    }

    void TexOnlyShader::Init(Engine *) {
        this->program = (int) this->ES2LoadProgram("TexOnlyShader.vsh", "TexOnlyShader.fsh");
        this->aPosition = glGetAttribLocation(this->program, "a_position");
        this->aTexCoord = glGetAttribLocation(this->program, "a_texCoord");
        this->uWorldMatrix = glGetUniformLocation(this->program, "u_WorldMatrix");
        this->sTexture = glGetUniformLocation(this->program, "s_texture");
        glUseProgram(this->program);
        glUniform1i(this->sTexture, 0);
    }

    void TexOnlyShader::SetInActive() {
        glDisableVertexAttribArray(this->aPosition);
        glDisableVertexAttribArray(this->aTexCoord);
    }

    void TexOnlyShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        glUniformMatrix4fv(this->uWorldMatrix, 1, 0, engine->worldViewProjMatrix);
        if (this->dirty != 0) {
            this->dirty = 0;
        }
        glEnableVertexAttribArray(this->aPosition);
        glEnableVertexAttribArray(this->aTexCoord);

        if (mesh->uploaded == 0) {
            glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, mesh->positions);
            glVertexAttribPointer(this->aTexCoord, 2, 0x1406, 0, 0, mesh->texCoords);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->aTexCoord, 2, 0x1406, 0, 0, 0);
        }
    }
}
