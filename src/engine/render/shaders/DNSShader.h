#ifndef GOF2_DNSSHADER_H
#define GOF2_DNSSHADER_H
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
    class DNSShader : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int aPositionLoc;
        int aNormalLoc;
        int aTangentLoc;
        int aBinormalLoc;
        int uM0Loc;
        int uM1Loc;
        int uM2Loc;
        int uM3Loc;
        int uM4Loc;
        int uM5Loc;
        int uM6Loc;
        int uM7Loc;
        int uM8Loc;
        int uM9Loc;
        int uM10Loc;
        int uM11Loc;
        int uM12Loc;

        DNSShader();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
