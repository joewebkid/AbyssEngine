#include "engine/render/shaders/TextureAlphaTestShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include "engine/render/shaders/EmbeddedTextureAlphaTestShader.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int TextureAlphaTestShader::ShaderIndex;

    TextureAlphaTestShader::TextureAlphaTestShader() {
        ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("TextureAlphaTestShader");
        this->name = shaderName;
    }

    void TextureAlphaTestShader::Init(Engine *) {
        this->program = this->ES2LoadProgram(EmbeddedTextureAlphaTestShader::vertex,
                                             EmbeddedTextureAlphaTestShader::fragment);
        ConnectShaderComponents(this->program, 0);

        this->alphaProgram = this->ES2LoadProgram(EmbeddedTextureAlphaTestShader::vertex,
                                                  EmbeddedTextureAlphaTestShader::fogFragment);
        ConnectShaderComponents(this->alphaProgram, 1);
    }

    void TextureAlphaTestShader::SetInActive() {
        for (int i = 0; i != 2; i++) {
            glDisableVertexAttribArray(this->aPositionLoc[i]);
            glDisableVertexAttribArray(this->aTexCoordLoc[i]);
        }
    }

    void TextureAlphaTestShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        uint8_t programIndex = Engine::fogEnabled ? 1 : 0;

        glUniformMatrix4fv(this->uMVPMatrixLoc[programIndex], 1, 0, engine->worldViewProjMatrix);

        if (this->dirty != 0) {
            int location = this->uColorLoc[programIndex];
            if (location >= 0) {
                glUniform4fv(location, 1, engine->glColor);
            }

            location = this->uLightPosLoc[programIndex];
            if (location >= 0) {
                glUniform3fv(location, 1, engine->fogColor);
            }

            location = this->uDiffuseLoc[programIndex];
            if (location >= 0) {
                glUniform1f(location, engine->fogMinDist);
            }

            location = this->uAmbientLoc[programIndex];
            if (location >= 0) {
                glUniform1f(location, engine->fogMaxDist);
            }

            location = this->uSamplerLoc[programIndex];
            if (location >= 0) {
                glUniform1i(location, Engine::fogEnabled ? 1 : 0);
            }

            location = this->uFogColorLoc[programIndex];
            if (location >= 0) {
                glUniform3f(location, engine->lightColor.x, engine->lightColor.y,
                            engine->lightColor.z);
            }

            this->dirty = 0;
        }

        if ((((uint32_t) mesh->vertexFormat) << 30) < 0x80000000u) {
            return;
        }

        int position = this->aPositionLoc[programIndex];
        int texcoord = this->aTexCoordLoc[programIndex];
        glEnableVertexAttribArray(position);
        glEnableVertexAttribArray(texcoord);

        if (mesh->uploaded == 0) {
            glVertexAttribPointer(position, 3, 0x1406, 0, 0, mesh->positions);
            glVertexAttribPointer(texcoord, 2, 0x1406, 0, 0, mesh->texCoords);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(position, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(texcoord, 2, 0x1406, 0, 0, 0);
        }
    }

    void TextureAlphaTestShader::ConnectShaderComponents(unsigned int program, int index) {
        this->uTextureLoc[index] = glGetUniformLocation(program, "s_texture");
        this->aPositionLoc[index] = glGetAttribLocation(program, "a_position");
        this->aTexCoordLoc[index] = glGetAttribLocation(program, "a_texCoord");
        this->uMVPMatrixLoc[index] = glGetUniformLocation(program, "u_WorldMatrix");
        this->uColorLoc[index] = glGetUniformLocation(program, "glColor");
        this->uLightPosLoc[index] = glGetUniformLocation(program, "u_fogColor");
        this->uAmbientLoc[index] = glGetUniformLocation(program, "u_fogMaxDist");
        this->uDiffuseLoc[index] = glGetUniformLocation(program, "u_fogMinDist");
        this->uSamplerLoc[index] = glGetUniformLocation(program, "u_EnableFog");
        this->uFogColorLoc[index] = glGetUniformLocation(program, "u_eyeposmodel");

        glUseProgram(program);
        glUniform1i(this->uTextureLoc[index], 0);
    }

    void TextureAlphaTestShader::UseShader(bool) {
        if (Engine::fogEnabled && this->alphaProgram != 0) {
            int program = this->alphaProgram;
            if (program != 0) {
                glUseProgram(program);
                return;
            }
        }
        glUseProgram(this->program);
    }
}
