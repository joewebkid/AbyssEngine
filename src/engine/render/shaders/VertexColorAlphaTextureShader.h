#ifndef GOF2_VERTEXCOLORALPHATEXTURESHADER_H
#define GOF2_VERTEXCOLORALPHATEXTURESHADER_H
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
    class VertexColorAlphaTextureShader : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int attrib1;
        int attrib2;
        int attrib3;
        int attrib4;
        int attrib5;
        int attrib0;
        int uniform0;
        int uniform1;
        int uniform2;
        int uniform3;
        int uniform4;
        int uniform5;
        int uniform6;
        int uniform7;
        int uniform8;

        VertexColorAlphaTextureShader();

        void Init(Engine *engine) override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;

        void SetInActive() override;
    };
}

#endif
