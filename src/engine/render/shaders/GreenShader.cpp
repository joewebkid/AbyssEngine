#include "engine/render/shaders/GreenShader.h"
#include "engine/render/shaders/EmbeddedGreenShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int GreenShader::ShaderIndex;

    GreenShader::GreenShader() {
        GreenShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("GreenShader");
        this->name = shaderName;
    }

    void GreenShader::Init(Engine *) {
        this->program = this->ES2LoadProgram(EmbeddedGreenShader::vertex,
                                             EmbeddedGreenShader::fragment);

        this->a0Loc = glGetAttribLocation(this->program, "a_position");
        this->a1Loc = glGetAttribLocation(this->program, "a_texCoord");
        this->a2Loc = glGetAttribLocation(this->program, "a_normal");
        this->a3Loc = glGetAttribLocation(this->program, "a_tangent");
        this->a4Loc = glGetAttribLocation(this->program, "a_bitangent");

        this->u0Loc = glGetUniformLocation(this->program, "u_ModelViewProjectionMatrix");
        this->u1Loc = glGetUniformLocation(this->program, "ModelMatrix");
        this->u2Loc = glGetUniformLocation(this->program, "lightdirmodel");
        this->u3Loc = glGetUniformLocation(this->program, "u_eyeposmodel");
        this->u4Loc = glGetUniformLocation(this->program, "s_texture");
        this->u5Loc = glGetUniformLocation(this->program, "glColor");
        this->u6Loc = glGetUniformLocation(this->program, "u_AmbientColor");
        this->u7Loc = glGetUniformLocation(this->program, "u_DiffuseColor");
        this->u8Loc = glGetUniformLocation(this->program, "u_SpecularColor");

        glUseProgram(this->program);
        glUniform1i(this->u4Loc, 0);
    }

    void GreenShader::SetInActive() {
        if (this->a0Loc >= 0)
            glDisableVertexAttribArray(this->a0Loc);
        if (this->a1Loc >= 0)
            glDisableVertexAttribArray(this->a1Loc);
        if (this->a2Loc >= 0)
            glDisableVertexAttribArray(this->a2Loc);
        if (this->a3Loc >= 0)
            glDisableVertexAttribArray(this->a3Loc);
        if (this->a4Loc >= 0)
            glDisableVertexAttribArray(this->a4Loc);
    }

    void GreenShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->u0Loc >= 0)
            glUniformMatrix4fv(this->u0Loc, 1, 0, engine->worldViewProjMatrix);

        if (this->a0Loc >= 0)
            glEnableVertexAttribArray(this->a0Loc);
        if (this->a1Loc >= 0)
            glEnableVertexAttribArray(this->a1Loc);
        if (this->a2Loc >= 0)
            glEnableVertexAttribArray(this->a2Loc);
        if (this->a3Loc >= 0)
            glEnableVertexAttribArray(this->a3Loc);
        if (this->a4Loc >= 0)
            glEnableVertexAttribArray(this->a4Loc);

        if (mesh->uploaded != 0) {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->a0Loc, 3, 0x1406, 0, 0, 0);
        } else {
            if (this->a0Loc < 0)
                return;
            glVertexAttribPointer(this->a0Loc, 3, 0x1406, 0, 0, mesh->positions);
        }
    }
}
