#ifndef GOF2_BLOOMSHADER_H
#define GOF2_BLOOMSHADER_H
#include "engine/core/Array.h"
#include "../../core/AEString.h"
#include "engine/render/ShaderBaseStruct.h"

namespace AbyssEngine { 
    class FBOContainer;
    class Mesh;
 }



namespace AbyssEngine {
    class Engine;
}
using ::AbyssEngine::Engine;

namespace AbyssEngine {
    class BloomShader : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        unsigned int downSampleProgram;
        int downSampleAttribPosition;
        int downSampleUniformWorldMatrix;
        unsigned int downSampleAttribTexCoord;
        int downSampleUniformTexture;
        FBOContainer *fboLuma;

        unsigned int blurHProgram;
        int blurHAttribPosition;
        int blurHUniformWorldMatrix;
        unsigned int blurHAttribTexCoord;
        int blurHUniformTexture;
        int blurHUniformTexSize;
        FBOContainer *fboBlurH;

        unsigned int blurVProgram;
        int blurVAttribPosition;
        int blurVUniformWorldMatrix;
        unsigned int blurVAttribTexCoord;
        int blurVUniformTexture;
        int blurVUniformTexSize;
        FBOContainer *fboBlurV;
        FBOContainer *fboBlack;

        unsigned int finalProgram;
        int finalAttribPosition;
        int finalUniformWorldMatrix;
        unsigned int finalAttribTexCoord;
        int finalUniformTexture;
        int finalUniformTextureBloom;

        int lumaAttribPosition;
        int lumaUniformWorldMatrix;
        unsigned int lumaAttribTexCoord;
        int lumaUniformTexture;

        BloomShader();

        ~BloomShader();

        void Init(Engine *engine) override;

        void InternalInit(Engine *engine);

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;

        void SetInActive() override;

        void RenderEffect(FBOContainer *source, Engine *engine) override;

        void RenderEffect(FBOContainer *source, FBOContainer *&target, Engine *engine) override;
    };
}

#endif
