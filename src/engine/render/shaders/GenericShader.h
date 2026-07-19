#ifndef GOF2_GENERICSHADER_H
#define GOF2_GENERICSHADER_H
#include "engine/core/Array.h"
#include "../../core/AEString.h"
#include "engine/render/ShaderBaseStruct.h"

namespace AbyssEngine { 
    class Mesh;
 }



namespace AbyssEngine {
    class Engine;
}
using ::AbyssEngine::Engine;

namespace AbyssEngine {
    class GenericShader : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int aPosition;
        int aTexCoord;
        int aNormal;
        int aTangent;
        int aBitangent;

        int uMvpMatrix;
        int uNormalMatrix;
        int uLightPosition;
        int uEyePosition;
        int uTexture0;
        int uTexture1;
        int uTexture2;
        int uAmbientColor;
        int uDiffuseColor;
        int uSpecularColor;
        int uEmissiveColor;
        int uMaterialShininess;

        GenericShader();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
