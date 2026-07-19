#ifndef GOF2_MESH_H
#define GOF2_MESH_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
namespace AbyssEngine {
    class Transform;

    class Mesh {
    public:
        union {
            uint8_t vertexFormat;
            uint8_t field_0x0;
        };

        union {
            uint16_t vertexCount;
            uint16_t field_0x2;
        };

        union {
            void *positions;
            void *field_0x4;
        };

        union {
            void *texCoords;
            void *field_0x8;
        };

        union {
            void *colors;
            void *field_0xc;
        };

        union {
            void *normals;
            void *field_0x10;
        };

        union {
            void *tangents;
            void *field_0x14;
        };

        union {
            void *binormals;
            void *field_0x18;
        };

        union {
            uint32_t materialId;
            uint32_t field_0x1c;
            int32_t meshPostEffectFlag;
        };

        union {
            float shaderAnimValue0;
            float localOffset;
            float field_0x20;
        };

        union {
            float shaderAnimValue1;
            float field_0x24;
        };

        union {
            uint16_t indexCount;
            uint16_t field_0x28;
        };

        uint16_t field_0x2a;

        union {
            void *indices;
            void *field_0x2c;
        };

        union {
            void *material;
            void *materialBlock;
            void *field_0x30;
        };

        union {
            Transform *animation;
            Transform *field_0x34;
        };

        union {
            uint8_t shared;
            uint8_t field_0x38;
        };

        union {
            float boundsCenterX;
            float field_0x3c;
        };

        union {
            float boundsCenterY;
            float field_0x40;
        };

        union {
            float boundsCenterZ;
            float field_0x44;
        };

        union {
            float boundsRadius;
            float field_0x48;
        };

        union {
            float boundsRadiusSq;
            float field_0x4c;
        };

        float pivotX;
        float pivotY;

        union {
            float pivotZ;
            float field_0x58;
        };

        union {
            uint8_t uploaded;
            uint8_t field_0x5c;
        };

        union {
            unsigned int positionVBO;
            unsigned int field_0x60;
        };

        union {
            unsigned int indexVBO;
            uint32_t field_0x64;
        };

        union {
            unsigned int texCoordVBO;
            unsigned int field_0x68;
        };

        union {
            unsigned int normalVBO;
            uint32_t field_0x6c;
        };

        union {
            unsigned int tangentVBO;
            uint32_t field_0x70;
        };

        union {
            unsigned int binormalVBO;
            uint32_t field_0x74;
        };

        union {
            unsigned int colorVBO;
            uint32_t field_0x78;
        };

        union {
            int vboByteSize;
            uint32_t field_0x7c;
        };

        union {
            uint32_t enhancedData;
            uint32_t field_0x80;
        };

        union {
            uint8_t vboEligible;
            uint8_t field_0x84;
        };

        union {
            uint8_t hasAnimation;
            uint8_t field_0x85;
        };

        Mesh(Mesh *src);

        int ReadEnhancedDataFromFile(unsigned int file, unsigned int flags);
    };
}

#endif
