#ifndef GOF2_COLORMIXADD_H
#define GOF2_COLORMIXADD_H
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
    class ColorMixAdd : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int a0Loc;
        int a1Loc;
        int u1Loc;
        int u2Loc;
        int u0Loc;
        int u4Loc;
        int u3Loc;
        int u5Loc;

        ColorMixAdd();

        void Init(Engine *engine) override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;

        void SetInActive() override;
    };
}

#endif
