#include "engine/render/shaders/TextureShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

static uint8_t g_TextureShader_hasSecondProgram = 0;
static uint8_t g_TextureShader_activeSlot = 0;

namespace AbyssEngine {
    int TextureShader::ShaderIndex;

    TextureShader::TextureShader() {
        TextureShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"TextureShader";
    }

    void TextureShader::Init(Engine *) {
        const char *vertexShader = "TextureShader.vsh";

        this->program = this->ES2LoadProgram(vertexShader, "TextureShader.fsh");
        ConnectShaderComponents(this->program, 0);

        this->programExt = this->ES2LoadProgram(vertexShader, "TextureShaderExt.fsh");
        ConnectShaderComponents(this->programExt, 1);
    }

    void TextureShader::UseShader(bool) {
        if (g_TextureShader_hasSecondProgram != 0 && this->programExt != 0) {
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
        int slot = g_TextureShader_activeSlot;

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
                glUniform3fv(location, 1, (float *) &engine->fogColor);
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
                glUniform1i(location, g_TextureShader_activeSlot);
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
        this->textureUniform[slot] = glGetUniformLocation(program, "texture");
        this->positionAttrib[slot] = glGetAttribLocation(program, "position");
        this->texcoordAttrib[slot] = glGetAttribLocation(program, "texcoord");
        this->mvpUniform[slot] = glGetUniformLocation(program, "mvp");
        this->colorUniform[slot] = glGetUniformLocation(program, "color");
        this->lightUniform[slot] = glGetUniformLocation(program, "light");
        this->fogNearUniform[slot] = glGetUniformLocation(program, "fogNear");
        this->fogFarUniform[slot] = glGetUniformLocation(program, "fogFar");
        this->activeTextureUniform[slot] = glGetUniformLocation(program, "activeTexture");
        this->fogColorUniform[slot] = glGetUniformLocation(program, "fogColor");
        this->alphaUniform[slot] = glGetUniformLocation(program, "alpha");
        this->textureModeUniform[slot] = glGetUniformLocation(program, "textureMode");
        this->worldViewUniform[slot] = glGetUniformLocation(program, "worldView");

        glUseProgram(program);
        glUniform1i(this->textureUniform[slot], 0);
    }
}
