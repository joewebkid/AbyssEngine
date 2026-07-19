#ifndef GOF2_SPECCUBEALPHAMAPPING_H
#define GOF2_SPECCUBEALPHAMAPPING_H
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
    class SpecCubeAlphaMapping : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int attrA0;
        int attrA1;
        int attrA2;
        int uniU0;
        int uniU1;
        int uniU2;
        int uniU3;
        int uniU5;
        int uniU4;
        int uniU6;
        int uniU7;
        int uniU8;
        int uniU9;
        int uniU10;
        int uniU11;

        SpecCubeAlphaMapping();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
