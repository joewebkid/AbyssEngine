#include "engine/render/shaders/BlurShader.h"
#include "engine/render/FBOContainer.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include "engine/render/PaintCanvas.h"
#include "engine/core/ApplicationManager.h"
#include <GLES2/gl2.h>

unsigned int Engine_GetDisplayWidth(::Engine * engine);
unsigned int Engine_GetDisplayHeight(::Engine * engine);

void Engine_DrawQuad(::Engine *engine, int x, int y, int width, int height);

void Engine_SetWorldViewMatrix(::Engine *engine, const uint32_t *matrix);

namespace AbyssEngine {
    int BlurShader::ShaderIndex;

    BlurShader::BlurShader() {
        BlurShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"BlurShader";
        this->strength = -4.051786824441422e-28f;
        this->blurScale = 2.0f;
    }

    BlurShader::~BlurShader() {
    }

    void BlurShader::Init(Engine *) {
        this->program = this->ES2LoadProgram("BlurShader.vsh", "BlurShader.fsh");
        this->aPosition = glGetAttribLocation(this->program, "a_position");
        this->aTexCoord = glGetAttribLocation(this->program, "a_texCoord");
        this->uMvpMatrix = glGetUniformLocation(this->program, "u_mvpMatrix");
        this->sTexture = glGetUniformLocation(this->program, "s_texture");
        this->uTexelSize = glGetUniformLocation(this->program, "u_texelSize");
        this->uCenter = glGetUniformLocation(this->program, "u_center");
        this->uBlurAmount = glGetUniformLocation(this->program, "u_blurAmount");
        this->uStrength = glGetUniformLocation(this->program, "u_strength");
        glUseProgram(this->program);
        glUniform1i(this->sTexture, 0);
    }

    void BlurShader::SetInActive() {
        glDisableVertexAttribArray(this->positionAttrib);
        glDisableVertexAttribArray(this->texCoordAttrib);
    }

    void BlurShader::RenderEffect(FBOContainer *fbo, FBOContainer *&target, Engine *engine,
                                  float amount, Vector vector) {
        float matrix[16] = {};
        matrix[0] = 1.0f;
        matrix[5] = 1.0f;
        matrix[10] = 1.0f;
        matrix[15] = 1.0f;

        engine->projMatrix[0] = 2.0f / (float) Engine_GetDisplayWidth(engine);
        engine->projMatrix[5] = -(2.0f / (float) Engine_GetDisplayHeight(engine));
        engine->projMatrix[10] = -1.0f;
        engine->projMatrix[12] = -1.0f;
        engine->projMatrix[13] = 1.0f;
        engine->projMatrix[15] = 1.0f;
        Engine_SetWorldViewMatrix(engine, (const uint32_t *) matrix);

        glDisable(0xb71);
        glDepthMask(0);
        glDisable(0xbe2);
        glUseProgram(this->program);
        glActiveTexture(0x84c0);
        fbo->Activate();

        if (target == 0) {
            glBindFramebuffer(0x8d40, engine->field_0x40c);
            int width;
            int height;
            if (engine->appManager->paintCanvas->gameOrientation == 2) {
                width = Engine_GetDisplayWidth(engine);
                height = Engine_GetDisplayHeight(engine);
            } else {
                width = Engine_GetDisplayHeight(engine);
                height = Engine_GetDisplayWidth(engine);
            }
            glViewport(0, 0, width, height);
        } else {
            target->BeginCapture();
        }

        int position = this->aPosition;
        if (position >= 0) {
            glEnableVertexAttribArray(position);
        }
        int texCoord = this->aTexCoord;
        if (texCoord >= 0) {
            glEnableVertexAttribArray(texCoord);
        }
        int matrixLocation = this->uMvpMatrix;
        if (matrixLocation >= 0) {
            glUniformMatrix4fv(matrixLocation, 1, 0, engine->worldViewProjMatrix);
        }
        int texelLocation = this->uTexelSize;
        if (texelLocation >= 0) {
            float width;
            int other;
            if (engine->appManager->paintCanvas->gameOrientation == 2) {
                width = (float) Engine_GetDisplayWidth(engine);
                other = Engine_GetDisplayHeight(engine);
            } else {
                width = (float) Engine_GetDisplayHeight(engine);
                other = Engine_GetDisplayWidth(engine);
            }
            glUniform2f(texelLocation, 1.0f / width, 1.0f / (float) other);
        }
        int centerLocation = this->uCenter;
        if (centerLocation >= 0) {
            glUniform2f(centerLocation, vector[0], vector[1]);
        }
        int strengthLocation = this->uStrength;
        if (strengthLocation >= 0) {
            glUniform1f(strengthLocation, this->strength);
        }
        int amountLocation = this->uBlurAmount;
        if (amountLocation >= 0) {
            float scaled = amount;
            if (scaled < 0.2f) {
                scaled = 0.2f;
            }
            glUniform1f(amountLocation, scaled * this->blurScale);
        }
        if (position >= 0) {
            glVertexAttribPointer(position, 3, 0x1406, 0, 0,
                                  engine->quadMesh->positions);
        }
        if (texCoord >= 0) {
            glVertexAttribPointer(texCoord, 2, 0x1406, 0, 0,
                                  engine->quadMesh->texCoords);
        }

        glClear(0x4000);
        Engine_DrawQuad(engine, 0, 0, Engine_GetDisplayWidth(engine), Engine_GetDisplayHeight(engine));
        if (position >= 0) {
            glDisableVertexAttribArray(position);
        }
        if (texCoord >= 0) {
            glDisableVertexAttribArray(texCoord);
        }
        if (target != 0) {
            target->EndCapture();
        }
        glEnable(0xbe2);
        glBlendFunc(0x302, 0x303);
        glActiveTexture(0x84c0);
    }

    void BlurShader::RenderEffect(FBOContainer *fbo, Engine *engine, float amount, Vector vector) {
        FBOContainer *target = nullptr;
        this->RenderEffect(fbo, target, engine, amount, vector);
    }

    void BlurShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        int matrixLocation = this->uMvpMatrix;
        if (matrixLocation >= 0) {
            glUniformMatrix4fv(matrixLocation, 1, 0, engine->worldViewProjMatrix);
        }

        int texelLocation = this->uTexelSize;
        if (texelLocation >= 0) {
            float width;
            float height;
            if (engine->appManager->paintCanvas->gameOrientation == 2) {
                width = (float) Engine_GetDisplayWidth(engine);
                height = (float) Engine_GetDisplayHeight(engine);
            } else {
                width = (float) Engine_GetDisplayHeight(engine);
                height = (float) Engine_GetDisplayWidth(engine);
            }
            glUniform2f(texelLocation, 1.0f / width, 1.0f / height);
        }

        int centerLocation = this->uCenter;
        if (centerLocation >= 0) {
            glUniform2f(centerLocation, 0.5f, 0.5f);
        }

        int strengthLocation = this->uStrength;
        if (strengthLocation >= 0) {
            glUniform1f(strengthLocation, this->strength);
        }

        int amountLocation = this->uBlurAmount;
        if (amountLocation >= 0) {
            glUniform1f(amountLocation, this->blurScale);
        }

        if (this->dirty != 0) {
            this->dirty = 0;
        }

        glEnableVertexAttribArray(this->positionAttrib);
        glEnableVertexAttribArray(this->texCoordAttrib);

        if (mesh->uploaded == 0) {
            glVertexAttribPointer(this->positionAttrib, 3, 0x1406, 0, 0, mesh->positions);
            glVertexAttribPointer(this->texCoordAttrib, 2, 0x1406, 0, 0, mesh->texCoords);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->positionAttrib, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->texCoordAttrib, 2, 0x1406, 0, 0, 0);
        }
    }
}
