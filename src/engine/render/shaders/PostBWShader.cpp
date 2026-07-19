#include "engine/render/shaders/PostBWShader.h"
#include "engine/render/FBOContainer.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include "engine/core/ApplicationManager.h"
#include "engine/render/PaintCanvas.h"
#include <GLES2/gl2.h>
#include <arm_neon.h>

unsigned int AbyssEngine_Engine_GetDisplayWidth(::Engine * engine);
unsigned int AbyssEngine_Engine_GetDisplayHeight(::Engine * engine);

void AbyssEngine_Engine_SetWorldViewMatrix(::Engine *engine,
                                           const uint32_t *matrix);

void AbyssEngine_Engine_DrawQuad(::Engine *engine, int x, int y, int width,
                                 int height);

namespace AbyssEngine {
    int PostBWShader::ShaderIndex;

    PostBWShader::PostBWShader() {
        PostBWShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"PostBWShader";
    }

    PostBWShader::~PostBWShader() {
    }

    void PostBWShader::Init(Engine *) {
        this->program = ES2LoadProgram(
            "attribute vec4 a_Position;attribute vec2 a_TexCoord;varying vec2 v_TexCoord;uniform mat4 u_MVPMatrix;void main(){gl_Position=u_MVPMatrix*a_Position;v_TexCoord=a_TexCoord;}",
            "precision mediump float;varying vec2 v_TexCoord;uniform sampler2D s_Texture;void main(){vec4 c=texture2D(s_Texture,v_TexCoord);float g=(c.r+c.g+c.b)/3.0;gl_FragColor=vec4(g,g,g,c.a);}");
        this->aPosition = glGetAttribLocation(this->program, "a_Position");
        this->aTexCoord = glGetAttribLocation(this->program, "a_TexCoord");
        this->uMvpMatrix = glGetUniformLocation(this->program, "u_MVPMatrix");
        this->sTexture = glGetUniformLocation(this->program, "s_Texture");
        glUseProgram(this->program);
        glUniform1i(this->sTexture, 0);
    }

    void PostBWShader::SetInActive() {
        glDisableVertexAttribArray(this->aPosition);
        glDisableVertexAttribArray(this->aTexCoord);
    }

    void PostBWShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        glUniformMatrix4fv(this->uMvpMatrix, 1, 0, engine->worldViewProjMatrix);
        this->dirty = 0;

        glEnableVertexAttribArray(this->aPosition);
        glEnableVertexAttribArray(this->aTexCoord);

        if (mesh->uploaded != 0) {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->aTexCoord, 2, 0x1406, 0, 0, 0);
        } else {
            glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, mesh->positions);
            glVertexAttribPointer(this->aTexCoord, 2, 0x1406, 0, 0, mesh->texCoords);
        }
    }

    void PostBWShader::RenderEffect(FBOContainer *fbo, Engine *engine) {
        uint32x4_t zero = vdupq_n_u32(0);
        vst1q_u32((uint32_t *) &engine->projMatrix[12], zero);
        vst1q_u32((uint32_t *) &engine->projMatrix[8], zero);
        vst1q_u32((uint32_t *) &engine->projMatrix[4], zero);
        vst1q_u32((uint32_t *) &engine->projMatrix[0], zero);
        engine->field_0x3e4 = this->program;

        engine->projMatrix[0] = 2.0f / (float) AbyssEngine_Engine_GetDisplayWidth(engine);
        engine->projMatrix[5] = -(2.0f / (float) AbyssEngine_Engine_GetDisplayHeight(engine));
        engine->projMatrix[10] = -1.0f;
        engine->projMatrix[12] = -1.0f;
        engine->projMatrix[13] = 1.0f;
        engine->projMatrix[15] = 1.0f;

        float matrix[16] = {};
        matrix[0] = 1.0f;
        matrix[5] = 1.0f;
        matrix[14] = 1.0f;
        AbyssEngine_Engine_SetWorldViewMatrix(engine, (const uint32_t *) matrix);

        glDisable(0xb71);
        glDepthMask(0);
        glDisable(0xbe2);
        glUseProgram(this->program);
        glActiveTexture(0x84c0);
        fbo->Activate();
        glBindFramebuffer(0x8d40, engine->field_0x40c);
        glClear(0x4100);

        int width;
        int height;
        if (engine->appManager->paintCanvas->gameOrientation == 2) {
            width = AbyssEngine_Engine_GetDisplayWidth(engine);
            height = AbyssEngine_Engine_GetDisplayHeight(engine);
        } else {
            width = AbyssEngine_Engine_GetDisplayHeight(engine);
            height = AbyssEngine_Engine_GetDisplayWidth(engine);
        }
        glViewport(0, 0, width, height);

        glEnableVertexAttribArray(this->aPosition);
        glEnableVertexAttribArray(this->aTexCoord);
        glUniformMatrix4fv(this->uMvpMatrix, 1, 0, engine->worldViewProjMatrix);
        glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0,
                              engine->quadMesh->positions);
        glVertexAttribPointer(this->aTexCoord, 2, 0x1406, 0, 0,
                              engine->quadMesh->texCoords);

        glClear(0x4000);
        AbyssEngine_Engine_DrawQuad(engine, 0, 0, AbyssEngine_Engine_GetDisplayWidth(engine),
                                    AbyssEngine_Engine_GetDisplayHeight(engine));

        glDisableVertexAttribArray(this->aPosition);
        glDisableVertexAttribArray(this->aTexCoord);
        glEnable(0xbe2);
        glBlendFunc(0x302, 0x303);
        glActiveTexture(0x84c0);
        engine->boundTextures[0] = 0xffffffff;
    }
}
