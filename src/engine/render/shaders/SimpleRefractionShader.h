#ifndef GOF2_SIMPLEREFRACTIONSHADER_H
#define GOF2_SIMPLEREFRACTIONSHADER_H
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
    class SimpleRefractionShader : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int aPositionLoc;
        int aNormalLoc;
        int aTangentLoc;
        int aTexCoordLoc;
        int uM0Loc;
        int uM1Loc;
        int uTex0Loc;
        int uTex1Loc;
        int uM4Loc;
        int uM3Loc;
        int uM2Loc;
        int uRefractLoc;
        int uM5Loc;
        int uM6Loc;

        SimpleRefractionShader();

        void Init(::Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, ::Engine *engine) override;
    };
}

#endif
