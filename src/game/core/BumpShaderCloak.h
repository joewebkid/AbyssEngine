#ifndef GOF2_BUMPSHADERCLOAK_H
#define GOF2_BUMPSHADERCLOAK_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/render/ShaderBaseStruct.h"

namespace AbyssEngine { 
    class Mesh;
 }



namespace AbyssEngine {
    class Engine;
}
using ::AbyssEngine::Engine;

namespace AbyssEngine {
    class BumpShaderCloak : public ShaderBaseStruct {
    public:
        int attrib_a0;
        int attrib_a1;
        int attrib_a2;
        int attrib_a3;
        int attrib_a4;

        int uniform_u0;
        int uniform_u1;
        int uniform_u2;
        int uniform_u3;
        int uniform_u4;
        int uniform_u5;
        int uniform_u6;
        int uniform_u7;
        int uniform_u8;
        int uniform_u9;
        int uniform_u12;
        int uniform_u10;
        int uniform_u13;
        int uniform_u11;
        int uniform_u14;
        int uniform_u15;
        int uniform_u16;
        int uniform_u18;
        int uniform_u19;
        int uniform_u17;
        int uniform_u20;

        static int ShaderIndex;

        BumpShaderCloak();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
