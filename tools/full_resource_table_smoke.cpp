// Execute the production table entries and bulk-registration body on a host canvas.
#include "engine/render/ResourceTexture.h"
#include "engine/render/ResourceMaterial.h"
#include "engine/render/ResourceMesh.h"
#include "game/core/String.h"
#include <cassert>
#include <cstring>

namespace AbyssEngine {
class Engine;
struct Resource;
class PaintCanvas {
public:
    Array<Resource *> resources;
    void SetResourceList(Resource *const *list, unsigned int count);
};
}

#include "full_resource_ctors_under_test.inc"
#include "full_resource_table_under_test.inc"
#include "full_resource_set_under_test.inc"

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
}
