#ifndef GOF2_DRAWFBOSHADER_H
#define GOF2_DRAWFBOSHADER_H
#include "engine/core/Array.h"
#include "../../core/AEString.h"
#include "engine/render/ShaderBaseStruct.h"

namespace AbyssEngine { 
    class FBOContainer;
    class Mesh;
 }



namespace AbyssEngine {
    class Engine;
}
using ::AbyssEngine::Engine;

namespace AbyssEngine {
    class DrawFBOShader : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int positionLoc;
        int worldViewMatrixLoc;
        int texCoordLoc;
        int textureLoc;

        DrawFBOShader();

        ~DrawFBOShader();

        void Init(Engine *engine) override;

        void SetInActive() override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;

        void RenderEffect(FBOContainer *fbo, Engine *engine) override;
    };
}

#endif
