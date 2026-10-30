#include "game/core/TextureConference.h"
#include "engine/render/shaders/EmbeddedTextureConference.h"
#include <GLES2/gl2.h>
#include "engine/core/ApplicationManager.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"

namespace AbyssEngine {
    int TextureConference::ShaderIndex;

    TextureConference::TextureConference() {
        ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("TextureConference");
        this->name = shaderName;
        this->animTime = 0;
    }

    void TextureConference::Init(Engine *) {
        this->program = this->ES2LoadProgram(EmbeddedTextureConference::vertex,
                                             EmbeddedTextureConference::fragment);

        this->sTexture = glGetUniformLocation(this->program, "s_texture");
        this->aPosition = glGetAttribLocation(this->program, "a_position");
        this->aTexCoord = glGetAttribLocation(this->program, "a_texCoord");
        this->uWorldMatrix = glGetUniformLocation(this->program, "u_WorldMatrix");
        this->uTime = glGetUniformLocation(this->program, "u_time");
        this->glColor = glGetUniformLocation(this->program, "glColor");

        glUseProgram(this->program);
        glUniform1i(this->sTexture, 0);
    }

    void TextureConference::UpdateMeshData(Mesh *mesh, Engine *engine) {
        glUniformMatrix4fv(this->uWorldMatrix, 1, 0, engine->worldViewProjMatrix);

        if (this->dirty != 0) {
            glUniform4fv(this->glColor, 1, engine->glColor);
            this->dirty = 0;
        }

        long long elapsed =
                (long long) ((ApplicationManager *) engine->field_0x30)->GetElapsedTimeMillis();
        long long t = this->animTime + elapsed / 5;
        this->animTime = (t > 0xe10) ? (t - 0xe10) : t;
        glUniform1i(this->uTime, (int) this->animTime);

        glEnableVertexAttribArray(this->aPosition);
        glEnableVertexAttribArray(this->aTexCoord);
        glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, mesh->positions);
        if ((mesh->vertexFormat & 2) != 0)
            glVertexAttribPointer(this->aTexCoord, 2, 0x1406, 0, 0, mesh->texCoords);
    }

    void TextureConference::SetInActive() {
        glDisableVertexAttribArray(this->aPosition);
        glDisableVertexAttribArray(this->aTexCoord);
    }
}
