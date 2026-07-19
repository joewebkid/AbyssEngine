#ifndef GOF2_SPECCUBEMAPPING_H
#define GOF2_SPECCUBEMAPPING_H
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
    class SpecCubeMapping : public ShaderBaseStruct {
    public:
        static int ShaderIndex;

        int attribPosition;
        int attribNormal;
        int attribTexCoord;
        int mvpMatrixLoc;
        int normalMatrixLoc;
        int uCameraPosition;

        union {
            int uLightDirection;
            int field_0x38;
        };

        int samplerLoc1;
        int samplerLoc0;

        union {
            int uParam6;
            int field_0x44;
        };

        union {
            int uLightAmbient;
            int field_0x48;
        };

        union {
            int uParam8;
            int field_0x4c;
        };

        union {
            int uLightDiffuse;
            int field_0x50;
        };

        int uShininess;

        union {
            int uColor;
            int field_0x58;
        };

        SpecCubeMapping();

        void Init(Engine *) override;

        void UpdateMeshData(Mesh *mesh, Engine *engine) override;

        void SetInActive() override;
    };
}
#endif
