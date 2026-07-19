#include "game/core/BumpShaderV2.h"
#include <GLES2/gl2.h>
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"

namespace AbyssEngine {
    int BumpShaderV2::ShaderIndex;

    BumpShaderV2::BumpShaderV2() {
        BumpShaderV2::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"BumpShaderV2";
    }

    void BumpShaderV2::Init(Engine *) {
        this->program = this->ES2LoadProgram("BumpShaderV2.vsh", "BumpShaderV2.fsh");

        this->aPosition = glGetAttribLocation(this->program, "a_position");
        this->aNormal = glGetAttribLocation(this->program, "a_normal");
        this->aTexCoord = glGetAttribLocation(this->program, "a_texCoord");
        this->aTangent = glGetAttribLocation(this->program, "a_tangent");
        this->aBinormal = glGetAttribLocation(this->program, "a_binormal");

        this->uMvpMatrix = glGetUniformLocation(this->program, "u_mvpMatrix");
        this->uNormalMatrix = glGetUniformLocation(this->program, "u_normalMatrix");
        this->uLightDir = glGetUniformLocation(this->program, "u_lightDir");
        this->uEyePos = glGetUniformLocation(this->program, "u_eyePos");
        this->uTexture0 = glGetUniformLocation(this->program, "u_texture0");
        this->uTexture1 = glGetUniformLocation(this->program, "u_texture1");
        this->uAmbient = glGetUniformLocation(this->program, "u_ambient");
        this->uDiffuse = glGetUniformLocation(this->program, "u_diffuse");
        this->uSpecular = glGetUniformLocation(this->program, "u_specular");
        this->uEmissive = glGetUniformLocation(this->program, "u_emissive");

        glUseProgram(this->program);
        for (int i = 0; i != 2; i++) {
            int loc = (&this->uTexture0)[i];
            if (loc >= 0)
                glUniform1i(loc, i);
        }
    }

    void BumpShaderV2::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->uMvpMatrix >= 0)
            glUniformMatrix4fv(this->uMvpMatrix, 1, 0, engine->worldViewProjMatrix);
        if (this->uNormalMatrix >= 0)
            glUniformMatrix3fv(this->uNormalMatrix, 1, 0, engine->normalMatrix);

        if (this->dirty) {
            if (this->uLightDir >= 0)
                glUniform3f(this->uLightDir, engine->lightDir.x,
                            engine->lightDir.y, engine->lightDir.z);
            if (this->uEyePos >= 0)
                glUniform3f(this->uEyePos, engine->lightColor.x,
                            engine->lightColor.y, engine->lightColor.z);
            if (this->uAmbient >= 0)
                glUniform4fv(this->uAmbient, 1, engine->glColor);
            if (this->uDiffuse >= 0)
                glUniform4fv(this->uDiffuse, 1, engine->materialAmbient);
            if (this->uSpecular >= 0)
                glUniform4fv(this->uSpecular, 1, engine->materialDiffuse);
            if (this->uEmissive >= 0)
                glUniform4fv(this->uEmissive, 1, engine->materialSpecular);
            this->dirty = 0;
        }

        if (this->aPosition >= 0)
            glEnableVertexAttribArray(this->aPosition);
        if (this->aNormal >= 0)
            glEnableVertexAttribArray(this->aNormal);
        if (this->aTexCoord >= 0)
            glEnableVertexAttribArray(this->aTexCoord);
        if (this->aTangent >= 0)
            glEnableVertexAttribArray(this->aTangent);
        if (this->aBinormal >= 0)
            glEnableVertexAttribArray(this->aBinormal);

        if (mesh->uploaded == 0) {
            if (this->aPosition >= 0)
                glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, mesh->positions);
            if (this->aNormal >= 0)
                glVertexAttribPointer(this->aNormal, 2, 0x1406, 0, 0, mesh->texCoords);
            if (this->aTexCoord >= 0)
                glVertexAttribPointer(this->aTexCoord, 3, 0x1406, 0, 0, mesh->normals);
            if (this->aTangent >= 0)
                glVertexAttribPointer(this->aTangent, 3, 0x1406, 0, 0, mesh->tangents);
            if (this->aBinormal >= 0)
                glVertexAttribPointer(this->aBinormal, 3, 0x1406, 0, 0, mesh->binormals);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->aNormal, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->aTexCoord, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->tangentVBO);
            glVertexAttribPointer(this->aTangent, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->binormalVBO);
            glVertexAttribPointer(this->aBinormal, 3, 0x1406, 0, 0, 0);
        }
    }

    void BumpShaderV2::SetInActive() {
        if (this->aPosition >= 0)
            glDisableVertexAttribArray(this->aPosition);
        if (this->aNormal >= 0)
            glDisableVertexAttribArray(this->aNormal);
        if (this->aTexCoord >= 0)
            glDisableVertexAttribArray(this->aTexCoord);
        if (this->aTangent >= 0)
            glDisableVertexAttribArray(this->aTangent);
        if (this->aBinormal >= 0)
            glDisableVertexAttribArray(this->aBinormal);
    }
}
