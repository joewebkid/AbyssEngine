#include "engine/render/shaders/TextureVtxColorShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    static inline bool EngineFogEnabled() { return Engine::fogEnabled; }

    int TextureVtxColorShader::ShaderIndex;

    TextureVtxColorShader::TextureVtxColorShader() {
        this->name = u"TextureVtxColorShader";
        TextureVtxColorShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
    }

    void TextureVtxColorShader::Init(Engine *) {
        const char *vertexShader =
                "attribute highp vec4 a_position;\n"
                "attribute mediump vec2 a_texCoord;\n"
                "attribute mediump vec4 a_VertexColor;\n";
        const char *fragmentShader =
                "precision lowp float;\n"
                "varying mediump vec2 v_texCoord;\n"
                "varying mediump vec4 v_VertexColor;\n";

        this->program = this->ES2LoadProgram(vertexShader, fragmentShader);
        ConnectShaderComponents(this->program, 0);

        this->fogProgram = this->ES2LoadProgram(vertexShader, "\n");
        ConnectShaderComponents(this->fogProgram, 1);
    }

    void TextureVtxColorShader::ConnectShaderComponents(uint32_t program, int index) {
        loc_s_texture[index] = glGetUniformLocation(program, "s_texture");
        loc_a_position[index] = glGetAttribLocation(program, "a_position");
        loc_a_texCoord[index] = glGetAttribLocation(program, "a_texCoord");
        loc_a_VertexColor[index] = glGetAttribLocation(program, "a_VertexColor");
        loc_u_WorldMatrix[index] = glGetUniformLocation(program, "u_WorldMatrix");
        loc_glColor[index] = glGetUniformLocation(program, "glColor");
        loc_u_DarkenValue[index] = glGetUniformLocation(program, "u_DarkenValue");
        loc_u_fogColor[index] = glGetUniformLocation(program, "u_fogColor");
        loc_u_fogMaxDist[index] = glGetUniformLocation(program, "u_fogMaxDist");
        loc_u_fogMinDist[index] = glGetUniformLocation(program, "u_fogMinDist");
        loc_u_EnableFog[index] = glGetUniformLocation(program, "u_EnableFog");
        loc_u_eyeposmodel[index] = glGetUniformLocation(program, "u_eyeposmodel");
        loc_u_UVAnimation[index] = glGetUniformLocation(program, "u_UVAnimation");
        loc_u_UvMatrix[index] = glGetUniformLocation(program, "u_UvMatrix");

        glUseProgram(program);
        glUniform1i(loc_s_texture[index], 0);
    }

    void TextureVtxColorShader::UseShader(bool) {
        if (EngineFogEnabled() && this->fogProgram != 0) {
            glUseProgram(this->fogProgram);
            return;
        }
        glUseProgram(this->program);
    }

    void TextureVtxColorShader::SetInActive() {
        for (int i = 0; i != 2; i++) {
            glDisableVertexAttribArray((uint32_t) loc_a_position[i]);
            glDisableVertexAttribArray((uint32_t) loc_a_texCoord[i]);
            glDisableVertexAttribArray((uint32_t) loc_a_VertexColor[i]);
        }
    }

    void TextureVtxColorShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        int index = EngineFogEnabled() ? 1 : 0;

        glUniformMatrix4fv(loc_u_WorldMatrix[index], 1, 0, engine->worldViewProjMatrix);

        int location = loc_u_UvMatrix[index];
        if (location >= 0) {
            glUniformMatrix4fv(location, 1, 0, engine->uvMatrix);
        }

        location = loc_u_DarkenValue[index];
        if (location >= 0) {
            glUniform1f(location, 1.0f - (float) mesh->materialId);
        }

        if (this->dirty != 0) {
            glUniform4fv(loc_glColor[index], 1, engine->glColor);

            location = loc_u_UVAnimation[index];
            if (location >= 0) {
                glUniform1i(location, mesh->hasAnimation);
            }

            location = loc_u_fogColor[index];
            if (location >= 0) {
                glUniform3fv(location, 1, (float *) &engine->fogColor);
            }

            location = loc_u_fogMinDist[index];
            if (location >= 0) {
                glUniform1f(location, engine->fogMinDist);
            }

            location = loc_u_fogMaxDist[index];
            if (location >= 0) {
                glUniform1f(location, engine->fogMaxDist);
            }

            location = loc_u_EnableFog[index];
            if (location >= 0) {
                glUniform1i(location, EngineFogEnabled() ? 1 : 0);
            }

            location = loc_u_eyeposmodel[index];
            if (location >= 0) {
                glUniform3f(location,
                            engine->lightColor.x,
                            engine->lightColor.y,
                            engine->lightColor.z);
            }

            this->dirty = 0;
        }

        glEnableVertexAttribArray((uint32_t) loc_a_position[index]);
        glEnableVertexAttribArray((uint32_t) loc_a_texCoord[index]);
        glEnableVertexAttribArray((uint32_t) loc_a_VertexColor[index]);

        if (mesh->uploaded != 0) {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer((uint32_t) loc_a_position[index], 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer((uint32_t) loc_a_texCoord[index], 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->colorVBO);
            glVertexAttribPointer((uint32_t) loc_a_VertexColor[index], 4, 0x1406, 0, 0, 0);
            return;
        }

        glVertexAttribPointer((uint32_t) loc_a_position[index], 3, 0x1406, 0, 0,
                              mesh->positions);
        glVertexAttribPointer((uint32_t) loc_a_texCoord[index], 2, 0x1406, 0, 0,
                              mesh->texCoords);
        glVertexAttribPointer((uint32_t) loc_a_VertexColor[index], 4, 0x1406, 0, 0,
                              mesh->colors);
    }
}
