#include "engine/render/shaders/BloomShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include "engine/render/FBOContainer.h"
#include "engine/core/ApplicationManager.h"
#include "engine/render/PaintCanvas.h"
#include <GLES2/gl2.h>
#include <arm_neon.h>


static unsigned char g_BloomShader_internalInitNeeded;
static unsigned int g_BloomShader_shaderMode;

unsigned int Engine_GetDisplayWidth(::Engine * engine);
unsigned int Engine_GetDisplayHeight(::Engine * engine);

void Engine_DrawQuad(::Engine *engine, int x, int y, int width, int height);

void Engine_SetWorldViewMatrix(::Engine *engine, const uint32_t *matrix);

namespace AbyssEngine {
    int BloomShader::ShaderIndex;

    void BloomShader::Init(Engine *) {
        const char *vertex = "BloomShader.vsh";
        const char *positionName = "a_position";
        const char *texCoordName = "a_texCoord";
        const char *matrixName = "u_WorldMatrix";
        const char *samplerName = "s_texture";
        const char *texSizeName = "texSize";

        this->program = this->ES2LoadProgram(vertex, "BloomShaderLuma.fsh");
        this->downSampleProgram = this->ES2LoadProgram(vertex, "BloomShaderDownSample.fsh");
        this->blurHProgram = this->ES2LoadProgram(vertex, "BloomShaderBlurH.fsh");
        this->blurVProgram = this->ES2LoadProgram(vertex, "BloomShaderBlurV.fsh");
        this->finalProgram = this->ES2LoadProgram(vertex, "BloomShaderFinal.fsh");

        this->lumaAttribPosition = glGetAttribLocation(this->program, positionName);
        this->lumaAttribTexCoord = glGetAttribLocation(this->program, texCoordName);
        this->lumaUniformWorldMatrix = glGetUniformLocation(this->program, matrixName);
        this->lumaUniformTexture = glGetUniformLocation(this->program, samplerName);
        glUseProgram(this->program);
        glUniform1i(this->lumaUniformTexture, 0);

        this->downSampleAttribPosition = glGetAttribLocation(this->downSampleProgram, positionName);
        this->downSampleAttribTexCoord = glGetAttribLocation(this->downSampleProgram, texCoordName);
        this->downSampleUniformWorldMatrix = glGetUniformLocation(this->downSampleProgram, matrixName);
        this->downSampleUniformTexture = glGetUniformLocation(this->downSampleProgram, samplerName);
        glUseProgram(this->downSampleProgram);
        glUniform1i(this->downSampleUniformTexture, 0);

        this->blurHAttribPosition = glGetAttribLocation(this->blurHProgram, positionName);
        this->blurHAttribTexCoord = glGetAttribLocation(this->blurHProgram, texCoordName);
        this->blurHUniformWorldMatrix = glGetUniformLocation(this->blurHProgram, matrixName);
        this->blurHUniformTexture = glGetUniformLocation(this->blurHProgram, samplerName);
        this->blurHUniformTexSize = glGetUniformLocation(this->blurHProgram, texSizeName);
        glUseProgram(this->blurHProgram);
        glUniform1i(this->blurHUniformTexture, 0);

        this->blurVAttribPosition = glGetAttribLocation(this->blurVProgram, positionName);
        this->blurVAttribTexCoord = glGetAttribLocation(this->blurVProgram, texCoordName);
        this->blurVUniformWorldMatrix = glGetUniformLocation(this->blurVProgram, matrixName);
        this->blurVUniformTexture = glGetUniformLocation(this->blurVProgram, samplerName);
        this->blurVUniformTexSize = glGetUniformLocation(this->blurVProgram, texSizeName);
        glUseProgram(this->blurVProgram);
        glUniform1i(this->blurVUniformTexture, 0);

        this->finalAttribPosition = glGetAttribLocation(this->finalProgram, positionName);
        this->finalAttribTexCoord = glGetAttribLocation(this->finalProgram, texCoordName);
        this->finalUniformWorldMatrix = glGetUniformLocation(this->finalProgram, matrixName);
        this->finalUniformTexture = glGetUniformLocation(this->finalProgram, samplerName);
        this->finalUniformTextureBloom = glGetUniformLocation(this->finalProgram, "s_texture_bloom");
        glUseProgram(this->finalProgram);
        glUniform1i(this->finalUniformTexture, 0);
        glUniform1i(this->finalUniformTextureBloom, 1);
    }

    BloomShader::BloomShader() {
        BloomShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"BloomShader";
    }

    BloomShader::~BloomShader() {
    }

    void BloomShader::InternalInit(Engine *engine) {
        this->fboLuma = new FBOContainer(engine, String("BloomShader fboLuma"));
        this->fboLuma->Create(0x100, 0x100, true, false);

        this->fboBlurH = new FBOContainer(engine, String("BloomShader fboBlurH"));
        this->fboBlurH->Create(0x100, 0x100, true, false);

        this->fboBlurV = new FBOContainer(engine, String("BloomShader fboBlurV"));
        this->fboBlurV->Create(0x100, 0x100, true, false);

        this->fboBlack = new FBOContainer(engine, String("BloomShader fboBlack"));
        this->fboBlack->Create(0x100, 0x100, true, false);
    }

    void BloomShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        glUniformMatrix4fv(this->lumaUniformWorldMatrix, 1, 0, engine->worldViewProjMatrix);
        if (this->dirty != 0) {
            this->dirty = 0;
        }

        glEnableVertexAttribArray(this->lumaAttribPosition);
        glEnableVertexAttribArray(this->lumaAttribTexCoord);

        if (mesh->uploaded != 0) {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->lumaAttribPosition, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->lumaAttribTexCoord, 2, 0x1406, 0, 0, 0);
        } else {
            glVertexAttribPointer(this->lumaAttribPosition, 3, 0x1406, 0, 0, mesh->positions);
            glVertexAttribPointer(this->lumaAttribTexCoord, 2, 0x1406, 0, 0, mesh->texCoords);
        }
    }

    void BloomShader::RenderEffect(FBOContainer *source, Engine *engine) {
        engine->field_0x3e4 = this->program;

        if (g_BloomShader_internalInitNeeded != 0) {
            g_BloomShader_internalInitNeeded = 0;
            InternalInit(engine);
            this->fboBlack->BeginCapture();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(0x4000);
            this->fboBlack->EndCapture();
        }

        const uint32x4_t zero = vdupq_n_u32(0);
        vst1q_u32((uint32_t *) &engine->projMatrix[12], zero);
        vst1q_u32((uint32_t *) &engine->projMatrix[8], zero);
        vst1q_u32((uint32_t *) &engine->projMatrix[4], zero);
        vst1q_u32((uint32_t *) &engine->projMatrix[0], zero);
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

        glUseProgram(this->downSampleProgram);
        glActiveTexture(0x84c0);
        source->Activate();
        this->fboLuma->BeginCapture();
        glEnableVertexAttribArray(this->downSampleAttribPosition);
        glEnableVertexAttribArray(this->downSampleAttribTexCoord);
        const float *mvp = engine->worldViewProjMatrix;
        glUniformMatrix4fv(this->downSampleUniformWorldMatrix, 1, 0, mvp);
        glVertexAttribPointer(this->downSampleAttribPosition, 3, 0x1406, 0, 0,
                              engine->quadMesh->positions);
        glVertexAttribPointer(this->downSampleAttribTexCoord, 2, 0x1406, 0, 0,
                              engine->quadMesh->texCoords);
        glClear(0x4000);
        Engine_DrawQuad(engine, 0, 0, Engine_GetDisplayWidth(engine), Engine_GetDisplayHeight(engine));
        glDisableVertexAttribArray(this->downSampleAttribPosition);
        glDisableVertexAttribArray(this->downSampleAttribTexCoord);

        FBOContainer *blurSource = this->fboLuma;
        for (int i = 6; i != 0; i -= 1) {
            glUseProgram(this->blurHProgram);
            glActiveTexture(0x84c0);
            blurSource->Activate();
            this->fboBlurH->BeginCapture();
            glEnableVertexAttribArray(this->blurHAttribPosition);
            glEnableVertexAttribArray(this->blurHAttribTexCoord);
            glUniformMatrix4fv(this->blurHUniformWorldMatrix, 1, 0, mvp);
            glVertexAttribPointer(this->blurHAttribPosition, 3, 0x1406, 0, 0,
                                  engine->quadMesh->positions);
            glVertexAttribPointer(this->blurHAttribTexCoord, 2, 0x1406, 0, 0,
                                  engine->quadMesh->texCoords);
            glUniform1f(this->blurHUniformTexSize, (float) this->fboBlurH->width);
            glClear(0x4000);
            Engine_DrawQuad(engine, 0, 0, Engine_GetDisplayWidth(engine), Engine_GetDisplayHeight(engine));
            glDisableVertexAttribArray(this->blurHAttribPosition);
            glDisableVertexAttribArray(this->blurHAttribTexCoord);

            glUseProgram(this->blurVProgram);
            glActiveTexture(0x84c0);
            this->fboBlurH->Activate();
            this->fboBlurV->BeginCapture();
            glEnableVertexAttribArray(this->blurVAttribPosition);
            glEnableVertexAttribArray(this->blurVAttribTexCoord);
            glUniformMatrix4fv(this->blurVUniformWorldMatrix, 1, 0, mvp);
            glVertexAttribPointer(this->blurVAttribPosition, 3, 0x1406, 0, 0,
                                  engine->quadMesh->positions);
            glVertexAttribPointer(this->blurVAttribTexCoord, 2, 0x1406, 0, 0,
                                  engine->quadMesh->texCoords);
            glUniform1f(this->blurVUniformTexSize, (float) this->fboBlurV->height);
            glClear(0x4000);
            Engine_DrawQuad(engine, 0, 0, Engine_GetDisplayWidth(engine), Engine_GetDisplayHeight(engine));
            glDisableVertexAttribArray(this->blurVAttribPosition);
            glDisableVertexAttribArray(this->blurVAttribTexCoord);
            blurSource = this->fboBlurV;
        }

        FBOContainer *base = source;
        FBOContainer *bloom = this->fboBlack;
        if (g_BloomShader_shaderMode < 4) {
            switch (g_BloomShader_shaderMode) {
                case 1:
                    base = this->fboLuma;
                    break;
                case 2:
                    base = this->fboBlurV;
                    break;
                case 3:
                    bloom = this->fboBlack;
                    break;
                default:
                    break;
            }
        }

        glUseProgram(this->finalProgram);
        glActiveTexture(0x84c0);
        base->Activate();
        glActiveTexture(0x84c1);
        bloom->Activate();
        glBindFramebuffer(0x8d40, engine->field_0x40c);
        unsigned int width;
        unsigned int height;
        if (engine->appManager->paintCanvas->gameOrientation == 2) {
            width = Engine_GetDisplayWidth(engine);
            height = Engine_GetDisplayHeight(engine);
        } else {
            width = Engine_GetDisplayHeight(engine);
            height = Engine_GetDisplayWidth(engine);
        }
        glViewport(0, 0, width, height);

        glEnableVertexAttribArray(this->finalAttribPosition);
        glEnableVertexAttribArray(this->finalAttribTexCoord);
        glUniformMatrix4fv(this->finalUniformWorldMatrix, 1, 0, mvp);
        glVertexAttribPointer(this->finalAttribPosition, 3, 0x1406, 0, 0,
                              engine->quadMesh->positions);
        glVertexAttribPointer(this->finalAttribTexCoord, 2, 0x1406, 0, 0,
                              engine->quadMesh->texCoords);
        glClear(0x4000);
        Engine_DrawQuad(engine, 0, 0, Engine_GetDisplayWidth(engine), Engine_GetDisplayHeight(engine));
        glDisableVertexAttribArray(this->finalAttribPosition);
        glDisableVertexAttribArray(this->finalAttribTexCoord);
        glEnable(0xbe2);
        glBlendFunc(0x302, 0x303);
        glActiveTexture(0x84c0);
    }

    void BloomShader::SetInActive() {
        glDisableVertexAttribArray(this->lumaAttribPosition);
        glDisableVertexAttribArray(this->lumaAttribTexCoord);
        glDisableVertexAttribArray(this->downSampleAttribTexCoord);
        glDisableVertexAttribArray(this->downSampleAttribPosition);
    }

    void BloomShader::RenderEffect(FBOContainer *source, FBOContainer *&target, Engine *engine) {
        if (g_BloomShader_internalInitNeeded != 0) {
            g_BloomShader_internalInitNeeded = 0;
            InternalInit(engine);
            this->fboBlack->BeginCapture();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(0x4000);
            this->fboBlack->EndCapture();
        }

        const uint32x4_t zero = vdupq_n_u32(0);
        vst1q_u32((uint32_t *) &engine->projMatrix[12], zero);
        vst1q_u32((uint32_t *) &engine->projMatrix[8], zero);
        vst1q_u32((uint32_t *) &engine->projMatrix[4], zero);
        vst1q_u32((uint32_t *) &engine->projMatrix[0], zero);
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

        glUseProgram(this->downSampleProgram);
        glActiveTexture(0x84c0);
        source->Activate();
        this->fboLuma->BeginCapture();
        glEnableVertexAttribArray(this->downSampleAttribPosition);
        glEnableVertexAttribArray(this->downSampleAttribTexCoord);
        const float *mvp = engine->worldViewProjMatrix;
        glUniformMatrix4fv(this->downSampleUniformWorldMatrix, 1, 0, mvp);
        glVertexAttribPointer(this->downSampleAttribPosition, 3, 0x1406, 0, 0,
                              engine->quadMesh->positions);
        glVertexAttribPointer(this->downSampleAttribTexCoord, 2, 0x1406, 0, 0,
                              engine->quadMesh->texCoords);
        glClear(0x4000);
        Engine_DrawQuad(engine, 0, 0, Engine_GetDisplayWidth(engine), Engine_GetDisplayHeight(engine));
        glDisableVertexAttribArray(this->downSampleAttribPosition);
        glDisableVertexAttribArray(this->downSampleAttribTexCoord);

        FBOContainer *blurSource = this->fboLuma;
        for (int i = 6; i != 0; i -= 1) {
            glUseProgram(this->blurHProgram);
            glActiveTexture(0x84c0);
            blurSource->Activate();
            this->fboBlurH->BeginCapture();
            glEnableVertexAttribArray(this->blurHAttribPosition);
            glEnableVertexAttribArray(this->blurHAttribTexCoord);
            glUniformMatrix4fv(this->blurHUniformWorldMatrix, 1, 0, mvp);
            glVertexAttribPointer(this->blurHAttribPosition, 3, 0x1406, 0, 0,
                                  engine->quadMesh->positions);
            glVertexAttribPointer(this->blurHAttribTexCoord, 2, 0x1406, 0, 0,
                                  engine->quadMesh->texCoords);
            glUniform1f(this->blurHUniformTexSize, (float) this->fboBlurH->width);
            glClear(0x4000);
            Engine_DrawQuad(engine, 0, 0, Engine_GetDisplayWidth(engine), Engine_GetDisplayHeight(engine));
            glDisableVertexAttribArray(this->blurHAttribPosition);
            glDisableVertexAttribArray(this->blurHAttribTexCoord);

            glUseProgram(this->blurVProgram);
            glActiveTexture(0x84c0);
            this->fboBlurH->Activate();
            this->fboBlurV->BeginCapture();
            glEnableVertexAttribArray(this->blurVAttribPosition);
            glEnableVertexAttribArray(this->blurVAttribTexCoord);
            glUniformMatrix4fv(this->blurVUniformWorldMatrix, 1, 0, mvp);
            glVertexAttribPointer(this->blurVAttribPosition, 3, 0x1406, 0, 0,
                                  engine->quadMesh->positions);
            glVertexAttribPointer(this->blurVAttribTexCoord, 2, 0x1406, 0, 0,
                                  engine->quadMesh->texCoords);
            glUniform1f(this->blurVUniformTexSize, (float) this->fboBlurV->height);
            glClear(0x4000);
            Engine_DrawQuad(engine, 0, 0, Engine_GetDisplayWidth(engine), Engine_GetDisplayHeight(engine));
            glDisableVertexAttribArray(this->blurVAttribPosition);
            glDisableVertexAttribArray(this->blurVAttribTexCoord);
            blurSource = this->fboBlurV;
        }

        FBOContainer *base = source;
        FBOContainer *bloom = this->fboBlack;
        if (g_BloomShader_shaderMode < 4) {
            switch (g_BloomShader_shaderMode) {
                case 1:
                    base = this->fboLuma;
                    break;
                case 2:
                    base = this->fboBlurV;
                    break;
                case 3:
                    bloom = this->fboBlack;
                    break;
                default:
                    break;
            }
        }

        glUseProgram(this->finalProgram);
        glActiveTexture(0x84c0);
        base->Activate();
        glActiveTexture(0x84c1);
        bloom->Activate();
        if (target != 0) {
            target->BeginCapture();
        }

        glEnableVertexAttribArray(this->finalAttribPosition);
        glEnableVertexAttribArray(this->finalAttribTexCoord);
        glUniformMatrix4fv(this->finalUniformWorldMatrix, 1, 0, mvp);
        glVertexAttribPointer(this->finalAttribPosition, 3, 0x1406, 0, 0,
                              engine->quadMesh->positions);
        glVertexAttribPointer(this->finalAttribTexCoord, 2, 0x1406, 0, 0,
                              engine->quadMesh->texCoords);
        glClear(0x4000);
        Engine_DrawQuad(engine, 0, 0, Engine_GetDisplayWidth(engine), Engine_GetDisplayHeight(engine));
        glDisableVertexAttribArray(this->finalAttribPosition);
        glDisableVertexAttribArray(this->finalAttribTexCoord);
        glEnable(0xbe2);
        glBlendFunc(0x302, 0x303);
        glActiveTexture(0x84c0);
        if (target != 0) {
            target->EndCapture();
        }
    }
}
