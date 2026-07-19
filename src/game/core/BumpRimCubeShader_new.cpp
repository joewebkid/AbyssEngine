#include "game/core/BumpRimCubeShader_new.h"
#include <GLES2/gl2.h>
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"

static float g_rimnGlobalA = 0.0f;
static float g_rimnGlobalB = 0.0f;
static float g_rimnGlobalC = 0.0f;
static unsigned char g_rimnByteGlobal = 0;

namespace AbyssEngine {
    int BumpRimCubeShader_new::ShaderIndex;

    BumpRimCubeShader_new::BumpRimCubeShader_new() {
        ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"BumpRimCubeShader_new";
    }

    void BumpRimCubeShader_new::Init(Engine *) {
        int program = this->LoadBindShader("BumpRimCubeShader_new.vsh", "BumpRimCubeShader_new.fsh");
        this->program = program;
        if (program == 0) {
            program = this->ES2LoadProgram("BumpRimCubeShader_new.vsh", "BumpRimCubeShader_new.fsh");
            this->program = program;
        }

        this->attrib0 = glGetAttribLocation(program, "a0");
        this->attrib1 = glGetAttribLocation(this->program, "a1");
        this->attrib2 = glGetAttribLocation(this->program, "a2");
        this->attrib3 = glGetAttribLocation(this->program, "a3");
        this->attrib4 = glGetAttribLocation(this->program, "a4");

        this->uniform0 = glGetUniformLocation(this->program, "u0");
        this->uniform1 = glGetUniformLocation(this->program, "u1");
        this->uniform2 = glGetUniformLocation(this->program, "u2");
        this->uniform3 = glGetUniformLocation(this->program, "u3");
        this->uniform4 = glGetUniformLocation(this->program, "u4");
        this->uniform5 = glGetUniformLocation(this->program, "u5");
        this->uniform6 = glGetUniformLocation(this->program, "u6");
        this->uniform7 = glGetUniformLocation(this->program, "u7");
        this->uniform8 = glGetUniformLocation(this->program, "u8");
        this->uniform9 = glGetUniformLocation(this->program, "u9");
        this->uniform10 = glGetUniformLocation(this->program, "u10");
        this->uniform11 = glGetUniformLocation(this->program, "u11");
        this->uniform13 = glGetUniformLocation(this->program, "u12");
        this->uniform12 = glGetUniformLocation(this->program, "u13");
        this->uniform14 = glGetUniformLocation(this->program, "u14");
        this->uniform15 = glGetUniformLocation(this->program, "u15");
        this->uniform16 = glGetUniformLocation(this->program, "u16");
        this->uniform17 = glGetUniformLocation(this->program, "u17");
        this->uniform18 = glGetUniformLocation(this->program, "u18");
        this->uniform19 = glGetUniformLocation(this->program, "u19");
        this->uniform20 = glGetUniformLocation(this->program, "u20");
        this->uniform21 = glGetUniformLocation(this->program, "u21");
        this->uniform22 = glGetUniformLocation(this->program, "u22");
        this->uniform23 = glGetUniformLocation(this->program, "u23");
        this->uniform24 = glGetUniformLocation(this->program, "u24");

        glUseProgram(this->program);
        for (int i = 0; i != 2; i++) {
            int loc = (&this->uniform6)[i];
            if (loc >= 0)
                glUniform1i(loc, i);
        }
        glUniform1i(this->uniform8, 7);
    }

    void BumpRimCubeShader_new::UpdateMeshData(Mesh *mesh, Engine *engine) {
        ::Engine *eng = (::Engine *) engine;
        if (this->uniform0 >= 0)
            glUniformMatrix4fv(this->uniform0, 1, 0, eng->worldViewProjMatrix);
        if (this->uniform1 >= 0)
            glUniformMatrix3fv(this->uniform1, 1, 0, eng->normalMatrix);
        if (this->uniform2 >= 0)
            glUniformMatrix4fv(this->uniform2, 1, 0, eng->modelMatrixGL);
        if (this->uniform17 >= 0)
            glUniform1f(this->uniform17, g_rimnGlobalA);
        if (this->uniform18 >= 0)
            glUniform1f(this->uniform18, g_rimnGlobalB);
        if (this->uniform24 >= 0)
            glUniform1f(this->uniform24, g_rimnGlobalC);

        if (this->dirty != 0) {
            glUniform3f(this->uniform3, eng->lightDir.x,
                        eng->lightDir.y, eng->lightDir.z);
            if (this->uniform5 >= 0)
                glUniform3f(this->uniform5, eng->lightColor.x,
                            eng->lightColor.y, eng->lightColor.z);
            if (this->uniform9 >= 0)
                glUniform4fv(this->uniform9, 1, eng->glColor);
            float buf[3];
            if (this->uniform10 >= 0) {
                buf[0] = eng->lightAmbientShaded.x + eng->lightSpecularShaded.x;
                buf[1] = eng->lightAmbientShaded.y + eng->lightSpecularShaded.y;
                buf[2] = eng->lightAmbientShaded.z + eng->lightSpecularShaded.z;
                glUniform3fv(this->uniform10, 1, buf);
            }
            if (this->uniform11 >= 0)
                glUniform3fv(this->uniform11, 1, (float *) &eng->field_0x2fc);
            if (this->uniform13 >= 0)
                glUniform3fv(this->uniform13, 1, (float *) &eng->lightDiffuseShaded);
            if (this->uniform15 >= 0)
                glUniform1f(this->uniform15, eng->materialShininess);
            if (this->uniform16 >= 0)
                glUniform3fv(this->uniform16, 1, (float *) &eng->rimColor);
            int loc84 = this->uniform20;
            if (loc84 >= 0) {
                float *v = (float *) &eng->fogColor;
                glUniform3fv(loc84, 1, v);
            }
            if (this->uniform22 >= 0)
                glUniform1f(this->uniform22, eng->fogMinDist);
            if (this->uniform21 >= 0)
                glUniform1f(this->uniform21, eng->fogMaxDist);
            if (this->uniform23 >= 0)
                glUniform1i(this->uniform23, g_rimnByteGlobal);
            if (this->uniform19 >= 0) {
                float v = 0.0f;
                int *m30 = (int *) mesh->field_0x30;
                if (m30 != 0) {
                    v = 1.0f;
                    if (m30[9] == 0)
                        v = 0.0f;
                }
                glUniform1f(this->uniform19, v);
            }
            int loc5c = this->uniform10;
            if (eng->field_0x32c >= 2) {
                if (loc5c >= 0) {
                    buf[0] = eng->lightAmbientShaded.x + eng->lightSpecularShaded.x;
                    buf[1] = eng->lightAmbientShaded.y + eng->lightSpecularShaded.y;
                    buf[2] = eng->lightAmbientShaded.z + eng->lightSpecularShaded.z;
                    glUniform3fv(loc5c, 1, buf);
                }
                glUniform3fv(this->uniform12, 1, (float *) &eng->field_0x308);
                glUniform3fv(this->uniform14, 1, (float *) &eng->particleAmbient);
                glUniform3f(this->uniform4, eng->field_0x33c.x,
                            eng->field_0x33c.y, eng->field_0x33c.z);
            } else {
                glUniform3f(loc5c, 0, 0, 0);
                glUniform3f(this->uniform12, 0, 0, 0);
                glUniform3f(this->uniform14, 0, 0, 0);
                glUniform3f(this->uniform4, eng->field_0x33c.x,
                            eng->field_0x33c.y, eng->field_0x33c.z);
            }
            this->dirty = 0;
        }

        if (this->attrib0 >= 0)
            glEnableVertexAttribArray(this->attrib0);
        if (this->attrib1 >= 0)
            glEnableVertexAttribArray(this->attrib1);
        if (this->attrib2 >= 0)
            glEnableVertexAttribArray(this->attrib2);
        if (this->attrib3 >= 0)
            glEnableVertexAttribArray(this->attrib3);
        if (this->attrib4 >= 0)
            glEnableVertexAttribArray(this->attrib4);

        if (mesh->uploaded == 0) {
            if (this->attrib0 >= 0)
                glVertexAttribPointer(this->attrib0, 3, 0x1406, 0, 0, mesh->positions);
            if (this->attrib1 >= 0)
                glVertexAttribPointer(this->attrib1, 2, 0x1406, 0, 0, mesh->texCoords);
            if (this->attrib2 >= 0)
                glVertexAttribPointer(this->attrib2, 3, 0x1406, 0, 0, mesh->normals);
            if (this->attrib3 >= 0)
                glVertexAttribPointer(this->attrib3, 3, 0x1406, 0, 0, mesh->tangents);
            if (this->attrib4 >= 0)
                glVertexAttribPointer(this->attrib4, 3, 0x1406, 0, 0, mesh->binormals);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->attrib0, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->attrib1, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->attrib2, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->tangentVBO);
            glVertexAttribPointer(this->attrib3, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->binormalVBO);
            glVertexAttribPointer(this->attrib4, 3, 0x1406, 0, 0, 0);
        }
    }

    void BumpRimCubeShader_new::SetInActive() {
        int loc;
        loc = this->attrib0;
        if (loc >= 0)
            glDisableVertexAttribArray(loc);
        loc = this->attrib1;
        if (loc >= 0)
            glDisableVertexAttribArray(loc);
        loc = this->attrib2;
        if (loc >= 0)
            glDisableVertexAttribArray(loc);
        loc = this->attrib3;
        if (loc >= 0)
            glDisableVertexAttribArray(loc);
        loc = this->attrib4;
        if (loc >= 0)
            glDisableVertexAttribArray(loc);
    }
}
