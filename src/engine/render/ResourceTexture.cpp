#include "engine/render/ResourceTexture.h"
#include "game/core/String.h"


namespace AbyssEngine {
    ResourceTexture::ResourceTexture(const char *name, float value) {
        uint32_t len = String::GetStringLength(name);
        this->name = new char[len + 1U];
        memcpy(this->name, name, len + 1U);
        this->value = value;
    }

    ResourceTexture::ResourceTexture(const String &name, float value) {
        char *utf8 = String_GetAEChar(const_cast<String *>(&name));
        uint32_t len = String::GetStringLength(utf8);
        this->name = new char[len + 1U];
        memcpy(this->name, utf8, len + 1U);
        this->value = value;
        delete[] utf8;
    }

    ResourceTexture::~ResourceTexture() {
        delete[] this->name;
        this->name = nullptr;
    }
}
