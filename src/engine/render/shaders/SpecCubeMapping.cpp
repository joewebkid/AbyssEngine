#include "engine/render/shaders/SpecCubeMapping.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int SpecCubeMapping::ShaderIndex;

    SpecCubeMapping::SpecCubeMapping() {
        SpecCubeMapping::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"SpecCubeMapping";
    }

    void SpecCubeMapping::Init(Engine *) {
        int program = this->LoadBindShader("SpecCubeMapping.vsh", "SpecCubeMapping.fsh");
        this->program = program;
        if (program == 0) {
            program = this->ES2LoadProgram("SpecCubeMapping.vsh", "SpecCubeMapping.fsh");
            this->program = program;
        }

        this->attribPosition = glGetAttribLocation(program, "a0");
        this->attribNormal = glGetAttribLocation(this->program, "a1");
        this->attribTexCoord = glGetAttribLocation(this->program, "a2");

        this->mvpMatrixLoc = glGetUniformLocation(this->program, "u0");
        this->normalMatrixLoc = glGetUniformLocation(this->program, "u1");
        this->uCameraPosition = glGetUniformLocation(this->program, "u2");
        this->uLightDirection = glGetUniformLocation(this->program, "u3");
        this->samplerLoc0 = glGetUniformLocation(this->program, "u4");
        this->samplerLoc1 = glGetUniformLocation(this->program, "u5");
        this->uParam6 = glGetUniformLocation(this->program, "u6");
        this->uLightAmbient = glGetUniformLocation(this->program, "u7");
        this->uParam8 = glGetUniformLocation(this->program, "u8");
        this->uLightDiffuse = glGetUniformLocation(this->program, "u9");
        this->uShininess = glGetUniformLocation(this->program, "u10");
        this->uColor = glGetUniformLocation(this->program, "u11");

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
