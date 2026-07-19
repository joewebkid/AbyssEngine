#ifndef GOF2_TEXTUREALPHATESTSHADER_H
#define GOF2_TEXTUREALPHATESTSHADER_H
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
    class TextureAlphaTestShader : public ShaderBaseStruct {
    public:
        int alphaProgram;

        int aPositionLoc[2];
        int aTexCoordLoc[2];
        int uMVPMatrixLoc[2];
        int uColorLoc[2];
        int uTextureLoc[2];
        int uLightPosLoc[2];
        int uAmbientLoc[2];
        int uDiffuseLoc[2];
        int uSamplerLoc[2];
        int uFogColorLoc[2];

        static int ShaderIndex;

        TextureAlphaTestShader();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;

        void ConnectShaderComponents(unsigned int program, int index);

        void UseShader(bool useExtra) override;
    };
}

#endif
