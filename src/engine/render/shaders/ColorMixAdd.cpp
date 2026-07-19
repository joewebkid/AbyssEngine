#include "engine/render/shaders/ColorMixAdd.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int ColorMixAdd::ShaderIndex;

    ColorMixAdd::ColorMixAdd() {
        ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"ColorMixAdd";
    }

    void ColorMixAdd::Init(Engine *) {
        int program = this->ES2LoadProgram("ColorMixAdd.vsh", "ColorMixAdd.fsh");
        this->program = program;

        this->u0Loc = glGetUniformLocation(program, "u0");
        this->a0Loc = glGetAttribLocation(this->program, "a0");
        this->a1Loc = glGetAttribLocation(this->program, "a1");
        this->u1Loc = glGetUniformLocation(this->program, "u1");
        this->u2Loc = glGetUniformLocation(this->program, "u2");
        this->u3Loc = glGetUniformLocation(this->program, "u3");
        this->u4Loc = glGetUniformLocation(this->program, "u4");
        this->u5Loc = glGetUniformLocation(this->program, "u5");

        glUseProgram(this->program);
        glUniform1i(this->u0Loc, 0);
    }

    void ColorMixAdd::UpdateMeshData(Mesh *mesh, Engine *engine) {
        glUniformMatrix4fv(this->u1Loc, 1, 0, engine->worldViewProjMatrix);
        if (this->u4Loc >= 0)
            glUniformMatrix4fv(this->u4Loc, 1, 0, engine->uvMatrix);
        if (this->u5Loc >= 0)
            glUniform1i(this->u5Loc, mesh->hasAnimation);

        if (this->dirty != 0) {
            glUniform4fv(this->u2Loc, 1, engine->glColor);
            if (this->u3Loc >= 0)
                glUniform1f(this->u3Loc, 1.0f - *(float *) &mesh->materialId);
            this->dirty = 0;
        }

        if ((mesh->vertexFormat & 2) != 0) {
            glEnableVertexAttribArray(this->a0Loc);
            glEnableVertexAttribArray(this->a1Loc);
            if (mesh->uploaded == 0) {
                glVertexAttribPointer(this->a0Loc, 3, 0x1406, 0, 0, mesh->positions);
                glVertexAttribPointer(this->a1Loc, 2, 0x1406, 0, 0, mesh->texCoords);
            } else {
                glBindBuffer(0x8892, mesh->positionVBO);
                glVertexAttribPointer(this->a0Loc, 3, 0x1406, 0, 0, 0);
                glBindBuffer(0x8892, mesh->texCoordVBO);
                glVertexAttribPointer(this->a1Loc, 2, 0x1406, 0, 0, 0);
            }
        }
    }

    void ColorMixAdd::SetInActive() {
        for (int i = 2; i != 0; i = i - 1) {
            glDisableVertexAttribArray(this->a0Loc);
            glDisableVertexAttribArray(this->a1Loc);
        }
    }
}
