#include "engine/render/shaders/BumpRimCubeShader.h"
#include "engine/render/shaders/EmbeddedBumpRimCubeShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int BumpRimCubeShader::ShaderIndex;

    BumpRimCubeShader::BumpRimCubeShader() {
        ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("BumpRimCubeShader");
        this->name = shaderName;
    }

    void BumpRimCubeShader::Init(Engine *) {
        int program = this->LoadBindShader("data/shader/BumpRimCubeShader.vs", "data/shader/BumpRimCubeShader.fs");
        this->program = program;
        if (program == 0) {
            program = this->ES2LoadProgram(EmbeddedBumpRimCubeShader::vertex,
                                           EmbeddedBumpRimCubeShader::fragment);
            this->program = program;
        }

        this->aPosition = glGetAttribLocation(program, "a_position");
        this->aTexCoord = glGetAttribLocation(this->program, "a_texCoord");
        this->aNormal = glGetAttribLocation(this->program, "a_normal");
        this->aTangent = glGetAttribLocation(this->program, "a_tangent");

        this->aBitangent = glGetUniformLocation(this->program, "a_bitangent");
        this->uModelViewProjectionMatrix = glGetUniformLocation(this->program, "u_ModelViewProjectionMatrix");
        this->uModelMatrix = glGetUniformLocation(this->program, "u_ModelMatrix");
        this->uModelMatrixFull = glGetUniformLocation(this->program, "u_ModelMatrixFull");
        this->uLightDirModel0 = glGetUniformLocation(this->program, "u_lightdirmodel[0]");
        this->uLightDirModel1 = glGetUniformLocation(this->program, "u_lightdirmodel[1]");
        this->uEyePosModel = glGetUniformLocation(this->program, "u_eyeposmodel");
        this->uTexDiffuse = glGetUniformLocation(this->program, "s_texture[0]");
        this->uTexNormal = glGetUniformLocation(this->program, "s_texture[1]");
        this->uTexCube = glGetUniformLocation(this->program, "u_texture_cubemap");
        this->uGlColor = glGetUniformLocation(this->program, "glColor");
        this->uAmbientColor0 = glGetUniformLocation(this->program, "u_AmbientColor[0]");
        this->uDiffuseColor0 = glGetUniformLocation(this->program, "u_DiffuseColor[0]");
        this->uSpecularColor0 = glGetUniformLocation(this->program, "u_SpecularColor[0]");
        this->uAmbientColor1 = glGetUniformLocation(this->program, "u_AmbientColor[1]");
        this->uDiffuseColor1 = glGetUniformLocation(this->program, "u_DiffuseColor[1]");
        this->uSpecularColor1 = glGetUniformLocation(this->program, "u_SpecularColor[1]");
        this->uSpecularPower = glGetUniformLocation(this->program, "u_SpecularPower");
        this->uRimColor = glGetUniformLocation(this->program, "u_RimColor");
        this->uTexBiasDiffuse = glGetUniformLocation(this->program, "u_TexBiasDiffuse");
        this->uTexBiasNormal = glGetUniformLocation(this->program, "u_TexBiasNormal");
        this->uIsGlowMat = glGetUniformLocation(this->program, "u_IsGlowMat");
        this->uFogColor = glGetUniformLocation(this->program, "u_fogColor");
        this->uFogMaxDist = glGetUniformLocation(this->program, "u_fogMaxDist");
        this->uFogMinDist = glGetUniformLocation(this->program, "u_fogMinDist");
        this->uEnableFog = glGetUniformLocation(this->program, "u_EnableFog");

        glUseProgram(this->program);
        for (int i = 0; i != 2; ++i) {
            int location = (&this->uTexDiffuse)[i];
            if (location >= 0)
                glUniform1i(location, i);
        }
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
            glUniform1f(this->uTexBiasDiffuse, Engine::lodBiasDiffuse);
        if (this->uTexBiasNormal >= 0)
            glUniform1f(this->uTexBiasNormal, Engine::lodBiasNormal);

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
                glUniform3fv(this->uDiffuseColor0, 1, (float *) &engine->field_0x2fc);
            if (this->uSpecularColor0 >= 0)
                glUniform3fv(this->uSpecularColor0, 1, (float *) &engine->lightDiffuseShaded);
            if (this->uSpecularPower >= 0)
                glUniform1f(this->uSpecularPower, engine->materialShininess);
            if (this->uRimColor >= 0)
                glUniform3fv(this->uRimColor, 1, (float *) &engine->rimColor);
            int locFogMax = this->uFogColor;
            if (locFogMax >= 0) {
                glUniform3fv(locFogMax, 1, engine->fogColor);
            }
            if (this->uFogMinDist >= 0)
                glUniform1f(this->uFogMinDist, engine->fogMinDist);
            if (this->uFogMaxDist >= 0)
                glUniform1f(this->uFogMaxDist, engine->fogMaxDist);
            if (this->uEnableFog >= 0)
                glUniform1i(this->uEnableFog, Engine::fogEnabled);
            if (this->uIsGlowMat >= 0) {
                float v = 0.0f;
                int *m30 = (int *) mesh->field_0x30;
                if (m30 != 0) {
                    v = 1.0f;
                    if (m30[9] == 0)
                        v = 0.0f;
                }
                glUniform1f(this->uIsGlowMat, v);
            }
            if (engine->field_0x32c >= 2) {
                glUniform3fv(this->uAmbientColor1, 1, (float *) &engine->lightSpecularShaded);
                glUniform3fv(this->uDiffuseColor1, 1, (float *) &engine->field_0x308);
                glUniform3fv(this->uSpecularColor1, 1, (float *) &engine->particleAmbient);
                glUniform3f(this->uLightDirModel1, engine->field_0x33c.x,
                            engine->field_0x33c.y, engine->field_0x33c.z);
            } else {
                glUniform3f(this->uAmbientColor1, 0, 0, 0);
                glUniform3f(this->uDiffuseColor1, 0, 0, 0);
                glUniform3f(this->uSpecularColor1, 0, 0, 0);
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
