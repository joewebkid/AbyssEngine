#include "engine/render/shaders/SimpleShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#if defined(__ANDROID__) && __SIZEOF_POINTER__ == 8
#include "engine/render/shaders/EmbeddedBasicShaders.h"
#endif
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int SimpleShader::ShaderIndex;

    SimpleShader::SimpleShader() {
        SimpleShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("SimpleShader");
        this->name = shaderName;
    }

    void SimpleShader::Init(Engine *) {
#if defined(__ANDROID__) && __SIZEOF_POINTER__ == 8
        this->program = (int) this->ES2LoadProgram(EmbeddedSimpleShader::vertex,
                                                   EmbeddedSimpleShader::fragment);
#else
        this->program = (int) this->ES2LoadProgram("SimpleShader.vsh", "SimpleShader.fsh");
#endif
        this->aPosition = glGetAttribLocation(this->program, "a_position");
        this->uWorldMatrix = glGetUniformLocation(this->program, "u_WorldMatrix");
#if defined(__ANDROID__) && __SIZEOF_POINTER__ == 8
        this->uColor = glGetUniformLocation(this->program, "glColor");
#else
        this->uColor = glGetUniformLocation(this->program, "u_color");
#endif
        glUseProgram(this->program);
    }

    void SimpleShader::SetInActive() {
        glDisableVertexAttribArray(this->aPosition);
    }

    void SimpleShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        glUniformMatrix4fv(this->uWorldMatrix, 1, 0, engine->worldViewProjMatrix);
        if (this->dirty != 0) {
            glUniform4fv(this->uColor, 1, engine->glColor);
            this->dirty = 0;
        }
        glEnableVertexAttribArray(this->aPosition);
        if (mesh->uploaded != 0) {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, 0);
            return;
        }
        glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, mesh->positions);
    }
}
