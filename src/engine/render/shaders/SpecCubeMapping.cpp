#include "engine/render/shaders/SpecCubeMapping.h"
#include "engine/render/shaders/EmbeddedSpecCubeShaders.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int SpecCubeMapping::ShaderIndex;

    SpecCubeMapping::SpecCubeMapping() {
        SpecCubeMapping::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("SpecCubeMapping");
        this->name = shaderName;
    }

    void SpecCubeMapping::Init(Engine *) {
        int program = this->LoadBindShader("data/shader/SpecCubeMapping.vs",
                                            "data/shader/SpecCubeMapping.fs");
        this->program = program;
        if (program == 0) {
            program = this->ES2LoadProgram(EmbeddedSpecCubeMapping::vertex,
                                           EmbeddedSpecCubeMapping::fragment);
            this->program = program;
        }

        this->attribPosition = glGetAttribLocation(program, "a_position");
        this->attribNormal = glGetAttribLocation(this->program, "a_normal");
        this->attribTexCoord = glGetAttribLocation(this->program, "a_texCoord");

        this->mvpMatrixLoc = glGetUniformLocation(this->program, "u_WorldMatrix");
        this->normalMatrixLoc = glGetUniformLocation(this->program, "u_ModelMatrix");
        this->uCameraPosition = glGetUniformLocation(this->program, "u_eyeposmodel");
        this->uLightDirection = glGetUniformLocation(this->program, "u_lightposmodel");
        this->samplerLoc0 = glGetUniformLocation(this->program, "u_texture_base");
        this->samplerLoc1 = glGetUniformLocation(this->program, "u_texture_cubemap");
        this->uParam6 = glGetUniformLocation(this->program, "u_envValue");
        this->uLightAmbient = glGetUniformLocation(this->program, "u_AmbientColor");
        this->uParam8 = glGetUniformLocation(this->program, "u_DiffuseColor");
        this->uLightDiffuse = glGetUniformLocation(this->program, "u_SpecularColor");
        this->uShininess = glGetUniformLocation(this->program, "u_SpecularPower");
        this->uColor = glGetUniformLocation(this->program, "u_glColor");

        glUseProgram(this->program);
        glUniform1i(this->samplerLoc0, 0);
        glUniform1i(this->samplerLoc1, 1);
    }

    void SpecCubeMapping::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->dirty != 0) {
            glUniform4fv(this->uColor, 1, engine->glColor);
            glUniform3fv(this->uLightAmbient, 1, (const float *) &engine->lightAmbientShaded);
            glUniform3fv(this->uParam8, 1, (const float *) &engine->field_0x2fc);
            glUniform3fv(this->uLightDiffuse, 1, (const float *) &engine->lightDiffuseShaded);
            glUniform1f(this->uShininess, engine->materialShininess);
            this->dirty = 0;
        }

        glUniform1f(this->uParam6, engine->field_0xcc);
        glUniformMatrix4fv(this->mvpMatrixLoc, 1, 0, engine->worldViewProjMatrix);
        glUniformMatrix3fv(this->normalMatrixLoc, 1, 0, engine->normalMatrix);
        glUniform4f(this->uLightDirection,
                    engine->lightDir.x, engine->lightDir.y,
                    engine->lightDir.z, engine->lightDirty[0]);
        glUniform3f(this->uCameraPosition,
                    engine->lightColor.x, engine->lightColor.y, engine->lightColor.z);

        glEnableVertexAttribArray(this->attribPosition);
        glEnableVertexAttribArray(this->attribTexCoord);
        glEnableVertexAttribArray(this->attribNormal);

        if (mesh->uploaded == 0) {
            glVertexAttribPointer(this->attribPosition, 3, 0x1406, 0, 0, mesh->positions);
            uint8_t flags = mesh->vertexFormat;
            if (flags & 2) {
                glVertexAttribPointer(this->attribTexCoord, 2, 0x1406, 0, 0, mesh->texCoords);
                flags = mesh->vertexFormat;
            }
            if (((uint32_t) flags << 0x1d) & 0x80000000u) {
                glVertexAttribPointer(this->attribNormal, 3, 0x1406, 0, 0, mesh->normals);
            }
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->attribPosition, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->attribTexCoord, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->attribNormal, 3, 0x1406, 0, 0, 0);
        }
    }

    void SpecCubeMapping::SetInActive() {
        glDisableVertexAttribArray(this->attribPosition);
        glDisableVertexAttribArray(this->attribNormal);
        glDisableVertexAttribArray(this->attribTexCoord);
    }
}
