#ifndef GOF2_NOTEXSHADER_H
#define GOF2_NOTEXSHADER_H
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
    class NoTexShader : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int aPosition;
        int uMvpMatrix;
        int uColor;

        NoTexShader();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;
    };
}

#endif
