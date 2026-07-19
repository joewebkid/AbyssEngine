#include "engine/render/shaders/PulseShader.h"
#include "game/core/String.h"
#include "engine/render/Mesh.h"
#include "engine/render/Engine.h"
#include "engine/core/ApplicationManager.h"
#include <GLES2/gl2.h>


extern "C" {
float sinf(float x);
}

static float PulseShader_timeScale = 0.0f;

namespace AbyssEngine {
    int PulseShader::ShaderIndex;

    PulseShader::PulseShader() {
        PulseShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"PulseShader";
    }

    void PulseShader::SetInActive() {
        if (this->a0Loc >= 0) glDisableVertexAttribArray(this->a0Loc);
        if (this->a1Loc >= 0) glDisableVertexAttribArray(this->a1Loc);
        if (this->a2Loc >= 0) glDisableVertexAttribArray(this->a2Loc);
        if (this->a3Loc >= 0) glDisableVertexAttribArray(this->a3Loc);
        if (this->a4Loc >= 0) glDisableVertexAttribArray(this->a4Loc);
    }

    void PulseShader::Init(Engine *) {
        int program = this->ES2LoadProgram("PulseShader.vsh", "PulseShader.fsh");
        this->program = program;

        this->a0Loc = glGetAttribLocation(program, "a0");
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
        this->u9Loc = glGetUniformLocation(this->program, "u9");

        glUseProgram(this->program);
        glUniform1i(this->u5Loc, 0);
    }

    void PulseShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->u0Loc >= 0)
            glUniformMatrix4fv(this->u0Loc, 1, 0, engine->worldViewProjMatrix);
        if (this->u1Loc >= 0)
            glUniformMatrix3fv(this->u1Loc, 1, 0, engine->normalMatrix);

        if (this->dirty != 0) {
            if (this->u2Loc >= 0)
                glUniform3f(this->u2Loc, engine->lightDir.x, engine->lightDir.y,
                            engine->lightDir.z);
            if (this->u3Loc >= 0)
                glUniform3f(this->u3Loc, engine->lightColor.x, engine->lightColor.y,
                            engine->lightColor.z);
            if (this->u6Loc >= 0)
                glUniform4fv(this->u6Loc, 1, engine->glColor);
            if (this->u7Loc >= 0)
                glUniform4fv(this->u7Loc, 1, engine->materialAmbient);
            if (this->u8Loc >= 0)
                glUniform4fv(this->u8Loc, 1, engine->materialDiffuse);
            if (this->u9Loc >= 0)
                glUniform4fv(this->u9Loc, 1, engine->materialSpecular);

            long long t = ApplicationManager::gAppManager->GetCurrentTimeMillis();
            float v = sinf((float) t / PulseShader_timeScale);
            glUniform1f(this->u4Loc, v + 2.0f);
            this->dirty = 0;
        }

        if (this->a0Loc >= 0) glEnableVertexAttribArray(this->a0Loc);
        if (this->a1Loc >= 0) glEnableVertexAttribArray(this->a1Loc);
        if (this->a2Loc >= 0) glEnableVertexAttribArray(this->a2Loc);
        if (this->a3Loc >= 0) glEnableVertexAttribArray(this->a3Loc);
        if (this->a4Loc >= 0) glEnableVertexAttribArray(this->a4Loc);

        if (mesh->uploaded == 0) {
            if (this->a0Loc >= 0)
                glVertexAttribPointer(this->a0Loc, 3, 0x1406, 0, 0, mesh->positions);
            if (this->a1Loc >= 0)
                glVertexAttribPointer(this->a1Loc, 2, 0x1406, 0, 0, mesh->texCoords);
            if (this->a2Loc >= 0)
                glVertexAttribPointer(this->a2Loc, 3, 0x1406, 0, 0, mesh->normals);
            if (this->a3Loc >= 0)
                glVertexAttribPointer(this->a3Loc, 3, 0x1406, 0, 0, mesh->tangents);
            if (this->a4Loc >= 0)
                glVertexAttribPointer(this->a4Loc, 3, 0x1406, 0, 0, mesh->binormals);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->a0Loc, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->a1Loc, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->a2Loc, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->tangentVBO);
            glVertexAttribPointer(this->a3Loc, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->binormalVBO);
            glVertexAttribPointer(this->a4Loc, 3, 0x1406, 0, 0, 0);
        }
    }
}
