#ifndef GOF2_GLOWSHADER_H
#define GOF2_GLOWSHADER_H
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
    class GlowShader : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int a_positionLoc;
        int a_texCoordLoc;
        int u_mvpLoc;
        int u_colorLoc;
        int u_textureLoc;

        GlowShader();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
