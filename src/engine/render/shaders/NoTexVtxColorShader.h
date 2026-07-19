#ifndef GOF2_NOTEXVTXCOLORSHADER_H
#define GOF2_NOTEXVTXCOLORSHADER_H
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
    class NoTexVtxColorShader : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int aPosition;
        int aColor;
        int uMvpMatrix;
        int uColor;

        NoTexVtxColorShader();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
