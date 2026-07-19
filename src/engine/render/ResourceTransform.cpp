#include "engine/render/ResourceTransform.h"

namespace AbyssEngine {
    ResourceTransform::~ResourceTransform() {
        if (this->dataA != nullptr) {
            ::operator delete[](this->dataA);
        }
        this->dataA = nullptr;

        if (this->dataB != nullptr) {
            ::operator delete[](this->dataB);
        }
        this->dataB = nullptr;
    }
}
