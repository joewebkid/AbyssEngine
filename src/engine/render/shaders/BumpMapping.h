#ifndef GOF2_BUMPMAPPING_H
#define GOF2_BUMPMAPPING_H
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
    class BumpMapping : public ShaderBaseStruct {
    public:
        int a0Loc;
        int a1Loc;
        int a2Loc;
        int a3Loc;
        int a4Loc;
        int u0Loc;
        int u1Loc;
        int u2Loc;
        int u3Loc;

        static int ShaderIndex;

        BumpMapping();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
