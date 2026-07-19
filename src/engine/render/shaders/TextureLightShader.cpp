#include "engine/render/shaders/TextureLightShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int TextureLightShader::ShaderIndex;

    TextureLightShader::TextureLightShader() {
        TextureLightShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"TextureLightShader";
    }

    void TextureLightShader::Init(Engine *) {
        this->program = this->ES2LoadProgram("TextureLightShader.vsh", "TextureLightShader.fsh");

        this->uModelMatrix = glGetUniformLocation(this->program, "u0");
        this->uHasScaleAnimation = glGetUniformLocation(this->program, "u1");

        this->aPosition = glGetAttribLocation(this->program, "a0");
        this->aTexCoord = glGetAttribLocation(this->program, "a1");

        this->aNormal = glGetUniformLocation(this->program, "u2");
        this->uMvpMatrix = glGetUniformLocation(this->program, "u3");
        this->uModelViewMatrix = glGetUniformLocation(this->program, "u4");
        this->uNormalMatrix = glGetUniformLocation(this->program, "u5");
        this->uColor0 = glGetUniformLocation(this->program, "u6");
        this->uColor1 = glGetUniformLocation(this->program, "u7");
        this->uSpecularColor = glGetUniformLocation(this->program, "u8");
        this->uTexture = glGetUniformLocation(this->program, "u9");
        this->uAmbientColor = glGetUniformLocation(this->program, "u10");
        this->uLight0Direction = glGetUniformLocation(this->program, "u11");
        this->uLight1Direction = glGetUniformLocation(this->program, "u12");
        this->uLight2Direction = glGetUniformLocation(this->program, "u13");
        this->uShininess = glGetUniformLocation(this->program, "u14");
        this->uLight0Color = glGetUniformLocation(this->program, "u15");
        this->uLight1Color = glGetUniformLocation(this->program, "u16");
        this->uLight2Color = glGetUniformLocation(this->program, "u17");

        glUseProgram(this->program);
        glUniform1i(this->uTexture, 0);
    }

    void TextureLightShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        glUniformMatrix4fv(this->uMvpMatrix, 1, 0, engine->worldViewProjMatrix);
        if (this->uModelMatrix >= 0)
            glUniformMatrix4fv(this->uModelMatrix, 1, 0, engine->uvMatrix);
        glUniformMatrix3fv(this->uNormalMatrix, 1, 0, engine->normalMatrix);
        if (this->uModelViewMatrix >= 0)
            glUniformMatrix4fv(this->uModelViewMatrix, 1, 0, engine->modelMatrixGL);

        if (this->dirty != 0) {
            if (this->uHasScaleAnimation >= 0)
                glUniform1i(this->uHasScaleAnimation, mesh->hasAnimation);
            glUniform4fv(this->uAmbientColor, 1, engine->glColor);

            glUniform3fv(this->uLight0Direction, 1, (float *) &engine->lightAmbientShaded);
            glUniform3fv(this->uLight1Direction, 1, (float *) &engine->field_0x2fc);
            glUniform3fv(this->uLight2Direction, 1, (float *) &engine->lightDiffuseShaded);

            glUniform4f(this->uColor0, engine->lightDir.x, engine->lightDir.y,
                        engine->lightDir.z, engine->lightDirty[0]);

            if (engine->field_0x32c < 2) {
                glUniform3f(this->uLight0Color, 0, 0, 0);
                glUniform3f(this->uLight1Color, 0, 0, 0);
                glUniform3f(this->uLight2Color, 0, 0, 0);
            } else {
                glUniform3fv(this->uLight0Color, 1, (float *) &engine->lightSpecularShaded);
                glUniform3fv(this->uLight1Color, 1, (float *) &engine->field_0x308);
                glUniform3fv(this->uLight2Color, 1, (float *) &engine->particleAmbient);
            }

            glUniform4f(this->uColor1, engine->field_0x33c.x, engine->field_0x33c.y,
                        engine->field_0x33c.z, engine->lightDirty[1]);
            glUniform1f(this->uShininess, engine->materialShininess);
            if (this->uSpecularColor >= 0)
                glUniform3f(this->uSpecularColor, engine->lightColor.x, engine->lightColor.y,
                            engine->lightColor.z);
            this->dirty = 0;
        }

        glEnableVertexAttribArray(this->aPosition);
        glEnableVertexAttribArray(this->aTexCoord);
        glEnableVertexAttribArray(this->aNormal);

        if (mesh->uploaded != 0) {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->aTexCoord, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->aNormal, 3, 0x1406, 0, 0, 0);
            return;
        }

        glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, mesh->positions);
        glVertexAttribPointer(this->aTexCoord, 2, 0x1406, 0, 0, mesh->texCoords);
        glVertexAttribPointer(this->aNormal, 3, 0x1406, 0, 0, mesh->normals);
    }

    void TextureLightShader::SetInActive() {
        glDisableVertexAttribArray(this->aPosition);
        glDisableVertexAttribArray(this->aTexCoord);
        glDisableVertexAttribArray(this->aNormal);
    }
}
