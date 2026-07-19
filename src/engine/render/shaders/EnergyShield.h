#ifndef GOF2_ENERGYSHIELD_H
#define GOF2_ENERGYSHIELD_H
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
    class EnergyShield : public ShaderBaseStruct {
    public:
        int aPosition;
        int aTexCoord;
        int uM0;
        int uM1;
        int uM2;
        int uM3;
        int uTex0;
        int uTex1;
        int uM4;
        int uM5;
        int uM7;
        int uRefract;
        int uM6;
        int uM8;

        static int ShaderIndex;

        EnergyShield();

        void Init(Engine *engine) override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;

        void SetInActive() override;
    };
}

#endif
