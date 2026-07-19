#include "engine/render/shaders/BumpRimCubeShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

static float g_rimGlobalA = 0.0f;
static float g_rimGlobalB = 0.0f;
static unsigned char g_rimByteGlobal = 0;

namespace AbyssEngine {
    int BumpRimCubeShader::ShaderIndex;

    BumpRimCubeShader::BumpRimCubeShader() {
        ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"BumpRimCubeShader";
    }

    void BumpRimCubeShader::Init(Engine *) {
        int program = this->LoadBindShader("BumpRimCubeShader.vsh", "BumpRimCubeShader.fsh");
        this->program = program;
        if (program == 0) {
            program = this->ES2LoadProgram("BumpRimCubeShader.vsh", "BumpRimCubeShader.fsh");
            this->program = program;
        }

        this->aPosition = glGetAttribLocation(program, "a0");
        this->aTexCoord = glGetAttribLocation(this->program, "a1");
        this->aNormal = glGetAttribLocation(this->program, "a2");
        this->aTangent = glGetAttribLocation(this->program, "a3");

        this->aBitangent = glGetUniformLocation(this->program, "u0");
        this->uModelViewProjectionMatrix = glGetUniformLocation(this->program, "u1");
        this->uModelMatrix = glGetUniformLocation(this->program, "u2");
        this->uModelMatrixFull = glGetUniformLocation(this->program, "u3");
        this->uLightDirModel0 = glGetUniformLocation(this->program, "u4");
        this->uLightDirModel1 = glGetUniformLocation(this->program, "u5");
        this->uEyePosModel = glGetUniformLocation(this->program, "u6");
        this->uTexDiffuse = glGetUniformLocation(this->program, "u7");
        this->uTexNormal = glGetUniformLocation(this->program, "u8");
        this->uTexCube = glGetUniformLocation(this->program, "u9");
        this->uGlColor = glGetUniformLocation(this->program, "u10");
        this->uAmbientColor0 = glGetUniformLocation(this->program, "u11");
        this->uDiffuseColor0 = glGetUniformLocation(this->program, "u12");
        this->uSpecularColor0 = glGetUniformLocation(this->program, "u13");
        this->uDiffuseColor1 = glGetUniformLocation(this->program, "u14");
        this->uSpecularColor1 = glGetUniformLocation(this->program, "u15");
        this->uRimColor = glGetUniformLocation(this->program, "u16");
        this->uSpecularPower = glGetUniformLocation(this->program, "u17");
        this->uIsGlowMat = glGetUniformLocation(this->program, "u18");
        this->uTexBiasDiffuse = glGetUniformLocation(this->program, "u19");
        this->uTexBiasNormal = glGetUniformLocation(this->program, "u20");
        this->uFogColor = glGetUniformLocation(this->program, "u21");
        this->uFogMaxDist = glGetUniformLocation(this->program, "u22");
        this->uFogMinDist = glGetUniformLocation(this->program, "u23");
        this->uEnableFog = glGetUniformLocation(this->program, "u24");
        this->uLodDist = glGetUniformLocation(this->program, "u25");

        glUseProgram(this->program);
        if (this->uTexDiffuse >= 0)
            glUniform1i(this->uTexDiffuse, 0);
        if (this->uTexNormal >= 0)
            glUniform1i(this->uTexNormal, 1);
        glUniform1i(this->uTexCube, 7);
    }

    void BumpRimCubeShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->uModelViewProjectionMatrix >= 0)
            glUniformMatrix4fv(this->uModelViewProjectionMatrix, 1, 0, engine->worldViewProjMatrix);
        if (this->uModelMatrix >= 0)
            glUniformMatrix3fv(this->uModelMatrix, 1, 0, engine->normalMatrix);
        if (this->uModelMatrixFull >= 0)
            glUniformMatrix4fv(this->uModelMatrixFull, 1, 0, engine->modelMatrixGL);
        if (this->uTexBiasDiffuse >= 0)
            glUniform1f(this->uTexBiasDiffuse, g_rimGlobalA);
        if (this->uTexBiasNormal >= 0)
            glUniform1f(this->uTexBiasNormal, g_rimGlobalB);

        if (this->dirty != 0) {
            glUniform3f(this->uLightDirModel0, engine->lightDir.x,
                        engine->lightDir.y, engine->lightDir.z);
            if (this->uEyePosModel >= 0)
                glUniform3f(this->uEyePosModel, engine->lightColor.x,
                            engine->lightColor.y, engine->lightColor.z);
            if (this->uGlColor >= 0)
                glUniform4fv(this->uGlColor, 1, engine->glColor);
            if (this->uAmbientColor0 >= 0)
                glUniform3fv(this->uAmbientColor0, 1, (float *) &engine->lightAmbientShaded);
            if (this->uDiffuseColor0 >= 0)
                glUniform3fv(this->uDiffuseColor0, 1, (float *) &engine->lightDiffuseShaded);
            if (this->uSpecularPower >= 0)
                glUniform1f(this->uSpecularPower, engine->materialShininess);
            if (this->uIsGlowMat >= 0)
                glUniform3fv(this->uIsGlowMat, 1, (float *) &engine->rimColor);
            int locFogMax = this->uFogMaxDist;
            if (locFogMax >= 0) {
                glUniform3fv(locFogMax, 1, (float *) &engine->fogColor);
            }
            if (this->uEnableFog >= 0)
                glUniform1f(this->uEnableFog, engine->fogMinDist);
            if (this->uFogMinDist >= 0)
                glUniform1f(this->uFogMinDist, engine->fogMaxDist);
            if (this->uLodDist >= 0)
                glUniform1i(this->uLodDist, g_rimByteGlobal);
            if (this->uFogColor >= 0) {
                float v = 0.0f;
                int *m30 = (int *) mesh->field_0x30;
                if (m30 != 0) {
                    v = 1.0f;
                    if (m30[9] == 0)
                        v = 0.0f;
                }
                glUniform1f(this->uFogColor, v);
            }
            if (engine->field_0x32c >= 2) {
                glUniform3fv(this->uDiffuseColor1, 1, (float *) &engine->lightSpecularShaded);
                glUniform3fv(this->uSpecularColor1, 1, (float *) &engine->field_0x308);
                glUniform3fv(this->uRimColor, 1, (float *) &engine->particleAmbient);
                glUniform3f(this->uLightDirModel1, engine->field_0x33c.x,
                            engine->field_0x33c.y, engine->field_0x33c.z);
            } else {
                glUniform3f(this->uDiffuseColor1, 0, 0, 0);
                glUniform3f(this->uSpecularColor1, 0, 0, 0);
                glUniform3f(this->uRimColor, 0, 0, 0);
                glUniform3f(this->uLightDirModel1, engine->field_0x33c.x,
                            engine->field_0x33c.y, engine->field_0x33c.z);
            }
            this->dirty = 0;
        }

        if (this->aPosition >= 0)
            glEnableVertexAttribArray(this->aPosition);
        if (this->aTexCoord >= 0)
            glEnableVertexAttribArray(this->aTexCoord);
        if (this->aNormal >= 0)
            glEnableVertexAttribArray(this->aNormal);
        if (this->aTangent >= 0)
            glEnableVertexAttribArray(this->aTangent);
        if (this->aBitangent >= 0)
            glEnableVertexAttribArray(this->aBitangent);

        if (mesh->uploaded == 0) {
            if (this->aPosition >= 0)
                glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, mesh->positions);
            if (this->aTexCoord >= 0)
                glVertexAttribPointer(this->aTexCoord, 2, 0x1406, 0, 0, mesh->texCoords);
            if (this->aNormal >= 0)
                glVertexAttribPointer(this->aNormal, 3, 0x1406, 0, 0, mesh->normals);
            if (this->aTangent >= 0)
                glVertexAttribPointer(this->aTangent, 3, 0x1406, 0, 0, mesh->tangents);
            if (this->aBitangent >= 0)
                glVertexAttribPointer(this->aBitangent, 3, 0x1406, 0, 0, mesh->binormals);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->aTexCoord, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->aNormal, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->tangentVBO);
            glVertexAttribPointer(this->aTangent, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->binormalVBO);
            glVertexAttribPointer(this->aBitangent, 3, 0x1406, 0, 0, 0);
        }
    }

    void BumpRimCubeShader::SetInActive() {
        int loc;
        loc = this->aPosition;
        if (loc >= 0)
            glDisableVertexAttribArray(loc);
        loc = this->aTexCoord;
        if (loc >= 0)
            glDisableVertexAttribArray(loc);
        loc = this->aNormal;
        if (loc >= 0)
            glDisableVertexAttribArray(loc);
        loc = this->aTangent;
        if (loc >= 0)
            glDisableVertexAttribArray(loc);
        loc = this->aBitangent;
        if (loc >= 0)
            glDisableVertexAttribArray(loc);
    }
}
