#ifndef GOF2_GENERICSHADER1_H
#define GOF2_GENERICSHADER1_H
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
    class GenericShader1 : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int aPosition;
        int aNormal;
        int aTangent;
        int aBinormal;
        int aTexCoord;
        int uM0;
        int uM1;
        int uM2;
        int uM3;
        int uM4;
        int uM5;
        int uM6;
        int uM7;
        int uM8;

        GenericShader1();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
