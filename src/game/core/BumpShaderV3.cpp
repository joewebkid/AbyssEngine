#include "game/core/BumpShaderV3.h"
#include "engine/render/Engine.h"
#include "engine/render/Material.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace {
    float *g_bsv3_floatA;
    float *g_bsv3_floatB;
}

namespace AbyssEngine {
    int BumpShaderV3::ShaderIndex;

    BumpShaderV3::BumpShaderV3() {
        ShaderBaseStruct::shaderIndexIntern = ShaderIndex;
        this->name = u"BumpShaderV3";
    }

    void BumpShaderV3::Init(Engine *) {
        this->program = this->ES2LoadProgram("BumpShaderV3.vsh", "BumpShaderV3.fsh");

        this->aPosition = glGetAttribLocation(this->program, "a0");
        this->aTexCoord = glGetAttribLocation(this->program, "a1");
        this->aNormal = glGetAttribLocation(this->program, "a2");
        this->aTangent = glGetAttribLocation(this->program, "a3");
        this->aBitangent = glGetAttribLocation(this->program, "a4");

        this->uModelViewProjectionMatrix = glGetUniformLocation(this->program, "u0");
        this->uModelMatrix = glGetUniformLocation(this->program, "u1");
        this->uLightDirModel0 = glGetUniformLocation(this->program, "u2");
        this->uLightDirModel1 = glGetUniformLocation(this->program, "u3");
        this->uEyePosModel = glGetUniformLocation(this->program, "u4");
        this->uTexDiffuse = glGetUniformLocation(this->program, "u5");
        this->uTexNormal = glGetUniformLocation(this->program, "u6");
        this->uTexSpecular = glGetUniformLocation(this->program, "u7");
        this->uGlColor = glGetUniformLocation(this->program, "u8");
        this->uAmbientColor0 = glGetUniformLocation(this->program, "u9");
        this->uAmbientColor1 = glGetUniformLocation(this->program, "u10");
        this->uDiffuseColor1 = glGetUniformLocation(this->program, "u11");
        this->uDiffuseColor0 = glGetUniformLocation(this->program, "u12");
        this->uSpecularColor0 = glGetUniformLocation(this->program, "u13");
        this->uSpecularColor1 = glGetUniformLocation(this->program, "u14");
        this->uSpecularPower = glGetUniformLocation(this->program, "u15");
        this->uRimColor = glGetUniformLocation(this->program, "u16");
        this->uTexBiasDiffuse = glGetUniformLocation(this->program, "u17");
        this->uTexBiasNormal = glGetUniformLocation(this->program, "u18");
        this->uIsGlowMat = glGetUniformLocation(this->program, "u19");

        glUseProgram(this->program);

        int textureUniforms[3] = {this->uTexDiffuse, this->uTexNormal, this->uTexSpecular};
        for (int i = 0; i != 3; i++) {
            int loc = textureUniforms[i];
            if (loc >= 0)
                glUniform1i(loc, i);
        }
    }

    void BumpShaderV3::SetInActive() {
        if (this->aPosition >= 0)
            glDisableVertexAttribArray(this->aPosition);
        if (this->aTexCoord >= 0)
            glDisableVertexAttribArray(this->aTexCoord);
        if (this->aNormal >= 0)
            glDisableVertexAttribArray(this->aNormal);
        if (this->aTangent >= 0)
            glDisableVertexAttribArray(this->aTangent);
        if (this->aBitangent >= 0)
            glDisableVertexAttribArray(this->aBitangent);
    }

    void BumpShaderV3::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->uModelViewProjectionMatrix >= 0)
            glUniformMatrix4fv(this->uModelViewProjectionMatrix, 1, 0, engine->worldViewProjMatrix);
        if (this->uModelMatrix >= 0)
            glUniformMatrix3fv(this->uModelMatrix, 1, 0, engine->normalMatrix);
        if (this->uTexBiasDiffuse >= 0)
            glUniform1f(this->uTexBiasDiffuse, *g_bsv3_floatA);
        if (this->uTexBiasNormal >= 0)
            glUniform1f(this->uTexBiasNormal, *g_bsv3_floatB);

        if (this->dirty != 0) {
            glUniform3f(this->uLightDirModel0, engine->lightDir.x, engine->lightDir.y,
                        engine->lightDir.z);
            if (this->uEyePosModel >= 0)
                glUniform3f(this->uEyePosModel, engine->lightColor.x, engine->lightColor.y,
                            engine->lightColor.z);
            if (this->uGlColor >= 0)
                glUniform4fv(this->uGlColor, 1, engine->glColor);
            if (this->uAmbientColor0 >= 0)
                glUniform3fv(this->uAmbientColor0, 1, (float *) &engine->lightAmbientShaded);
            if (this->uAmbientColor1 >= 0)
                glUniform3fv(this->uAmbientColor1, 1, (float *) &engine->field_0x2fc);
            if (this->uDiffuseColor1 >= 0)
                glUniform3fv(this->uDiffuseColor1, 1, (float *) &engine->lightDiffuseShaded);
            if (this->uSpecularPower >= 0)
                glUniform1f(this->uSpecularPower, engine->materialShininess);
            if (this->uRimColor >= 0)
                glUniform3fv(this->uRimColor, 1, (float *) &engine->rimColor);
            if (this->uIsGlowMat >= 0) {
                float w = 0.85f;
                if (static_cast<Material *>(mesh->material)->addData != nullptr)
                    w = 1.0f;
                glUniform1f(this->uIsGlowMat, w);
            }
            if (engine->field_0x32c >= 2) {
                glUniform3fv(this->uDiffuseColor0, 1, (float *) &engine->lightSpecularShaded);
                glUniform3fv(this->uSpecularColor0, 1, (float *) &engine->field_0x308);
                glUniform3fv(this->uSpecularColor1, 1, (float *) &engine->particleAmbient);
                glUniform3f(this->uLightDirModel1, engine->field_0x33c.x, engine->field_0x33c.y,
                            engine->field_0x33c.z);
            } else {
                glUniform3f(this->uDiffuseColor0, 0, 0, 0);
                glUniform3f(this->uSpecularColor0, 0, 0, 0);
                glUniform3f(this->uSpecularColor1, 0, 0, 0);
                glUniform3f(this->uLightDirModel1, engine->field_0x33c.x, engine->field_0x33c.y,
                            engine->field_0x33c.z);
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
}
