#ifndef GOF2_POSTBWSHADER_H
#define GOF2_POSTBWSHADER_H
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
    class PostBWShader : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int aPosition;
        int uMvpMatrix;
        int aTexCoord;
        int sTexture;

        PostBWShader();

        ~PostBWShader();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;

        void RenderEffect(FBOContainer *fbo, Engine *engine) override;
    };
}

#endif
