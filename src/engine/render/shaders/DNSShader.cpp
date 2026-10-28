#include "engine/render/shaders/DNSShader.h"
#include "engine/render/shaders/EmbeddedDNSShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include "game/core/String.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int DNSShader::ShaderIndex;

    DNSShader::DNSShader() {
        DNSShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("DNSShader");
        this->name = shaderName;
    }

    void DNSShader::Init(Engine *) {
        int program = this->LoadBindShader("data/shader/DNSShader.vs", "data/shader/DNSShader.fs");
        this->program = program;
        if (program == 0)
            this->program = this->ES2LoadProgram(DNSShaderSource::vertex, DNSShaderSource::fragment);

        this->aPositionLoc = glGetAttribLocation(this->program, "a_position");
        this->aTexCoordLoc = glGetAttribLocation(this->program, "a_texCoord");
        this->aNormalLoc = glGetAttribLocation(this->program, "a_normal");
        this->aTangentLoc = glGetAttribLocation(this->program, "a_tangent");
        this->aBitangentLoc = glGetAttribLocation(this->program, "a_bitangent");

        this->uM1Loc = glGetUniformLocation(this->program, "u_ModelViewProjectionMatrix");
        this->uM2Loc = glGetUniformLocation(this->program, "u_ModelMatrix");
        this->uM3Loc = glGetUniformLocation(this->program, "u_ModelMatrixFull");
        this->uM4Loc = glGetUniformLocation(this->program, "u_lightdirmodel");
        this->uM5Loc = glGetUniformLocation(this->program, "u_eyeposmodel");
        this->uM6Loc = glGetUniformLocation(this->program, "glColor");
        this->uM7Loc = glGetUniformLocation(this->program, "u_AmbientColor");
        this->uM8Loc = glGetUniformLocation(this->program, "u_DiffuseColor");
        this->uM9Loc = glGetUniformLocation(this->program, "u_SpecularColor");
        this->uM10Loc = glGetUniformLocation(this->program, "u_SpecularPower");
        this->uM11Loc = glGetUniformLocation(this->program, "u_TexBiasDiffuse");
        this->uM12Loc = glGetUniformLocation(this->program, "u_TexBiasNormal");

        glUseProgram(this->program);
    }

    void DNSShader::SetInActive() {
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

    void DNSShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->uM1Loc >= 0)
            glUniformMatrix4fv(this->uM1Loc, 1, 0, engine->worldViewProjMatrix);
        if (this->uM2Loc >= 0)
            glUniformMatrix3fv(this->uM2Loc, 1, 0, engine->normalMatrix);
        if (this->uM3Loc >= 0)
            glUniformMatrix4fv(this->uM3Loc, 1, 0, engine->modelMatrixGL);
        if (this->uM11Loc >= 0)
            glUniform1f(this->uM11Loc, Engine::lodBiasDiffuse);
        if (this->uM12Loc >= 0)
            glUniform1f(this->uM12Loc, Engine::lodBiasNormal);

        if (this->dirty != 0) {
            glUniform3f(this->uM4Loc, engine->lightDir.x, engine->lightDir.y,
                        engine->lightDir.z);
            if (this->uM5Loc >= 0)
                glUniform3f(this->uM5Loc, engine->lightColor.x, engine->lightColor.y,
                            engine->lightColor.z);
            if (this->uM6Loc >= 0)
                glUniform4fv(this->uM6Loc, 1, engine->glColor);
            if (this->uM7Loc >= 0)
                glUniform3fv(this->uM7Loc, 1, (float *) &engine->lightAmbientShaded);
            if (this->uM8Loc >= 0)
                glUniform3fv(this->uM8Loc, 1, (float *) &engine->field_0x2fc);
            if (this->uM9Loc >= 0)
                glUniform3fv(this->uM9Loc, 1, (float *) &engine->lightDiffuseShaded);
            if (this->uM10Loc >= 0)
                glUniform1f(this->uM10Loc, engine->materialShininess);
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
