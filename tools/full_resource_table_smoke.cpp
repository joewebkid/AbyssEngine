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
    void AddResource(Resource *resource) { ArrayAdd(resource, resources); }
};
}
struct Globals {
    static unsigned char iPad;
    static unsigned char iPadHD;
    static unsigned char retinaDisplay;
};
unsigned char Globals::iPad;
unsigned char Globals::iPadHD;
unsigned char Globals::retinaDisplay;

#include "full_resource_ctors_under_test.inc"
#include "full_resource_table_under_test.inc"
#include "full_resource_set_under_test.inc"
#include "full_resource_hd_under_test.inc"

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

    struct TextureCase { unsigned short id; const char *path; };
    const TextureCase ordinary[] = {
        {11635, "data/textures/gof2_font_ar.aei"},
        {11637, "data/textures/gof2_font_cht.aei"},
        {11639, "data/textures/gof2_font_chs.aei"},
        {11643, "data/textures/gof2_font_kr.aei"},
        {11645, "data/textures/gof2_font_ja.aei"},
        {11641, "data/textures/gof2_font_langselect.aei"},
        {10062, "data/textures/gof2_interface.aei"},
        {10002, "data/textures/gof2_logos_ipad2.aei"},
        {27340, "data/textures/gof2_credits.aei"},
        {10089, "data/textures/gof2_interface3.aei"},
        {10064, "data/textures/gof2_items.aei"},
        {11632, "data/textures/gof2_items_2.aei"},
        {27341, "data/textures/gof2_challenge_interface.aei"},
        {27342, "data/textures/gof2_campaign_select.aei"},
        {27343, "data/textures/gof2_shop_button.aei"},
    };
    auto assertTexture = [&](unsigned int index, const TextureCase &expected) {
        auto *resource = canvas.resources.data_[index];
        assert(resource->id == expected.id && resource->kind == 2);
        assert(std::strcmp(static_cast<AbyssEngine::ResourceTexture *>(resource->payload)->name,
                           expected.path) == 0);
    };

    Globals::iPadHD = 0;
    registerHdResources(&canvas);
    assert(canvas.resources.count == 2503);
    for (unsigned int i = 0; i < 15; ++i)
        assertTexture(2488 + i, ordinary[i]);
    Globals::iPadHD = 1;
    registerHdResources(&canvas);
    assert(canvas.resources.count == 2515);
    auto *hdFont = canvas.resources.data_[2503];
    assert(hdFont->id == 11635 && hdFont->kind == 2);
    auto *hdShop = canvas.resources.data_[2514];
    assert(hdShop->id == 27343 && hdShop->kind == 2);
    assert(std::strcmp(static_cast<AbyssEngine::ResourceTexture *>(hdShop->payload)->name,
                       "data/textures/gof2_shop_button_ipad_1440.aei") == 0);

    Globals::iPad = 1;
    registerHdResources(&canvas);
    assert(canvas.resources.count == 2527);
    Globals::retinaDisplay = 1;
    registerHdResources(&canvas);
    assert(canvas.resources.count == 2547);
    auto *largeInterface = canvas.resources.data_[2539];
    assert(largeInterface->id == 10089 && largeInterface->kind == 2);
    auto *largeShop = canvas.resources.data_[2546];
    assert(largeShop->id == 27343 && largeShop->kind == 2);
    assert(std::strcmp(static_cast<AbyssEngine::ResourceTexture *>(largeShop->payload)->name,
                       "data/textures/gof2_shop_button_ipad_large.aei") == 0);

    Globals::iPadHD = 0;
    Globals::iPad = 0;
    registerHdResources(&canvas);
    assert(canvas.resources.count == 2563);
    auto *phoneFont = canvas.resources.data_[2547];
    assert(phoneFont->id == 11635 && phoneFont->kind == 2);
    auto *phoneShop = canvas.resources.data_[2562];
    assert(phoneShop->id == 27343 && phoneShop->kind == 2);
    assert(std::strcmp(static_cast<AbyssEngine::ResourceTexture *>(phoneShop->payload)->name,
                       "data/textures/gof2_shop_button_iphone4.aei") == 0);
    Globals::retinaDisplay = 0;
    registerHdResources(&canvas);
    assert(canvas.resources.count == 2578);
    for (unsigned int i = 0; i < 15; ++i)
        assertTexture(2563 + i, ordinary[i]);
    Globals::iPad = 1;
    registerHdResources(&canvas);
    assert(canvas.resources.count == 2587);
    for (unsigned int i = 0; i < 9; ++i)
        assertTexture(2578 + i, ordinary[i]);
}
