#include "engine/render/shaders/BumpShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

static float gBumpFloatAStorage = 0.0f;
static float gBumpFloatBStorage = 0.0f;
static uint8_t gBumpFlagStorage = 0;
static float *const gBumpFloatA = &gBumpFloatAStorage;
static float *const gBumpFloatB = &gBumpFloatBStorage;
static uint8_t *const gBumpFlag = &gBumpFlagStorage;

namespace AbyssEngine {
    int BumpShader::ShaderIndex;

    BumpShader::BumpShader() {
        BumpShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"BumpShader";
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
        this->program = this->ES2LoadProgram("BumpShader.vsh", "BumpShader.fsh");

        this->a0Loc = glGetAttribLocation(this->program, "a0");
        this->a1Loc = glGetAttribLocation(this->program, "a1");
        this->a2Loc = glGetAttribLocation(this->program, "a2");

        this->u0Loc = glGetUniformLocation(this->program, "u0");
        this->u1Loc = glGetUniformLocation(this->program, "u1");
        this->u2Loc = glGetUniformLocation(this->program, "u2");
        this->u3Loc = glGetUniformLocation(this->program, "u3");
        this->u4Loc = glGetUniformLocation(this->program, "u4");
        this->u5Loc = glGetUniformLocation(this->program, "u5");
        this->u6Loc = glGetUniformLocation(this->program, "u6");
        this->u7Loc = glGetUniformLocation(this->program, "u7");
        this->u8Loc = glGetUniformLocation(this->program, "u8");
        this->u9Loc = glGetUniformLocation(this->program, "u9");
        this->u10Loc = glGetUniformLocation(this->program, "u10");
        this->u11Loc = glGetUniformLocation(this->program, "u11");
        this->u12Loc = glGetUniformLocation(this->program, "u12");
        this->u13Loc = glGetUniformLocation(this->program, "u13");
        this->u14Loc = glGetUniformLocation(this->program, "u14");
        this->u15Loc = glGetUniformLocation(this->program, "u15");
        this->u16Loc = glGetUniformLocation(this->program, "u16");
        this->u17Loc = glGetUniformLocation(this->program, "u17");
        this->u18Loc = glGetUniformLocation(this->program, "u18");
        this->u19Loc = glGetUniformLocation(this->program, "u19");
        this->u20Loc = glGetUniformLocation(this->program, "u20");
        this->u21Loc = glGetUniformLocation(this->program, "u21");

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
            glUniform1f(this->u16Loc, *gBumpFloatA);
        if (this->u17Loc >= 0)
            glUniform1f(this->u17Loc, *gBumpFloatB);

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
                float *v = reinterpret_cast<float *>(&ctx->fogColor);
                glUniform3fv(this->u18Loc, 1, v);
            }
            if (this->u20Loc >= 0)
                glUniform1f(this->u20Loc, ctx->fogMinDist);
            if (this->u19Loc >= 0)
                glUniform1f(this->u19Loc, ctx->fogMaxDist);
            if (this->u21Loc >= 0)
                glUniform1i(this->u21Loc, *gBumpFlag);

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
