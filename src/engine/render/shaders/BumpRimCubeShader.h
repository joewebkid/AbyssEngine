#ifndef GOF2_BUMPRIMCUBESHADER_H
#define GOF2_BUMPRIMCUBESHADER_H
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
    class BumpRimCubeShader : public ShaderBaseStruct {
    public:
        int aPosition;
        int aTexCoord;
        int aNormal;
        int aTangent;
        int aBitangent;
        int uModelViewProjectionMatrix;
        int uModelMatrix;
        int uModelMatrixFull;
        int uLightDirModel0;
        int uLightDirModel1;
        int uEyePosModel;
        int uTexDiffuse;
        int uTexNormal;
        int uTexCube;
        int uGlColor;
        int uAmbientColor0;
        int uDiffuseColor1;
        int uDiffuseColor0;
        int uSpecularColor1;
        int uSpecularColor0;
        int uRimColor;
        int uSpecularPower;
        int uIsGlowMat;
        int uTexBiasDiffuse;
        int uTexBiasNormal;
        int uFogColor;
        int uFogMaxDist;
        int uFogMinDist;
        int uEnableFog;
        int uLodDist;

        static int ShaderIndex;

        BumpRimCubeShader();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
