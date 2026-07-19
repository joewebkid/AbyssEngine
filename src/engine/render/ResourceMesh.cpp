#include "engine/render/ResourceMesh.h"
#include "game/core/String.h"


namespace AbyssEngine {
    ResourceMesh::ResourceMesh(const char *name, unsigned short id, bool flag)
        : wResourceId(id), flag(static_cast<uint8_t>(flag)) {
        uint32_t len = static_cast<uint32_t>(String::GetStringLength(name));
        this->name = new char[len + 1];
        memcpy(this->name, name, len + 1);
    }

    ResourceMesh::~ResourceMesh() {
        delete[] name;
        name = nullptr;
    }
}
