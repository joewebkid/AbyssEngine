#include "engine/render/shaders/GenericShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

static void *GenericShader_registerSrc = nullptr;
static void **GenericShader_registerDst = nullptr;

namespace AbyssEngine {
    int GenericShader::ShaderIndex;

    void GenericShader::Init(Engine *) {
        this->program = this->ES2LoadProgram("GenericShader.vsh", "GenericShader.fsh");

        this->aPosition = glGetAttribLocation(this->program, "a0");
        this->aTexCoord = glGetAttribLocation(this->program, "a1");
        this->aNormal = glGetAttribLocation(this->program, "a2");
        this->aTangent = glGetAttribLocation(this->program, "a3");
        this->aBitangent = glGetAttribLocation(this->program, "a4");

        this->uMvpMatrix = glGetUniformLocation(this->program, "u0");
        this->uNormalMatrix = glGetUniformLocation(this->program, "u1");
        this->uLightPosition = glGetUniformLocation(this->program, "u2");
        this->uEyePosition = glGetUniformLocation(this->program, "u3");
        this->uTexture0 = glGetUniformLocation(this->program, "u4");
        this->uTexture1 = glGetUniformLocation(this->program, "u5");
        this->uTexture2 = glGetUniformLocation(this->program, "u6");
        this->uAmbientColor = glGetUniformLocation(this->program, "u7");
        this->uDiffuseColor = glGetUniformLocation(this->program, "u8");
        this->uSpecularColor = glGetUniformLocation(this->program, "u9");
        this->uEmissiveColor = glGetUniformLocation(this->program, "u10");
        this->uMaterialShininess = glGetUniformLocation(this->program, "u11");

        glUseProgram(this->program);

        int *samplers = &this->uTexture0;
        for (int i = 0; i != 3; i++) {
            if (samplers[i] >= 0)
                glUniform1i(samplers[i], i);
        }
    }

    void GenericShader::SetInActive() {
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

    void GenericShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->uMvpMatrix >= 0)
            glUniformMatrix4fv(this->uMvpMatrix, 1, 0, engine->worldViewProjMatrix);
        if (this->uNormalMatrix >= 0)
            glUniformMatrix3fv(this->uNormalMatrix, 1, 0, engine->normalMatrix);

        if (this->dirty != 0) {
            if (this->uLightPosition >= 0)
                glUniform3f(this->uLightPosition, engine->lightDir.x, engine->lightDir.y, engine->lightDir.z);
            if (this->uEyePosition >= 0)
                glUniform3f(this->uEyePosition, engine->lightColor.x, engine->lightColor.y, engine->lightColor.z);
            if (this->uAmbientColor >= 0)
                glUniform4fv(this->uAmbientColor, 1, engine->glColor);
            if (this->uDiffuseColor >= 0)
                glUniform3fv(this->uDiffuseColor, 1, (const float *) &engine->lightAmbientShaded);
            if (this->uSpecularColor >= 0)
                glUniform3fv(this->uSpecularColor, 1, (const float *) &engine->field_0x2fc);
            if (this->uEmissiveColor >= 0)
                glUniform3fv(this->uEmissiveColor, 1, (const float *) &engine->lightDiffuseShaded);
            if (this->uMaterialShininess >= 0)
                glUniform1f(this->uMaterialShininess, engine->materialShininess);
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

        if (mesh->uploaded != 0) {
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
            return;
        }

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
    }

    GenericShader::GenericShader() {
        GenericShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        *GenericShader_registerDst = GenericShader_registerSrc;
        this->name = u"GenericShader";
    }
}
