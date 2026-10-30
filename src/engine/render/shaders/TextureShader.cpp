#include "engine/render/shaders/TextureShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include "engine/render/shaders/EmbeddedTextureShader.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int TextureShader::ShaderIndex;

    TextureShader::TextureShader() {
        TextureShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("TextureShader");
        this->name = shaderName;
    }

    void TextureShader::Init(Engine *) {
        this->program = this->ES2LoadProgram(EmbeddedTextureShader::vertex,
                                             EmbeddedTextureShader::fragment);
        ConnectShaderComponents(this->program, 0);
        this->programExt = this->ES2LoadProgram(EmbeddedTextureShader::vertex,
                                                EmbeddedTextureShader::fogFragment);
        ConnectShaderComponents(this->programExt, 1);
    }

    void TextureShader::UseShader(bool) {
        if (Engine::fogEnabled && this->programExt != 0) {
            glUseProgram(this->programExt);
            return;
        }
        glUseProgram(this->program);
    }

    void TextureShader::SetInActive() {
        for (uint32_t i = 0; i != 2; i += 1) {
            glDisableVertexAttribArray(this->positionAttrib[i]);
            glDisableVertexAttribArray(this->texcoordAttrib[i]);
        }
    }

    void TextureShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        int slot = Engine::fogEnabled ? 1 : 0;

        glUniformMatrix4fv(this->mvpUniform[slot], 1, 0, engine->worldViewProjMatrix);

        int location = this->worldViewUniform[slot];
        if (location >= 0) {
            glUniformMatrix4fv(location, 1, 0, engine->uvMatrix);
        }

        location = this->alphaUniform[slot];
        if (location >= 0) {
            glUniform1f(location, 1.0f - reinterpret_cast<float &>(mesh->materialId));
        }

        if (this->dirty != 0) {
            glUniform4fv(this->colorUniform[slot], 1, engine->glColor);

            location = this->textureModeUniform[slot];
            if (location >= 0) {
                glUniform1i(location, mesh->hasAnimation);
            }

            location = this->lightUniform[slot];
            if (location >= 0) {
                glUniform3fv(location, 1, engine->fogColor);
            }

            location = this->fogFarUniform[slot];
            if (location >= 0) {
                glUniform1f(location, engine->fogMinDist);
            }

            location = this->fogNearUniform[slot];
            if (location >= 0) {
                glUniform1f(location, engine->fogMaxDist);
            }

            location = this->activeTextureUniform[slot];
            if (location >= 0) {
                glUniform1i(location, Engine::fogEnabled ? 1 : 0);
            }

            location = this->fogColorUniform[slot];
            if (location >= 0) {
                glUniform3f(location, engine->lightColor.x, engine->lightColor.y, engine->lightColor.z);
            }

            this->dirty = 0;
        }

        if ((int) (mesh->vertexFormat << 30) < 0) {
            glEnableVertexAttribArray(this->positionAttrib[slot]);
            glEnableVertexAttribArray(this->texcoordAttrib[slot]);

            if (mesh->uploaded == 0) {
                glVertexAttribPointer(this->positionAttrib[slot], 3, 0x1406, 0, 0, mesh->positions);
                glVertexAttribPointer(this->texcoordAttrib[slot], 2, 0x1406, 0, 0, mesh->texCoords);
            } else {
                glBindBuffer(0x8892, mesh->positionVBO);
                glVertexAttribPointer(this->positionAttrib[slot], 3, 0x1406, 0, 0, 0);
                glBindBuffer(0x8892, mesh->texCoordVBO);
                glVertexAttribPointer(this->texcoordAttrib[slot], 2, 0x1406, 0, 0, 0);
            }
        }
    }

    void TextureShader::ConnectShaderComponents(uint32_t program, int slot) {
        this->textureUniform[slot] = glGetUniformLocation(program, "s_texture");
        this->positionAttrib[slot] = glGetAttribLocation(program, "a_position");
        this->texcoordAttrib[slot] = glGetAttribLocation(program, "a_texCoord");
        this->mvpUniform[slot] = glGetUniformLocation(program, "u_WorldMatrix");
        this->colorUniform[slot] = glGetUniformLocation(program, "glColor");
        this->lightUniform[slot] = glGetUniformLocation(program, "u_fogColor");
        this->fogNearUniform[slot] = glGetUniformLocation(program, "u_fogMaxDist");
        this->fogFarUniform[slot] = glGetUniformLocation(program, "u_fogMinDist");
        this->activeTextureUniform[slot] = glGetUniformLocation(program, "u_EnableFog");
        this->fogColorUniform[slot] = glGetUniformLocation(program, "u_eyeposmodel");
        this->alphaUniform[slot] = glGetUniformLocation(program, "u_DarkenValue");
        this->textureModeUniform[slot] = glGetUniformLocation(program, "u_UVAnimation");
        this->worldViewUniform[slot] = glGetUniformLocation(program, "u_UvMatrix");

        glUseProgram(program);
        glUniform1i(this->textureUniform[slot], 0);
    }
}
