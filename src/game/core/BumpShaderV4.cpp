#include "game/core/BumpShaderV4.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int BumpShaderV4::ShaderIndex;

    BumpShaderV4::BumpShaderV4() {
        ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"BumpShaderV4";
    }

    void BumpShaderV4::Init(Engine *) {
        int program = this->ES2LoadProgram("BumpShaderV4.vsh", "BumpShaderV4.fsh");
        this->program = program;

        this->aPosition = glGetAttribLocation(program, "a0");
        this->aTexCoord = glGetAttribLocation(this->program, "a1");
        this->aNormal = glGetAttribLocation(this->program, "a2");
        this->aTangent = glGetAttribLocation(this->program, "a3");
        this->aBitangent = glGetAttribLocation(this->program, "a4");

        this->uMvpMatrix = glGetUniformLocation(this->program, "u0");
        this->uModelMatrix = glGetUniformLocation(this->program, "u1");
        this->uLightDirModel = glGetUniformLocation(this->program, "u2");
        this->uEyePosModel = glGetUniformLocation(this->program, "u3");
        this->uTexture0 = glGetUniformLocation(this->program, "u4");
        this->uTexture1 = glGetUniformLocation(this->program, "u5");
        this->uColor = glGetUniformLocation(this->program, "u6");
        this->uAmbientColor = glGetUniformLocation(this->program, "u7");
        this->uDiffuseColor = glGetUniformLocation(this->program, "u8");
        this->uSpecularColor = glGetUniformLocation(this->program, "u9");

        glUseProgram(this->program);
        for (int i = 0; i != 2; i++) {
            int loc = (&this->uTexture0)[i];
            if (loc >= 0)
                glUniform1i(loc, i);
        }
    }

    void BumpShaderV4::SetInActive() {
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

    void BumpShaderV4::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->uMvpMatrix >= 0)
            glUniformMatrix4fv(this->uMvpMatrix, 1, 0, engine->worldViewProjMatrix);
        if (this->uModelMatrix >= 0)
            glUniformMatrix3fv(this->uModelMatrix, 1, 0, engine->normalMatrix);

        if (this->dirty != 0) {
            if (this->uLightDirModel >= 0)
                glUniform3f(this->uLightDirModel, engine->lightDir.x,
                            engine->lightDir.y, engine->lightDir.z);
            if (this->uEyePosModel >= 0)
                glUniform3f(this->uEyePosModel, engine->lightColor.x,
                            engine->lightColor.y, engine->lightColor.z);
            if (this->uColor >= 0)
                glUniform4fv(this->uColor, 1, engine->glColor);
            if (this->uAmbientColor >= 0)
                glUniform4fv(this->uAmbientColor, 1, engine->materialAmbient);
            if (this->uDiffuseColor >= 0)
                glUniform4fv(this->uDiffuseColor, 1, engine->materialDiffuse);
            if (this->uSpecularColor >= 0)
                glUniform4fv(this->uSpecularColor, 1, engine->materialSpecular);
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
