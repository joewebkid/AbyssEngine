#ifndef GOF2_BUMPRIMCUBESHADER_NEW_H
#define GOF2_BUMPRIMCUBESHADER_NEW_H
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
    class BumpRimCubeShader_new : public ShaderBaseStruct {
    public:
        int attrib0;
        int attrib1;
        int attrib2;
        int attrib3;
        int attrib4;
        int uniform0;
        int uniform1;
        int uniform2;
        int uniform3;
        int uniform4;
        int uniform5;
        int uniform6;
        int uniform7;
        int uniform8;
        int uniform9;
        int uniform10;
        int uniform11;
        int uniform12;
        int uniform13;
        int uniform14;
        int uniform15;
        int uniform16;
        int uniform17;
        int uniform18;
        int uniform19;
        int uniform20;
        int uniform21;
        int uniform22;
        int uniform23;
        int uniform24;

        static int ShaderIndex;

        BumpRimCubeShader_new();

        void Init(Engine *);

        void UpdateMeshData(Mesh *mesh, Engine *engine);

        void SetInActive();
    };
}
#endif
