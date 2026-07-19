#ifndef GOF2_CUBEMAPPING_H
#define GOF2_CUBEMAPPING_H
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
    class CubeMapping : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int aPosition;
        int aNormal;
        int aTexCoord;
        int uMvp;
        int uNormalMatrix;
        int uniform2;
        int uniform3;
        int uniform5;
        int uniform4;
        int uniform6;
        int uniform7;
        int uniform8;
        int uniform9;
        int uniform10;
        int uniform11;

        CubeMapping();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
