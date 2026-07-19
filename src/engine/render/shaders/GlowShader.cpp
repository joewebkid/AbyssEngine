#include "engine/render/shaders/GlowShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int GlowShader::ShaderIndex;

    GlowShader::GlowShader() {
        GlowShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"GlowShader";
    }

    void GlowShader::Init(Engine *) {
        this->program = this->ES2LoadProgram("GlowShader.vsh", "GlowShader.fsh");

        this->a_positionLoc = glGetAttribLocation(this->program, "a_position");
        this->a_texCoordLoc = glGetAttribLocation(this->program, "a_texCoord");

        this->u_mvpLoc = glGetUniformLocation(this->program, "u_mvp");
        this->u_colorLoc = glGetUniformLocation(this->program, "u_color");
        this->u_textureLoc = glGetUniformLocation(this->program, "u_texture");

        glUseProgram(this->program);
        glUniform1i(this->u_textureLoc, 0);
    }

    void GlowShader::SetInActive() {
        if (this->a_positionLoc >= 0)
            glDisableVertexAttribArray(this->a_positionLoc);
        if (this->a_texCoordLoc >= 0)
            glDisableVertexAttribArray(this->a_texCoordLoc);
    }

    void GlowShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->u_mvpLoc >= 0)
            glUniformMatrix4fv(this->u_mvpLoc, 1, 0, engine->worldViewProjMatrix);
        if (this->u_colorLoc >= 0)
            glUniformMatrix3fv(this->u_colorLoc, 1, 0, engine->normalMatrix);

        if (this->a_positionLoc >= 0)
            glEnableVertexAttribArray(this->a_positionLoc);
        if (this->a_texCoordLoc >= 0)
            glEnableVertexAttribArray(this->a_texCoordLoc);

        if (mesh->uploaded != 0) {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->a_positionLoc, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->a_texCoordLoc, 2, 0x1406, 0, 0, 0);
            return;
        }

        if (this->a_positionLoc >= 0)
            glVertexAttribPointer(this->a_positionLoc, 3, 0x1406, 0, 0, mesh->positions);
        if (this->a_texCoordLoc >= 0)
            glVertexAttribPointer(this->a_texCoordLoc, 2, 0x1406, 0, 0, mesh->texCoords);
    }
}
