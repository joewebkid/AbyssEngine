#ifndef GOF2_BUMPSHADERREFRACT_H
#define GOF2_BUMPSHADERREFRACT_H
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
    class BumpShaderRefract : public ShaderBaseStruct {
    public:
        int aPosition;
        int aTexCoord;
        int uSampler0;
        int uMvpMatrix;
        int uSampler2;
        int uSampler3;
        int uPixelSize;
        int uRefractSampler;
        int uViewMatrix;
        int uColorIndex;
        int uColor;

        static int ShaderIndex;

        BumpShaderRefract();

        void Init(::Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, ::Engine *engine) override;
    };
}

#endif
