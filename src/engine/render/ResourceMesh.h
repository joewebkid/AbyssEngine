#ifndef GOF2_RESOURCEMESH_H
#define GOF2_RESOURCEMESH_H
#include <cstdint>

namespace AbyssEngine {
    class ResourceMesh {
    public:
        char *name;
        uint16_t wResourceId;
        uint8_t flag;

        ResourceMesh(const char *name, unsigned short id, bool flag);

        ~ResourceMesh();
    };
}

#endif
