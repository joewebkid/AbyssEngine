#ifndef GOF2_BUMPSHADERPARTICLE_H
#define GOF2_BUMPSHADERPARTICLE_H
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
    class BumpShaderParticle : public ShaderBaseStruct {
    public:
        int attribA0;
        int attribA1;
        int attribA2;
        int attribA3;
        int attribA4;
        int uniformU0;
        int uniformU1;
        int uniformU2;
        int uniformU3;
        int uniformU4;
        int uniformU5;
        int uniformU6;
        int uniformU7;
        int uniformU8;
        int uniformU9;
        int uniformU10;
        int uniformU11;
        int uniformU12;
        int uniformU13;

        static int ShaderIndex;

        BumpShaderParticle();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
