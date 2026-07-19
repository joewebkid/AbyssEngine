#ifndef GOF2_TEXTUREVTXCOLORSHADER_H
#define GOF2_TEXTUREVTXCOLORSHADER_H
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
    class TextureVtxColorShader : public ShaderBaseStruct {
    public:
        int fogProgram;

        int loc_a_position[2];
        int loc_a_texCoord[2];
        int loc_a_VertexColor[2];
        int loc_u_WorldMatrix[2];
        int loc_glColor[2];
        int loc_s_texture[2];
        int loc_u_UVAnimation[2];
        int loc_u_UvMatrix[2];
        int loc_u_fogColor[2];
        int loc_u_fogMaxDist[2];
        int loc_u_fogMinDist[2];
        int loc_u_EnableFog[2];
        int loc_u_eyeposmodel[2];
        int loc_u_DarkenValue[2];

        static int ShaderIndex;

        TextureVtxColorShader();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void ConnectShaderComponents(uint32_t program, int index);

        void UseShader(bool useExtra) override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
