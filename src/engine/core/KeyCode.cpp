#include "engine/core/KeyCode.h"

namespace AbyssEngine {
    KeyCode &KeyCode::operator=(const KeyCode &other) {
        this->code = other.code;
        this->name = other.name;
        return *this;
    }
}
