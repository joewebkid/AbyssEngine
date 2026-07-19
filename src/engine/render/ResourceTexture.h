#ifndef GOF2_RESOURCETEXTURE_H
#define GOF2_RESOURCETEXTURE_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
namespace AbyssEngine {
    class ResourceTexture {
    public:
        char *name;
        float value;

        ResourceTexture(const char *name, float value);

        ResourceTexture(const String &name, float value);

        ~ResourceTexture();
    };
}

#endif
