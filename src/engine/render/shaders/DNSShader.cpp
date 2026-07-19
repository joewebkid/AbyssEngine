#include "engine/render/shaders/DNSShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include "game/core/String.h"
#include <GLES2/gl2.h>

static float DNSShader_g0;
static float DNSShader_g1;

namespace AbyssEngine {
    int DNSShader::ShaderIndex;

    DNSShader::DNSShader() {
        DNSShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"DNSShader";
    }

    void DNSShader::Init(Engine *) {
        int program = this->ES2LoadProgram("DNSShader.vsh", "DNSShader.fsh");
        this->program = program;

        this->aPositionLoc = glGetAttribLocation(program, "a_position");
        this->aNormalLoc = glGetAttribLocation(this->program, "a_normal");
        this->aTangentLoc = glGetAttribLocation(this->program, "a_tangent");
        this->aBinormalLoc = glGetAttribLocation(this->program, "a_binormal");

        this->uM0Loc = glGetUniformLocation(this->program, "u_m0");
        this->uM1Loc = glGetUniformLocation(this->program, "u_m1");
        this->uM2Loc = glGetUniformLocation(this->program, "u_m2");
        this->uM3Loc = glGetUniformLocation(this->program, "u_m3");
        this->uM4Loc = glGetUniformLocation(this->program, "u_m4");
        this->uM5Loc = glGetUniformLocation(this->program, "u_m5");
        this->uM6Loc = glGetUniformLocation(this->program, "u_m6");
        this->uM7Loc = glGetUniformLocation(this->program, "u_m7");
        this->uM8Loc = glGetUniformLocation(this->program, "u_m8");
        this->uM9Loc = glGetUniformLocation(this->program, "u_m9");
        this->uM10Loc = glGetUniformLocation(this->program, "u_m10");
        this->uM11Loc = glGetUniformLocation(this->program, "u_m11");
        this->uM12Loc = glGetUniformLocation(this->program, "u_m12");

        glUseProgram(this->program);
    }

    void DNSShader::SetInActive() {
        if (this->aPositionLoc >= 0)
            glDisableVertexAttribArray(this->aPositionLoc);
        if (this->aNormalLoc >= 0)
            glDisableVertexAttribArray(this->aNormalLoc);
        if (this->aTangentLoc >= 0)
            glDisableVertexAttribArray(this->aTangentLoc);
        if (this->aBinormalLoc >= 0)
            glDisableVertexAttribArray(this->aBinormalLoc);
        if (this->uM0Loc >= 0)
            glDisableVertexAttribArray(this->uM0Loc);
    }

    void DNSShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->uM1Loc >= 0)
            glUniformMatrix4fv(this->uM1Loc, 1, 0, engine->worldViewProjMatrix);
        if (this->uM2Loc >= 0)
            glUniformMatrix3fv(this->uM2Loc, 1, 0, engine->normalMatrix);
        if (this->uM3Loc >= 0)
            glUniformMatrix4fv(this->uM3Loc, 1, 0, engine->modelMatrixGL);
        if (this->uM11Loc >= 0)
            glUniform1f(this->uM11Loc, DNSShader_g0);
        if (this->uM12Loc >= 0)
            glUniform1f(this->uM12Loc, DNSShader_g1);

        if (this->dirty != 0) {
            glUniform3f(this->uM4Loc, engine->lightDir.x, engine->lightDir.y,
                        engine->lightDir.z);
            if (this->uM5Loc >= 0)
                glUniform3f(this->uM5Loc, engine->lightColor.x, engine->lightColor.y,
                            engine->lightColor.z);
            if (this->uM6Loc >= 0)
                glUniform4fv(this->uM6Loc, 1, engine->glColor);
            if (this->uM7Loc >= 0)
                glUniform3fv(this->uM7Loc, 1, (float *) &engine->lightAmbientShaded);
            if (this->uM8Loc >= 0)
                glUniform3fv(this->uM8Loc, 1, (float *) &engine->field_0x2fc);
            if (this->uM9Loc >= 0)
                glUniform3fv(this->uM9Loc, 1, (float *) &engine->lightDiffuseShaded);
            if (this->uM10Loc >= 0)
                glUniform1f(this->uM10Loc, engine->materialShininess);
            this->dirty = 0;
        }

        if (this->aPositionLoc >= 0)
            glEnableVertexAttribArray(this->aPositionLoc);
        if (this->aNormalLoc >= 0)
            glEnableVertexAttribArray(this->aNormalLoc);
        if (this->aTangentLoc >= 0)
            glEnableVertexAttribArray(this->aTangentLoc);
        if (this->aBinormalLoc >= 0)
            glEnableVertexAttribArray(this->aBinormalLoc);
        if (this->uM0Loc >= 0)
            glEnableVertexAttribArray(this->uM0Loc);

        if (mesh->uploaded == 0) {
            if (this->aPositionLoc >= 0)
                glVertexAttribPointer(this->aPositionLoc, 3, 0x1406, 0, 0, mesh->positions);
            if (this->aNormalLoc >= 0)
                glVertexAttribPointer(this->aNormalLoc, 2, 0x1406, 0, 0, mesh->texCoords);
            if (this->aTangentLoc >= 0)
                glVertexAttribPointer(this->aTangentLoc, 3, 0x1406, 0, 0, mesh->normals);
            if (this->aBinormalLoc >= 0)
                glVertexAttribPointer(this->aBinormalLoc, 3, 0x1406, 0, 0, mesh->tangents);
            if (this->uM0Loc >= 0)
                glVertexAttribPointer(this->uM0Loc, 3, 0x1406, 0, 0, mesh->binormals);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->aPositionLoc, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->aNormalLoc, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->aTangentLoc, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->tangentVBO);
            glVertexAttribPointer(this->aBinormalLoc, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->binormalVBO);
            glVertexAttribPointer(this->uM0Loc, 3, 0x1406, 0, 0, 0);
        }
    }
}
