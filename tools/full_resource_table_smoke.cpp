// Execute the production table entries and bulk-registration body on a host canvas.
#include "engine/render/ResourceTexture.h"
#include "engine/render/ResourceMaterial.h"
#include "engine/render/ResourceMesh.h"
#include "game/core/String.h"
#include <cassert>
#include <cstring>
#include <initializer_list>

namespace AbyssEngine {
class Engine;
struct Resource;
class PaintCanvas {
public:
    Array<Resource *> resources;
    void SetResourceList(Resource *const *list, unsigned int count);
    void AddResource(Resource *resource) { ArrayAdd(resource, resources); }
};
}
struct Globals {
    static unsigned char iPad;
    static unsigned char iPadHD;
    static unsigned char iPadLarge;
    static unsigned char retinaDisplay;
};
unsigned char Globals::iPad;
unsigned char Globals::iPadHD;
unsigned char Globals::iPadLarge;
unsigned char Globals::retinaDisplay;

#include "full_resource_ctors_under_test.inc"
#include "full_resource_table_under_test.inc"
#include "full_resource_set_under_test.inc"
#include "full_resource_hd_under_test.inc"
#include "full_resource_tail_oracle.inc"

int main() {
    AbyssEngine::PaintCanvas canvas;
    buildResourceTable(&canvas);
    assert(canvas.resources.count == 2488);
    for (unsigned int i = 0; i < 2488; ++i) {
        auto *resource = canvas.resources.data_[i];
        assert(resource && resource->unused == -1);
    }

    auto *texture = canvas.resources.data_[310];
    assert(texture->id == 33010 && texture->kind == 2);
    assert(std::strcmp(static_cast<AbyssEngine::ResourceTexture *>(texture->payload)->name,
                       "data/assets/main/3d/textures/low/etc/stations/stations_terran_diffuse.aei") == 0);
    auto *material = canvas.resources.data_[977];
    assert(material->id == 34817 && material->kind == 6);
    assert(static_cast<AbyssEngine::ResourceMaterial *>(material->payload)->texIndices[0] == 34816);
    material = canvas.resources.data_[997];
    assert(material->id == 34810 && material->kind == 6);
    assert(static_cast<AbyssEngine::ResourceMaterial *>(material->payload)->texIndices[0] == 24204);
    assert(static_cast<AbyssEngine::ResourceMaterial *>(material->payload)->texIndices[1] == 24205);
    material = canvas.resources.data_[998];
    assert(material->id == 34811 && material->kind == 6);
    assert(static_cast<AbyssEngine::ResourceMaterial *>(material->payload)->texIndices[0] == 24206);
    assert(static_cast<AbyssEngine::ResourceMaterial *>(material->payload)->texIndices[1] == 24207);
    auto *mesh = canvas.resources.data_[1700];
    assert(mesh->id == 6801 && mesh->kind == 4);
    assert(std::strcmp(static_cast<AbyssEngine::ResourceMesh *>(mesh->payload)->name,
                       "data/assets/main/3d/meshes/fx/projectile_025_anim_add.aem") == 0);
    assert(static_cast<AbyssEngine::ResourceMesh *>(mesh->payload)->wResourceId == 24201);

    // Counts from original ARM routes; index bits: HD, iPad, Retina, iPadLarge.
    const unsigned int counts[] = {234, 231, 264, 264, 235, 231, 271, 267,
                                   234, 231, 264, 264, 228, 231, 264, 267};
    for (unsigned char hd : {0, 1, 2, 255}) {
        for (unsigned char pad : {0, 1, 2, 255}) {
            for (unsigned char retina : {0, 1, 2, 255}) {
                for (unsigned char large : {0, 1, 2, 255}) {
                    Globals::iPadHD = hd;
                    Globals::iPad = pad;
                    Globals::retinaDisplay = retina;
                    Globals::iPadLarge = large;
                    unsigned int route = (hd != 0) | ((pad != 0) << 1) | ((retina != 0) << 2) | ((large != 0) << 3);
#ifdef HAVE_ARM_TAIL_ORACLE
                    assert(counts[route] == expectedTailCounts[route]);
#endif
                    unsigned int start = canvas.resources.count;
                    registerHdResources(&canvas);
                    assert(canvas.resources.count == start + counts[route]);
                    for (unsigned int i = 0; i < counts[route]; ++i) {
                        auto *resource = canvas.resources.data_[start + i];
                        assert(resource && resource->unused == -1);
                        assert(resource->id && (resource->kind == 2 || resource->kind == 3));
#ifdef HAVE_ARM_TAIL_ORACLE
                        const auto &expected = expectedTails[route][i];
                        assert(resource->id == expected.id && resource->kind == expected.kind);
                        if (resource->kind == 2) {
                            auto *payload = static_cast<AbyssEngine::ResourceTexture *>(resource->payload);
                            assert(payload->value == 0.0f);
                            assert(std::strcmp(payload->name, expected.path) == 0);
                        } else {
                            auto *payload = static_cast<AbyssEngine::ResourceImage *>(resource->payload);
                            assert(payload->lo == (expected.image & 65535u));
                            assert(payload->hi == (expected.image >> 16));
                        }
#endif
                    }
                    // Release each bounded run so the 256-case matrix stays small.
                    for (unsigned int i = start; i < canvas.resources.count; ++i) {
                        auto *resource = canvas.resources.data_[i];
                        if (resource->kind == 2)
                            delete static_cast<AbyssEngine::ResourceTexture *>(resource->payload);
                        else
                            delete static_cast<AbyssEngine::ResourceImage *>(resource->payload);
                        delete resource;
                    }
                    canvas.resources.count = start;
                }
            }
        }
    }
}
