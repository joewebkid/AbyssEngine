#ifndef GOF2_BUMPSHADERV2_H
#define GOF2_BUMPSHADERV2_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/render/ShaderBaseStruct.h"

namespace AbyssEngine { 
    class Mesh;
 }



namespace AbyssEngine {
    class Engine;
}
using ::AbyssEngine::Engine;

namespace AbyssEngine {
    class BumpShaderV2 : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int aPosition;
        int aNormal;
        int aTexCoord;
        int aTangent;
        int aBinormal;
        int uMvpMatrix;
        int uNormalMatrix;
        int uLightDir;
        int uEyePos;
        int uTexture0;
        int uTexture1;
        int uAmbient;
        int uDiffuse;
        int uSpecular;
        int uEmissive;

        BumpShaderV2();

        void Init(Engine *engine) override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;

        void SetInActive() override;
    };
}

#endif
