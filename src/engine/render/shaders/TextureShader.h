#ifndef GOF2_TEXTURESHADER_H
#define GOF2_TEXTURESHADER_H
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
    class TextureShader : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        uint32_t programExt;
        int positionAttrib[2];
        int texcoordAttrib[2];
        int mvpUniform[2];
        int colorUniform[2];
        int textureUniform[2];
        int worldViewUniform[2];
        int textureModeUniform[2];
        int lightUniform[2];
        int fogNearUniform[2];
        int fogFarUniform[2];
        int activeTextureUniform[2];
        int fogColorUniform[2];
        int alphaUniform[2];

        TextureShader();

        void Init(Engine *engine) override;

        void UseShader(bool flag) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;

        void ConnectShaderComponents(uint32_t program, int slot);
    };
}

#endif
