#include "engine/render/shaders/SpecCubeAlphaMapping.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int SpecCubeAlphaMapping::ShaderIndex;

    SpecCubeAlphaMapping::SpecCubeAlphaMapping() {
        SpecCubeAlphaMapping::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"SpecCubeAlphaMapping";
    }

    void SpecCubeAlphaMapping::Init(Engine *) {
        this->program = this->LoadBindShader("SpecCubeAlphaMapping.vsh", "SpecCubeAlphaMapping.fsh");
        if (this->program == 0) {
            this->program = this->ES2LoadProgram("SpecCubeAlphaMapping.vsh", "SpecCubeAlphaMapping2.fsh");
        }

        this->attrA0 = glGetAttribLocation(this->program, "a0");
        this->attrA1 = glGetAttribLocation(this->program, "a1");
        this->attrA2 = glGetAttribLocation(this->program, "a2");

        this->uniU0 = glGetUniformLocation(this->program, "u0");
        this->uniU1 = glGetUniformLocation(this->program, "u1");
        this->uniU2 = glGetUniformLocation(this->program, "u2");
        this->uniU3 = glGetUniformLocation(this->program, "u3");
        this->uniU4 = glGetUniformLocation(this->program, "u4");
        this->uniU5 = glGetUniformLocation(this->program, "u5");
        this->uniU6 = glGetUniformLocation(this->program, "u6");
        this->uniU7 = glGetUniformLocation(this->program, "u7");
        this->uniU8 = glGetUniformLocation(this->program, "u8");
        this->uniU9 = glGetUniformLocation(this->program, "u9");
        this->uniU10 = glGetUniformLocation(this->program, "u10");
        this->uniU11 = glGetUniformLocation(this->program, "u11");

        glUseProgram(this->program);
        glUniform1i(this->uniU4, 0);
        glUniform1i(this->uniU5, 1);
    }

    void SpecCubeAlphaMapping::SetInActive() {
        glDisableVertexAttribArray(this->attrA0);
        glDisableVertexAttribArray(this->attrA1);
        glDisableVertexAttribArray(this->attrA2);
    }

    void SpecCubeAlphaMapping::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->dirty != 0) {
            float envFactor = 1.0f;
            char *mat = (char *) mesh->field_0x30;
            if (mat != 0 && *(void **) (mat + 0x24) != 0 && *(int *) (mat + 0x28) == 4) {
                envFactor = **(float **) (mat + 0x24);
            }
            glUniform1f(this->uniU11, envFactor);
            glUniform4fv(this->uniU10, 1, engine->glColor);
            glUniform3fv(this->uniU6, 1, (const float *) &engine->lightAmbientShaded);
            glUniform3fv(this->uniU7, 1, (const float *) &engine->field_0x2fc);
            glUniform3fv(this->uniU8, 1, (const float *) &engine->lightDiffuseShaded);
            glUniform1f(this->uniU9, engine->materialShininess);
            this->dirty = 0;
        }

        glUniformMatrix4fv(this->uniU0, 1, 0, engine->worldViewProjMatrix);
        glUniformMatrix3fv(this->uniU1, 1, 0, engine->normalMatrix);
        glUniform4f(this->uniU3, engine->lightDir.x, engine->lightDir.y,
                    engine->lightDir.z, engine->lightDirty[0]);
        glUniform3f(this->uniU2, engine->lightColor.x, engine->lightColor.y,
                    engine->lightColor.z);

        glEnableVertexAttribArray(this->attrA0);
        glEnableVertexAttribArray(this->attrA2);
        glEnableVertexAttribArray(this->attrA1);

        if (mesh->uploaded != 0) {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->attrA0, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->attrA2, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->attrA1, 3, 0x1406, 0, 0, 0);
        } else {
            glVertexAttribPointer(this->attrA0, 3, 0x1406, 0, 0, mesh->positions);
            uint8_t flags = mesh->vertexFormat;
            if ((flags & 2) != 0) {
                glVertexAttribPointer(this->attrA2, 2, 0x1406, 0, 0, mesh->texCoords);
                flags = mesh->vertexFormat;
            }
            if (((unsigned) flags << 0x1d) & 0x80000000u) {
                glVertexAttribPointer(this->attrA1, 3, 0x1406, 0, 0, mesh->normals);
            }
        }
    }
}
