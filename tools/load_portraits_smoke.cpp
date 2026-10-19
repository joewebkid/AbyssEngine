#include "engine/render/ResourceTexture.h"
#include "game/core/String.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

struct Globals {
    static unsigned char iPadLarge, iPadHD, retinaDisplay, iPad, n9;
};
unsigned char Globals::iPadLarge, Globals::iPadHD, Globals::retinaDisplay,
              Globals::iPad, Globals::n9;

namespace AbyssEngine {
struct Resource;
// Canvas ownership and Engine/ApplicationManager are bounded host services.
struct PaintCanvas {
    std::vector<Resource *> resources;
    void AddResource(Resource *resource) { resources.push_back(resource); }
};
struct ApplicationManager { PaintCanvas *paintCanvas; };
struct Engine { ApplicationManager *appManager; };
}

// All tested String constructors pass false; language-dependent reversal is out of scope.
void String::Reverse() { assert(false); }
char *String_GetAEChar(String *value) { return value->GetAEChar(); }

// PRODUCTION_BODIES

int main(int argc, char **argv) {
    // ARM registration order: race, part, then zero-based variant.
    const int counts[][4] = {
        {11, 11, 11, 11}, {4, 5, 6, 9}, {5, 5, 5, 5}, {0, 0, 0, 0},
        {3, 3, 5, 4}, {0, 0, 0, 0}, {2, 3, 5, 0}, {2, 2, 3, 2},
        {0, 0, 0, 0}, {1, 1, 1, 1}, {4, 4, 5, 7}, {0, 0, 0, 2}, {1, 1, 1, 1},
    };
    for (unsigned mask = 0; mask < 32; ++mask) {
        for (unsigned value : {1u, 0x80u, 0xffu}) {
            Globals::iPadLarge = mask & 1 ? value : 0;
            Globals::iPadHD = mask & 2 ? value : 0;
            Globals::retinaDisplay = mask & 4 ? value : 0;
            Globals::iPad = mask & 8 ? value : 0;
            Globals::n9 = mask & 16 ? value : 0;
            const char *suffix = mask & 1 ? "_ipad_large" : mask & 2 ? "_ipad_1440" :
                                 mask & 28 ? "_ipad" : "";
            AbyssEngine::PaintCanvas canvas;
            AbyssEngine::ApplicationManager app{&canvas};
            AbyssEngine::Engine engine{&app};
            loadPortraits(&engine);
            unsigned index = 0;
            unsigned missing = 0;
            for (unsigned race = 0; race < 13; ++race) {
                for (unsigned part = 0; part < 4; ++part) {
                    for (int variant = 0; variant < counts[race][part]; ++variant) {
                        assert(index < canvas.resources.size());
                        auto *resource = canvas.resources[index];
                        assert(resource->id == 10200 + index);
                        assert(resource->kind == 2 && resource->unused == -1);
                        auto *texture = static_cast<AbyssEngine::ResourceTexture *>(resource->payload);
                        const int fileVariant = variant + (race == 11 ? 2 : 0);
                        const std::string expected = "data/textures/" + std::to_string(race) + "_" +
                            std::to_string(part) + "_" + std::to_string(fileVariant) + suffix + ".aei";
                        assert(expected == texture->name && texture->value == 0.0f);
                        if (argc == 2) {
                            const bool exists = std::filesystem::is_regular_file(
                                std::filesystem::path(argv[1]) / expected);
                            const bool knownMissing =
                                (std::strcmp(suffix, "_ipad") == 0 && race == 4 && part == 2 && variant >= 3) ||
                                (std::strcmp(suffix, "_ipad_1440") == 0 && race == 12);
                            assert(exists != knownMissing);
                            missing += !exists;
                        }
                        delete texture;
                        delete resource;
                        ++index;
                    }
                }
            }
            assert(index == 152 && canvas.resources.size() == index);
            if (argc == 2) {
                const unsigned expectedMissing = mask & 1 ? 0 : mask & 2 ? 4 : mask & 28 ? 2 : 0;
                assert(missing == expectedMissing);
            }
        }
    }
    std::puts("loadPortraits: 96 flag/value cases, 152 ordered IDs/paths each: OK");
    if (argc == 2)
        std::puts("Original Android assets: empty=152/152, ipad=150/152, 1440=148/152, large=152/152");
}
