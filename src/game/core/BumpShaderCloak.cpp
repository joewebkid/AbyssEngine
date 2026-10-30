#include "game/core/BumpShaderCloak.h"
#include "game/core/EmbeddedBumpShaderCloak.h"
#include <GLES2/gl2.h>
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"

namespace AbyssEngine {
    int BumpShaderCloak::ShaderIndex;

    BumpShaderCloak::BumpShaderCloak() {
        BumpShaderCloak::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("BumpShaderCloak");
        this->name = shaderName;
    }

    void BumpShaderCloak::Init(Engine *engine) {
        int program = this->ES2LoadProgram(EmbeddedBumpShaderCloak::vertex,
                                           EmbeddedBumpShaderCloak::fragment);
        this->program = program;

        this->attrib_a0 = glGetAttribLocation(program, "a_position");
        this->attrib_a1 = glGetAttribLocation(this->program, "a_texCoord");
        this->attrib_a2 = glGetAttribLocation(this->program, "a_normal");
        this->attrib_a3 = glGetAttribLocation(this->program, "a_tangent");
        this->attrib_a4 = glGetAttribLocation(this->program, "a_bitangent");

        this->uniform_u0 = glGetUniformLocation(this->program, "u_ModelViewProjectionMatrix");
        this->uniform_u1 = glGetUniformLocation(this->program, "u_ModelMatrix");
        this->uniform_u2 = glGetUniformLocation(this->program, "u_lightdirmodel[0]");
        this->uniform_u3 = glGetUniformLocation(this->program, "u_lightdirmodel[1]");
        this->uniform_u4 = glGetUniformLocation(this->program, "u_eyeposmodel");
        this->uniform_u5 = glGetUniformLocation(this->program, "s_texture[0]");
        this->uniform_u6 = glGetUniformLocation(this->program, "s_texture[1]");
        this->uniform_u7 = glGetUniformLocation(this->program, "s_texture[2]");
        this->uniform_u8 = glGetUniformLocation(this->program, "glColor");
        this->uniform_u9 = glGetUniformLocation(this->program, "u_AmbientColor[0]");
        this->uniform_u10 = glGetUniformLocation(this->program, "u_DiffuseColor[0]");
        this->uniform_u11 = glGetUniformLocation(this->program, "u_SpecularColor[0]");
        this->uniform_u12 = glGetUniformLocation(this->program, "u_AmbientColor[1]");
        this->uniform_u13 = glGetUniformLocation(this->program, "u_DiffuseColor[1]");
        this->uniform_u14 = glGetUniformLocation(this->program, "u_SpecularColor[1]");
        this->uniform_u15 = glGetUniformLocation(this->program, "u_SpecularPower");
        this->uniform_u16 = glGetUniformLocation(this->program, "u_RimColor");
        this->uniform_u17 = glGetUniformLocation(this->program, "u_texelSize");

        glActiveTexture(0x84c7);
        ((::Engine *) engine)->ActivateRefractFBO();

        this->uniform_u18 = glGetUniformLocation(this->program, "u_AnimValue");
        this->uniform_u19 = glGetUniformLocation(this->program, "u_CloakRate");
        this->uniform_u20 = glGetUniformLocation(this->program, "s_texture_base");

        glUseProgram(this->program);
        glUniform1i(this->uniform_u5, 0);
        glUniform1i(this->uniform_u6, 1);
        glUniform1i(this->uniform_u7, 6);
        glUniform1i(this->uniform_u20, 7);
    }

    void BumpShaderCloak::SetInActive() {
        if (this->attrib_a0 >= 0)
            glDisableVertexAttribArray(this->attrib_a0);
        if (this->attrib_a1 >= 0)
            glDisableVertexAttribArray(this->attrib_a1);
        if (this->attrib_a2 >= 0)
            glDisableVertexAttribArray(this->attrib_a2);
        if (this->attrib_a3 >= 0)
            glDisableVertexAttribArray(this->attrib_a3);
        if (this->attrib_a4 >= 0)
            glDisableVertexAttribArray(this->attrib_a4);
    }

    void BumpShaderCloak::UpdateMeshData(Mesh *mesh, Engine *engine) {
        ::Engine *eng = (::Engine *) engine;

        if (this->uniform_u0 >= 0)
            glUniformMatrix4fv(this->uniform_u0, 1, 0, eng->worldViewProjMatrix);
        if (this->uniform_u1 >= 0)
            glUniformMatrix3fv(this->uniform_u1, 1, 0, eng->normalMatrix);

        if (this->dirty) {
            glUniform3f(this->uniform_u2, eng->lightDir.x, eng->lightDir.y, eng->lightDir.z);
            if (this->uniform_u4 >= 0)
                glUniform3f(this->uniform_u4, eng->lightColor.x, eng->lightColor.y, eng->lightColor.z);
            if (this->uniform_u8 >= 0)
                glUniform4fv(this->uniform_u8, 1, eng->glColor);
            if (this->uniform_u9 >= 0)
                glUniform3fv(this->uniform_u9, 1, (const float *) &eng->lightAmbientShaded);
            if (this->uniform_u10 >= 0)
                glUniform3fv(this->uniform_u10, 1, (const float *) &eng->field_0x2fc);
            if (this->uniform_u11 >= 0)
                glUniform3fv(this->uniform_u11, 1, (const float *) &eng->lightDiffuseShaded);
            if (this->uniform_u15 >= 0)
                glUniform1f(this->uniform_u15, eng->materialShininess);
            if (this->uniform_u16 >= 0)
                glUniform3fv(this->uniform_u16, 1, (const float *) &eng->rimColor);

            int viewportLoc = this->uniform_u17;
            if (viewportLoc >= 0) {
                float invW, invH;
                if (*(int *) (eng->field_0x30[0] + 0x30) == 2) {
                    invW = 1.0f / static_cast<float>(static_cast<int32_t>(eng->GetDisplayWidth()));
                    invH = 1.0f / static_cast<float>(static_cast<int32_t>(eng->GetDisplayHeight()));
                } else {
                    invW = 1.0f / static_cast<float>(static_cast<int32_t>(eng->GetDisplayHeight()));
                    invH = 1.0f / static_cast<float>(static_cast<int32_t>(eng->GetDisplayWidth()));
                }
                glUniform2f(viewportLoc, invW, invH);
            }

            glActiveTexture(0x84c7);
            eng->ActivateRefractFBO();

            if (this->uniform_u18 >= 0)
                glUniform1f(this->uniform_u18, reinterpret_cast<float &>(mesh->materialId));
            if (this->uniform_u19 >= 0)
                glUniform1f(this->uniform_u19, mesh->field_0x20);

            if (eng->field_0x32c >= 2) {
                glUniform3fv(this->uniform_u12, 1, (const float *) &eng->lightSpecularShaded);
                glUniform3fv(this->uniform_u13, 1, (const float *) &eng->field_0x308);
                glUniform3fv(this->uniform_u14, 1, (const float *) &eng->particleAmbient);
                glUniform3f(this->uniform_u3, eng->field_0x33c.x, eng->field_0x33c.y, eng->field_0x33c.z);
            } else {
                glUniform3f(this->uniform_u12, 0.0f, 0.0f, 0.0f);
                glUniform3f(this->uniform_u13, 0.0f, 0.0f, 0.0f);
                glUniform3f(this->uniform_u14, 0.0f, 0.0f, 0.0f);
                glUniform3f(this->uniform_u3, eng->field_0x33c.x, eng->field_0x33c.y, eng->field_0x33c.z);
            }
            this->dirty = 0;
        }

        if (this->attrib_a0 >= 0)
            glEnableVertexAttribArray(this->attrib_a0);
        if (this->attrib_a1 >= 0)
            glEnableVertexAttribArray(this->attrib_a1);
        if (this->attrib_a2 >= 0)
            glEnableVertexAttribArray(this->attrib_a2);
        if (this->attrib_a3 >= 0)
            glEnableVertexAttribArray(this->attrib_a3);
        if (this->attrib_a4 >= 0)
            glEnableVertexAttribArray(this->attrib_a4);

        if (mesh->uploaded == 0) {
            if (this->attrib_a0 >= 0)
                glVertexAttribPointer(this->attrib_a0, 3, 0x1406, 0, 0, mesh->positions);
            if (this->attrib_a1 >= 0)
                glVertexAttribPointer(this->attrib_a1, 2, 0x1406, 0, 0, mesh->texCoords);
            if (this->attrib_a2 >= 0)
                glVertexAttribPointer(this->attrib_a2, 3, 0x1406, 0, 0, mesh->normals);
            if (this->attrib_a3 >= 0)
                glVertexAttribPointer(this->attrib_a3, 3, 0x1406, 0, 0, mesh->tangents);
            if (this->attrib_a4 >= 0)
                glVertexAttribPointer(this->attrib_a4, 3, 0x1406, 0, 0, mesh->binormals);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->attrib_a0, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->attrib_a1, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->attrib_a2, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->tangentVBO);
            glVertexAttribPointer(this->attrib_a3, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->binormalVBO);
            glVertexAttribPointer(this->attrib_a4, 3, 0x1406, 0, 0, 0);
        }
    }
}
