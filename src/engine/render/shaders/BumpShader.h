#ifndef GOF2_BUMPSHADER_H
#define GOF2_BUMPSHADER_H
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
    class BumpShader : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int a0Loc;
        int a1Loc;
        int a2Loc;
        int u0Loc;
        int u1Loc;
        int u2Loc;
        int u3Loc;
        int u4Loc;
        int u5Loc;
        int u6Loc;
        int u7Loc;
        int u8Loc;
        int u11Loc;
        int u9Loc;
        int u12Loc;
        int u10Loc;
        int u13Loc;
        int u14Loc;
        int u15Loc;
        int u16Loc;
        int u17Loc;
        int u18Loc;
        int u19Loc;
        int u20Loc;
        int u21Loc;

        BumpShader();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
