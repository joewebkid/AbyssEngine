#include "engine/render/shaders/EnergyShield.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int EnergyShield::ShaderIndex;

    void EnergyShield::UpdateMeshData(Mesh *mesh, Engine *engine) {
        ::Engine *host = (::Engine *) engine;
        if (this->uM1 >= 0)
            glUniformMatrix4fv(this->uM1, 1, 0, host->worldViewProjMatrix);
        if (this->uM2 >= 0)
            glUniformMatrix4fv(this->uM2, 1, 0, host->worldViewMatrixGL);

        if (this->dirty != 0) {
            if (this->uM3 >= 0)
                glUniform3f(this->uM3, host->lightColor.x, host->lightColor.y,
                            host->lightColor.z);
            if (this->uM4 >= 0)
                glUniform4fv(this->uM4, 1, host->glColor);
            if (this->uM5 >= 0)
                glUniform3fv(this->uM5, 1, (float *) &host->rimColor);
            int loc = this->uM6;
            if (loc >= 0) {
                float w = (float) ((::Engine *) engine)->GetDisplayWidth();
                float h = (float) ((::Engine *) engine)->GetDisplayHeight();
                glUniform2f(loc, 1.0f / w, 1.0f / h);
            }
            glActiveTexture(0x84c7);
            ((::Engine *) engine)->ActivateRefractFBO();
            glUniform1f(this->uM7, mesh->field_0x24);
            glUniform1f(this->uRefract, mesh->field_0x24 / 3.0f);
            this->dirty = 0;
        }

        if (this->aPosition >= 0)
            glEnableVertexAttribArray(this->aPosition);
        if (this->aTexCoord >= 0)
            glEnableVertexAttribArray(this->aTexCoord);
        if (this->uM0 >= 0)
            glEnableVertexAttribArray(this->uM0);

        if (mesh->uploaded == 0) {
            if (this->aPosition >= 0)
                glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, mesh->positions);
            if (this->aTexCoord >= 0)
                glVertexAttribPointer(this->aTexCoord, 2, 0x1406, 0, 0, mesh->texCoords);
            if (this->uM0 >= 0)
                glVertexAttribPointer(this->uM0, 3, 0x1406, 0, 0, mesh->normals);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->aTexCoord, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->uM0, 3, 0x1406, 0, 0, 0);
        }
    }

    void EnergyShield::SetInActive() {
        if (this->aPosition >= 0)
            glDisableVertexAttribArray(this->aPosition);
        if (this->aTexCoord >= 0)
            glDisableVertexAttribArray(this->aTexCoord);
        if (this->uM0 >= 0)
            glDisableVertexAttribArray(this->uM0);
    }

    void EnergyShield::Init(Engine *engine) {
        int program = this->ES2LoadProgram("EnergyShield.vsh", "EnergyShield.fsh");
        this->program = program;

        this->aPosition = glGetAttribLocation(program, "a_position");
        this->aTexCoord = glGetAttribLocation(this->program, "a_texCoord");

        this->uM0 = glGetUniformLocation(this->program, "u_m0");
        this->uM1 = glGetUniformLocation(this->program, "u_m1");
        this->uM2 = glGetUniformLocation(this->program, "u_m2");
        this->uM3 = glGetUniformLocation(this->program, "u_m3");
        this->uTex0 = glGetUniformLocation(this->program, "u_tex0");
        this->uTex1 = glGetUniformLocation(this->program, "u_tex1");
        this->uM4 = glGetUniformLocation(this->program, "u_m4");
        this->uM5 = glGetUniformLocation(this->program, "u_m5");
        this->uM6 = glGetUniformLocation(this->program, "u_m6");

        glActiveTexture(0x84c7);
        ((::Engine *) engine)->ActivateRefractFBO();

        this->uRefract = glGetUniformLocation(this->program, "u_refract");
        this->uM7 = glGetUniformLocation(this->program, "u_m7");
        this->uM8 = glGetUniformLocation(this->program, "u_m8");

        glUseProgram(this->program);

        for (int i = 0; i != 2; i = i + 1) {
            if ((&this->uTex0)[i] >= 0)
                glUniform1i((&this->uTex0)[i], i);
        }
        glUniform1i(this->uM8, 7);
    }

    EnergyShield::EnergyShield() {
        EnergyShield::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"EnergyShield";
    }
}
