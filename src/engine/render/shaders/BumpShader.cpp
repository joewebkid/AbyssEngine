#include "engine/render/shaders/BumpShader.h"
#include "engine/render/shaders/EmbeddedBumpShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int BumpShader::ShaderIndex;

    BumpShader::BumpShader() {
        BumpShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("BumpShader");
        this->name = shaderName;
    }

    void BumpShader::SetInActive() {
        if (this->a0Loc >= 0)
            glDisableVertexAttribArray(this->a0Loc);
        if (this->a1Loc >= 0)
            glDisableVertexAttribArray(this->a1Loc);
        if (this->a2Loc >= 0)
            glDisableVertexAttribArray(this->a2Loc);
    }

    void BumpShader::Init(Engine *) {
        this->program = this->ES2LoadProgram(EmbeddedBumpShader::vertex,
                                             EmbeddedBumpShader::fragment);

        this->a0Loc = glGetAttribLocation(this->program, "a_position");
        this->a1Loc = glGetAttribLocation(this->program, "a_texCoord");
        this->a2Loc = glGetAttribLocation(this->program, "a_normal");

        this->u0Loc = glGetUniformLocation(this->program, "u_ModelViewProjectionMatrix");
        this->u1Loc = glGetUniformLocation(this->program, "u_ModelMatrix");
        this->u2Loc = glGetUniformLocation(this->program, "u_lightdirmodel[0]");
        this->u3Loc = glGetUniformLocation(this->program, "u_lightdirmodel[1]");
        this->u4Loc = glGetUniformLocation(this->program, "u_eyeposmodel");
        this->u5Loc = glGetUniformLocation(this->program, "s_texture");
        this->u6Loc = glGetUniformLocation(this->program, "u_texture_cubemap");
        this->u7Loc = glGetUniformLocation(this->program, "glColor");
        this->u8Loc = glGetUniformLocation(this->program, "u_AmbientColor[0]");
        this->u9Loc = glGetUniformLocation(this->program, "u_DiffuseColor[0]");
        this->u10Loc = glGetUniformLocation(this->program, "u_SpecularColor[0]");
        this->u11Loc = glGetUniformLocation(this->program, "u_AmbientColor[1]");
        this->u12Loc = glGetUniformLocation(this->program, "u_DiffuseColor[1]");
        this->u13Loc = glGetUniformLocation(this->program, "u_SpecularColor[1]");
        this->u14Loc = glGetUniformLocation(this->program, "u_SpecularPower");
        this->u15Loc = glGetUniformLocation(this->program, "u_RimColor");
        this->u16Loc = glGetUniformLocation(this->program, "u_TexBiasDiffuse");
        this->u17Loc = glGetUniformLocation(this->program, "u_TexBiasNormal");
        this->u18Loc = glGetUniformLocation(this->program, "u_fogColor");
        this->u19Loc = glGetUniformLocation(this->program, "u_fogMaxDist");
        this->u20Loc = glGetUniformLocation(this->program, "u_fogMinDist");
        this->u21Loc = glGetUniformLocation(this->program, "u_EnableFog");

        glUseProgram(this->program);
        glUniform1i(this->u5Loc, 0);
        glUniform1i(this->u6Loc, 7);
    }

    void BumpShader::UpdateMeshData(Mesh *mesh, Engine *ctx) {
        if (this->u0Loc >= 0)
            glUniformMatrix4fv(this->u0Loc, 1, 0, ctx->worldViewProjMatrix);
        if (this->u1Loc >= 0)
            glUniformMatrix3fv(this->u1Loc, 1, 0, ctx->normalMatrix);
        if (this->u16Loc >= 0)
            glUniform1f(this->u16Loc, Engine::lodBiasDiffuse);
        if (this->u17Loc >= 0)
            glUniform1f(this->u17Loc, Engine::lodBiasNormal);

        if (this->dirty != 0) {
            glUniform3f(this->u2Loc, ctx->lightDir.x, ctx->lightDir.y, ctx->lightDir.z);
            if (this->u4Loc >= 0)
                glUniform3f(this->u4Loc, ctx->lightColor.x, ctx->lightColor.y, ctx->lightColor.z);
            if (this->u7Loc >= 0)
                glUniform4fv(this->u7Loc, 1, ctx->glColor);
            if (this->u8Loc >= 0)
                glUniform3fv(this->u8Loc, 1, (const float *) &ctx->lightAmbientShaded);
            if (this->u9Loc >= 0)
                glUniform3fv(this->u9Loc, 1, (const float *) &ctx->field_0x2fc);
            if (this->u10Loc >= 0)
                glUniform3fv(this->u10Loc, 1, (const float *) &ctx->lightDiffuseShaded);
            if (this->u14Loc >= 0)
                glUniform1f(this->u14Loc, ctx->materialShininess);
            if (this->u15Loc >= 0)
                glUniform3fv(this->u15Loc, 1, (const float *) &ctx->rimColor);
            if (this->u18Loc >= 0) {
                glUniform3fv(this->u18Loc, 1, ctx->fogColor);
            }
            if (this->u20Loc >= 0)
                glUniform1f(this->u20Loc, ctx->fogMinDist);
            if (this->u19Loc >= 0)
                glUniform1f(this->u19Loc, ctx->fogMaxDist);
            if (this->u21Loc >= 0)
                glUniform1i(this->u21Loc, Engine::fogEnabled);

            if (ctx->field_0x32c < 2) {
                glUniform3f(this->u11Loc, 0, 0, 0);
                glUniform3f(this->u12Loc, 0, 0, 0);
                glUniform3f(this->u13Loc, 0, 0, 0);
                glUniform3f(this->u3Loc, ctx->field_0x33c.x, ctx->field_0x33c.y,
                            ctx->field_0x33c.z);
            } else {
                glUniform3fv(this->u11Loc, 1, (const float *) &ctx->lightSpecularShaded);
                glUniform3fv(this->u12Loc, 1, (const float *) &ctx->field_0x308);
                glUniform3fv(this->u13Loc, 1, (const float *) &ctx->particleAmbient);
                glUniform3f(this->u3Loc, ctx->field_0x33c.x, ctx->field_0x33c.y,
                            ctx->field_0x33c.z);
            }
            this->dirty = 0;
        }

        if (this->a0Loc >= 0)
            glEnableVertexAttribArray(this->a0Loc);
        if (this->a1Loc >= 0)
            glEnableVertexAttribArray(this->a1Loc);
        if (this->a2Loc >= 0)
            glEnableVertexAttribArray(this->a2Loc);

        if (mesh->uploaded == 0) {
            if (this->a0Loc >= 0)
                glVertexAttribPointer(this->a0Loc, 3, 0x1406, 0, 0, mesh->positions);
            if (this->a1Loc >= 0)
                glVertexAttribPointer(this->a1Loc, 2, 0x1406, 0, 0, mesh->texCoords);
            if (this->a2Loc >= 0)
                glVertexAttribPointer(this->a2Loc, 3, 0x1406, 0, 0, mesh->normals);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->a0Loc, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->a1Loc, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->a2Loc, 3, 0x1406, 0, 0, 0);
        }
    }
}
