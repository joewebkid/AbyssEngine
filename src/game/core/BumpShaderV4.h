#ifndef GOF2_BUMPSHADERV4_H
#define GOF2_BUMPSHADERV4_H
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
    class BumpShaderV4 : public ShaderBaseStruct {
    public:
        int aPosition;
        int aTexCoord;
        int aNormal;
        int aTangent;
        int aBitangent;
        int uMvpMatrix;
        int uModelMatrix;
        int uLightDirModel;
        int uEyePosModel;
        int uTexture0;
        int uTexture1;
        int uColor;
        int uAmbientColor;
        int uDiffuseColor;
        int uSpecularColor;

        static int ShaderIndex;

        BumpShaderV4();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
