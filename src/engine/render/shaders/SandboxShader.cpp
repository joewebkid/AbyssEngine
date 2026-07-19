#include "engine/render/shaders/SandboxShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

static void *SandboxShader_registerSrc = nullptr;
static void **SandboxShader_registerDst = &SandboxShader_registerSrc;

namespace AbyssEngine {
    int SandboxShader::ShaderIndex;

    void SandboxShader::UpdateMeshData(Mesh *meshArg, Engine *engine) {
        AbyssEngine::Mesh *mesh = meshArg;

        glUniformMatrix4fv(this->uniformA, 1, 0, engine->worldViewProjMatrix);

        if (this->dirty != 0) {
            glUniform4fv(this->uniformF, 1, engine->glColor);
            glUniform3f(this->uniformB, engine->lightDir.x, engine->lightDir.y,
                        engine->lightDir.z);
            glUniform3f(this->uniformC, engine->lightColor.x, engine->lightColor.y,
                        engine->lightColor.z);
            this->dirty = 0;
        }

        glEnableVertexAttribArray(this->attrPosition);
        glEnableVertexAttribArray(this->attrNormal);
        glEnableVertexAttribArray(this->attrTangent);
        glEnableVertexAttribArray(this->attrBinormal);
        glEnableVertexAttribArray(this->attrTexCoord);

        if (mesh->uploaded != 0) {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->attrPosition, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->attrNormal, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->attrTangent, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->tangentVBO);
            glVertexAttribPointer(this->attrBinormal, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->binormalVBO);
            glVertexAttribPointer(this->attrTexCoord, 3, 0x1406, 0, 0, 0);
            return;
        }

        glVertexAttribPointer(this->attrPosition, 3, 0x1406, 0, 0, mesh->positions);
        glVertexAttribPointer(this->attrNormal, 2, 0x1406, 0, 0, mesh->texCoords);
        glVertexAttribPointer(this->attrTangent, 3, 0x1406, 0, 0, mesh->normals);
        glVertexAttribPointer(this->attrBinormal, 3, 0x1406, 0, 0, mesh->tangents);
        glVertexAttribPointer(this->attrTexCoord, 3, 0x1406, 0, 0, mesh->binormals);
    }

    void SandboxShader::Init(Engine *) {
        this->program = this->ES2LoadProgram("SandboxShader.vsh", "SandboxShader.fsh");

        this->attrPosition = glGetAttribLocation(this->program, "a_position");
        this->attrNormal = glGetAttribLocation(this->program, "a_normal");
        this->attrTangent = glGetAttribLocation(this->program, "a_tangent");
        this->attrBinormal = glGetAttribLocation(this->program, "a_binormal");
        this->attrTexCoord = glGetAttribLocation(this->program, "a_texCoord");

        this->uniformA = glGetUniformLocation(this->program, "u_a");
        this->uniformB = glGetUniformLocation(this->program, "u_b");
        this->uniformC = glGetUniformLocation(this->program, "u_c");
        this->uniformD = glGetUniformLocation(this->program, "u_d");
        this->uniformE = glGetUniformLocation(this->program, "u_e");
        this->uniformF = glGetUniformLocation(this->program, "u_f");
        this->uniformG = glGetUniformLocation(this->program, "u_g");

        glUseProgram(this->program);
        glUniform1i(this->uniformD, 0);
        glUniform1i(this->uniformE, 1);
    }

    void SandboxShader::SetInActive() {
        glDisableVertexAttribArray(this->attrPosition);
        glDisableVertexAttribArray(this->attrNormal);
        glDisableVertexAttribArray(this->attrTangent);
        glDisableVertexAttribArray(this->attrBinormal);
        glDisableVertexAttribArray(this->attrTexCoord);
    }

    SandboxShader::SandboxShader() {
        SandboxShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        *SandboxShader_registerDst = SandboxShader_registerSrc;
        this->name = u"SandboxShader";
    }
}
