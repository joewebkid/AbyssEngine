#include "engine/render/shaders/CubeNormalMapping.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int CubeNormalMapping::ShaderIndex;

    CubeNormalMapping::CubeNormalMapping() {
        CubeNormalMapping::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String tmp;
        tmp = u"CubeNormalMapping";
        this->name = tmp;
    }

    void CubeNormalMapping::Init(Engine *) {
        int program = this->ES2LoadProgram("CubeNormalMapping.vsh", "CubeNormalMapping.fsh");
        this->program = program;

        this->attribA0 = glGetAttribLocation(program, "a0");
        this->attribA1 = glGetAttribLocation(this->program, "a1");
        this->attribA2 = glGetAttribLocation(this->program, "a2");
        this->attribA3 = glGetAttribLocation(this->program, "a3");
        this->attribA4 = glGetAttribLocation(this->program, "a4");

        this->uniformU0 = glGetUniformLocation(this->program, "u0");
        this->uniformU1 = glGetUniformLocation(this->program, "u1");
        this->uniformU2 = glGetUniformLocation(this->program, "u2");
        this->uniformU3 = glGetUniformLocation(this->program, "u3");
        this->uniformU4 = glGetUniformLocation(this->program, "u4");
        this->uniformU5 = glGetUniformLocation(this->program, "u5");
        this->uniformU6 = glGetUniformLocation(this->program, "u6");
        this->uniformU7 = glGetUniformLocation(this->program, "u7");
        this->uniformU8 = glGetUniformLocation(this->program, "u8");
        this->uniformU9 = glGetUniformLocation(this->program, "u9");
        this->uniformU10 = glGetUniformLocation(this->program, "u10");
        this->uniformU11 = glGetUniformLocation(this->program, "u11");
        this->uniformU12 = glGetUniformLocation(this->program, "u12");

        glUseProgram(this->program);
        glUniform1i(this->uniformU4, 0);
        glUniform1i(this->uniformU6, 1);
        glUniform1i(this->uniformU5, 2);
    }

    void CubeNormalMapping::UpdateMeshData(Mesh *meshArg, Engine *engine) {
        Mesh *mesh = meshArg;
        if (this->dirty != 0) {
            glUniform4fv(this->uniformU12, 1, engine->glColor);
            glUniform4fv(this->uniformU8, 1, (float *) &engine->lightAmbientShaded);
            glUniform4fv(this->uniformU9, 1, (float *) &engine->field_0x2fc);
            glUniform4fv(this->uniformU10, 1, (float *) &engine->lightDiffuseShaded);
            glUniform1f(this->uniformU11, engine->materialShininess);
            this->dirty = 0;
        }

        glUniform1f(this->uniformU7, engine->field_0xcc);
        glUniformMatrix4fv(this->uniformU0, 1, 0, engine->worldViewProjMatrix);
        glUniformMatrix3fv(this->uniformU1, 1, 0, engine->normalMatrix);
        glUniform3f(this->uniformU3, engine->lightDir.x, engine->lightDir.y,
                    engine->lightDir.z);
        glUniform3f(this->uniformU2, engine->lightColor.x, engine->lightColor.y,
                    engine->lightColor.z);

        glEnableVertexAttribArray(this->attribA0);
        glEnableVertexAttribArray(this->attribA2);
        glEnableVertexAttribArray(this->attribA1);
        glEnableVertexAttribArray(this->attribA3);
        glEnableVertexAttribArray(this->attribA4);

        if (mesh->uploaded == 0) {
            glVertexAttribPointer(this->attribA0, 3, 0x1406, 0, 0, mesh->positions);
            if ((mesh->vertexFormat & 2) != 0)
                glVertexAttribPointer(this->attribA2, 2, 0x1406, 0, 0, mesh->texCoords);
            if ((mesh->vertexFormat & 4) != 0)
                glVertexAttribPointer(this->attribA1, 3, 0x1406, 0, 0, mesh->normals);
            if (this->attribA3 >= 0)
                glVertexAttribPointer(this->attribA3, 3, 0x1406, 0, 0, mesh->tangents);
            if (this->attribA4 >= 0)
                glVertexAttribPointer(this->attribA4, 3, 0x1406, 0, 0, mesh->binormals);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->attribA0, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->attribA2, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->attribA1, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->tangentVBO);
            glVertexAttribPointer(this->attribA3, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->binormalVBO);
            glVertexAttribPointer(this->attribA4, 3, 0x1406, 0, 0, 0);
        }
    }

    void CubeNormalMapping::SetInActive() {
        glDisableVertexAttribArray(this->attribA0);
        glDisableVertexAttribArray(this->attribA1);
        glDisableVertexAttribArray(this->attribA2);
        glDisableVertexAttribArray(this->attribA3);
        glDisableVertexAttribArray(this->attribA4);
    }
}
