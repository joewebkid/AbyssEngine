#include "engine/render/shaders/GlowPPShader.h"
#include "engine/render/FBOContainer.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

static uint8_t *g_GlowPPShader_internalInitNeededPtr = nullptr;
static uint32_t *g_GlowPPShader_shaderModePtr = nullptr;

unsigned int Engine_GetDisplayWidth(::Engine * engine);
unsigned int Engine_GetDisplayHeight(::Engine * engine);

void Engine_DrawQuad(::Engine *engine, int x, int y, int width, int height);

void Engine_SetWorldViewMatrix(::Engine *engine, const uint32_t *matrix);

namespace AbyssEngine {
    int GlowPPShader::ShaderIndex;

    void GlowPPShader::SetInActive() {
        glDisableVertexAttribArray(this->meshAttribPosition);
        glDisableVertexAttribArray(this->meshAttribTexCoord);
    }

    void GlowPPShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        glUniformMatrix4fv(this->combineUniformWorldView, 1, 0, engine->worldViewProjMatrix);
        this->dirty = 0;

        glEnableVertexAttribArray(this->meshAttribPosition);
        glEnableVertexAttribArray(this->meshAttribTexCoord);
        if (mesh->uploaded == 0) {
            glVertexAttribPointer(this->meshAttribPosition, 3, 0x1406, 0, 0, mesh->positions);
            glVertexAttribPointer(this->meshAttribTexCoord, 2, 0x1406, 0, 0, mesh->texCoords);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->meshAttribPosition, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->meshAttribTexCoord, 2, 0x1406, 0, 0, 0);
        }
    }

    void GlowPPShader::RenderEffect(FBOContainer *source, FBOContainer *&target, Engine *engine) {
        if (*g_GlowPPShader_internalInitNeededPtr != 0) {
            *g_GlowPPShader_internalInitNeededPtr = 0;
            this->InternalInit(engine);
            this->backgroundTarget->BeginCapture();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(0x4000);
            this->backgroundTarget->EndCapture();
        }

        engine->projMatrix[12] = 0.0f;
        engine->projMatrix[13] = 0.0f;
        engine->projMatrix[14] = 0.0f;
        engine->projMatrix[15] = 0.0f;
        engine->projMatrix[8] = 0.0f;
        engine->projMatrix[9] = 0.0f;
        engine->projMatrix[10] = 0.0f;
        engine->projMatrix[11] = 0.0f;
        engine->projMatrix[4] = 0.0f;
        engine->projMatrix[5] = 0.0f;
        engine->projMatrix[6] = 0.0f;
        engine->projMatrix[7] = 0.0f;
        engine->projMatrix[0] = 0.0f;
        engine->projMatrix[1] = 0.0f;
        engine->projMatrix[2] = 0.0f;
        engine->projMatrix[3] = 0.0f;

        engine->projMatrix[0] = 2.0f / (float) (int32_t) Engine_GetDisplayWidth(engine);
        engine->projMatrix[5] = -(2.0f / (float) (int32_t) Engine_GetDisplayHeight(engine));
        engine->projMatrix[10] = -1.0f;
        engine->projMatrix[12] = -1.0f;
        engine->projMatrix[13] = 1.0f;
        engine->projMatrix[15] = 1.0f;

        float matrix[16];
        matrix[0] = 1.0f;
        matrix[1] = 0.0f;
        matrix[2] = 0.0f;
        matrix[3] = 0.0f;
        matrix[4] = 0.0f;
        matrix[5] = 1.0f;
        matrix[6] = 0.0f;
        matrix[7] = 0.0f;
        matrix[8] = 0.0f;
        matrix[9] = 0.0f;
        matrix[10] = 1.0f;
        matrix[11] = 0.0f;
        matrix[12] = 0.0f;
        matrix[13] = 0.0f;
        matrix[14] = 0.0f;
        matrix[15] = 1.0f;
        Engine_SetWorldViewMatrix(engine, (const uint32_t *) matrix);
        glDisable(0xb71);
        glDepthMask(0);
        glDisable(0xbe2);

        glUseProgram(this->copyProgram);
        glActiveTexture(0x84c0);
        source->Activate();
        this->copyTarget->BeginCapture();
        glEnableVertexAttribArray(this->copyAttribPosition);
        glEnableVertexAttribArray(this->copyAttribTexCoord);
        glUniformMatrix4fv(this->copyUniformWorldView, 1, 0, engine->worldViewProjMatrix);
        glVertexAttribPointer(this->copyAttribPosition, 3, 0x1406, 0, 0, *(void **) (engine->field_0x380 + 4));
        glVertexAttribPointer(this->copyAttribTexCoord, 2, 0x1406, 0, 0, *(void **) (engine->field_0x380 + 8));
        glClear(0x4000);
        Engine_DrawQuad(engine, 0, 0, Engine_GetDisplayWidth(engine), Engine_GetDisplayHeight(engine));
        glDisableVertexAttribArray(this->copyAttribPosition);
        glDisableVertexAttribArray(this->copyAttribTexCoord);

        for (int32_t i = 3; i != 0; --i) {
            glUseProgram(this->blurXProgram);
            glActiveTexture(0x84c0);
            this->copyTarget->Activate();
            this->blurXTarget->BeginCapture();
            glEnableVertexAttribArray(this->blurXAttribPosition);
            glEnableVertexAttribArray(this->blurXAttribTexCoord);
            glUniformMatrix4fv(this->blurXUniformWorldView, 1, 0, engine->worldViewProjMatrix);
            glVertexAttribPointer(this->blurXAttribPosition, 3, 0x1406, 0, 0, *(void **) (engine->field_0x380 + 4));
            glVertexAttribPointer(this->blurXAttribTexCoord, 2, 0x1406, 0, 0, *(void **) (engine->field_0x380 + 8));
            glUniform1f(this->blurXUniformSampleSize, 1.0f / (float) this->blurXTarget->width);
            glClear(0x4000);
            Engine_DrawQuad(engine, 0, 0, Engine_GetDisplayWidth(engine), Engine_GetDisplayHeight(engine));
            glDisableVertexAttribArray(this->blurXAttribPosition);
            glDisableVertexAttribArray(this->blurXAttribTexCoord);

            glUseProgram(this->blurYProgram);
            glActiveTexture(0x84c0);
            this->blurXTarget->Activate();
            this->blurYTarget->BeginCapture();
            glEnableVertexAttribArray(this->blurYAttribPosition);
            glEnableVertexAttribArray(this->blurYAttribTexCoord);
            glUniformMatrix4fv(this->blurYUniformWorldView, 1, 0, engine->worldViewProjMatrix);
            glVertexAttribPointer(this->blurYAttribPosition, 3, 0x1406, 0, 0, *(void **) (engine->field_0x380 + 4));
            glVertexAttribPointer(this->blurYAttribTexCoord, 2, 0x1406, 0, 0, *(void **) (engine->field_0x380 + 8));
            glUniform1f(this->blurYUniformSampleSize, 1.0f / (float) this->blurYTarget->height);
            glClear(0x4000);
            Engine_DrawQuad(engine, 0, 0, Engine_GetDisplayWidth(engine), Engine_GetDisplayHeight(engine));
            glDisableVertexAttribArray(this->blurYAttribPosition);
            glDisableVertexAttribArray(this->blurYAttribTexCoord);
        }

        FBOContainer *firstTexture = this->blurYTarget;
        FBOContainer *secondTexture = this->backgroundTarget;
        uint32_t mode = *g_GlowPPShader_shaderModePtr;
        if (mode == 0) {
            firstTexture = this->copyTarget;
        } else if (mode == 1) {
            firstTexture = source;
        }

        glUseProgram(this->combineProgram);
        glActiveTexture(0x84c0);
        firstTexture->Activate();
        glActiveTexture(0x84c1);
        secondTexture->Activate();

        if (target == 0) {
            glBindFramebuffer(0x8d40, engine->field_0x40c);
            uint32_t width;
            uint32_t height;
            if (*(int32_t *) (*(char **) engine->field_0x30 + 0x30) == 2) {
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

        glEnableVertexAttribArray(this->combineAttribPosition);
        glEnableVertexAttribArray(this->combineAttribTexCoord);
        glUniformMatrix4fv(this->combineUniformWorldView, 1, 0, engine->worldViewProjMatrix);
        glVertexAttribPointer(this->combineAttribPosition, 3, 0x1406, 0, 0, *(void **) (engine->field_0x380 + 4));
        glVertexAttribPointer(this->combineAttribTexCoord, 2, 0x1406, 0, 0, *(void **) (engine->field_0x380 + 8));
        glClear(0x4000);
        Engine_DrawQuad(engine, 0, 0, Engine_GetDisplayWidth(engine), Engine_GetDisplayHeight(engine));
        glDisableVertexAttribArray(this->combineAttribPosition);
        glDisableVertexAttribArray(this->combineAttribTexCoord);
        if (target != 0) {
            target->EndCapture();
        }

        glEnable(0xbe2);
        glBlendFunc(0x302, 0x303);
        glActiveTexture(0x84c0);
    }

    void GlowPPShader::RenderEffect(FBOContainer *source, Engine *engine) {
        FBOContainer *target = 0;
        this->RenderEffect(source, target, engine);
    }

    void GlowPPShader::InternalInit(Engine *engine) {
        this->copyTarget = new FBOContainer(engine, String("GlowPPShader0"));
        this->copyTarget->Create(0x200, 0x200, true, false);

        this->blurXTarget = new FBOContainer(engine, String("GlowPPShader1"));
        this->blurXTarget->Create(0x200, 0x200, true, false);

        this->blurYTarget = new FBOContainer(engine, String("GlowPPShader2"));
        this->blurYTarget->Create(0x200, 0x200, true, false);

        this->backgroundTarget = new FBOContainer(engine, String("GlowPPShader3"));
        this->backgroundTarget->Create(0x200, 0x200, true, false);
    }

    void GlowPPShader::Init(Engine *) {
        const char *vertex = "GlowPPShader.vert";
        this->copyProgram = this->ES2LoadProgram(vertex, "GlowPPShader.copy.frag");
        this->blurXProgram = this->ES2LoadProgram(vertex, "GlowPPShader.blurX.frag");
        this->blurYProgram = this->ES2LoadProgram(vertex, "GlowPPShader.blurY.frag");
        this->combineProgram = this->ES2LoadProgram(vertex, "GlowPPShader.combine.frag");

        const char *position = "position";
        const char *texcoord = "texcoord";
        const char *worldView = "worldView";
        const char *texture = "texture";
        const char *sampleSize = "sampleSize";
        const char *texture2 = "texture2";

        this->copyAttribPosition = glGetAttribLocation(this->copyProgram, position);
        this->copyAttribTexCoord = glGetAttribLocation(this->copyProgram, texcoord);
        this->copyUniformWorldView = glGetUniformLocation(this->copyProgram, worldView);
        this->copyUniformTexture = glGetUniformLocation(this->copyProgram, texture);
        glUseProgram(this->copyProgram);
        glUniform1i(this->copyUniformTexture, 0);

        this->blurXAttribPosition = glGetAttribLocation(this->blurXProgram, position);
        this->blurXAttribTexCoord = glGetAttribLocation(this->blurXProgram, texcoord);
        this->blurXUniformWorldView = glGetUniformLocation(this->blurXProgram, worldView);
        this->blurXUniformTexture = glGetUniformLocation(this->blurXProgram, texture);
        this->blurXUniformSampleSize = glGetUniformLocation(this->blurXProgram, sampleSize);
        glUseProgram(this->blurXProgram);
        glUniform1i(this->blurXUniformTexture, 0);

        this->blurYAttribPosition = glGetAttribLocation(this->blurYProgram, position);
        this->blurYAttribTexCoord = glGetAttribLocation(this->blurYProgram, texcoord);
        this->blurYUniformWorldView = glGetUniformLocation(this->blurYProgram, worldView);
        this->blurYUniformTexture = glGetUniformLocation(this->blurYProgram, texture);
        this->blurYUniformSampleSize = glGetUniformLocation(this->blurYProgram, sampleSize);
        glUseProgram(this->blurYProgram);
        glUniform1i(this->blurYUniformTexture, 0);

        this->combineAttribPosition = glGetAttribLocation(this->combineProgram, position);
        this->combineAttribTexCoord = glGetAttribLocation(this->combineProgram, texcoord);
        this->combineUniformWorldView = glGetUniformLocation(this->combineProgram, worldView);
        this->combineUniformTexture = glGetUniformLocation(this->combineProgram, texture);
        this->combineUniformTexture2 = glGetUniformLocation(this->combineProgram, texture2);
        glUseProgram(this->combineProgram);
        glUniform1i(this->combineUniformTexture, 0);
        glUniform1i(this->combineUniformTexture2, 1);
    }

    GlowPPShader::GlowPPShader() {
        GlowPPShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"GlowPPShader";
    }

    GlowPPShader::~GlowPPShader() {
    }
}
