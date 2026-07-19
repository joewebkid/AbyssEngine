#include "game/core/TextureConference.h"
#include <GLES2/gl2.h>
#include "engine/core/ApplicationManager.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"

namespace AbyssEngine {
    int TextureConference::ShaderIndex;

    TextureConference::TextureConference() {
        ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"TextureConference";
        this->animTime = 0;
    }

    void TextureConference::Init(Engine *) {
        this->program = this->ES2LoadProgram("TextureConference.vsh", "TextureConference.fsh");

        this->sTexture = glGetUniformLocation(this->program, "u_texture");
        this->aPosition = glGetAttribLocation(this->program, "a_position");
        this->aTexCoord = glGetAttribLocation(this->program, "a_texCoord");
        this->uMvpMatrix = glGetUniformLocation(this->program, "u_mvp");
        this->uColor = glGetUniformLocation(this->program, "u_color");
        this->uOffset = glGetUniformLocation(this->program, "u_offset");

        glUseProgram(this->program);
        glUniform1i(this->sTexture, 0);
    }

    void TextureConference::UpdateMeshData(Mesh *mesh, Engine *engine) {
        glUniformMatrix4fv(this->uMvpMatrix, 1, 0, engine->worldViewProjMatrix);

        if (this->dirty != 0) {
            glUniform4fv(this->uOffset, 1, engine->glColor);
            this->dirty = 0;
        }

        long long elapsed =
                (long long) ((ApplicationManager *) engine->field_0x30)->GetElapsedTimeMillis();
        long long t = this->animTime + elapsed / 5;
        this->animTime = (t < 0xe10) ? t : (t - 0xe10);
        glUniform1i(this->uColor, (int) this->animTime);

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
