#include "engine/render/shaders/SimpleRefractionShader.h"
#include "engine/render/shaders/EmbeddedSimpleRefractionShader.h"

#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int SimpleRefractionShader::ShaderIndex;

    SimpleRefractionShader::SimpleRefractionShader() {
        SimpleRefractionShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("SimpleRefractionShader");
        this->name = shaderName;
    }

    void SimpleRefractionShader::Init(::Engine *engine) {
        int program = this->ES2LoadProgram(EmbeddedSimpleRefractionShader::vertex,
                                           EmbeddedSimpleRefractionShader::fragment);
        this->program = program;

        this->aPositionLoc = glGetAttribLocation(program, "a_position");
        this->aTexCoordLoc = glGetAttribLocation(this->program, "a_texCoord");
        this->aNormalLoc = glGetAttribLocation(this->program, "a_normal");
        this->aTangentLoc = glGetAttribLocation(this->program, "a_tangent");

        this->aBitangentLoc = glGetAttribLocation(this->program, "a_bitangent");
        this->uM1Loc = glGetUniformLocation(this->program, "u_ModelViewProjectionMatrix");
        this->uM2Loc = glGetUniformLocation(this->program, "u_eyeposmodel");
        this->uTex0Loc = glGetUniformLocation(this->program, "s_texture[0]");
        this->uTex1Loc = glGetUniformLocation(this->program, "s_texture[1]");
        this->uM3Loc = glGetUniformLocation(this->program, "glColor");
        this->uM4Loc = glGetUniformLocation(this->program, "u_TexBias");
        this->uM5Loc = glGetUniformLocation(this->program, "u_texelSize");

        glActiveTexture(0x84c7);
        engine->ActivateRefractFBO();

        this->uRefractLoc = glGetUniformLocation(this->program, "u_CloakRate");
        this->uM6Loc = glGetUniformLocation(this->program, "s_texture_base");

        glUseProgram(this->program);

        for (int i = 0; i != 2; i = i + 1) {
            if ((&this->uTex0Loc)[i] >= 0)
                glUniform1i((&this->uTex0Loc)[i], i);
        }
        glUniform1i(this->uM6Loc, 7);
    }

    void SimpleRefractionShader::SetInActive() {
        if (this->aPositionLoc >= 0)
            glDisableVertexAttribArray(this->aPositionLoc);
        if (this->aTexCoordLoc >= 0)
            glDisableVertexAttribArray(this->aTexCoordLoc);
        if (this->aNormalLoc >= 0)
            glDisableVertexAttribArray(this->aNormalLoc);
        if (this->aTangentLoc >= 0)
            glDisableVertexAttribArray(this->aTangentLoc);
        if (this->aBitangentLoc >= 0)
            glDisableVertexAttribArray(this->aBitangentLoc);
    }

    void SimpleRefractionShader::UpdateMeshData(Mesh *mesh, ::Engine *engine) {
        if (this->uM1Loc >= 0)
            glUniformMatrix4fv(this->uM1Loc, 1, 0, engine->worldViewProjMatrix);

        if (this->dirty != 0) {
            if (this->uM4Loc >= 0)
                glUniform1f(this->uM4Loc, -2.0f);
            if (this->uM3Loc >= 0)
                glUniform4fv(this->uM3Loc, 1, engine->glColor);
            if (this->uM2Loc >= 0)
                glUniform3f(this->uM2Loc, engine->lightColor.x, engine->lightColor.y,
                            engine->lightColor.z);
            int loc = this->uM5Loc;
            float w = static_cast<float>(static_cast<int32_t>(engine->GetDisplayWidth()));
            float h = static_cast<float>(static_cast<int32_t>(engine->GetDisplayHeight()));
            glUniform2f(loc, 1.0f / w, 1.0f / h);
            glActiveTexture(0x84c7);
            engine->ActivateRefractFBO();
            glUniform1f(this->uRefractLoc, mesh->field_0x20);
            this->dirty = 0;
        }

        if (this->aPositionLoc >= 0)
            glEnableVertexAttribArray(this->aPositionLoc);
        if (this->aTexCoordLoc >= 0)
            glEnableVertexAttribArray(this->aTexCoordLoc);
        if (this->aNormalLoc >= 0)
            glEnableVertexAttribArray(this->aNormalLoc);
        if (this->aTangentLoc >= 0)
            glEnableVertexAttribArray(this->aTangentLoc);
        if (this->aBitangentLoc >= 0)
            glEnableVertexAttribArray(this->aBitangentLoc);

        if (mesh->uploaded == 0) {
            if (this->aPositionLoc >= 0)
                glVertexAttribPointer(this->aPositionLoc, 3, 0x1406, 0, 0, mesh->positions);
            if (this->aTexCoordLoc >= 0)
                glVertexAttribPointer(this->aTexCoordLoc, 2, 0x1406, 0, 0, mesh->texCoords);
            if (this->aNormalLoc >= 0)
                glVertexAttribPointer(this->aNormalLoc, 3, 0x1406, 0, 0, mesh->normals);
            if (this->aTangentLoc >= 0)
                glVertexAttribPointer(this->aTangentLoc, 3, 0x1406, 0, 0, mesh->tangents);
            if (this->aBitangentLoc >= 0)
                glVertexAttribPointer(this->aBitangentLoc, 3, 0x1406, 0, 0, mesh->binormals);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->aPositionLoc, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->aTexCoordLoc, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->aNormalLoc, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->tangentVBO);
            glVertexAttribPointer(this->aTangentLoc, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->binormalVBO);
            glVertexAttribPointer(this->aBitangentLoc, 3, 0x1406, 0, 0, 0);
        }
    }
}
