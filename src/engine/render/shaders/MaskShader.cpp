#include "engine/render/shaders/MaskShader.h"
#include "engine/render/shaders/EmbeddedMaskShader.h"
#include "engine/core/ApplicationManager.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/Image2D.h"

#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int MaskShader::ShaderIndex;

    MaskShader::MaskShader() {
        ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("MaskShader");
        this->name = shaderName;
    }

    void MaskShader::Init(::Engine *) {
        this->program = (int) ES2LoadProgram(EmbeddedMaskShader::vertex, EmbeddedMaskShader::fragment);

        this->a_position = glGetAttribLocation(this->program, "a_position");
        this->a_texCoord0 = glGetAttribLocation(this->program, "a_texCoord_0");
        this->a_texCoord1 = glGetAttribLocation(this->program, "a_texCoord_1");

        this->uWorldMatrix = glGetUniformLocation(this->program, "u_WorldMatrix");
        this->sTexture0 = glGetUniformLocation(this->program, "s_texture[0]");
        this->sTexture1 = glGetUniformLocation(this->program, "s_texture[1]");
        this->glColorLoc = glGetUniformLocation(this->program, "glColor");

        glUseProgram(this->program);
        for (int i = 0; i != 2; i++) {
            int loc = (&this->sTexture0)[i];
            if (loc >= 0)
                glUniform1i(loc, i);
        }
    }

    void MaskShader::SetInActive() {
        if (this->a_position >= 0)
            glDisableVertexAttribArray(this->a_position);
        if (this->a_texCoord0 >= 0)
            glDisableVertexAttribArray(this->a_texCoord0);
        if (this->a_texCoord1 >= 0)
            glDisableVertexAttribArray(this->a_texCoord1);
    }

    void MaskShader::UpdateMeshData(Mesh *mesh, ::Engine *engine) {
        if (this->uWorldMatrix >= 0)
            glUniformMatrix4fv(this->uWorldMatrix, 1, 0, engine->worldViewProjMatrix);

        if (this->dirty != 0) {
            if (this->glColorLoc >= 0)
                glUniform4fv(this->glColorLoc, 1, engine->glColor);
            this->dirty = 0;
        }

        if (this->a_position >= 0)
            glEnableVertexAttribArray(this->a_position);
        if (this->a_texCoord0 >= 0)
            glEnableVertexAttribArray(this->a_texCoord0);
        if (this->a_texCoord1 >= 0)
            glEnableVertexAttribArray(this->a_texCoord1);

        Image2D *mask = reinterpret_cast<Image2D *>(engine->appManager->paintCanvas->mask2dImage);
        if (mask != 0)
            ((::Engine *) engine)->SetTextureSlot(mask->textureId, 1);

        if (mesh->uploaded == 0) {
            if (this->a_position >= 0)
                glVertexAttribPointer(this->a_position, 3, 0x1406, 0, 0, mesh->positions);
            if (this->a_texCoord0 >= 0)
                glVertexAttribPointer(this->a_texCoord0, 2, 0x1406, 0, 0, mesh->texCoords);
            if (mask == 0)
                return;
            if (this->a_texCoord1 < 0)
                return;
            glVertexAttribPointer(this->a_texCoord1, 2, 0x1406, 0, 0, mask->mesh->texCoords);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->a_position, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->a_texCoord0, 2, 0x1406, 0, 0, 0);
            if (mask == 0)
                return;
            glBindBuffer(0x8892, mask->mesh->texCoordVBO);
            glVertexAttribPointer(this->a_texCoord1, 2, 0x1406, 0, 0, 0);
        }
    }
}
