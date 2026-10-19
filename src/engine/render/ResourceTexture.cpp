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
        char *bytes = name.GetAEChar();
        // ARM 0x11b6e8 reads String+8 after conversion, not strlen(bytes).
        uint32_t len = static_cast<uint32_t>(name.length);
        this->name = new char[len + 1U];
        memcpy(this->name, bytes, len + 1U);
        this->value = value;
        delete[] bytes;
    }

    ResourceTexture::~ResourceTexture() {
        delete[] this->name;
        this->name = nullptr;
    }
}
