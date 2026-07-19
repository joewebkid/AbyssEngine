#ifndef GOF2_TEXTURECONFERENCE_H
#define GOF2_TEXTURECONFERENCE_H
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
    class TextureConference : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int aPosition;
        int aTexCoord;
        int sTexture;
        int uMvpMatrix;
        int uColor;
        int uOffset;
        long long animTime;

        TextureConference();

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;

        void Init(Engine *engine) override;
    };
}

#endif
