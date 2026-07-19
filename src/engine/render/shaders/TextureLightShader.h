#ifndef GOF2_TEXTURELIGHTSHADER_H
#define GOF2_TEXTURELIGHTSHADER_H
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
    class TextureLightShader : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int aPosition;
        int aTexCoord;
        int aNormal;
        int uMvpMatrix;
        int uModelViewMatrix;
        int uNormalMatrix;
        int uColor0;
        int uColor1;
        int uAmbientColor;
        int uTexture;
        int uLight0Direction;
        int uLight0Color;
        int uLight1Direction;
        int uLight1Color;
        int uLight2Direction;
        int uLight2Color;
        int uShininess;
        int uSpecularColor;
        int uModelMatrix;
        int uHasScaleAnimation;

        TextureLightShader();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
