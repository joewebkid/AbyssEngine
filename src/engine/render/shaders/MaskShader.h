#ifndef GOF2_MASKSHADER_H
#define GOF2_MASKSHADER_H
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
    class MaskShader : public ShaderBaseStruct {
    public:
        int a_position;
        int a_texCoord;
        int a_color;
        int u_mvpMatrix;
        int u_texture0;
        int u_texture1;
        int u_color;

        static int ShaderIndex;

        MaskShader();

        void Init(::Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, ::Engine *engine) override;
    };
}

#endif
