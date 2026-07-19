#include "game/core/BumpShaderParticle.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

static float g_particleGlobalA;
static float g_particleGlobalB;

namespace AbyssEngine {
    int BumpShaderParticle::ShaderIndex;

    BumpShaderParticle::BumpShaderParticle() {
        ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"BumpShaderParticle";
    }

    void BumpShaderParticle::Init(Engine *) {
        this->program = this->ES2LoadProgram("BumpShaderParticle.vsh", "BumpShaderParticle.fsh");

        attribA0 = glGetAttribLocation(this->program, "a0");
        attribA1 = glGetAttribLocation(this->program, "a1");
        attribA2 = glGetAttribLocation(this->program, "a2");
        attribA3 = glGetAttribLocation(this->program, "a3");
        attribA4 = glGetAttribLocation(this->program, "a4");

        uniformU0 = glGetUniformLocation(this->program, "u0");
        uniformU1 = glGetUniformLocation(this->program, "u1");
        uniformU2 = glGetUniformLocation(this->program, "u2");
        uniformU3 = glGetUniformLocation(this->program, "u3");
        uniformU4 = glGetUniformLocation(this->program, "u4");
        uniformU5 = glGetUniformLocation(this->program, "u5");
        uniformU6 = glGetUniformLocation(this->program, "u6");
        uniformU7 = glGetUniformLocation(this->program, "u7");
        uniformU8 = glGetUniformLocation(this->program, "u8");
        uniformU9 = glGetUniformLocation(this->program, "u9");
        uniformU10 = glGetUniformLocation(this->program, "u10");
        uniformU11 = glGetUniformLocation(this->program, "u11");
        uniformU12 = glGetUniformLocation(this->program, "u12");
        uniformU13 = glGetUniformLocation(this->program, "u13");

        glUseProgram(this->program);

        int *samplers = &uniformU5;
        for (int i = 0; i != 2; i++) {
            int loc = samplers[i];
            if (loc >= 0)
                glUniform1i(loc, i);
        }
    }

    void BumpShaderParticle::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (uniformU1 >= 0)
            glUniformMatrix4fv(uniformU1, 1, 0, engine->worldViewProjMatrix);
        if (uniformU2 >= 0)
            glUniformMatrix3fv(uniformU2, 1, 0, engine->normalMatrix);
        if (uniformU12 >= 0)
            glUniform1f(uniformU12, g_particleGlobalA);
        if (uniformU13 >= 0)
            glUniform1f(uniformU13, g_particleGlobalB);

        if (this->dirty != 0) {
            glUniform3f(uniformU3, engine->lightDir.x,
                        engine->lightDir.y, engine->lightDir.z);
            if (uniformU4 >= 0)
                glUniform3f(uniformU4, engine->lightColor.x,
                            engine->lightColor.y, engine->lightColor.z);
            if (uniformU7 >= 0)
                glUniform4fv(uniformU7, 1, engine->glColor);
            if (uniformU8 >= 0)
                glUniform3fv(uniformU8, 1, (float *) &engine->field_0x314);
            if (uniformU9 >= 0)
                glUniform3fv(uniformU9, 1, (float *) &engine->field_0x2fc);
            if (uniformU10 >= 0)
                glUniform3fv(uniformU10, 1, (float *) &engine->lightDiffuseShaded);
            if (uniformU11 >= 0)
                glUniform1f(uniformU11, engine->materialShininess);
            this->dirty = 0;
        }

        if (attribA0 >= 0)
            glEnableVertexAttribArray(attribA0);
        if (attribA1 >= 0)
            glEnableVertexAttribArray(attribA1);
        if (attribA2 >= 0)
            glEnableVertexAttribArray(attribA2);
        if (attribA3 >= 0)
            glEnableVertexAttribArray(attribA3);
        if (attribA4 >= 0)
            glEnableVertexAttribArray(attribA4);
        if (uniformU0 >= 0)
            glEnableVertexAttribArray(uniformU0);

        if (mesh->uploaded == 0) {
            if (attribA0 >= 0)
                glVertexAttribPointer(attribA0, 3, 0x1406, 0, 0, mesh->positions);
            if (attribA1 >= 0)
                glVertexAttribPointer(attribA1, 2, 0x1406, 0, 0, mesh->texCoords);
            if (attribA2 >= 0)
                glVertexAttribPointer(attribA2, 3, 0x1406, 0, 0, mesh->normals);
            if (attribA3 >= 0)
                glVertexAttribPointer(attribA3, 3, 0x1406, 0, 0, mesh->tangents);
            if (attribA4 >= 0)
                glVertexAttribPointer(attribA4, 3, 0x1406, 0, 0, mesh->binormals);
            if (uniformU0 >= 0)
                glVertexAttribPointer(uniformU0, 4, 0x1406, 0, 0, mesh->colors);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(attribA0, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(attribA1, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(attribA2, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->tangentVBO);
            glVertexAttribPointer(attribA3, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->binormalVBO);
            glVertexAttribPointer(attribA4, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->colorVBO);
            glVertexAttribPointer(uniformU0, 4, 0x1406, 0, 0, 0);
        }
    }

    void BumpShaderParticle::SetInActive() {
        if (attribA0 >= 0)
            glDisableVertexAttribArray(attribA0);
        if (attribA1 >= 0)
            glDisableVertexAttribArray(attribA1);
        if (attribA2 >= 0)
            glDisableVertexAttribArray(attribA2);
        if (attribA3 >= 0)
            glDisableVertexAttribArray(attribA3);
        if (attribA4 >= 0)
            glDisableVertexAttribArray(attribA4);
        if (uniformU0 >= 0)
            glDisableVertexAttribArray(uniformU0);
    }
}
