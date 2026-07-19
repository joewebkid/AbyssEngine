#include "engine/render/shaders/SimpleRefractionShader.h"

#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int SimpleRefractionShader::ShaderIndex;

    SimpleRefractionShader::SimpleRefractionShader() {
        SimpleRefractionShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"SimpleRefractionShader";
    }

    void SimpleRefractionShader::Init(::Engine *engine) {
        int program = this->ES2LoadProgram(
            "SimpleRefractionShader.vsh", "SimpleRefractionShader.fsh");
        this->program = program;

        this->aPositionLoc = glGetAttribLocation(program, "a_position");
        this->aNormalLoc = glGetAttribLocation(this->program, "a_normal");
        this->aTangentLoc = glGetAttribLocation(this->program, "a_tangent");
        this->aTexCoordLoc = glGetAttribLocation(this->program, "a_texCoord");

        this->uM0Loc = glGetUniformLocation(this->program, "u_m0");
        this->uM1Loc = glGetUniformLocation(this->program, "u_m1");
        this->uM2Loc = glGetUniformLocation(this->program, "u_m2");
        this->uTex0Loc = glGetUniformLocation(this->program, "u_tex0");
        this->uTex1Loc = glGetUniformLocation(this->program, "u_tex1");
        this->uM3Loc = glGetUniformLocation(this->program, "u_m3");
        this->uM4Loc = glGetUniformLocation(this->program, "u_m4");
        this->uM5Loc = glGetUniformLocation(this->program, "u_m5");

        glActiveTexture(0x84c7);
        engine->ActivateRefractFBO();

        this->uRefractLoc = glGetUniformLocation(this->program, "u_refract");
        this->uM6Loc = glGetUniformLocation(this->program, "u_m6");

        glUseProgram(this->program);

        int samplerLocs[2] = {this->uTex0Loc, this->uTex1Loc};
        for (int i = 0; i != 2; i = i + 1) {
            if (samplerLocs[i] >= 0)
                glUniform1i(samplerLocs[i], i);
        }
        glUniform1i(this->uM6Loc, 7);
    }

    void SimpleRefractionShader::SetInActive() {
        if (this->aPositionLoc >= 0)
            glDisableVertexAttribArray(this->aPositionLoc);
        if (this->aNormalLoc >= 0)
            glDisableVertexAttribArray(this->aNormalLoc);
        if (this->aTangentLoc >= 0)
            glDisableVertexAttribArray(this->aTangentLoc);
        if (this->aTexCoordLoc >= 0)
            glDisableVertexAttribArray(this->aTexCoordLoc);
        if (this->uM0Loc >= 0)
            glDisableVertexAttribArray(this->uM0Loc);
    }

    void SimpleRefractionShader::UpdateMeshData(Mesh *mesh, ::Engine *engine) {
        if (this->uM1Loc >= 0)
            glUniformMatrix4fv(this->uM1Loc, 1, 0, engine->worldViewProjMatrix);

        if (this->dirty != 0) {
            if (this->uM4Loc >= 0)
                glUniform1f(this->uM4Loc, -2.0f);
            if (this->uM3Loc >= 0)
                glUniform4fv(this->uM3Loc, 1, engine->glColor);
            if (this->uM2Loc >= 0)
                glUniform3f(this->uM2Loc, engine->lightColor.x, engine->lightColor.y,
                            engine->lightColor.z);
            int loc = this->uM5Loc;
            float w = (float) engine->GetDisplayWidth();
            float h = (float) engine->GetDisplayHeight();
            glUniform2f(loc, 1.0f / w, 1.0f / h);
            glActiveTexture(0x84c7);
            engine->ActivateRefractFBO();
            glUniform1f(this->uRefractLoc, mesh->field_0x20);
            this->dirty = 0;
        }

        if (this->aPositionLoc >= 0)
            glEnableVertexAttribArray(this->aPositionLoc);
        if (this->aNormalLoc >= 0)
            glEnableVertexAttribArray(this->aNormalLoc);
        if (this->aTangentLoc >= 0)
            glEnableVertexAttribArray(this->aTangentLoc);
        if (this->aTexCoordLoc >= 0)
            glEnableVertexAttribArray(this->aTexCoordLoc);
        if (this->uM0Loc >= 0)
            glEnableVertexAttribArray(this->uM0Loc);

        if (mesh->uploaded == 0) {
            if (this->aPositionLoc >= 0)
                glVertexAttribPointer(this->aPositionLoc, 3, 0x1406, 0, 0, mesh->positions);
            if (this->aNormalLoc >= 0)
                glVertexAttribPointer(this->aNormalLoc, 2, 0x1406, 0, 0, mesh->texCoords);
            if (this->aTangentLoc >= 0)
                glVertexAttribPointer(this->aTangentLoc, 3, 0x1406, 0, 0, mesh->normals);
            if (this->aTexCoordLoc >= 0)
                glVertexAttribPointer(this->aTexCoordLoc, 3, 0x1406, 0, 0, mesh->tangents);
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
            glVertexAttribPointer(this->aTexCoordLoc, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->binormalVBO);
            glVertexAttribPointer(this->uM0Loc, 3, 0x1406, 0, 0, 0);
        }
    }
}
