#include "engine/render/Engine.h"
#include "engine/render/ShaderBaseStruct.h"

namespace AbyssEngine {

void Engine::ShaderUpdate() {
    uint32_t index = 0;
    while (index < this->shaders.size()) {
        this->shaders[index]->Update();
        index += 1;
    }
}

}
