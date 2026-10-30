#include "engine/render/shaders/CubeMapping.h"
#include "engine/render/shaders/EmbeddedCubeMappingShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int CubeMapping::ShaderIndex;

    CubeMapping::CubeMapping() {
        CubeMapping::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("CubeMapping");
        this->name = shaderName;
    }

    void CubeMapping::Init(Engine *) {
        this->program = this->ES2LoadProgram(EmbeddedCubeMappingShader::vertex, EmbeddedCubeMappingShader::fragment);

        this->aPosition = glGetAttribLocation(this->program, "a_position");
        this->aNormal = glGetAttribLocation(this->program, "a_normal");
        this->aTexCoord = glGetAttribLocation(this->program, "a_texCoord");

        this->uMvp = glGetUniformLocation(this->program, "u_WorldMatrix");
        this->uNormalMatrix = glGetUniformLocation(this->program, "u_ModelMatrix");
        this->uniform2 = glGetUniformLocation(this->program, "u_eyeposmodel");
        this->uniform3 = glGetUniformLocation(this->program, "u_lightposmodel");
        this->uniform4 = glGetUniformLocation(this->program, "u_texture_base");
        this->uniform5 = glGetUniformLocation(this->program, "u_texture_cubemap");
        this->uniform6 = glGetUniformLocation(this->program, "u_envValue");
        this->uniform7 = glGetUniformLocation(this->program, "u_AmbientColor");
        this->uniform8 = glGetUniformLocation(this->program, "u_DiffuseColor");
        this->uniform9 = glGetUniformLocation(this->program, "u_SpecularColor");
        this->uniform10 = glGetUniformLocation(this->program, "u_SpecularPower");
        this->uniform11 = glGetUniformLocation(this->program, "u_glColor");

        glUseProgram(this->program);
        glUniform1i(this->uniform4, 0);
        glUniform1i(this->uniform5, 1);
    }

    void CubeMapping::SetInActive() {
        glDisableVertexAttribArray(this->aPosition);
        glDisableVertexAttribArray(this->aNormal);
        glDisableVertexAttribArray(this->aTexCoord);
    }

    void CubeMapping::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->dirty != 0) {
            glUniform4fv(this->uniform11, 1, engine->glColor);
            glUniform4fv(this->uniform7, 1, (const float *) &engine->lightAmbientShaded);
            glUniform4fv(this->uniform8, 1, (const float *) &engine->field_0x2fc);
            glUniform4fv(this->uniform9, 1, (const float *) &engine->lightDiffuseShaded);
            glUniform1f(this->uniform10, engine->materialShininess);
            this->dirty = 0;
        }

        glUniform1f(this->uniform6, engine->field_0xcc);
        glUniformMatrix4fv(this->uMvp, 1, 0, engine->worldViewProjMatrix);
        glUniformMatrix3fv(this->uNormalMatrix, 1, 0, engine->normalMatrix);
        glUniform4f(this->uniform3, engine->lightDir.x, engine->lightDir.y,
                    engine->lightDir.z, engine->lightDirty[0]);
        glUniform3f(this->uniform2, engine->lightColor.x, engine->lightColor.y,
                    engine->lightColor.z);

        glEnableVertexAttribArray(this->aPosition);
        glEnableVertexAttribArray(this->aTexCoord);
        glEnableVertexAttribArray(this->aNormal);

        if (mesh->uploaded == 0) {
            glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, mesh->positions);
            if ((mesh->vertexFormat & 2) != 0)
                glVertexAttribPointer(this->aTexCoord, 2, 0x1406, 0, 0, mesh->texCoords);
            if ((mesh->vertexFormat & 4) != 0)
                glVertexAttribPointer(this->aNormal, 3, 0x1406, 0, 0, mesh->normals);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->aTexCoord, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->aNormal, 3, 0x1406, 0, 0, 0);
        }
    }
}
