#include "engine/render/shaders/GreenShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int GreenShader::ShaderIndex;

    GreenShader::GreenShader() {
        GreenShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"GreenShader";
    }

    void GreenShader::Init(Engine *) {
        this->program = this->ES2LoadProgram("GreenShader.vsh", "GreenShader.fsh");

        this->a0Loc = glGetAttribLocation(this->program, "a0");
        this->a1Loc = glGetAttribLocation(this->program, "a1");
        this->a2Loc = glGetAttribLocation(this->program, "a2");
        this->a3Loc = glGetAttribLocation(this->program, "a3");
        this->a4Loc = glGetAttribLocation(this->program, "a4");

        this->u0Loc = glGetUniformLocation(this->program, "u0");
        this->u1Loc = glGetUniformLocation(this->program, "u1");
        this->u2Loc = glGetUniformLocation(this->program, "u2");
        this->u3Loc = glGetUniformLocation(this->program, "u3");
        this->u4Loc = glGetUniformLocation(this->program, "u4");
        this->u5Loc = glGetUniformLocation(this->program, "u5");
        this->u6Loc = glGetUniformLocation(this->program, "u6");
        this->u7Loc = glGetUniformLocation(this->program, "u7");
        this->u8Loc = glGetUniformLocation(this->program, "u8");

        glUseProgram(this->program);
        glUniform1i(this->u4Loc, 0);
    }

    void GreenShader::SetInActive() {
        if (this->a0Loc >= 0)
            glDisableVertexAttribArray(this->a0Loc);
        if (this->a1Loc >= 0)
            glDisableVertexAttribArray(this->a1Loc);
        if (this->a2Loc >= 0)
            glDisableVertexAttribArray(this->a2Loc);
        if (this->a3Loc >= 0)
            glDisableVertexAttribArray(this->a3Loc);
        if (this->a4Loc >= 0)
            glDisableVertexAttribArray(this->a4Loc);
    }

    void GreenShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->u0Loc >= 0)
            glUniformMatrix4fv(this->u0Loc, 1, 0, engine->worldViewProjMatrix);

        if (this->a0Loc >= 0)
            glEnableVertexAttribArray(this->a0Loc);
        if (this->a1Loc >= 0)
            glEnableVertexAttribArray(this->a1Loc);
        if (this->a2Loc >= 0)
            glEnableVertexAttribArray(this->a2Loc);
        if (this->a3Loc >= 0)
            glEnableVertexAttribArray(this->a3Loc);
        if (this->a4Loc >= 0)
            glEnableVertexAttribArray(this->a4Loc);

        if (mesh->uploaded != 0) {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->a0Loc, 3, 0x1406, 0, 0, 0);
        } else {
            if (this->a0Loc < 0)
                return;
            glVertexAttribPointer(this->a0Loc, 3, 0x1406, 0, 0, mesh->positions);
        }
    }
}
