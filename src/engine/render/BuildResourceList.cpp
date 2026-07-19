

#include "engine/render/ResourceTexture.h"
#include "engine/render/ResourceMaterial.h"
#include "engine/render/ResourceMesh.h"
#ifndef GOF2_ENUM_BlendMode
#define GOF2_ENUM_BlendMode
#endif
#include "engine/render/Engine.h"
#include "engine/core/ApplicationManager.h"
#include "engine/render/PaintCanvas.h"

namespace AbyssEngine {
    struct Resource {
        unsigned short id;
        int kind;
        int unused;
        void *payload; // lint: void_ptr (heterogeneous resource payload dispatched by kind; no common base)
    };

    struct ResourceImage {
        unsigned short lo;
        unsigned short hi;
    };
}

void loadPortraits(AbyssEngine::Engine * engine);
void loadLowTexturesAndMaterials(AbyssEngine::Engine * engine);

namespace {
    inline AbyssEngine::Resource *makeRes(unsigned short id, int kind, void *payload) {
        // lint: void_ptr (heterogeneous resource payload dispatched by kind; no common base)
        AbyssEngine::Resource *r = new AbyssEngine::Resource;
        r->id = id;
        r->kind = kind;
        r->unused = -1;
        r->payload = payload;
        return r;
    }

    inline AbyssEngine::ResourceImage *newImage(unsigned short lo, unsigned short hi) {
        AbyssEngine::ResourceImage *p = new AbyssEngine::ResourceImage;
        p->lo = lo;
        p->hi = hi;
        return p;
    }
}

// Build one Resource inline: the original allocates the Resource first, then its
// payload, then fills the fields. A called helper would not inline under -Oz, so
// the per-entry construction is spelled out via this macro to match the codegen.
#define ADD_RES(slot, idv, kindv, payloadv)                       \
    do {                                                          \
        AbyssEngine::Resource *_r = new AbyssEngine::Resource;    \
        auto _p = (payloadv);                                     \
        _r->id = (idv);                                           \
        _r->kind = (kindv);                                       \
        _r->unused = -1;                                          \
        _r->payload = _p;                                         \
        (slot) = _r;                                              \
    } while (0)

void BuildResourceList(AbyssEngine::Engine *engine) {
    using namespace AbyssEngine;
    PaintCanvas *canvas = engine->appManager->paintCanvas;
    canvas->TextureCreateGlobal(String("data/assets/main/3d/textures/low/etc/fx/cloak_map.aei"), 6);

    Resource *resources[2488];
    ADD_RES(resources[0], 
        11742, 2, new AbyssEngine::ResourceTexture("data/assets/supernova/3d/textures/low/etc/suns/sn_sun_011.aei",
                                                   0.0f));
    ADD_RES(resources[1], 27335, 2, new AbyssEngine::ResourceTexture(
                               "data/assets/supernova/3d/textures/low/etc/misc/sn_fireworks_rocket_diffuse.aei", 0.0f));
    ADD_RES(resources[2], 27336, 2, new AbyssEngine::ResourceTexture(
                               "data/assets/supernova/3d/textures/low/etc/misc/sn_fireworks_rocket_normal_specular.aei",
                               0.0f));
    ADD_RES(resources[3], 27337, 6, new AbyssEngine::ResourceMaterial(27335, 27336, static_cast<BlendMode>(28)));
    ADD_RES(resources[4], 
        27338, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_fireworks_rocket.aem", 27337,
                                                false));
    ADD_RES(resources[5], 
        27309, 2, new AbyssEngine::ResourceTexture("data/assets/supernova/3d/textures/low/etc/fx/sn_fireworks.aei",
                                                   0.0f));
    ADD_RES(resources[6], 27308, 6, new AbyssEngine::ResourceMaterial(27309, BlendMode_2));
    ADD_RES(resources[7], 
        16809, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/fx/sn_fireworks_lookat_anim_add.aem",
                                                27308, false));
    ADD_RES(resources[8], 27320, 2, new AbyssEngine::ResourceTexture(
                               "data/assets/supernova/3d/textures/low/etc/fx/sn_sprite_fireworks_rocket_sparks.aei",
                               0.0f));
    ADD_RES(resources[9], 27321, 6, new AbyssEngine::ResourceMaterial(27320, BlendMode_2));
    ADD_RES(resources[10], 
        29100, 2, new AbyssEngine::ResourceTexture("data/assets/supernova/3d/textures/low/etc/fx/sn_projectiles.aei",
                                                   0.0f));
    ADD_RES(resources[11], 29101, 6, new AbyssEngine::ResourceMaterial(29100, BlendMode_2));
    ADD_RES(resources[12], 29102, 6, new AbyssEngine::ResourceMaterial(29100, BlendMode_1));
    ADD_RES(resources[13], 
        19090, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/fx/sn_projectile_228_anim_add.aem",
                                                29101, false));
    ADD_RES(resources[14], 
        19091, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/fx/sn_projectile_229_anim_alpha.aem",
                                                29102, false));
    ADD_RES(resources[15], 
        19094, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/fx/sn_projectile_231_anim_add.aem",
                                                29101, false));
    ADD_RES(resources[16], 
        19092, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/fx/sn_projectile_207_anim_add.aem",
                                                29101, false));
    ADD_RES(resources[17], 
        19093, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/fx/sn_projectile_222_anim_add.aem",
                                                29101, false));
    ADD_RES(resources[18], 
        29001, 2, new AbyssEngine::ResourceTexture("data/assets/supernova/3d/textures/low/etc/fx/sn_plasma_stream.aei",
                                                   0.0f));
    ADD_RES(resources[19], 
        29002, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/fx/sn_plasma_stream_normal.aei", 0.0f));
    ADD_RES(resources[20], 29003, 6, new AbyssEngine::ResourceMaterial(29001, static_cast<BlendMode>(3)));
    ADD_RES(resources[21], 
        19071, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/fx/sn_plasma_stream_anim_add.aem",
                                                29003, false));
    ADD_RES(resources[22], 
        29004, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/fx/sn_plasma_explosion.aei", 0.0f));
    ADD_RES(resources[23], 29005, 6, new AbyssEngine::ResourceMaterial(29004, static_cast<BlendMode>(3)));
    ADD_RES(resources[24], 19070, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/fx/sn_plasma_explosion_anim_lookat_add.aem", 29005,
                                false));
    ADD_RES(resources[25], 
        29006, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/fx/sn_shock_blast_sphere.aei", 0.0f));
    ADD_RES(resources[26], 
        29008, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/fx/sn_shock_blast_glow.aei", 0.0f));
    ADD_RES(resources[27], 29007, 6, new AbyssEngine::ResourceMaterial(29006, static_cast<BlendMode>(3)));
    ADD_RES(resources[28], 29009, 6, new AbyssEngine::ResourceMaterial(29008, static_cast<BlendMode>(3)));
    ADD_RES(resources[29], 
        18995, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/fx/sn_shock_blast_sphere_anim_add.aem",
                                                29007, false));
    ADD_RES(resources[30], 18996, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/fx/sn_shock_blast_glow_anim_lookat_add.aem", 29009,
                                false));
    ADD_RES(resources[31], 
        29046, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/misc/sn_sentry_gun_001_diffuse.aei", 0.0f));
    ADD_RES(resources[32], 29047, 2, new AbyssEngine::ResourceTexture(
                                "data/assets/supernova/3d/textures/low/etc/misc/sn_sentry_gun_001_normal_specular.aei",
                                0.0f));
    ADD_RES(resources[33], 29048, 6, new AbyssEngine::ResourceMaterial(29046, 29047, static_cast<BlendMode>(28)));
    ADD_RES(resources[34], 29049, 6, new AbyssEngine::ResourceMaterial(29046, BlendMode_2));
    ADD_RES(resources[35], 
        18880, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_sentry_gun_001.aem", 29048,
                                                false));
    ADD_RES(resources[36], 
        18883, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_sentry_gun_001_anim.aem",
                                                29048, false));
    ADD_RES(resources[37], 
        18886, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_sentry_gun_001_add.aem", 29049,
                                                false));
    ADD_RES(resources[38], 
        18889, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_sentry_gun_001_anim_add.aem",
                                                29049, false));
    ADD_RES(resources[39], 
        29050, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/misc/sn_sentry_gun_002_diffuse.aei", 0.0f));
    ADD_RES(resources[40], 29051, 2, new AbyssEngine::ResourceTexture(
                                "data/assets/supernova/3d/textures/low/etc/misc/sn_sentry_gun_002_normal_specular.aei",
                                0.0f));
    ADD_RES(resources[41], 29052, 6, new AbyssEngine::ResourceMaterial(29050, 29051, static_cast<BlendMode>(28)));
    ADD_RES(resources[42], 29053, 6, new AbyssEngine::ResourceMaterial(29050, BlendMode_2));
    ADD_RES(resources[43], 
        18881, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_sentry_gun_002.aem", 29052,
                                                false));
    ADD_RES(resources[44], 
        18884, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_sentry_gun_002_anim.aem",
                                                29052, false));
    ADD_RES(resources[45], 
        18887, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_sentry_gun_002_add.aem", 29053,
                                                false));
    ADD_RES(resources[46], 
        18890, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_sentry_gun_002_anim_add.aem",
                                                29053, false));
    ADD_RES(resources[47], 
        29054, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/misc/sn_sentry_gun_003_diffuse.aei", 0.0f));
    ADD_RES(resources[48], 29055, 2, new AbyssEngine::ResourceTexture(
                                "data/assets/supernova/3d/textures/low/etc/misc/sn_sentry_gun_003_normal_specular.aei",
                                0.0f));
    ADD_RES(resources[49], 29056, 6, new AbyssEngine::ResourceMaterial(29054, 29055, static_cast<BlendMode>(28)));
    ADD_RES(resources[50], 29057, 6, new AbyssEngine::ResourceMaterial(29054, BlendMode_2));
    ADD_RES(resources[51], 
        18882, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_sentry_gun_003.aem", 29056,
                                                false));
    ADD_RES(resources[52], 
        18885, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_sentry_gun_003_anim.aem",
                                                29056, false));
    ADD_RES(resources[53], 
        18888, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_sentry_gun_003_add.aem", 29057,
                                                false));
    ADD_RES(resources[54], 
        18891, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_sentry_gun_003_anim_add.aem",
                                                29057, false));
    ADD_RES(resources[55], 18847, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/skyboxes/sn_skybox_planet_ring_alpha.aem", 65535,
                                false));
    ADD_RES(resources[56], 
        29018, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/skyboxes/sn_skybox_planet_ring.aei", 0.0f));
    ADD_RES(resources[57], 
        18848, 4, new AbyssEngine::ResourceMesh(
            "data/assets/supernova/3d/meshes/skyboxes/sn_skybox_storms_anim_add.aem", 65535, false));
    ADD_RES(resources[58], 
        29019, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/skyboxes/sn_skybox_storms.aei", 0.0f));
    ADD_RES(resources[59], 11700, 2, new AbyssEngine::ResourceTexture(
                                "data/assets/supernova/3d/textures/low/etc/stations/sn_station_mining_plant_part1_diffuse.aei",
                                0.0f));
    ADD_RES(resources[60], 11702, 2, new AbyssEngine::ResourceTexture(
                                "data/assets/supernova/3d/textures/low/etc/stations/sn_station_mining_plant_part2_diffuse.aei",
                                0.0f));
    ADD_RES(resources[61], 11701, 2, new AbyssEngine::ResourceTexture(
                                "data/assets/supernova/3d/textures/low/etc/stations/sn_station_mining_plant_part1_normal_specular.aei",
                                0.0f));
    ADD_RES(resources[62], 11703, 2, new AbyssEngine::ResourceTexture(
                                "data/assets/supernova/3d/textures/low/etc/stations/sn_station_mining_plant_part2_normal_specular.aei",
                                0.0f));
    ADD_RES(resources[63], 11704, 6, new AbyssEngine::ResourceMaterial(11700, 11701, static_cast<BlendMode>(28)));
    ADD_RES(resources[64], 11708, 6, new AbyssEngine::ResourceMaterial(11702, 11703, static_cast<BlendMode>(28)));
    ADD_RES(resources[65], 11705, 6, new AbyssEngine::ResourceMaterial(11700, BlendMode_2));
    ADD_RES(resources[66], 11706, 6, new AbyssEngine::ResourceMaterial(11700, BlendMode_1));
    ADD_RES(resources[67], 11707, 6, new AbyssEngine::ResourceMaterial(11700, BlendMode_dummy));
    ADD_RES(resources[68], 11709, 6, new AbyssEngine::ResourceMaterial(11702, BlendMode_2));
    ADD_RES(resources[69], 11710, 6, new AbyssEngine::ResourceMaterial(11702, BlendMode_1));
    ADD_RES(resources[70], 11711, 6, new AbyssEngine::ResourceMaterial(11702, BlendMode_dummy));
    ADD_RES(resources[71], 19080, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/stations/sn_station_mining_plant_part1.aem", 11704,
                                false));
    ADD_RES(resources[72], 19081, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/stations/sn_station_mining_plant_part1_add.aem", 11705,
                                false));
    ADD_RES(resources[73], 19084, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/stations/sn_station_mining_plant_part1_alpha.aem",
                                11706, false));
    ADD_RES(resources[74], 19085, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/stations/sn_station_mining_plant_part1_emissive.aem",
                                11707, false));
    ADD_RES(resources[75], 19082, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/stations/sn_station_mining_plant_part1_glow_anim_add.aem",
                                11705, false));
    ADD_RES(resources[76], 19083, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/stations/sn_station_mining_plant_part1_fire_anim_add.aem",
                                11705, false));
    ADD_RES(resources[77], 19086, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/stations/sn_station_mining_plant_part2.aem", 11708,
                                false));
    ADD_RES(resources[78], 19087, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/stations/sn_station_mining_plant_part2_add.aem", 11709,
                                false));
    ADD_RES(resources[79], 19088, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/stations/sn_station_mining_plant_part2_emissive.aem",
                                11711, false));
    ADD_RES(resources[80], 11712, 2, new AbyssEngine::ResourceTexture(
                                "data/assets/supernova/3d/textures/low/etc/misc/sn_plasma_array_midorian_diffuse.aei",
                                0.0f));
    ADD_RES(resources[81], 11713, 2, new AbyssEngine::ResourceTexture(
                                "data/assets/supernova/3d/textures/low/etc/misc/sn_plasma_array_midorian_normal_specular.aei",
                                0.0f));
    ADD_RES(resources[82], 11714, 6, new AbyssEngine::ResourceMaterial(11712, 11713, static_cast<BlendMode>(28)));
    ADD_RES(resources[83], 11715, 6, new AbyssEngine::ResourceMaterial(11712, BlendMode_2));
    ADD_RES(resources[84], 11716, 6, new AbyssEngine::ResourceMaterial(11712, BlendMode_1));
    ADD_RES(resources[85], 18750, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_001.aem", 11714,
                                false));
    ADD_RES(resources[86], 18751, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_001_anim_add.aem",
                                11715, false));
    ADD_RES(resources[87], 18752, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_001_alpha.aem",
                                11716, false));
    ADD_RES(resources[88], 18753, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_002.aem", 11714,
                                false));
    ADD_RES(resources[89], 18754, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_002_anim_add.aem",
                                11715, false));
    ADD_RES(resources[90], 18755, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_002_alpha.aem",
                                11716, false));
    ADD_RES(resources[91], 18756, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_003.aem", 11714,
                                false));
    ADD_RES(resources[92], 18757, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_003_anim_add.aem",
                                11715, false));
    ADD_RES(resources[93], 18758, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_003_alpha.aem",
                                11716, false));
    ADD_RES(resources[94], 18759, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_004.aem", 11714,
                                false));
    ADD_RES(resources[95], 18760, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_004_anim_add.aem",
                                11715, false));
    ADD_RES(resources[96], 18761, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_004_alpha.aem",
                                11716, false));
    ADD_RES(resources[97], 18762, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_005.aem", 11714,
                                false));
    ADD_RES(resources[98], 18763, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_005_lights_anim_add.aem",
                                11715, false));
    ADD_RES(resources[99], 18765, 4, new AbyssEngine::ResourceMesh(
                                "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_005_glow_anim_add.aem",
                                11715, false));
    ADD_RES(resources[100], 18764, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_stage_005_alpha.aem",
                                 11716, false));
    ADD_RES(resources[101], 19050, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/misc/sn_plasma_array_midorian_explosion_anim.aem",
                                 11714, false));
    ADD_RES(resources[102], 11725, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/misc/sn_plasma_gun_valkyrie_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[103], 11726, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/misc/sn_plasma_gun_valkyrie_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[104], 11727, 6, new AbyssEngine::ResourceMaterial(11725, 11726, static_cast<BlendMode>(28)));
    ADD_RES(resources[105], 11728, 6, new AbyssEngine::ResourceMaterial(11725, BlendMode_2));
    ADD_RES(resources[106], 11729, 6, new AbyssEngine::ResourceMaterial(11725, static_cast<BlendMode>(10)));
    ADD_RES(resources[107], 
        18768, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_plasma_gun_valkyrie.aem",
                                                11727, false));
    ADD_RES(resources[108], 18769, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/misc/sn_plasma_gun_valkyrie_lights_add.aem", 11728,
                                 false));
    ADD_RES(resources[109], 
        18770, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_plasma_gun_valkyrie_alpha.aem",
                                                11729, false));
    ADD_RES(resources[110], 
        11850, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/fx/sn_plasma_gun_fx_valkyrie.aei", 0.0f));
    ADD_RES(resources[111], 11851, 6, new AbyssEngine::ResourceMaterial(11850, static_cast<BlendMode>(3)));
    ADD_RES(resources[112], 19061, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/fx/sn_plasma_gun_fx_valkyrie_beam_anim_add.aem",
                                 11851, false));
    ADD_RES(resources[113], 19062, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/fx/sn_plasma_gun_fx_valkyrie_cone_anim_add.aem",
                                 11851, false));
    ADD_RES(resources[114], 19063, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/fx/sn_plasma_gun_fx_valkyrie_star_add.aem", 11851,
                                 false));
    ADD_RES(resources[115], 29093, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_cargo_midorian_wrecked_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[116], 29094, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_cargo_midorian_wrecked_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[117], 29090, 6, new AbyssEngine::ResourceMaterial(29093, 29094, static_cast<BlendMode>(28)));
    ADD_RES(resources[118], 29091, 6, new AbyssEngine::ResourceMaterial(29093, BlendMode_2));
    ADD_RES(resources[119], 29092, 6, new AbyssEngine::ResourceMaterial(29093, BlendMode_dummy));
    ADD_RES(resources[120], 18766, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_cargo_001_midorian_wrecked.aem", 29090,
                                 false));
    ADD_RES(resources[121], 19072, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_cargo_001_midorian_wrecked_anim.aem", 29090,
                                 false));
    ADD_RES(resources[122], 19073, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_cargo_001_midorian_wrecked_glow_add.aem",
                                 29091, false));
    ADD_RES(resources[123], 19074, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_cargo_001_midorian_wrecked_glow_anim_add.aem",
                                 29091, false));
    ADD_RES(resources[124], 19075, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_cargo_001_midorian_wrecked_lights_add.aem",
                                 0, false));
    ADD_RES(resources[125], 19076, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_cargo_001_midorian_wrecked_lights_emissive.aem",
                                 29092, false));
    ADD_RES(resources[126], 11730, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/stations/sn_stations_midorian_wrecked_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[127], 11731, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/stations/sn_stations_midorian_wrecked_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[128], 11732, 6, new AbyssEngine::ResourceMaterial(11730, 11731, static_cast<BlendMode>(28)));
    ADD_RES(resources[129], 11733, 6, new AbyssEngine::ResourceMaterial(11730, static_cast<BlendMode>(10)));
    ADD_RES(resources[130], 18771, 6, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/stations/sn_station_midorian_wrecked.aem", 11732,
                                 false));
    ADD_RES(resources[131], 18772, 6, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/stations/sn_station_midorian_wrecked_alpha_emissive.aem",
                                 11733, false));
    ADD_RES(resources[132], 11747, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/stations/sn_burning_station_fire.aei",
                                 0.0f));
    ADD_RES(resources[133], 11750, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/stations/sn_burning_station_fire_normal.aei",
                                 0.0f));
    ADD_RES(resources[134], 11748, 6, new AbyssEngine::ResourceMaterial(11747, static_cast<BlendMode>(3)));
    ADD_RES(resources[135], 11749, 6, new AbyssEngine::ResourceMaterial(11747, BlendMode_1));
    ADD_RES(resources[136], 
        18781, 6, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/stations/sn_burning_station.aem",
                                                20007, false));
    ADD_RES(resources[137], 18782, 6, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/stations/sn_burning_station_emissive.aem", 20008,
                                 false));
    ADD_RES(resources[138], 18783, 6, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/stations/sn_burning_station_fire_anim_add.aem", 11748,
                                 false));
    ADD_RES(resources[139], 18784, 6, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/stations/sn_burning_station_fire_anim_alpha.aem",
                                 11749, false));
    ADD_RES(resources[140], 18832, 6, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/stations/sn_burning_station_fire_intro_anim_add.aem",
                                 11748, false));
    ADD_RES(resources[141], 18833, 6, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/stations/sn_burning_station_fire_intro_anim_alpha.aem",
                                 11749, false));
    ADD_RES(resources[142], 19095, 6, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/stations/sn_burning_valkyrie_stage_1_fire_anim_add.aem",
                                 11748, false));
    ADD_RES(resources[143], 19096, 6, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/stations/sn_burning_valkyrie_stage_1_smoke_anim_alpha.aem",
                                 11749, false));
    ADD_RES(resources[144], 19097, 6, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/stations/sn_burning_valkyrie_stage_2_fire_anim_add.aem",
                                 11748, false));
    ADD_RES(resources[145], 19098, 6, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/stations/sn_burning_valkyrie_stage_2_smoke_anim_alpha.aem",
                                 11749, false));
    ADD_RES(resources[146], 
        18785, 6, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_secure_container_nivelian.aem",
                                                0, false));
    ADD_RES(resources[147], 
        18831, 6, new AbyssEngine::ResourceMesh(
            "data/assets/supernova/3d/meshes/misc/sn_secure_container_nivelian_add.aem", 0, false));
    ADD_RES(resources[148], 
        29095, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/misc/sn_junk_field_diffuse.aei", 0.0f));
    ADD_RES(resources[149], 29096, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/misc/sn_junk_field_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[150], 29097, 6, new AbyssEngine::ResourceMaterial(29095, 29096, static_cast<BlendMode>(28)));
    ADD_RES(resources[151], 
        18786, 6, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_junk_field.aem", 29097,
                                                false));
    ADD_RES(resources[152], 
        11755, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/fx/sn_ship_blaze_flames.aei", 0.0f));
    ADD_RES(resources[153], 
        11759, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/fx/sn_ship_blaze_flames_normal.aei", 0.0f));
    ADD_RES(resources[154], 11756, 6, new AbyssEngine::ResourceMaterial(11755, static_cast<BlendMode>(3)));
    ADD_RES(resources[155], 
        11757, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/fx/sn_ship_blaze_glow.aei", 0.0f));
    ADD_RES(resources[156], 11758, 6, new AbyssEngine::ResourceMaterial(11757, static_cast<BlendMode>(3)));
    ADD_RES(resources[157], 
        18803, 6, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/fx/sn_ship_blaze_flames_anim_add.aem",
                                                11756, false));
    ADD_RES(resources[158], 
        18802, 6, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/fx/sn_ship_blaze_glow_anim_add.aem",
                                                11758, false));
    ADD_RES(resources[159], 29021, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/turrets/sn_plasma_collector_001_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[160], 29022, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/turrets/sn_plasma_collector_001_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[161], 29023, 6, new AbyssEngine::ResourceMaterial(29021, 29022, static_cast<BlendMode>(28)));
    ADD_RES(resources[162], 29024, 6, new AbyssEngine::ResourceMaterial(29021, BlendMode_2));
    ADD_RES(resources[163], 29025, 6, new AbyssEngine::ResourceMaterial(29021, BlendMode_8));
    ADD_RES(resources[164], 
        18787, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/turrets/sn_plasma_collector_001.aem",
                                                29023, false));
    ADD_RES(resources[165], 18788, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/turrets/sn_plasma_collector_001_add.aem", 29024,
                                 false));
    ADD_RES(resources[166], 18791, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/turrets/sn_plasma_collector_001_gun_anim.aem", 29023,
                                 false));
    ADD_RES(resources[167], 18790, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/turrets/sn_plasma_collector_001_gun_anim_add.aem",
                                 29024, false));
    ADD_RES(resources[168], 29027, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/turrets/sn_plasma_collector_002_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[169], 29028, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/turrets/sn_plasma_collector_002_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[170], 29029, 6, new AbyssEngine::ResourceMaterial(29027, 29028, static_cast<BlendMode>(28)));
    ADD_RES(resources[171], 29030, 6, new AbyssEngine::ResourceMaterial(29027, BlendMode_2));
    ADD_RES(resources[172], 29031, 6, new AbyssEngine::ResourceMaterial(29027, BlendMode_8));
    ADD_RES(resources[173], 
        18792, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/turrets/sn_plasma_collector_002.aem",
                                                29029, false));
    ADD_RES(resources[174], 18793, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/turrets/sn_plasma_collector_002_add.aem", 29030,
                                 false));
    ADD_RES(resources[175], 18795, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/turrets/sn_plasma_collector_002_gun_anim.aem", 29029,
                                 false));
    ADD_RES(resources[176], 18794, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/turrets/sn_plasma_collector_002_gun_anim_add.aem",
                                 29030, false));
    ADD_RES(resources[177], 29033, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/turrets/sn_plasma_collector_003_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[178], 29034, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/turrets/sn_plasma_collector_003_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[179], 29035, 6, new AbyssEngine::ResourceMaterial(29033, 29034, static_cast<BlendMode>(28)));
    ADD_RES(resources[180], 29036, 6, new AbyssEngine::ResourceMaterial(29033, BlendMode_2));
    ADD_RES(resources[181], 29037, 6, new AbyssEngine::ResourceMaterial(29033, BlendMode_8));
    ADD_RES(resources[182], 29038, 6, new AbyssEngine::ResourceMaterial(29033, BlendMode_dummy));
    ADD_RES(resources[183], 
        18796, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/turrets/sn_plasma_collector_003.aem",
                                                29035, false));
    ADD_RES(resources[184], 18797, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/turrets/sn_plasma_collector_003_add.aem", 29036,
                                 false));
    ADD_RES(resources[185], 18799, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/turrets/sn_plasma_collector_003_gun_anim.aem", 29035,
                                 false));
    ADD_RES(resources[186], 18800, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/turrets/sn_plasma_collector_003_gun_emissive.aem",
                                 29038, false));
    ADD_RES(resources[187], 18798, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/turrets/sn_plasma_collector_003_gun_anim_add.aem",
                                 29036, false));
    ADD_RES(resources[188], 11770, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_carrier_terran_1_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[189], 11771, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_carrier_terran_2_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[190], 11772, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_carrier_terran_3_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[191], 11773, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_carrier_terran_1_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[192], 11774, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_carrier_terran_2_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[193], 11775, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_carrier_terran_3_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[194], 11776, 6, new AbyssEngine::ResourceMaterial(11770, 11773, static_cast<BlendMode>(28)));
    ADD_RES(resources[195], 11777, 6, new AbyssEngine::ResourceMaterial(11771, 11774, static_cast<BlendMode>(28)));
    ADD_RES(resources[196], 11778, 6, new AbyssEngine::ResourceMaterial(11772, 11775, static_cast<BlendMode>(28)));
    ADD_RES(resources[197], 11779, 6, new AbyssEngine::ResourceMaterial(11770, BlendMode_dummy));
    ADD_RES(resources[198], 11780, 6, new AbyssEngine::ResourceMaterial(11771, BlendMode_dummy));
    ADD_RES(resources[199], 11781, 6, new AbyssEngine::ResourceMaterial(11772, BlendMode_dummy));
    ADD_RES(resources[200], 11782, 6, new AbyssEngine::ResourceMaterial(11772, BlendMode_2));
    ADD_RES(resources[201], 
        18804, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_carrier_terran_1.aem", 11776,
                                                false));
    ADD_RES(resources[202], 
        18805, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_carrier_terran_2.aem", 11777,
                                                false));
    ADD_RES(resources[203], 
        18807, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_carrier_terran_3.aem", 11778,
                                                false));
    ADD_RES(resources[204], 
        18808, 4, new AbyssEngine::ResourceMesh(
            "data/assets/supernova/3d/meshes/ships/sn_carrier_terran_1_emissive.aem", 11779, false));
    ADD_RES(resources[205], 
        18809, 4, new AbyssEngine::ResourceMesh(
            "data/assets/supernova/3d/meshes/ships/sn_carrier_terran_2_emissive.aem", 11780, false));
    ADD_RES(resources[206], 
        18810, 4, new AbyssEngine::ResourceMesh(
            "data/assets/supernova/3d/meshes/ships/sn_carrier_terran_3_emissive.aem", 11781, false));
    ADD_RES(resources[207], 
        18806, 4, new AbyssEngine::ResourceMesh(
            "data/assets/supernova/3d/meshes/ships/sn_carrier_terran_lights_add.aem", 11782, false));
    ADD_RES(resources[208], 29039, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_battleship_vossk_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[209], 29040, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_battleship_vossk_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[210], 29041, 6, new AbyssEngine::ResourceMaterial(29039, 29040, static_cast<BlendMode>(28)));
    ADD_RES(resources[211], 29042, 6, new AbyssEngine::ResourceMaterial(29039, BlendMode_2));
    ADD_RES(resources[212], 
        19052, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_battleship_vossk_add.aem",
                                                29042, false));
    ADD_RES(resources[213], 19053, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_battleship_vossk_lights_add.aem", 29042,
                                 false));
    ADD_RES(resources[214], 
        19051, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_battleship_vossk.aem", 29041,
                                                false));
    ADD_RES(resources[215], 
        17251, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_044_elite_nivelian.aem",
                                                0, false));
    ADD_RES(resources[216], 18779, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_044_elite_nivelian_lights.aem", 32544,
                                 false));
    ADD_RES(resources[217], 18044, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_044_elite_nivelian_engine_add.aem",
                                 32544, false));
    ADD_RES(resources[218], 
        17252, 4, new AbyssEngine::ResourceMesh(
            "data/assets/supernova/3d/meshes/ships/sn_ship_044_elite_nivelian_lod_1.aem", 0, false));
    ADD_RES(resources[219], 
        17253, 4, new AbyssEngine::ResourceMesh(
            "data/assets/supernova/3d/meshes/ships/sn_ship_044_elite_nivelian_lod_2.aem", 0, false));
    ADD_RES(resources[220], 
        17257, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_045_most_wanted.aem", 0,
                                                false));
    ADD_RES(resources[221], 18045, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_045_most_wanted_engine_add.aem", 32545,
                                 false));
    ADD_RES(resources[222], 18811, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_045_most_wanted_lights.aem", 32545,
                                 false));
    ADD_RES(resources[223], 
        17263, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_046_most_wanted.aem", 0,
                                                false));
    ADD_RES(resources[224], 18046, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_046_most_wanted_engine_add.aem", 32546,
                                 false));
    ADD_RES(resources[225], 18812, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_046_most_wanted_lights_add.aem", 32546,
                                 false));
    ADD_RES(resources[226], 
        17269, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_047_most_wanted.aem", 0,
                                                false));
    ADD_RES(resources[227], 18813, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_047_most_wanted_lights_add.aem", 32547,
                                 false));
    ADD_RES(resources[228], 18047, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_047_most_wanted_engine_add.aem", 32547,
                                 false));
    ADD_RES(resources[229], 
        17275, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_048_most_wanted.aem", 0,
                                                false));
    ADD_RES(resources[230], 18780, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_048_most_wanted_lights.aem", 32548,
                                 false));
    ADD_RES(resources[231], 18048, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_048_most_wanted_engine_add.aem", 32548,
                                 false));
    ADD_RES(resources[232], 
        17281, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_049_boss_nivelian.aem",
                                                0, false));
    ADD_RES(resources[233], 18049, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_049_boss_nivelian_engine_add.aem",
                                 32549, false));
    ADD_RES(resources[234], 17286, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_049_boss_nivelian_lights_add.aem",
                                 32549, false));
    ADD_RES(resources[235], 
        17293, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_051_dropship_terran.aem",
                                                0, false));
    ADD_RES(resources[236], 18051, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_051_dropship_terran_engine_add.aem",
                                 32551, false));
    ADD_RES(resources[237], 18814, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_051_dropship_terran_lights.aem", 32551,
                                 false));
    ADD_RES(resources[238], 
        17299, 4,
        new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_052_retro.aem", 0, false));
    ADD_RES(resources[239], 
        18052, 4, new AbyssEngine::ResourceMesh(
            "data/assets/supernova/3d/meshes/ships/sn_ship_052_retro_engine_add.aem", 32552, false));
    ADD_RES(resources[240], 
        18815, 4, new AbyssEngine::ResourceMesh(
            "data/assets/supernova/3d/meshes/ships/sn_ship_052_retro_lights_add.aem", 32552, false));
    ADD_RES(resources[241], 
        17311, 4,
        new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_054_vossk.aem", 0, false));
    ADD_RES(resources[242], 
        18054, 4, new AbyssEngine::ResourceMesh(
            "data/assets/supernova/3d/meshes/ships/sn_ship_054_vossk_engine_add.aem", 32554, false));
    ADD_RES(resources[243], 
        17316, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_054_vossk_emissive.aem",
                                                0, false));
    ADD_RES(resources[244], 
        17317, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_055_modified.aem", 0,
                                                false));
    ADD_RES(resources[245], 18818, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_055_modified_lights_add.aem", 32555,
                                 false));
    ADD_RES(resources[246], 
        17323, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_056_modified.aem", 0,
                                                false));
    ADD_RES(resources[247], 18819, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_056_modified_lights_add.aem", 32556,
                                 false));
    ADD_RES(resources[248], 
        17329, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_057_modified.aem", 0,
                                                false));
    ADD_RES(resources[249], 18820, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_057_modified_lights_add.aem", 32557,
                                 false));
    ADD_RES(resources[250], 
        17335, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_058_modified.aem", 0,
                                                false));
    ADD_RES(resources[251], 18821, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_058_modified_lights_add.aem", 32558,
                                 false));
    ADD_RES(resources[252], 
        17341, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_059_modified.aem", 0,
                                                false));
    ADD_RES(resources[253], 18822, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/ships/sn_ship_059_modified_lights_add.aem", 32559,
                                 false));
    ADD_RES(resources[254], 
        17347, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/ships/sn_ship_060_modified.aem", 0,
                                                false));
    ADD_RES(resources[255], 
        18823, 4, new AbyssEngine::ResourceMesh(
            "data/assets/supernova/3d/meshes/ships/sn_ship_060_modified_lights_add.aem", 0, false));
    ADD_RES(resources[256], 
        17353, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/ship_061_elite_nivelian_prototype.aem", 0, false));
    ADD_RES(resources[257], 18824, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/main/3d/meshes/ships/ship_061_elite_nivelian_prototype_lights_add.aem",
                                 32561, false));
    ADD_RES(resources[258], 
        17359, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_062_prototype.aem", 0, false));
    ADD_RES(resources[259], 
        18825, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_062_prototype_lights_add.aem",
                                                32562, false));
    ADD_RES(resources[260], 
        17365, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_063_vossk_prototype.aem", 0,
                                                false));
    ADD_RES(resources[261], 18826, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/main/3d/meshes/ships/ship_063_vossk_prototype_lights_add.aem", 32563,
                                 false));
    ADD_RES(resources[262], 17370, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/main/3d/meshes/ships/ship_063_vossk_prototype_lights_emissive.aem", 0,
                                 false));
    ADD_RES(resources[263], 
        11741, 2, new AbyssEngine::ResourceTexture("data/assets/supernova/3d/textures/low/etc/fx/sn_supernova.aei",
                                                   0.0f));
    ADD_RES(resources[264], 
        11761, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/meshes/fx/sn_sun_explosion_ring_anim_add.aem", 0.0f));
    ADD_RES(resources[265], 
        11762, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/meshes/fx/sn_sun_explosion_core_anim_add.aem", 0.0f));
    ADD_RES(resources[266], 
        11764, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/fx/sn_sun_explosion_ring.aei", 0.0f));
    ADD_RES(resources[267], 
        11763, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/fx/sn_sun_explosion_core.aei", 0.0f));
    ADD_RES(resources[268], 
        11870, 2, new AbyssEngine::ResourceTexture("data/assets/supernova/3d/textures/low/etc/misc/sn_gas_plasma.aei",
                                                   0.0f));
    ADD_RES(resources[269], 
        11871, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/misc/sn_gas_plasma_normal.aei", 0.0f));
    ADD_RES(resources[270], 29045, 6, new AbyssEngine::ResourceMaterial(11870, BlendMode_2));
    ADD_RES(resources[271], 18997, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/misc/sn_gas_cloud_green_anim_lookat_add.aem", 29045,
                                 false));
    ADD_RES(resources[272], 18998, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/misc/sn_gas_cloud_blue_anim_lookat_add.aem", 29045,
                                 false));
    ADD_RES(resources[273], 18999, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/misc/sn_gas_cloud_violet_anim_lookat_add.aem", 29045,
                                 false));
    ADD_RES(resources[274], 19000, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/misc/sn_gas_cloud_red_anim_lookat_add.aem", 29045,
                                 false));
    ADD_RES(resources[275], 19001, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/misc/sn_plasma_green_anim_lookat_add.aem", 29045,
                                 false));
    ADD_RES(resources[276], 19002, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/misc/sn_plasma_blue_anim_lookat_add.aem", 29045,
                                 false));
    ADD_RES(resources[277], 19003, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/misc/sn_plasma_violet_anim_lookat_add.aem", 29045,
                                 false));
    ADD_RES(resources[278], 
        19004, 4, new AbyssEngine::ResourceMesh(
            "data/assets/supernova/3d/meshes/misc/sn_plasma_red_anim_lookat_add.aem", 29045, false));
    ADD_RES(resources[279], 11753, 6, new AbyssEngine::ResourceMaterial(10080, BlendMode_dummy));
    ADD_RES(resources[280], 11754, 6, new AbyssEngine::ResourceMaterial(10084, BlendMode_dummy));
    ADD_RES(resources[281], 11783, 6, new AbyssEngine::ResourceMaterial(10081, BlendMode_dummy));
    ADD_RES(resources[282], 29060, 6, new AbyssEngine::ResourceMaterial(10082, BlendMode_dummy));
    ADD_RES(resources[283], 29061, 6, new AbyssEngine::ResourceMaterial(10083, BlendMode_dummy));
    ADD_RES(resources[284], 
        17815, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/skyboxes/sn_skybox_015.aem", 11753,
                                                false));
    ADD_RES(resources[285], 17824, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/skyboxes/sn_skybox_015_flares_1_anim.aem", 65535,
                                 false));
    ADD_RES(resources[286], 17825, 4, new AbyssEngine::ResourceMesh(
                                 "data/assets/supernova/3d/meshes/skyboxes/sn_skybox_015_flares_2_anim.aem", 65535,
                                 false));
    ADD_RES(resources[287], 
        17816, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/skyboxes/sn_skybox_016.aem", 11783,
                                                false));
    ADD_RES(resources[288], 
        17817, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/skyboxes/sn_skybox_017.aem", 29060,
                                                false));
    ADD_RES(resources[289], 
        17818, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/skyboxes/sn_skybox_018.aem", 29061,
                                                false));
    ADD_RES(resources[290], 10150, 4, new AbyssEngine::ResourceMesh("data/textures/lens_flare_0.aei", 65535, false));
    ADD_RES(resources[291], 10151, 4, new AbyssEngine::ResourceMesh("data/textures/lens_flare_1.aei", 65535, false));
    ADD_RES(resources[292], 10152, 4, new AbyssEngine::ResourceMesh("data/textures/lens_flare_2.aei", 65535, false));
    ADD_RES(resources[293], 
        34500, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/glow_midorian.aei", 0.0f));
    ADD_RES(resources[294], 
        34501, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/glow_nivelian.aei", 0.0f));
    ADD_RES(resources[295], 
        34502, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/glow_pirates.aei", 0.0f));
    ADD_RES(resources[296], 
        34503, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/glow_terran.aei", 0.0f));
    ADD_RES(resources[297], 
        34504, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/glow_vossk.aei", 0.0f));
    ADD_RES(resources[298], 
        34505, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/glow_void.aei", 0.0f));
    ADD_RES(resources[299], 
        10031, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/suns/sun_000.aei", 0.0f));
    ADD_RES(resources[300], 
        10032, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/suns/sun_001.aei", 0.0f));
    ADD_RES(resources[301], 
        10033, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/suns/sun_002.aei", 0.0f));
    ADD_RES(resources[302], 
        10034, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/suns/sun_003.aei", 0.0f));
    ADD_RES(resources[303], 
        10035, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/suns/sun_004.aei", 0.0f));
    ADD_RES(resources[304], 
        10036, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/suns/sun_005.aei", 0.0f));
    ADD_RES(resources[305], 
        10037, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/suns/sun_006.aei", 0.0f));
    ADD_RES(resources[306], 
        10038, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/suns/sun_007.aei", 0.0f));
    ADD_RES(resources[307], 
        10039, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/suns/sun_008.aei", 0.0f));
    ADD_RES(resources[308], 
        10040, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/suns/sun_009.aei", 0.0f));
    ADD_RES(resources[309], 
        10041, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/suns/sun_010.aei", 0.0f));
    ADD_RES(resources[310], 6801, 4, newImage(0x0u, 0x0u));
    ADD_RES(resources[311], 33011, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/stations/stations_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[312], 
        33008, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_pirates_diffuse.aei", 0.0f));
    ADD_RES(resources[313], 33009, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/stations/stations_pirates_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[314], 
        33006, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_nivelian_diffuse.aei", 0.0f));
    ADD_RES(resources[315], 33007, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/stations/stations_nivelian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[316], 
        33004, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_midorian_diffuse.aei", 0.0f));
    ADD_RES(resources[317], 33005, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/stations/stations_midorian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[318], 
        33014, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_vossk_diffuse.aei", 0.0f));
    ADD_RES(resources[319], 33015, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/stations/stations_vossk_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[320], 
        33012, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_void_diffuse.aei", 0.0f));
    ADD_RES(resources[321], 33013, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/stations/stations_void_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[322], 
        33016, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_void_glow.aei", 0.0f));
    ADD_RES(resources[323], 20048, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[324], 
        34000, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_000_midorian_diffuse.aei", 0.0f));
    ADD_RES(resources[325], 
        34001, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_001_terran_diffuse.aei", 0.0f));
    ADD_RES(resources[326], 
        34002, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_002_pirates_diffuse.aei", 0.0f));
    ADD_RES(resources[327], 
        34003, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_003_midorian_diffuse.aei", 0.0f));
    ADD_RES(resources[328], 
        34004, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_004_nivelian_diffuse.aei", 0.0f));
    ADD_RES(resources[329], 
        34005, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_005_terran_diffuse.aei", 0.0f));
    ADD_RES(resources[330], 
        34006, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_006_midorian_diffuse.aei", 0.0f));
    ADD_RES(resources[331], 
        34007, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_007_terran_diffuse.aei", 0.0f));
    ADD_RES(resources[332], 
        34008, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_008_void_diffuse.aei", 0.0f));
    ADD_RES(resources[333], 
        34009, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_009_vossk_diffuse.aei", 0.0f));
    ADD_RES(resources[334], 
        34010, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_010_terran_diffuse.aei", 0.0f));
    ADD_RES(resources[335], 
        34011, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_011_pirates_diffuse.aei", 0.0f));
    ADD_RES(resources[336], 
        34012, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_012_nivelian_diffuse.aei", 0.0f));
    ADD_RES(resources[337], 
        34016, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_016_nivelian_diffuse.aei", 0.0f));
    ADD_RES(resources[338], 
        34017, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_017_terran_diffuse.aei", 0.0f));
    ADD_RES(resources[339], 
        34018, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_018_nivelian_diffuse.aei", 0.0f));
    ADD_RES(resources[340], 
        34019, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_019_midorian_diffuse.aei", 0.0f));
    ADD_RES(resources[341], 
        34020, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_020_midorian_diffuse.aei", 0.0f));
    ADD_RES(resources[342], 
        34021, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_021_nivelian_diffuse.aei", 0.0f));
    ADD_RES(resources[343], 
        34022, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_022_terran_diffuse.aei", 0.0f));
    ADD_RES(resources[344], 
        34023, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_023_pirates_diffuse.aei", 0.0f));
    ADD_RES(resources[345], 
        34024, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_024_pirates_diffuse.aei", 0.0f));
    ADD_RES(resources[346], 
        34025, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_025_pirates_diffuse.aei", 0.0f));
    ADD_RES(resources[347], 
        34026, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_026_terran_diffuse.aei", 0.0f));
    ADD_RES(resources[348], 
        34027, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_027_terran_diffuse.aei", 0.0f));
    ADD_RES(resources[349], 
        34028, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_028_terran_diffuse.aei", 0.0f));
    ADD_RES(resources[350], 
        34029, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_029_pirates_diffuse.aei", 0.0f));
    ADD_RES(resources[351], 
        34030, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_030_midorian_diffuse.aei", 0.0f));
    ADD_RES(resources[352], 
        34031, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_031_nivelian_diffuse.aei", 0.0f));
    ADD_RES(resources[353], 
        34032, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_032_pirates_diffuse.aei", 0.0f));
    ADD_RES(resources[354], 
        34033, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_033_terran_diffuse.aei", 0.0f));
    ADD_RES(resources[355], 
        34034, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_034_terran_diffuse.aei", 0.0f));
    ADD_RES(resources[356], 
        34035, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_035_nivelian_diffuse.aei", 0.0f));
    ADD_RES(resources[357], 
        34036, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_036_terran_diffuse.aei", 0.0f));
    ADD_RES(resources[358], 34037, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/ships/v_ship_037_deep_science_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[359], 34038, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/ships/v_ship_038_deep_science_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[360], 
        34039, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/ships/v_ship_039_vossk_diffuse.aei", 0.0f));
    ADD_RES(resources[361], 34040, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/ships/v_ship_040_deep_science_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[362], 
        34041, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/ships/v_ship_041_vossk_diffuse.aei", 0.0f));
    ADD_RES(resources[363], 
        18083, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27284, false));
    ADD_RES(resources[364], 
        34043, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_043_retro_diffuse.aei", 0.0f));
    ADD_RES(resources[365], 34044, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_044_elite_nivelian_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[366], 34045, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_045_most_wanted_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[367], 34046, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_046_most_wanted_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[368], 34047, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_047_most_wanted_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[369], 34048, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_048_most_wanted_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[370], 34049, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_049_boss_nivelian_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[371], 34051, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_051_dropship_terran_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[372], 
        34052, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_052_retro_diffuse.aei", 0.0f));
    ADD_RES(resources[373], 
        34054, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_054_vossk_diffuse.aei", 0.0f));
    ADD_RES(resources[374], 34055, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_055_modified_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[375], 34056, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_056_modified_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[376], 34057, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_057_modified_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[377], 34058, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_058_modified_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[378], 34059, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_059_modified_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[379], 34060, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_060_modified_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[380], 34061, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_061_elite_nivelian_prototype_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[381], 
        34062, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_062_prototype_diffuse.aei", 0.0f));
    ADD_RES(resources[382], 
        18086, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27287, false));
    ADD_RES(resources[383], 34100, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_000_midorian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[384], 34101, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_001_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[385], 34102, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_002_pirates_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[386], 34103, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_003_midorian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[387], 34104, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_004_nivelian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[388], 34105, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_005_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[389], 34106, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_006_midorian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[390], 34107, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_007_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[391], 
        34108, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_008_void_normal_specular.aei", 0.0f));
    ADD_RES(resources[392], 
        34109, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_009_vossk_normal_specular.aei", 0.0f));
    ADD_RES(resources[393], 34110, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_010_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[394], 34111, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_011_pirates_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[395], 34112, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_012_nivelian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[396], 34116, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_016_nivelian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[397], 34117, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_017_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[398], 34118, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_018_nivelian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[399], 34119, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_019_midorian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[400], 34120, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_020_midorian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[401], 34121, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_021_nivelian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[402], 34122, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_022_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[403], 34123, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_023_pirates_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[404], 34124, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_024_pirates_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[405], 34125, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_025_pirates_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[406], 34126, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_026_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[407], 34127, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_027_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[408], 34128, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_028_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[409], 34129, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_029_pirates_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[410], 34130, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_030_midorian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[411], 34131, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_031_nivelian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[412], 34132, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_032_pirates_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[413], 34133, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_033_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[414], 34134, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_034_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[415], 34135, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_035_nivelian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[416], 34136, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_036_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[417], 34137, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/ships/v_ship_037_deep_science_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[418], 34138, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/ships/v_ship_038_deep_science_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[419], 34139, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/ships/v_ship_039_vossk_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[420], 34140, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/ships/v_ship_040_deep_science_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[421], 34141, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/ships/v_ship_041_vossk_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[422], 
        34142, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_042_retro_normal_specular.aei", 0.0f));
    ADD_RES(resources[423], 
        34143, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_043_retro_normal_specular.aei", 0.0f));
    ADD_RES(resources[424], 34144, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_044_elite_nivelian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[425], 34145, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_045_most_wanted_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[426], 34146, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_046_most_wanted_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[427], 34147, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_047_most_wanted_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[428], 34148, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_048_most_wanted_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[429], 34149, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_049_boss_nivelian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[430], 34151, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_051_dropship_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[431], 34152, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_052_retro_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[432], 34154, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_054_vossk_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[433], 34155, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_055_modified_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[434], 34156, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_056_modified_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[435], 34157, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_057_modified_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[436], 34158, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_058_modified_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[437], 34159, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_059_modified_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[438], 34160, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/ships/sn_ship_060_modified_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[439], 34161, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_061_elite_nivelian_prototype_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[440], 34162, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/ships/ship_062_prototype_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[441], 
        34163, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_063_vossk_normal_specular.aei", 0.0f));
    ADD_RES(resources[442], 
        10107, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/misc/asteroid_explosion.aei",
                                                   0.0f));
    ADD_RES(resources[443], 
        10155, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/misc/v_asteroid_ice_diffuse.aei", 0.0f));
    ADD_RES(resources[444], 10156, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/misc/v_asteroid_ice_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[445], 27314, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/misc/sn_asteroid_magma_explosion.aei",
                                 0.0f));
    ADD_RES(resources[446], 
        28999, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/misc/sn_asteroid_magma_diffuse.aei", 0.0f));
    ADD_RES(resources[447], 29000, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/misc/sn_asteroid_magma_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[448], 
        10104, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/misc/wormhole.aei", 0.0f));
    ADD_RES(resources[449], 
        10121, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/fog.aei", 0.0f));
    ADD_RES(resources[450], 
        10132, 2, new AbyssEngine::ResourceTexture("data/assets/valkyrie/3d/textures/low/etc/fx/v_fog_ice.aei", 0.0f));
    ADD_RES(resources[451], 
        10100, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/galaxymap/galaxymap_bg.aei",
                                                   0.0f));
    ADD_RES(resources[452], 
        10101, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/galaxymap/galaxymap_fog_layer_0.aei", 0.0f));
    ADD_RES(resources[453], 
        10102, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/galaxymap/galaxymap_fog_layer_1.aei", 0.0f));
    ADD_RES(resources[454], 
        10145, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/galaxymap/galaxymap_planets_diffuse.aei", 0.0f));
    ADD_RES(resources[455], 
        10144, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/galaxymap/galaxymap_orbit.aei",
                                                   0.0f));
    ADD_RES(resources[456], 
        10159, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/galaxymap/v_galaxymap_planets.aei", 0.0f));
    ADD_RES(resources[457], 
        18180, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_000.aem", 20027, false));
    ADD_RES(resources[458], 
        0, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/bars/bar_visitor_glow_vossk.aei",
                                               0.0f));
    ADD_RES(resources[459], 
        18186, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_006.aem", 20027, false));
    ADD_RES(resources[460], 
        33403, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/bars/bar_visitor_glow_midorian.aei", 0.0f));
    ADD_RES(resources[461], 
        33404, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/bars/bar_visitor_shadow.aei",
                                                   0.0f));
    ADD_RES(resources[462], 
        10161, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/galaxymap/sn_galaxymap_planets.aei", 0.0f));
    ADD_RES(resources[463], 
        11606, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/bars/visitor_bobolan.aei",
                                                   0.0f));
    ADD_RES(resources[464], 
        11607, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/bars/visitor_grey.aei", 0.0f));
    ADD_RES(resources[465], 
        11608, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/bars/visitor_multipod.aei",
                                                   0.0f));
    ADD_RES(resources[466], 
        11609, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/bars/visitor_nivelian.aei",
                                                   0.0f));
    ADD_RES(resources[467], 
        11610, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/bars/visitor_terran_f.aei",
                                                   0.0f));
    ADD_RES(resources[468], 
        11611, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/bars/visitor_terran_m.aei",
                                                   0.0f));
    ADD_RES(resources[469], 
        11612, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/bars/visitor_vossk.aei",
                                                   0.0f));
    ADD_RES(resources[470], 10157, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/hangars/v_hangar_battlestation_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[471], 10158, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/hangars/v_hangar_battlestation_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[472], 10134, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/hangars/v_hangar_deep_science_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[473], 10135, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/hangars/v_hangar_deep_science_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[474], 11600, 2, new AbyssEngine::ResourceTexture("data/textures/col_test.aei", 0.0f));
    ADD_RES(resources[475], 
        12000, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/misc/space_junk_diffuse.aei",
                                                   0.0f));
    ADD_RES(resources[476], 
        12001, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/space_junk_normal_specular.aei", 0.0f));
    ADD_RES(resources[477], 
        11601, 2,
        new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/sprite_explosion.aei", 0.0f));
    ADD_RES(resources[478], 
        11602, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/sprite_smoke.aei", 0.0f));
    ADD_RES(resources[479], 
        11599, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/sprite_fire.aei", 0.0f));
    ADD_RES(resources[480], 
        11598, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/explosion.aei", 0.0f));
    ADD_RES(resources[481], 
        12002, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/turrets/turret_001_diffuse.aei", 0.0f));
    ADD_RES(resources[482], 
        12003, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/turrets/turret_001_normal_specular.aei", 0.0f));
    ADD_RES(resources[483], 
        12004, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/turrets/turret_002_diffuse.aei", 0.0f));
    ADD_RES(resources[484], 
        12005, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/turrets/turret_002_normal_specular.aei", 0.0f));
    ADD_RES(resources[485], 
        12006, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/turrets/turret_003_diffuse.aei", 0.0f));
    ADD_RES(resources[486], 
        12007, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/turrets/turret_003_normal_specular.aei", 0.0f));
    ADD_RES(resources[487], 
        11860, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/turrets/sn_turret_004_diffuse.aei", 0.0f));
    ADD_RES(resources[488], 11861, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/supernova/3d/textures/low/etc/turrets/sn_turret_004_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[489], 
        12050, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/turrets/v_autoturret_001_diffuse.aei", 0.0f));
    ADD_RES(resources[490], 12051, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/turrets/v_autoturret_001_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[491], 
        12052, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/turrets/v_autoturret_002_diffuse.aei", 0.0f));
    ADD_RES(resources[492], 12053, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/turrets/v_autoturret_002_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[493], 
        12054, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/turrets/v_autoturret_003_diffuse.aei", 0.0f));
    ADD_RES(resources[494], 12055, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/turrets/v_autoturret_003_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[495], 
        12008, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/scanner_probe_diffuse.aei", 0.0f));
    ADD_RES(resources[496], 
        12009, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/scanner_probe_normal_specular.aei", 0.0f));
    ADD_RES(resources[497], 
        11594, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/misc/beer_and_bra_diffuse.aei",
                                                   0.0f));
    ADD_RES(resources[498], 
        11595, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/beer_and_bra_normal_specular.aei", 0.0f));
    ADD_RES(resources[499], 
        10148, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/misc/v_guided_missile_diffuse.aei", 0.0f));
    ADD_RES(resources[500], 10149, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/misc/v_guided_missile_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[501], 
        10126, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/fx/v_scattergun_000_explosion.aei", 0.0f));
    ADD_RES(resources[502], 
        10127, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/fx/v_scattergun_001_explosion.aei", 0.0f));
    ADD_RES(resources[503], 
        10128, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/fx/v_scattergun_002_explosion.aei", 0.0f));
    ADD_RES(resources[504], 
        24210, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/misc/v_mine_001_diffuse.aei", 0.0f));
    ADD_RES(resources[505], 
        24211, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/misc/v_mine_001_normal_specular.aei", 0.0f));
    ADD_RES(resources[506], 
        24212, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/misc/v_mine_002_diffuse.aei", 0.0f));
    ADD_RES(resources[507], 
        24213, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/misc/v_mine_002_normal_specular.aei", 0.0f));
    ADD_RES(resources[508], 
        24214, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/misc/v_mine_003_diffuse.aei", 0.0f));
    ADD_RES(resources[509], 
        24215, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/misc/v_mine_003_normal_specular.aei", 0.0f));
    ADD_RES(resources[510], 11617, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/stations/v_station_battlestation_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[511], 11618, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/stations/v_station_battlestation_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[512], 11615, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/stations/v_station_deep_science_diffuse.aei",
                                 0.0f));
    ADD_RES(resources[513], 11616, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/stations/v_station_deep_science_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[514], 11620, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/stations/v_station_deep_science_glow.aei",
                                 0.0f));
    ADD_RES(resources[515], 34816, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/valkyrie/3d/textures/low/etc/stations/v_station_deep_science_emitters.aei",
                                 0.0f));
    ADD_RES(resources[516], 
        11628, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/tex_player_station_fx.aei", 0.0f));
    ADD_RES(resources[517], 
        11633, 2, new AbyssEngine::ResourceTexture("data/assets/valkyrie/3d/textures/low/etc/fx/v_shield_normal.aei",
                                                   0.0f));
    ADD_RES(resources[518], 
        11634, 2, new AbyssEngine::ResourceTexture("data/assets/valkyrie/3d/textures/low/etc/fx/v_shield_noise.aei",
                                                   0.0f));
    ADD_RES(resources[519], 
        33300, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/khador_jump.aei", 0.0f));
    ADD_RES(resources[520], 
        24203, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/hyper_drive.aei", 0.0f));
    ADD_RES(resources[521], 11647, 2, new AbyssEngine::ResourceTexture("data/textures/gas_cloud.aei", 0.0f));
    ADD_RES(resources[523], 33411, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/skyboxes/skybox_asteroid_belt_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[524], 27160, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[526], 33241, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/misc/container_003_terran_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[527], 
        33242, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/container_004_vossk_diffuse.aei", 0.0f));
    ADD_RES(resources[528], 33243, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/misc/container_004_vossk_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[529], 
        33244, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/container_002_nivelian_diffuse.aei", 0.0f));
    ADD_RES(resources[530], 33245, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/misc/container_002_nivelian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[531], 
        33246, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/container_001_midorian_diffuse.aei", 0.0f));
    ADD_RES(resources[532], 33247, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/misc/container_001_midorian_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[533], 
        24202, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/particles.aei", 0.0f));
    ADD_RES(resources[534], 
        24208, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/space_particle_diffuse.aei", 0.0f));
    ADD_RES(resources[535], 
        24209, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/space_particle_normal_specular.aei", 0.0f));
    ADD_RES(resources[536], 20092, 6, new AbyssEngine::ResourceMaterial(24208, 24209, static_cast<BlendMode>(36)));
    ADD_RES(resources[537], 20090, 6, new AbyssEngine::ResourceMaterial(24202, static_cast<BlendMode>(3)));
    ADD_RES(resources[539], 27300, 6, new AbyssEngine::ResourceMaterial(33301, BlendMode_2));
    ADD_RES(resources[541], 33341, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/stations/stations_pirates_dmg_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[542], 33357, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[543], 33352, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[544], 33353, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[545], 33354, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[546], 33355, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[547], 33356, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[548], 
        12020, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_hangar_terran.aei", 0.0f));
    ADD_RES(resources[549], 
        12021, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_hangar_vossk.aei", 0.0f));
    ADD_RES(resources[550], 
        12022, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_hangar_nivelian.aei", 0.0f));
    ADD_RES(resources[551], 
        12023, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_hangar_midorian.aei", 0.0f));
    ADD_RES(resources[552], 
        12030, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_skybox_000.aei", 0.0f));
    ADD_RES(resources[553], 
        12031, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_skybox_001.aei", 0.0f));
    ADD_RES(resources[554], 
        12032, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_skybox_002.aei", 0.0f));
    ADD_RES(resources[555], 
        12033, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_skybox_003.aei", 0.0f));
    ADD_RES(resources[556], 
        12034, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_skybox_004.aei", 0.0f));
    ADD_RES(resources[557], 
        12035, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_skybox_005.aei", 0.0f));
    ADD_RES(resources[558], 
        12036, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_skybox_006.aei", 0.0f));
    ADD_RES(resources[559], 
        12037, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_skybox_007.aei", 0.0f));
    ADD_RES(resources[560], 
        12038, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_skybox_008.aei", 0.0f));
    ADD_RES(resources[561], 
        12039, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_skybox_009.aei", 0.0f));
    ADD_RES(resources[562], 
        12040, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/cubemaps/cubemap_skybox_010.aei", 0.0f));
    ADD_RES(resources[563], 
        12041, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/cubemaps/v_cubemap_skybox_011.aei", 0.0f));
    ADD_RES(resources[564], 
        12042, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/cubemaps/v_cubemap_skybox_012.aei", 0.0f));
    ADD_RES(resources[565], 
        12043, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/cubemaps/v_cubemap_skybox_013.aei", 0.0f));
    ADD_RES(resources[566], 
        12044, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/cubemaps/v_cubemap_skybox_014.aei", 0.0f));
    ADD_RES(resources[567], 
        12045, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/cubemaps/sn_cubemap_skybox_015.aei", 0.0f));
    ADD_RES(resources[568], 
        12046, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/cubemaps/sn_cubemap_skybox_016.aei", 0.0f));
    ADD_RES(resources[569], 
        12047, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/cubemaps/sn_cubemap_skybox_017.aei", 0.0f));
    ADD_RES(resources[570], 
        12048, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/cubemaps/sn_cubemap_skybox_018.aei", 0.0f));
    ADD_RES(resources[572], 
        33304, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/bomb_explosive_a_diffuse.aei", 0.0f));
    ADD_RES(resources[573], 33305, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/misc/bomb_explosive_a_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[574], 
        33307, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/bomb_explosive_b_diffuse.aei", 0.0f));
    ADD_RES(resources[575], 33308, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/misc/bomb_explosive_b_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[576], 
        33310, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/misc/bomb_emp_a_diffuse.aei",
                                                   0.0f));
    ADD_RES(resources[578], 
        24204, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/rocket_explosive_diffuse.aei", 0.0f));
    ADD_RES(resources[579], 24205, 2, new AbyssEngine::ResourceTexture(
                                 "data/assets/main/3d/textures/low/etc/misc/rocket_explosive_normal_specular.aei",
                                 0.0f));
    ADD_RES(resources[580], 
        24206, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/misc/rocket_emp_diffuse.aei",
                                                   0.0f));
    ADD_RES(resources[581], 
        24207, 2, new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/rocket_emp_normal_specular.aei", 0.0f));
    ADD_RES(resources[583], 
        34812, 2,
        new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/ship_engine_glow.aei", 0.0f));
    ADD_RES(resources[584], 34813, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[585], 
        34814, 2, new AbyssEngine::ResourceTexture("data/assets/valkyrie/3d/textures/low/etc/fx/v_ship_engine_glow.aei",
                                                   0.0f));
    ADD_RES(resources[586], 34815, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[587], 27301, 6, new AbyssEngine::ResourceMaterial(0, static_cast<BlendMode>(3)));
    ADD_RES(resources[588], 27302, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_1));
    ADD_RES(resources[590], 33306, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[591], 33309, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[593], 20004, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[594], 20003, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_dummy));
    ADD_RES(resources[595], 20005, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[596], 20006, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_dummy));
    ADD_RES(resources[597], 20104, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[598], 20105, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_dummy));
    ADD_RES(resources[599], 20007, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[600], 20008, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_dummy));
    ADD_RES(resources[601], 20046, 6, new AbyssEngine::ResourceMaterial(10094, BlendMode_dummy));
    ADD_RES(resources[602], 20045, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[603], 20047, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_2));
    ADD_RES(resources[604], 20082, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_1));
    ADD_RES(resources[605], 20032, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[606], 20031, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_dummy));
    ADD_RES(resources[607], 20033, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_2));
    ADD_RES(resources[610], 34702, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_8));
    ADD_RES(resources[612], 0, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[613], 34501, 6, new AbyssEngine::ResourceMaterial(34501, BlendMode_2));
    ADD_RES(resources[614], 34706, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_8));
    ADD_RES(resources[615], 34707, 6, new AbyssEngine::ResourceMaterial(34501, BlendMode_dummy));
    ADD_RES(resources[616], 34708, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[617], 34709, 6, new AbyssEngine::ResourceMaterial(34700, BlendMode_2));
    ADD_RES(resources[618], 34710, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_8));
    ADD_RES(resources[619], 34711, 6, new AbyssEngine::ResourceMaterial(34501, BlendMode_dummy));
    ADD_RES(resources[620], 34712, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[621], 34713, 6, new AbyssEngine::ResourceMaterial(34703, BlendMode_2));
    ADD_RES(resources[622], 34714, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_8));
    ADD_RES(resources[623], 34715, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_dummy));
    ADD_RES(resources[624], 34716, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[625], 34717, 6, new AbyssEngine::ResourceMaterial(34700, BlendMode_2));
    ADD_RES(resources[626], 34718, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_8));
    ADD_RES(resources[627], 34719, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_dummy));
    ADD_RES(resources[628], 34600, 6, new AbyssEngine::ResourceMaterial(33301, BlendMode_2));
    ADD_RES(resources[630], 65535, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[631], 34603, 6, new AbyssEngine::ResourceMaterial(34700, BlendMode_2));
    ADD_RES(resources[633], 34605, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[635], 34201, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[636], 34202, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[641], 34207, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[642], 34208, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[644], 34210, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[649], 34218, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[654], 34223, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[659], 34228, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[664], 34233, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[666], 34235, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[668], 34237, 6, new AbyssEngine::ResourceMaterial(34234, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[670], 34239, 6, new AbyssEngine::ResourceMaterial(34237, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[671], 34240, 6, new AbyssEngine::ResourceMaterial(34237, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[672], 34241, 6, new AbyssEngine::ResourceMaterial(34232, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[674], 34243, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[675], 34236, 6, new AbyssEngine::ResourceMaterial(34236, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[676], 34234, 6, new AbyssEngine::ResourceMaterial(34234, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[677], 65535, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[678], 65535, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[679], 34048, 6, new AbyssEngine::ResourceMaterial(34048, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[680], 65535, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[682], 65535, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[683], 65535, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[684], 34253, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[685], 65535, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[686], 65535, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[687], 65535, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[688], 65535, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[689], 65535, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[690], 65535, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[691], 65535, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[692], 65535, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[693], 65535, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[694], 0, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[695], 34400, 6, new AbyssEngine::ResourceMaterial(34042, BlendMode_8));
    ADD_RES(resources[696], 34401, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[697], 34402, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[698], 34403, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[699], 34404, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[700], 34405, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[701], 34406, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[702], 34407, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[703], 34408, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[704], 34409, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[705], 34410, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[706], 34411, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[707], 34412, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[708], 34416, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[709], 34417, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[710], 34418, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[711], 34419, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[712], 34420, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[713], 34421, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[714], 34422, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[715], 34423, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[716], 34424, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[717], 34425, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[718], 34426, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[719], 34427, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[720], 34428, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[721], 34429, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[722], 34430, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[723], 34431, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[724], 34432, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[725], 34433, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[726], 34434, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[727], 34435, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[728], 34436, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[729], 34442, 6, new AbyssEngine::ResourceMaterial(34042, BlendMode_8));
    ADD_RES(resources[730], 34443, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[731], 34444, 6, new AbyssEngine::ResourceMaterial(34236, BlendMode_8));
    ADD_RES(resources[732], 34445, 6, new AbyssEngine::ResourceMaterial(34234, BlendMode_8));
    ADD_RES(resources[733], 34446, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[734], 34447, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[735], 34448, 6, new AbyssEngine::ResourceMaterial(34048, BlendMode_8));
    ADD_RES(resources[736], 34449, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[737], 34450, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[738], 34451, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[739], 34452, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[740], 34453, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[741], 34454, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[742], 34455, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[743], 34456, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[744], 34457, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[745], 34458, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[746], 34459, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[747], 34460, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[748], 34461, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[749], 34462, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_8));
    ADD_RES(resources[750], 34463, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_8));
    ADD_RES(resources[751], 34300, 6, new AbyssEngine::ResourceMaterial(34042, BlendMode_dummy));
    ADD_RES(resources[752], 34301, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[753], 34302, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[754], 34303, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[755], 34304, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[756], 34305, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[757], 34306, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[758], 34307, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[759], 34308, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[760], 34309, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[761], 34310, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[762], 34311, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[763], 34312, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[764], 34313, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[765], 34314, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[766], 34315, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[767], 34316, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[768], 34317, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[769], 34318, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[770], 34319, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[771], 34320, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[772], 34321, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[773], 34322, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[774], 34323, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[775], 34324, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[776], 34325, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[777], 34326, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[778], 34327, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[779], 34328, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[780], 34329, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[781], 34330, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[782], 34331, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[783], 34332, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[784], 34333, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[785], 34334, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[786], 34335, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[787], 34336, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[788], 32537, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[789], 32538, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[790], 32539, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[791], 32540, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[792], 32541, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[793], 34342, 6, new AbyssEngine::ResourceMaterial(34042, BlendMode_dummy));
    ADD_RES(resources[794], 32542, 6, new AbyssEngine::ResourceMaterial(34042, BlendMode_2));
    ADD_RES(resources[795], 34343, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[796], 32543, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[797], 34344, 6, new AbyssEngine::ResourceMaterial(34236, BlendMode_dummy));
    ADD_RES(resources[798], 32544, 6, new AbyssEngine::ResourceMaterial(34236, BlendMode_2));
    ADD_RES(resources[799], 34345, 6, new AbyssEngine::ResourceMaterial(34234, BlendMode_dummy));
    ADD_RES(resources[800], 32545, 6, new AbyssEngine::ResourceMaterial(34234, BlendMode_2));
    ADD_RES(resources[801], 34346, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[802], 32546, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[803], 34347, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[804], 32547, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[805], 34348, 6, new AbyssEngine::ResourceMaterial(34048, BlendMode_dummy));
    ADD_RES(resources[806], 32548, 6, new AbyssEngine::ResourceMaterial(34048, BlendMode_2));
    ADD_RES(resources[807], 34349, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[808], 32549, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[809], 34350, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[810], 32550, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[811], 34351, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[812], 32551, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[813], 34352, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[814], 32552, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[815], 34353, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[816], 32553, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[817], 34354, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[818], 32554, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[819], 34355, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[820], 32555, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[821], 34356, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[822], 32556, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[823], 34357, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[824], 32557, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[825], 34358, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[826], 32558, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[827], 34359, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[828], 32559, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[829], 34360, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[830], 32560, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[831], 34361, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[832], 32561, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[833], 34362, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_dummy));
    ADD_RES(resources[834], 32562, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[835], 0, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_dummy));
    ADD_RES(resources[836], 32563, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_2));
    ADD_RES(resources[837], 20128, 6, new AbyssEngine::ResourceMaterial(10129, static_cast<BlendMode>(6)));
    ADD_RES(resources[838], 20129, 6, new AbyssEngine::ResourceMaterial(10129, BlendMode_2));
    ADD_RES(resources[839], 20001, 6, new AbyssEngine::ResourceMaterial(10031, BlendMode_2));
    ADD_RES(resources[840], 27280, 6, new AbyssEngine::ResourceMaterial(10031, BlendMode_2));
    ADD_RES(resources[841], 27281, 6, new AbyssEngine::ResourceMaterial(10032, BlendMode_2));
    ADD_RES(resources[842], 27282, 6, new AbyssEngine::ResourceMaterial(10033, BlendMode_2));
    ADD_RES(resources[843], 27283, 6, new AbyssEngine::ResourceMaterial(10034, BlendMode_2));
    ADD_RES(resources[844], 27284, 6, new AbyssEngine::ResourceMaterial(10035, BlendMode_2));
    ADD_RES(resources[845], 27285, 6, new AbyssEngine::ResourceMaterial(10036, BlendMode_2));
    ADD_RES(resources[846], 27286, 6, new AbyssEngine::ResourceMaterial(10037, BlendMode_2));
    ADD_RES(resources[847], 27287, 6, new AbyssEngine::ResourceMaterial(10038, BlendMode_2));
    ADD_RES(resources[848], 27288, 6, new AbyssEngine::ResourceMaterial(10039, BlendMode_2));
    ADD_RES(resources[849], 27289, 6, new AbyssEngine::ResourceMaterial(10040, BlendMode_2));
    ADD_RES(resources[850], 27290, 6, new AbyssEngine::ResourceMaterial(10041, BlendMode_2));
    ADD_RES(resources[851], 20093, 6, new AbyssEngine::ResourceMaterial(10104, BlendMode_2));
    ADD_RES(resources[852], 20094, 6, new AbyssEngine::ResourceMaterial(12000, 12001, static_cast<BlendMode>(28)));
    ADD_RES(resources[853], 20095, 6, new AbyssEngine::ResourceMaterial(10121, BlendMode_2));
    ADD_RES(resources[854], 20137, 6, new AbyssEngine::ResourceMaterial(10132, BlendMode_2));
    ADD_RES(resources[855], 20009, 6, new AbyssEngine::ResourceMaterial(10065, BlendMode_dummy));
    ADD_RES(resources[856], 20010, 6, new AbyssEngine::ResourceMaterial(10066, BlendMode_dummy));
    ADD_RES(resources[857], 20011, 6, new AbyssEngine::ResourceMaterial(10067, BlendMode_dummy));
    ADD_RES(resources[858], 20012, 6, new AbyssEngine::ResourceMaterial(10068, BlendMode_dummy));
    ADD_RES(resources[859], 20013, 6, new AbyssEngine::ResourceMaterial(10069, BlendMode_dummy));
    ADD_RES(resources[860], 20014, 6, new AbyssEngine::ResourceMaterial(10070, BlendMode_dummy));
    ADD_RES(resources[861], 20015, 6, new AbyssEngine::ResourceMaterial(10071, BlendMode_dummy));
    ADD_RES(resources[862], 20016, 6, new AbyssEngine::ResourceMaterial(10072, BlendMode_dummy));
    ADD_RES(resources[863], 20017, 6, new AbyssEngine::ResourceMaterial(10073, BlendMode_dummy));
    ADD_RES(resources[864], 20018, 6, new AbyssEngine::ResourceMaterial(10074, BlendMode_dummy));
    ADD_RES(resources[865], 20019, 6, new AbyssEngine::ResourceMaterial(10075, BlendMode_dummy));
    ADD_RES(resources[866], 20130, 6, new AbyssEngine::ResourceMaterial(10076, BlendMode_dummy));
    ADD_RES(resources[867], 20131, 6, new AbyssEngine::ResourceMaterial(10077, BlendMode_dummy));
    ADD_RES(resources[868], 20132, 6, new AbyssEngine::ResourceMaterial(10078, BlendMode_dummy));
    ADD_RES(resources[869], 27149, 6, new AbyssEngine::ResourceMaterial(10079, BlendMode_dummy));
    ADD_RES(resources[870], 27200, 6, new AbyssEngine::ResourceMaterial(10086, BlendMode_dummy));
    ADD_RES(resources[871], 27201, 6, new AbyssEngine::ResourceMaterial(10087, BlendMode_dummy));
    ADD_RES(resources[872], 27202, 6, new AbyssEngine::ResourceMaterial(10088, BlendMode_dummy));
    ADD_RES(resources[873], 20021, 6, new AbyssEngine::ResourceMaterial(10001, BlendMode_1));
    ADD_RES(resources[874], 20022, 6, new AbyssEngine::ResourceMaterial(10001, BlendMode_2));
    ADD_RES(resources[875], 20052, 6, new AbyssEngine::ResourceMaterial(10001, static_cast<BlendMode>(3)));
    ADD_RES(resources[876], 20100, 6, new AbyssEngine::ResourceMaterial(10001, static_cast<BlendMode>(6)));
    ADD_RES(resources[877], 20108, 6, new AbyssEngine::ResourceMaterial(10001, BlendMode_dummy));
    ADD_RES(resources[878], 20124, 6, new AbyssEngine::ResourceMaterial(10001, static_cast<BlendMode>(4)));
    ADD_RES(resources[879], 20023, 6, new AbyssEngine::ResourceMaterial(33000, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[880], 20049, 6, new AbyssEngine::ResourceMaterial(33000, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[881], 20050, 6, new AbyssEngine::ResourceMaterial(10107, BlendMode_2));
    ADD_RES(resources[882], 20051, 6, new AbyssEngine::ResourceMaterial(10107, BlendMode_1));
    ADD_RES(resources[883], 20134, 6, new AbyssEngine::ResourceMaterial(10155, 10156, static_cast<BlendMode>(28)));
    ADD_RES(resources[884], 20133, 6, new AbyssEngine::ResourceMaterial(10155, BlendMode_1));
    ADD_RES(resources[885], 27312, 6, new AbyssEngine::ResourceMaterial(28999, 29000, static_cast<BlendMode>(28)));
    ADD_RES(resources[886], 27313, 6, new AbyssEngine::ResourceMaterial(27314, BlendMode_1));
    ADD_RES(resources[887], 20024, 6, new AbyssEngine::ResourceMaterial(10100, BlendMode_dummy));
    ADD_RES(resources[888], 20025, 6, new AbyssEngine::ResourceMaterial(10101, BlendMode_2));
    ADD_RES(resources[889], 20026, 6, new AbyssEngine::ResourceMaterial(10102, BlendMode_2));
    ADD_RES(resources[890], 20027, 6, new AbyssEngine::ResourceMaterial(10145, static_cast<BlendMode>(6)));
    ADD_RES(resources[891], 27305, 6, new AbyssEngine::ResourceMaterial(10159, static_cast<BlendMode>(6)));
    ADD_RES(resources[892], 20029, 6, new AbyssEngine::ResourceMaterial(10144, BlendMode_2));
    ADD_RES(resources[893], 20030, 6, new AbyssEngine::ResourceMaterial(10145, BlendMode_1));
    ADD_RES(resources[894], 27306, 6, new AbyssEngine::ResourceMaterial(10161, static_cast<BlendMode>(6)));
    ADD_RES(resources[895], 27307, 6, new AbyssEngine::ResourceMaterial(10161, BlendMode_2));
    ADD_RES(resources[898], 34802, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[901], 20114, 6, new AbyssEngine::ResourceMaterial(11606, static_cast<BlendMode>(10)));
    ADD_RES(resources[902], 20115, 6, new AbyssEngine::ResourceMaterial(11607, static_cast<BlendMode>(10)));
    ADD_RES(resources[903], 20116, 6, new AbyssEngine::ResourceMaterial(11608, static_cast<BlendMode>(10)));
    ADD_RES(resources[904], 20117, 6, new AbyssEngine::ResourceMaterial(11609, static_cast<BlendMode>(10)));
    ADD_RES(resources[905], 20118, 6, new AbyssEngine::ResourceMaterial(11610, static_cast<BlendMode>(10)));
    ADD_RES(resources[906], 20119, 6, new AbyssEngine::ResourceMaterial(11611, static_cast<BlendMode>(10)));
    ADD_RES(resources[907], 20120, 6, new AbyssEngine::ResourceMaterial(11612, static_cast<BlendMode>(10)));
    ADD_RES(resources[908], 20056, 6, new AbyssEngine::ResourceMaterial(10136, 10140, static_cast<BlendMode>(28)));
    ADD_RES(resources[909], 20057, 6, new AbyssEngine::ResourceMaterial(10136, BlendMode_2));
    ADD_RES(resources[910], 20058, 6, new AbyssEngine::ResourceMaterial(10136, BlendMode_1));
    ADD_RES(resources[911], 20059, 6, new AbyssEngine::ResourceMaterial(10137, 10141, static_cast<BlendMode>(28)));
    ADD_RES(resources[912], 20060, 6, new AbyssEngine::ResourceMaterial(10137, BlendMode_2));
    ADD_RES(resources[913], 20061, 6, new AbyssEngine::ResourceMaterial(10137, static_cast<BlendMode>(24)));
    ADD_RES(resources[914], 20062, 6, new AbyssEngine::ResourceMaterial(10138, 10142, static_cast<BlendMode>(28)));
    ADD_RES(resources[915], 20063, 6, new AbyssEngine::ResourceMaterial(10138, BlendMode_2));
    ADD_RES(resources[916], 20064, 6, new AbyssEngine::ResourceMaterial(10138, BlendMode_1));
    ADD_RES(resources[917], 20065, 6, new AbyssEngine::ResourceMaterial(10139, 10143, static_cast<BlendMode>(28)));
    ADD_RES(resources[918], 20066, 6, new AbyssEngine::ResourceMaterial(10139, BlendMode_2));
    ADD_RES(resources[919], 20067, 6, new AbyssEngine::ResourceMaterial(10139, BlendMode_1));
    ADD_RES(resources[920], 24070, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[921], 20077, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_1));
    ADD_RES(resources[922], 20078, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_2));
    ADD_RES(resources[923], 24074, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[924], 20071, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_2));
    ADD_RES(resources[925], 24073, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[926], 20074, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_1));
    ADD_RES(resources[927], 20075, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_2));
    ADD_RES(resources[928], 24071, 6, new AbyssEngine::ResourceMaterial(0, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[929], 20106, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_1));
    ADD_RES(resources[930], 20069, 6, new AbyssEngine::ResourceMaterial(0, BlendMode_2));
    ADD_RES(resources[931], 20157, 6, new AbyssEngine::ResourceMaterial(10157, 10158, static_cast<BlendMode>(28)));
    ADD_RES(resources[932], 27138, 6, new AbyssEngine::ResourceMaterial(10157, BlendMode_dummy));
    ADD_RES(resources[933], 27139, 6, new AbyssEngine::ResourceMaterial(10157, BlendMode_1));
    ADD_RES(resources[934], 27140, 6, new AbyssEngine::ResourceMaterial(10157, BlendMode_2));
    ADD_RES(resources[935], 27141, 6, new AbyssEngine::ResourceMaterial(10134, BlendMode_dummy));
    ADD_RES(resources[936], 27142, 6, new AbyssEngine::ResourceMaterial(10134, BlendMode_1));
    ADD_RES(resources[937], 27143, 6, new AbyssEngine::ResourceMaterial(10134, BlendMode_2));
    ADD_RES(resources[938], 27144, 6, new AbyssEngine::ResourceMaterial(10134, 10135, static_cast<BlendMode>(28)));
    ADD_RES(resources[939], 20087, 6, new AbyssEngine::ResourceMaterial(11600, BlendMode_2));
    ADD_RES(resources[940], 27252, 6, new AbyssEngine::ResourceMaterial(11598, BlendMode_2));
    ADD_RES(resources[941], 27253, 6, new AbyssEngine::ResourceMaterial(11598, static_cast<BlendMode>(18)));
    ADD_RES(resources[942], 27250, 6, new AbyssEngine::ResourceMaterial(11599, BlendMode_2));
    ADD_RES(resources[943], 20099, 6, new AbyssEngine::ResourceMaterial(11601, BlendMode_2));
    ADD_RES(resources[944], 20101, 6, new AbyssEngine::ResourceMaterial(11602, BlendMode_1));
    ADD_RES(resources[945], 24075, 6, new AbyssEngine::ResourceMaterial(12002, 12003, static_cast<BlendMode>(28)));
    ADD_RES(resources[946], 24076, 6, new AbyssEngine::ResourceMaterial(12004, 12005, static_cast<BlendMode>(28)));
    ADD_RES(resources[947], 24077, 6, new AbyssEngine::ResourceMaterial(12006, 12007, static_cast<BlendMode>(28)));
    ADD_RES(resources[948], 24078, 6, new AbyssEngine::ResourceMaterial(12002, BlendMode_8));
    ADD_RES(resources[949], 24079, 6, new AbyssEngine::ResourceMaterial(12004, BlendMode_8));
    ADD_RES(resources[950], 24080, 6, new AbyssEngine::ResourceMaterial(12006, BlendMode_8));
    ADD_RES(resources[951], 24085, 6, new AbyssEngine::ResourceMaterial(11860, 11861, static_cast<BlendMode>(28)));
    ADD_RES(resources[952], 24086, 6, new AbyssEngine::ResourceMaterial(11860, BlendMode_2));
    ADD_RES(resources[953], 24087, 6, new AbyssEngine::ResourceMaterial(11860, BlendMode_8));
    ADD_RES(resources[954], 24081, 6, new AbyssEngine::ResourceMaterial(12050, 12051, static_cast<BlendMode>(28)));
    ADD_RES(resources[955], 24082, 6, new AbyssEngine::ResourceMaterial(12052, 12053, static_cast<BlendMode>(28)));
    ADD_RES(resources[956], 24083, 6, new AbyssEngine::ResourceMaterial(12054, 12055, static_cast<BlendMode>(28)));
    ADD_RES(resources[957], 20103, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[958], 20111, 6, new AbyssEngine::ResourceMaterial(11594, 11595, static_cast<BlendMode>(28)));
    ADD_RES(resources[959], 20113, 6, new AbyssEngine::ResourceMaterial(12008, 12009, static_cast<BlendMode>(28)));
    ADD_RES(resources[960], 20121, 6, new AbyssEngine::ResourceMaterial(24210, 24211, static_cast<BlendMode>(28)));
    ADD_RES(resources[961], 20122, 6, new AbyssEngine::ResourceMaterial(24212, 24213, static_cast<BlendMode>(28)));
    ADD_RES(resources[962], 20123, 6, new AbyssEngine::ResourceMaterial(24214, 24215, static_cast<BlendMode>(28)));
    ADD_RES(resources[963], 20154, 6, new AbyssEngine::ResourceMaterial(24210, BlendMode_2));
    ADD_RES(resources[964], 20155, 6, new AbyssEngine::ResourceMaterial(24212, BlendMode_2));
    ADD_RES(resources[965], 20156, 6, new AbyssEngine::ResourceMaterial(24214, BlendMode_2));
    ADD_RES(resources[966], 20125, 6, new AbyssEngine::ResourceMaterial(10148, 10149, static_cast<BlendMode>(28)));
    ADD_RES(resources[967], 20126, 6, new AbyssEngine::ResourceMaterial(10148, BlendMode_2));
    ADD_RES(resources[968], 20151, 6, new AbyssEngine::ResourceMaterial(10126, BlendMode_2));
    ADD_RES(resources[969], 20152, 6, new AbyssEngine::ResourceMaterial(10127, BlendMode_2));
    ADD_RES(resources[970], 20153, 6, new AbyssEngine::ResourceMaterial(10128, BlendMode_2));
    ADD_RES(resources[971], 20135, 6, new AbyssEngine::ResourceMaterial(11617, 11618, static_cast<BlendMode>(28)));
    ADD_RES(resources[972], 20136, 6, new AbyssEngine::ResourceMaterial(11617, BlendMode_dummy));
    ADD_RES(resources[973], 20158, 6, new AbyssEngine::ResourceMaterial(11617, BlendMode_2));
    ADD_RES(resources[974], 27145, 6, new AbyssEngine::ResourceMaterial(11615, 11616, static_cast<BlendMode>(28)));
    ADD_RES(resources[975], 27146, 6, new AbyssEngine::ResourceMaterial(11615, BlendMode_dummy));
    ADD_RES(resources[976], 27147, 6, new AbyssEngine::ResourceMaterial(11620, BlendMode_2));
    ADD_RES(resources[978], 27162, 6, new AbyssEngine::ResourceMaterial(11628, static_cast<BlendMode>(6)));
    ADD_RES(resources[979], 27163, 6, new AbyssEngine::ResourceMaterial(11628, BlendMode_dummy));
    ADD_RES(resources[980], 27164, 6, new AbyssEngine::ResourceMaterial(11628, BlendMode_2));
    ADD_RES(resources[981], 27165, 6, new AbyssEngine::ResourceMaterial(11628, BlendMode_1));
    ADD_RES(resources[982], 27148, 6, new AbyssEngine::ResourceMaterial(10129, BlendMode_8));
    ADD_RES(resources[983], 27150, 6, new AbyssEngine::ResourceMaterial(11634, 11633, static_cast<BlendMode>(34)));
    ADD_RES(resources[984], 27154, 6, new AbyssEngine::ResourceMaterial(11629, static_cast<BlendMode>(6)));
    ADD_RES(resources[985], 27155, 6, new AbyssEngine::ResourceMaterial(11629, BlendMode_dummy));
    ADD_RES(resources[986], 27156, 6, new AbyssEngine::ResourceMaterial(11629, BlendMode_8));
    ADD_RES(resources[987], 27157, 6, new AbyssEngine::ResourceMaterial(11631, static_cast<BlendMode>(6)));
    ADD_RES(resources[988], 27158, 6, new AbyssEngine::ResourceMaterial(11631, BlendMode_dummy));
    ADD_RES(resources[989], 27159, 6, new AbyssEngine::ResourceMaterial(11631, BlendMode_8));
    ADD_RES(resources[990], 27311, 6, new AbyssEngine::ResourceMaterial(11647, BlendMode_2));
    ADD_RES(resources[991], 27254, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[992], 27255, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[993], 27256, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[994], 27257, 6, new AbyssEngine::ResourceMaterial(65535, 0, static_cast<BlendMode>(28)));
    ADD_RES(resources[995], 27260, 6, new AbyssEngine::ResourceMaterial(65535, BlendMode_2));
    ADD_RES(resources[996], 27262, 6, new AbyssEngine::ResourceMaterial(24203, BlendMode_2));
    ADD_RES(resources[999], 11636, 1, newImage(0x2d73u, 0x0u));
    ADD_RES(resources[1000], 11638, 1, newImage(0x2d75u, 0x0u));
    ADD_RES(resources[1001], 11640, 1, newImage(0x2d77u, 0x0u));
    ADD_RES(resources[1002], 11642, 1, newImage(0x2d79u, 0x0u));
    ADD_RES(resources[1003], 11644, 1, newImage(0x2d7bu, 0x0u));
    ADD_RES(resources[1004], 11646, 1, newImage(0x2d7du, 0x0u));
    ADD_RES(resources[1005], 1102, 3, newImage(0x274eu, 0x2u));
    ADD_RES(resources[1006], 1106, 3, newImage(0x274eu, 0x6u));
    ADD_RES(resources[1007], 1108, 3, newImage(0x274eu, 0x8u));
    ADD_RES(resources[1008], 1109, 3, newImage(0x274eu, 0x9u));
    ADD_RES(resources[1009], 1110, 3, newImage(0x274eu, 0xau));
    ADD_RES(resources[1010], 1111, 1, newImage(0x274eu, 0x0u));
    ADD_RES(resources[1011], 1112, 3, newImage(0x274eu, 0xbu));
    ADD_RES(resources[1012], 1113, 3, newImage(0x274eu, 0xcu));
    ADD_RES(resources[1013], 1114, 3, newImage(0x274eu, 0xdu));
    ADD_RES(resources[1014], 1115, 3, newImage(0x274eu, 0xeu));
    ADD_RES(resources[1015], 1116, 3, newImage(0x274eu, 0xfu));
    ADD_RES(resources[1016], 1117, 3, newImage(0x274eu, 0x10u));
    ADD_RES(resources[1017], 1118, 3, newImage(0x274eu, 0x11u));
    ADD_RES(resources[1018], 1119, 3, newImage(0x274eu, 0x12u));
    ADD_RES(resources[1019], 1120, 3, newImage(0x274eu, 0x13u));
    ADD_RES(resources[1020], 1121, 3, newImage(0x274eu, 0x14u));
    ADD_RES(resources[1021], 1122, 3, newImage(0x274eu, 0x15u));
    ADD_RES(resources[1022], 1123, 3, newImage(0x274eu, 0x16u));
    ADD_RES(resources[1023], 1124, 3, newImage(0x274eu, 0x17u));
    ADD_RES(resources[1024], 1125, 3, newImage(0x274eu, 0x18u));
    ADD_RES(resources[1025], 1126, 3, newImage(0x274eu, 0x19u));
    ADD_RES(resources[1026], 1127, 3, newImage(0x274eu, 0x1au));
    ADD_RES(resources[1027], 1128, 3, newImage(0x274eu, 0x1bu));
    ADD_RES(resources[1028], 1129, 3, newImage(0x274eu, 0x1cu));
    ADD_RES(resources[1029], 1130, 3, newImage(0x274eu, 0x1du));
    ADD_RES(resources[1030], 1131, 3, newImage(0x274eu, 0x1eu));
    ADD_RES(resources[1031], 1132, 3, newImage(0x274eu, 0x1fu));
    ADD_RES(resources[1032], 1133, 3, newImage(0x274eu, 0x20u));
    ADD_RES(resources[1033], 1134, 3, newImage(0x274eu, 0x21u));
    ADD_RES(resources[1034], 1135, 3, newImage(0x274eu, 0x22u));
    ADD_RES(resources[1035], 1136, 3, newImage(0x274eu, 0x23u));
    ADD_RES(resources[1036], 1137, 3, newImage(0x274eu, 0x24u));
    ADD_RES(resources[1037], 1138, 3, newImage(0x274eu, 0x25u));
    ADD_RES(resources[1038], 1139, 3, newImage(0x274eu, 0x26u));
    ADD_RES(resources[1039], 1140, 3, newImage(0x274eu, 0x27u));
    ADD_RES(resources[1040], 1141, 3, newImage(0x274eu, 0x28u));
    ADD_RES(resources[1041], 1142, 3, newImage(0x274eu, 0x29u));
    ADD_RES(resources[1042], 1143, 3, newImage(0x274eu, 0x2au));
    ADD_RES(resources[1043], 1144, 3, newImage(0x274eu, 0x2bu));
    ADD_RES(resources[1044], 1145, 3, newImage(0x274eu, 0x2cu));
    ADD_RES(resources[1045], 1146, 3, newImage(0x274eu, 0x2du));
    ADD_RES(resources[1046], 1147, 3, newImage(0x274eu, 0x2eu));
    ADD_RES(resources[1047], 1148, 3, newImage(0x274eu, 0x2fu));
    ADD_RES(resources[1048], 1149, 3, newImage(0x274eu, 0x30u));
    ADD_RES(resources[1049], 1150, 3, newImage(0x274eu, 0x31u));
    ADD_RES(resources[1050], 1151, 3, newImage(0x274eu, 0x32u));
    ADD_RES(resources[1051], 1152, 3, newImage(0x274eu, 0x33u));
    ADD_RES(resources[1052], 1153, 3, newImage(0x274eu, 0x34u));
    ADD_RES(resources[1053], 1154, 3, newImage(0x274eu, 0x35u));
    ADD_RES(resources[1054], 1155, 3, newImage(0x274eu, 0x36u));
    ADD_RES(resources[1055], 1156, 3, newImage(0x274eu, 0x37u));
    ADD_RES(resources[1056], 1158, 3, newImage(0x274eu, 0x39u));
    ADD_RES(resources[1057], 1159, 3, newImage(0x274eu, 0x3au));
    ADD_RES(resources[1058], 1160, 3, newImage(0x274eu, 0x3bu));
    ADD_RES(resources[1059], 1161, 3, newImage(0x274eu, 0x3cu));
    ADD_RES(resources[1060], 1162, 3, newImage(0x274eu, 0x3du));
    ADD_RES(resources[1061], 1163, 3, newImage(0x274eu, 0x3eu));
    ADD_RES(resources[1062], 1164, 3, newImage(0x274eu, 0x3fu));
    ADD_RES(resources[1063], 1165, 3, newImage(0x274eu, 0x40u));
    ADD_RES(resources[1064], 1166, 3, newImage(0x274eu, 0x41u));
    ADD_RES(resources[1065], 1167, 3, newImage(0x274eu, 0x42u));
    ADD_RES(resources[1066], 1168, 3, newImage(0x274eu, 0x43u));
    ADD_RES(resources[1067], 1169, 3, newImage(0x274eu, 0x44u));
    ADD_RES(resources[1068], 1170, 3, newImage(0x274eu, 0x45u));
    ADD_RES(resources[1069], 1171, 3, newImage(0x274eu, 0x46u));
    ADD_RES(resources[1070], 1172, 3, newImage(0x274eu, 0x47u));
    ADD_RES(resources[1071], 1173, 3, newImage(0x274eu, 0x48u));
    ADD_RES(resources[1072], 1174, 3, newImage(0x274eu, 0x49u));
    ADD_RES(resources[1073], 1175, 3, newImage(0x274eu, 0x4au));
    ADD_RES(resources[1074], 1176, 3, newImage(0x274eu, 0x4bu));
    ADD_RES(resources[1075], 1177, 3, newImage(0x274eu, 0x4cu));
    ADD_RES(resources[1076], 1178, 3, newImage(0x274eu, 0x4du));
    ADD_RES(resources[1077], 1179, 3, newImage(0x274eu, 0x4eu));
    ADD_RES(resources[1078], 1180, 3, newImage(0x274eu, 0x4fu));
    ADD_RES(resources[1079], 1181, 3, newImage(0x274eu, 0x50u));
    ADD_RES(resources[1080], 1182, 3, newImage(0x274eu, 0x51u));
    ADD_RES(resources[1081], 1183, 3, newImage(0x274eu, 0x52u));
    ADD_RES(resources[1082], 1184, 3, newImage(0x274eu, 0x53u));
    ADD_RES(resources[1083], 1185, 3, newImage(0x274eu, 0x54u));
    ADD_RES(resources[1084], 1186, 3, newImage(0x274eu, 0x55u));
    ADD_RES(resources[1085], 1187, 3, newImage(0x274eu, 0x56u));
    ADD_RES(resources[1086], 1188, 3, newImage(0x274eu, 0x57u));
    ADD_RES(resources[1087], 1189, 3, newImage(0x274eu, 0x58u));
    ADD_RES(resources[1088], 1190, 3, newImage(0x274eu, 0x59u));
    ADD_RES(resources[1089], 1194, 3, newImage(0x274eu, 0x5du));
    ADD_RES(resources[1090], 1195, 3, newImage(0x274eu, 0x5eu));
    ADD_RES(resources[1091], 1196, 3, newImage(0x274eu, 0x5fu));
    ADD_RES(resources[1092], 1197, 3, newImage(0x274eu, 0x60u));
    ADD_RES(resources[1093], 1216, 3, newImage(0x274eu, 0x73u));
    ADD_RES(resources[1094], 1218, 3, newImage(0x274eu, 0x75u));
    ADD_RES(resources[1095], 1219, 3, newImage(0x274eu, 0x76u));
    ADD_RES(resources[1096], 1220, 3, newImage(0x274eu, 0x77u));
    ADD_RES(resources[1097], 1221, 3, newImage(0x274eu, 0x78u));
    ADD_RES(resources[1098], 1230, 3, newImage(0x274eu, 0x81u));
    ADD_RES(resources[1099], 1231, 3, newImage(0x274eu, 0x82u));
    ADD_RES(resources[1100], 1232, 3, newImage(0x274eu, 0x83u));
    ADD_RES(resources[1101], 1233, 3, newImage(0x274eu, 0x84u));
    ADD_RES(resources[1102], 1245, 3, newImage(0x274eu, 0x90u));
    ADD_RES(resources[1103], 1246, 3, newImage(0x274eu, 0x91u));
    ADD_RES(resources[1104], 1247, 3, newImage(0x274eu, 0x92u));
    ADD_RES(resources[1105], 1248, 3, newImage(0x274eu, 0x93u));
    ADD_RES(resources[1106], 1249, 3, newImage(0x274eu, 0x94u));
    ADD_RES(resources[1107], 1250, 3, newImage(0x274eu, 0x95u));
    ADD_RES(resources[1108], 1251, 3, newImage(0x274eu, 0x96u));
    ADD_RES(resources[1109], 1252, 3, newImage(0x274eu, 0x97u));
    ADD_RES(resources[1110], 1253, 3, newImage(0x274eu, 0x98u));
    ADD_RES(resources[1111], 1254, 3, newImage(0x274eu, 0x99u));
    ADD_RES(resources[1112], 1255, 3, newImage(0x274eu, 0x9au));
    ADD_RES(resources[1113], 1256, 3, newImage(0x274eu, 0x9bu));
    ADD_RES(resources[1114], 1259, 3, newImage(0x274eu, 0x9eu));
    ADD_RES(resources[1115], 1260, 3, newImage(0x274eu, 0x9fu));
    ADD_RES(resources[1116], 1261, 3, newImage(0x274eu, 0xa0u));
    ADD_RES(resources[1117], 1267, 3, newImage(0x274eu, 0xa6u));
    ADD_RES(resources[1118], 1268, 3, newImage(0x274eu, 0xa7u));
    ADD_RES(resources[1119], 1269, 3, newImage(0x274eu, 0xa8u));
    ADD_RES(resources[1120], 1270, 3, newImage(0x274eu, 0xa9u));
    ADD_RES(resources[1121], 1277, 3, newImage(0x274eu, 0xb0u));
    ADD_RES(resources[1122], 1278, 3, newImage(0x274eu, 0xb1u));
    ADD_RES(resources[1123], 1279, 3, newImage(0x274eu, 0xb2u));
    ADD_RES(resources[1124], 1280, 3, newImage(0x274eu, 0xb3u));
    ADD_RES(resources[1125], 1281, 3, newImage(0x274eu, 0xb4u));
    ADD_RES(resources[1126], 1282, 3, newImage(0x274eu, 0xb5u));
    ADD_RES(resources[1127], 1283, 3, newImage(0x274eu, 0xb6u));
    ADD_RES(resources[1128], 1284, 3, newImage(0x274eu, 0xb7u));
    ADD_RES(resources[1129], 1285, 3, newImage(0x274eu, 0xb8u));
    ADD_RES(resources[1130], 1286, 3, newImage(0x274eu, 0xb9u));
    ADD_RES(resources[1131], 1287, 3, newImage(0x274eu, 0xbau));
    ADD_RES(resources[1132], 1288, 3, newImage(0x274eu, 0xbbu));
    ADD_RES(resources[1133], 1289, 3, newImage(0x274eu, 0xbcu));
    ADD_RES(resources[1134], 1290, 3, newImage(0x274eu, 0xbdu));
    ADD_RES(resources[1135], 1291, 3, newImage(0x274eu, 0xbeu));
    ADD_RES(resources[1136], 1292, 3, newImage(0x274eu, 0xbfu));
    ADD_RES(resources[1137], 1293, 3, newImage(0x274eu, 0xc0u));
    ADD_RES(resources[1138], 1294, 3, newImage(0x274eu, 0xc1u));
    ADD_RES(resources[1139], 1295, 3, newImage(0x274eu, 0xc2u));
    ADD_RES(resources[1140], 1296, 3, newImage(0x274eu, 0xc3u));
    ADD_RES(resources[1141], 1298, 3, newImage(0x274eu, 0xc5u));
    ADD_RES(resources[1142], 1299, 3, newImage(0x274eu, 0xc6u));
    ADD_RES(resources[1143], 1300, 3, newImage(0x274eu, 0xc7u));
    ADD_RES(resources[1144], 1301, 3, newImage(0x274eu, 0xc8u));
    ADD_RES(resources[1145], 1302, 3, newImage(0x274eu, 0xc9u));
    ADD_RES(resources[1146], 1303, 3, newImage(0x274eu, 0xcau));
    ADD_RES(resources[1147], 1304, 3, newImage(0x274eu, 0xcbu));
    ADD_RES(resources[1148], 1305, 3, newImage(0x274eu, 0xccu));
    ADD_RES(resources[1149], 1306, 3, newImage(0x274eu, 0xcdu));
    ADD_RES(resources[1150], 1307, 3, newImage(0x274eu, 0xceu));
    ADD_RES(resources[1151], 1308, 3, newImage(0x274eu, 0xcfu));
    ADD_RES(resources[1152], 1309, 3, newImage(0x274eu, 0xd0u));
    ADD_RES(resources[1153], 1310, 1, newImage(0x274eu, 0x1u));
    ADD_RES(resources[1154], 1311, 3, newImage(0x274eu, 0xd1u));
    ADD_RES(resources[1155], 1313, 3, newImage(0x274eu, 0xd3u));
    ADD_RES(resources[1156], 1314, 3, newImage(0x274eu, 0xd4u));
    ADD_RES(resources[1157], 1315, 3, newImage(0x274eu, 0xd5u));
    ADD_RES(resources[1158], 1335, 3, newImage(0x274eu, 0xe9u));
    ADD_RES(resources[1159], 1336, 3, newImage(0x274eu, 0xeau));
    ADD_RES(resources[1160], 1337, 3, newImage(0x274eu, 0xebu));
    ADD_RES(resources[1161], 1338, 3, newImage(0x274eu, 0xecu));
    ADD_RES(resources[1162], 1339, 3, newImage(0x274eu, 0xedu));
    ADD_RES(resources[1163], 1340, 3, newImage(0x274eu, 0xeeu));
    ADD_RES(resources[1164], 1341, 3, newImage(0x274eu, 0xefu));
    ADD_RES(resources[1165], 1342, 3, newImage(0x274eu, 0xf0u));
    ADD_RES(resources[1166], 1349, 3, newImage(0x274eu, 0xf7u));
    ADD_RES(resources[1167], 1352, 3, newImage(0x274eu, 0xfau));
    ADD_RES(resources[1168], 8000, 3, newImage(0x2769u, 0x0u));
    ADD_RES(resources[1169], 8001, 3, newImage(0x2769u, 0x1u));
    ADD_RES(resources[1170], 8002, 3, newImage(0x2769u, 0x2u));
    ADD_RES(resources[1171], 8003, 3, newImage(0x2769u, 0x3u));
    ADD_RES(resources[1172], 8004, 3, newImage(0x2769u, 0x4u));
    ADD_RES(resources[1173], 8005, 3, newImage(0x2769u, 0x5u));
    ADD_RES(resources[1174], 8006, 3, newImage(0x2769u, 0x6u));
    ADD_RES(resources[1175], 8007, 3, newImage(0x2769u, 0x7u));
    ADD_RES(resources[1176], 8008, 3, newImage(0x2769u, 0x8u));
    ADD_RES(resources[1177], 8009, 3, newImage(0x2769u, 0x9u));
    ADD_RES(resources[1178], 8010, 3, newImage(0x2769u, 0xau));
    ADD_RES(resources[1179], 8011, 3, newImage(0x2769u, 0xbu));
    ADD_RES(resources[1180], 8012, 3, newImage(0x2769u, 0xcu));
    ADD_RES(resources[1181], 8013, 3, newImage(0x2769u, 0xdu));
    ADD_RES(resources[1182], 8014, 3, newImage(0x2769u, 0xeu));
    ADD_RES(resources[1183], 8015, 3, newImage(0x2769u, 0xfu));
    ADD_RES(resources[1184], 8016, 3, newImage(0x2769u, 0x10u));
    ADD_RES(resources[1185], 8017, 3, newImage(0x2769u, 0x11u));
    ADD_RES(resources[1186], 8018, 3, newImage(0x2769u, 0x12u));
    ADD_RES(resources[1187], 8019, 3, newImage(0x2769u, 0x13u));
    ADD_RES(resources[1188], 8020, 3, newImage(0x2769u, 0x14u));
    ADD_RES(resources[1189], 8021, 3, newImage(0x2769u, 0x15u));
    ADD_RES(resources[1190], 8022, 3, newImage(0x2769u, 0x16u));
    ADD_RES(resources[1191], 8023, 3, newImage(0x2769u, 0x17u));
    ADD_RES(resources[1192], 8024, 3, newImage(0x2769u, 0x18u));
    ADD_RES(resources[1193], 2200, 3, newImage(0x2750u, 0x0u));
    ADD_RES(resources[1194], 2201, 3, newImage(0x2750u, 0x1u));
    ADD_RES(resources[1195], 2202, 3, newImage(0x2750u, 0x2u));
    ADD_RES(resources[1196], 2203, 3, newImage(0x2750u, 0x3u));
    ADD_RES(resources[1197], 2204, 3, newImage(0x2750u, 0x4u));
    ADD_RES(resources[1198], 2205, 3, newImage(0x2750u, 0x5u));
    ADD_RES(resources[1199], 2206, 3, newImage(0x2750u, 0x6u));
    ADD_RES(resources[1200], 2207, 3, newImage(0x2750u, 0x7u));
    ADD_RES(resources[1201], 2208, 3, newImage(0x2750u, 0x8u));
    ADD_RES(resources[1202], 2209, 3, newImage(0x2750u, 0x9u));
    ADD_RES(resources[1203], 2210, 3, newImage(0x2750u, 0xau));
    ADD_RES(resources[1204], 2211, 3, newImage(0x2750u, 0xbu));
    ADD_RES(resources[1205], 2212, 3, newImage(0x2750u, 0xcu));
    ADD_RES(resources[1206], 2213, 3, newImage(0x2750u, 0xdu));
    ADD_RES(resources[1207], 2214, 3, newImage(0x2750u, 0xeu));
    ADD_RES(resources[1208], 2215, 3, newImage(0x2750u, 0xfu));
    ADD_RES(resources[1209], 2216, 3, newImage(0x2750u, 0x10u));
    ADD_RES(resources[1210], 2217, 3, newImage(0x2750u, 0x11u));
    ADD_RES(resources[1211], 2218, 3, newImage(0x2750u, 0x12u));
    ADD_RES(resources[1212], 2219, 3, newImage(0x2750u, 0x13u));
    ADD_RES(resources[1213], 2220, 3, newImage(0x2750u, 0x14u));
    ADD_RES(resources[1214], 2221, 3, newImage(0x2750u, 0x15u));
    ADD_RES(resources[1215], 2222, 3, newImage(0x2750u, 0x16u));
    ADD_RES(resources[1216], 2223, 3, newImage(0x2750u, 0x17u));
    ADD_RES(resources[1217], 2224, 3, newImage(0x2750u, 0x18u));
    ADD_RES(resources[1218], 2225, 3, newImage(0x2750u, 0x19u));
    ADD_RES(resources[1219], 2226, 3, newImage(0x2750u, 0x1au));
    ADD_RES(resources[1220], 2227, 3, newImage(0x2750u, 0x1bu));
    ADD_RES(resources[1221], 2228, 3, newImage(0x2750u, 0x1cu));
    ADD_RES(resources[1222], 2229, 3, newImage(0x2750u, 0x1du));
    ADD_RES(resources[1223], 2230, 3, newImage(0x2750u, 0x1eu));
    ADD_RES(resources[1224], 2231, 3, newImage(0x2750u, 0x1fu));
    ADD_RES(resources[1225], 2232, 3, newImage(0x2750u, 0x20u));
    ADD_RES(resources[1226], 2233, 3, newImage(0x2750u, 0x21u));
    ADD_RES(resources[1227], 2234, 3, newImage(0x2750u, 0x22u));
    ADD_RES(resources[1228], 2235, 3, newImage(0x2750u, 0x23u));
    ADD_RES(resources[1229], 2236, 3, newImage(0x2750u, 0x24u));
    ADD_RES(resources[1230], 2237, 3, newImage(0x2750u, 0x25u));
    ADD_RES(resources[1231], 2238, 3, newImage(0x2750u, 0x26u));
    ADD_RES(resources[1232], 2239, 3, newImage(0x2750u, 0x27u));
    ADD_RES(resources[1233], 2240, 3, newImage(0x2750u, 0x28u));
    ADD_RES(resources[1234], 2241, 3, newImage(0x2750u, 0x29u));
    ADD_RES(resources[1235], 2242, 3, newImage(0x2750u, 0x2au));
    ADD_RES(resources[1236], 2243, 3, newImage(0x2750u, 0x2bu));
    ADD_RES(resources[1237], 2244, 3, newImage(0x2750u, 0x2cu));
    ADD_RES(resources[1238], 2245, 3, newImage(0x2750u, 0x2du));
    ADD_RES(resources[1239], 2246, 3, newImage(0x2750u, 0x2eu));
    ADD_RES(resources[1240], 2247, 3, newImage(0x2750u, 0x2fu));
    ADD_RES(resources[1241], 2248, 3, newImage(0x2750u, 0x30u));
    ADD_RES(resources[1242], 2249, 3, newImage(0x2750u, 0x31u));
    ADD_RES(resources[1243], 2250, 3, newImage(0x2750u, 0x32u));
    ADD_RES(resources[1244], 2251, 3, newImage(0x2750u, 0x33u));
    ADD_RES(resources[1245], 2252, 3, newImage(0x2750u, 0x34u));
    ADD_RES(resources[1246], 2253, 3, newImage(0x2750u, 0x35u));
    ADD_RES(resources[1247], 2254, 3, newImage(0x2750u, 0x36u));
    ADD_RES(resources[1248], 2255, 3, newImage(0x2750u, 0x37u));
    ADD_RES(resources[1249], 2256, 3, newImage(0x2750u, 0x38u));
    ADD_RES(resources[1250], 2257, 3, newImage(0x2750u, 0x39u));
    ADD_RES(resources[1251], 2258, 3, newImage(0x2750u, 0x3au));
    ADD_RES(resources[1252], 2259, 3, newImage(0x2750u, 0x3bu));
    ADD_RES(resources[1253], 2260, 3, newImage(0x2750u, 0x3cu));
    ADD_RES(resources[1254], 2261, 3, newImage(0x2750u, 0x3du));
    ADD_RES(resources[1255], 2262, 3, newImage(0x2750u, 0x3eu));
    ADD_RES(resources[1256], 2263, 3, newImage(0x2750u, 0x3fu));
    ADD_RES(resources[1257], 2264, 3, newImage(0x2750u, 0x40u));
    ADD_RES(resources[1258], 2265, 3, newImage(0x2750u, 0x41u));
    ADD_RES(resources[1259], 2266, 3, newImage(0x2750u, 0x42u));
    ADD_RES(resources[1260], 2267, 3, newImage(0x2750u, 0x43u));
    ADD_RES(resources[1261], 2268, 3, newImage(0x2750u, 0x44u));
    ADD_RES(resources[1262], 2269, 3, newImage(0x2750u, 0x45u));
    ADD_RES(resources[1263], 2270, 3, newImage(0x2750u, 0x46u));
    ADD_RES(resources[1264], 2271, 3, newImage(0x2750u, 0x47u));
    ADD_RES(resources[1265], 2272, 3, newImage(0x2750u, 0x48u));
    ADD_RES(resources[1266], 2273, 3, newImage(0x2750u, 0x49u));
    ADD_RES(resources[1267], 2274, 3, newImage(0x2750u, 0x4au));
    ADD_RES(resources[1268], 2275, 3, newImage(0x2750u, 0x4bu));
    ADD_RES(resources[1269], 2276, 3, newImage(0x2750u, 0x4cu));
    ADD_RES(resources[1270], 2277, 3, newImage(0x2750u, 0x4du));
    ADD_RES(resources[1271], 2278, 3, newImage(0x2750u, 0x4eu));
    ADD_RES(resources[1272], 2279, 3, newImage(0x2750u, 0x4fu));
    ADD_RES(resources[1273], 2280, 3, newImage(0x2750u, 0x50u));
    ADD_RES(resources[1274], 2281, 3, newImage(0x2750u, 0x51u));
    ADD_RES(resources[1275], 2282, 3, newImage(0x2750u, 0x52u));
    ADD_RES(resources[1276], 2283, 3, newImage(0x2750u, 0x53u));
    ADD_RES(resources[1277], 2284, 3, newImage(0x2750u, 0x54u));
    ADD_RES(resources[1278], 2285, 3, newImage(0x2750u, 0x55u));
    ADD_RES(resources[1279], 2286, 3, newImage(0x2750u, 0x56u));
    ADD_RES(resources[1280], 2287, 3, newImage(0x2750u, 0x57u));
    ADD_RES(resources[1281], 2288, 3, newImage(0x2750u, 0x58u));
    ADD_RES(resources[1282], 2289, 3, newImage(0x2750u, 0x59u));
    ADD_RES(resources[1283], 2290, 3, newImage(0x2750u, 0x5au));
    ADD_RES(resources[1284], 2291, 3, newImage(0x2750u, 0x5bu));
    ADD_RES(resources[1285], 2292, 3, newImage(0x2750u, 0x5cu));
    ADD_RES(resources[1286], 2293, 3, newImage(0x2750u, 0x5du));
    ADD_RES(resources[1287], 2294, 3, newImage(0x2750u, 0x5eu));
    ADD_RES(resources[1288], 2295, 3, newImage(0x2750u, 0x5fu));
    ADD_RES(resources[1289], 2296, 3, newImage(0x2750u, 0x60u));
    ADD_RES(resources[1290], 2297, 3, newImage(0x2750u, 0x61u));
    ADD_RES(resources[1291], 2298, 3, newImage(0x2750u, 0x62u));
    ADD_RES(resources[1292], 2299, 3, newImage(0x2750u, 0x63u));
    ADD_RES(resources[1293], 2300, 3, newImage(0x2750u, 0x64u));
    ADD_RES(resources[1294], 2301, 3, newImage(0x2750u, 0x65u));
    ADD_RES(resources[1295], 2302, 3, newImage(0x2750u, 0x66u));
    ADD_RES(resources[1296], 2303, 3, newImage(0x2750u, 0x67u));
    ADD_RES(resources[1297], 2304, 3, newImage(0x2750u, 0x68u));
    ADD_RES(resources[1298], 2305, 3, newImage(0x2750u, 0x69u));
    ADD_RES(resources[1299], 2306, 3, newImage(0x2750u, 0x6au));
    ADD_RES(resources[1300], 2307, 3, newImage(0x2750u, 0x6bu));
    ADD_RES(resources[1301], 2308, 3, newImage(0x2750u, 0x6cu));
    ADD_RES(resources[1302], 2309, 3, newImage(0x2750u, 0x6du));
    ADD_RES(resources[1303], 2310, 3, newImage(0x2750u, 0x6eu));
    ADD_RES(resources[1304], 2311, 3, newImage(0x2750u, 0x6fu));
    ADD_RES(resources[1305], 2312, 3, newImage(0x2750u, 0x70u));
    ADD_RES(resources[1306], 2313, 3, newImage(0x2750u, 0x71u));
    ADD_RES(resources[1307], 2314, 3, newImage(0x2750u, 0x72u));
    ADD_RES(resources[1308], 2315, 3, newImage(0x2750u, 0x73u));
    ADD_RES(resources[1309], 2316, 3, newImage(0x2750u, 0x74u));
    ADD_RES(resources[1310], 2317, 3, newImage(0x2750u, 0x75u));
    ADD_RES(resources[1311], 2318, 3, newImage(0x2750u, 0x76u));
    ADD_RES(resources[1312], 2319, 3, newImage(0x2750u, 0x77u));
    ADD_RES(resources[1313], 2320, 3, newImage(0x2750u, 0x78u));
    ADD_RES(resources[1314], 2321, 3, newImage(0x2750u, 0x79u));
    ADD_RES(resources[1315], 2322, 3, newImage(0x2750u, 0x7au));
    ADD_RES(resources[1316], 2323, 3, newImage(0x2750u, 0x7bu));
    ADD_RES(resources[1317], 2324, 3, newImage(0x2750u, 0x7cu));
    ADD_RES(resources[1318], 2325, 3, newImage(0x2750u, 0x7du));
    ADD_RES(resources[1319], 2326, 3, newImage(0x2750u, 0x7eu));
    ADD_RES(resources[1320], 2327, 3, newImage(0x2750u, 0x7fu));
    ADD_RES(resources[1321], 2328, 3, newImage(0x2750u, 0x80u));
    ADD_RES(resources[1322], 2329, 3, newImage(0x2750u, 0x81u));
    ADD_RES(resources[1323], 2330, 3, newImage(0x2750u, 0x82u));
    ADD_RES(resources[1324], 2331, 3, newImage(0x2750u, 0x83u));
    ADD_RES(resources[1325], 2332, 3, newImage(0x2750u, 0x84u));
    ADD_RES(resources[1326], 2333, 3, newImage(0x2750u, 0x85u));
    ADD_RES(resources[1327], 2334, 3, newImage(0x2750u, 0x86u));
    ADD_RES(resources[1328], 2335, 3, newImage(0x2750u, 0x87u));
    ADD_RES(resources[1329], 2336, 3, newImage(0x2750u, 0x88u));
    ADD_RES(resources[1330], 2337, 3, newImage(0x2750u, 0x89u));
    ADD_RES(resources[1331], 2338, 3, newImage(0x2750u, 0x8au));
    ADD_RES(resources[1332], 2339, 3, newImage(0x2750u, 0x8bu));
    ADD_RES(resources[1333], 2340, 3, newImage(0x2750u, 0x8cu));
    ADD_RES(resources[1334], 2341, 3, newImage(0x2750u, 0x8du));
    ADD_RES(resources[1335], 2342, 3, newImage(0x2750u, 0x8eu));
    ADD_RES(resources[1336], 2343, 3, newImage(0x2750u, 0x8fu));
    ADD_RES(resources[1337], 2344, 3, newImage(0x2750u, 0x90u));
    ADD_RES(resources[1338], 2345, 3, newImage(0x2750u, 0x91u));
    ADD_RES(resources[1339], 2346, 3, newImage(0x2750u, 0x92u));
    ADD_RES(resources[1340], 2347, 3, newImage(0x2750u, 0x93u));
    ADD_RES(resources[1341], 2348, 3, newImage(0x2750u, 0x94u));
    ADD_RES(resources[1342], 2349, 3, newImage(0x2750u, 0x95u));
    ADD_RES(resources[1343], 2350, 3, newImage(0x2750u, 0x96u));
    ADD_RES(resources[1344], 2351, 3, newImage(0x2750u, 0x97u));
    ADD_RES(resources[1345], 2352, 3, newImage(0x2750u, 0x98u));
    ADD_RES(resources[1346], 2353, 3, newImage(0x2750u, 0x99u));
    ADD_RES(resources[1347], 2354, 3, newImage(0x2750u, 0x9au));
    ADD_RES(resources[1348], 2355, 3, newImage(0x2750u, 0x9bu));
    ADD_RES(resources[1349], 2356, 3, newImage(0x2750u, 0x9cu));
    ADD_RES(resources[1350], 2357, 3, newImage(0x2750u, 0x9du));
    ADD_RES(resources[1351], 2358, 3, newImage(0x2750u, 0x9eu));
    ADD_RES(resources[1352], 2359, 3, newImage(0x2750u, 0x9fu));
    ADD_RES(resources[1353], 2360, 3, newImage(0x2750u, 0xa0u));
    ADD_RES(resources[1354], 2361, 3, newImage(0x2750u, 0xa1u));
    ADD_RES(resources[1355], 2362, 3, newImage(0x2750u, 0xa2u));
    ADD_RES(resources[1356], 2363, 3, newImage(0x2750u, 0xa3u));
    ADD_RES(resources[1357], 2364, 3, newImage(0x2750u, 0xa4u));
    ADD_RES(resources[1358], 2365, 3, newImage(0x2750u, 0xa5u));
    ADD_RES(resources[1359], 2366, 3, newImage(0x2750u, 0xa6u));
    ADD_RES(resources[1360], 2367, 3, newImage(0x2750u, 0xa7u));
    ADD_RES(resources[1361], 2368, 3, newImage(0x2750u, 0xa8u));
    ADD_RES(resources[1362], 2369, 3, newImage(0x2750u, 0xa9u));
    ADD_RES(resources[1363], 2370, 3, newImage(0x2750u, 0xaau));
    ADD_RES(resources[1364], 2371, 3, newImage(0x2750u, 0xabu));
    ADD_RES(resources[1365], 2372, 3, newImage(0x2750u, 0xacu));
    ADD_RES(resources[1366], 2373, 3, newImage(0x2750u, 0xadu));
    ADD_RES(resources[1367], 2374, 3, newImage(0x2750u, 0xaeu));
    ADD_RES(resources[1368], 2375, 3, newImage(0x2750u, 0xafu));
    ADD_RES(resources[1369], 2376, 3, newImage(0x2750u, 0xb0u));
    ADD_RES(resources[1370], 2377, 3, newImage(0x2750u, 0xb1u));
    ADD_RES(resources[1371], 2378, 3, newImage(0x2750u, 0xb2u));
    ADD_RES(resources[1372], 2379, 3, newImage(0x2750u, 0xb3u));
    ADD_RES(resources[1373], 2380, 3, newImage(0x2750u, 0xb4u));
    ADD_RES(resources[1374], 2381, 3, newImage(0x2750u, 0xb5u));
    ADD_RES(resources[1375], 2382, 3, newImage(0x2750u, 0xb6u));
    ADD_RES(resources[1376], 2383, 3, newImage(0x2750u, 0xb7u));
    ADD_RES(resources[1377], 2384, 3, newImage(0x2750u, 0xb8u));
    ADD_RES(resources[1378], 2385, 3, newImage(0x2750u, 0xb9u));
    ADD_RES(resources[1379], 2386, 3, newImage(0x2750u, 0xbau));
    ADD_RES(resources[1380], 2387, 3, newImage(0x2750u, 0xbbu));
    ADD_RES(resources[1381], 2388, 3, newImage(0x2750u, 0xbcu));
    ADD_RES(resources[1382], 2389, 3, newImage(0x2750u, 0xbdu));
    ADD_RES(resources[1383], 2390, 3, newImage(0x2750u, 0xbeu));
    ADD_RES(resources[1384], 2391, 3, newImage(0x2750u, 0xbfu));
    ADD_RES(resources[1385], 2392, 3, newImage(0x2750u, 0xc0u));
    ADD_RES(resources[1386], 2393, 3, newImage(0x2750u, 0xc1u));
    ADD_RES(resources[1387], 2394, 3, newImage(0x2750u, 0xc2u));
    ADD_RES(resources[1388], 2395, 3, newImage(0x2750u, 0xc3u));
    ADD_RES(resources[1389], 2396, 3, newImage(0x2750u, 0xc4u));
    ADD_RES(resources[1390], 2397, 3, newImage(0x2750u, 0xc5u));
    ADD_RES(resources[1391], 2398, 3, newImage(0x2750u, 0xc6u));
    ADD_RES(resources[1392], 2399, 3, newImage(0x2750u, 0xc7u));
    ADD_RES(resources[1393], 2400, 3, newImage(0x2750u, 0xc8u));
    ADD_RES(resources[1394], 2401, 3, newImage(0x2750u, 0xc9u));
    ADD_RES(resources[1395], 2402, 3, newImage(0x2750u, 0xcau));
    ADD_RES(resources[1396], 2403, 3, newImage(0x2750u, 0xcbu));
    ADD_RES(resources[1397], 2404, 3, newImage(0x2750u, 0xccu));
    ADD_RES(resources[1398], 2405, 3, newImage(0x2750u, 0xcdu));
    ADD_RES(resources[1399], 2406, 3, newImage(0x2750u, 0xceu));
    ADD_RES(resources[1400], 2407, 3, newImage(0x2750u, 0xcfu));
    ADD_RES(resources[1401], 2408, 3, newImage(0x2750u, 0xd0u));
    ADD_RES(resources[1402], 2409, 3, newImage(0x2750u, 0xd1u));
    ADD_RES(resources[1403], 2410, 3, newImage(0x2750u, 0xd2u));
    ADD_RES(resources[1404], 2411, 3, newImage(0x2750u, 0xd3u));
    ADD_RES(resources[1405], 2412, 3, newImage(0x2750u, 0xd4u));
    ADD_RES(resources[1406], 2413, 3, newImage(0x2750u, 0xd5u));
    ADD_RES(resources[1407], 2414, 3, newImage(0x2750u, 0xd6u));
    ADD_RES(resources[1408], 2415, 3, newImage(0x2750u, 0xd7u));
    ADD_RES(resources[1409], 2416, 3, newImage(0x2750u, 0xd8u));
    ADD_RES(resources[1410], 2417, 3, newImage(0x2750u, 0xd9u));
    ADD_RES(resources[1411], 2418, 3, newImage(0x2750u, 0xdau));
    ADD_RES(resources[1412], 2419, 3, newImage(0x2750u, 0xdbu));
    ADD_RES(resources[1413], 2420, 3, newImage(0x2750u, 0xdcu));
    ADD_RES(resources[1414], 2421, 3, newImage(0x2750u, 0xddu));
    ADD_RES(resources[1415], 2422, 3, newImage(0x2750u, 0xdeu));
    ADD_RES(resources[1416], 2423, 3, newImage(0x2750u, 0xdfu));
    ADD_RES(resources[1417], 2424, 3, newImage(0x2750u, 0xe0u));
    ADD_RES(resources[1418], 2425, 3, newImage(0x2750u, 0xe1u));
    ADD_RES(resources[1419], 2426, 3, newImage(0x2750u, 0xe2u));
    ADD_RES(resources[1420], 2427, 3, newImage(0x2750u, 0xe3u));
    ADD_RES(resources[1421], 2428, 3, newImage(0x2750u, 0xe4u));
    ADD_RES(resources[1422], 2429, 3, newImage(0x2750u, 0xe5u));
    ADD_RES(resources[1423], 2430, 3, newImage(0x2750u, 0xe6u));
    ADD_RES(resources[1424], 2431, 3, newImage(0x2750u, 0xe7u));
    ADD_RES(resources[1425], 2432, 3, newImage(0x2750u, 0xe8u));
    ADD_RES(resources[1426], 2433, 3, newImage(0x2750u, 0xe9u));
    ADD_RES(resources[1427], 2434, 3, newImage(0x2750u, 0xeau));
    ADD_RES(resources[1428], 2435, 3, newImage(0x2750u, 0xebu));
    ADD_RES(resources[1429], 2436, 3, newImage(0x2750u, 0xecu));
    ADD_RES(resources[1430], 2437, 3, newImage(0x2750u, 0xedu));
    ADD_RES(resources[1431], 2438, 3, newImage(0x2750u, 0xeeu));
    ADD_RES(resources[1432], 2439, 3, newImage(0x2750u, 0xefu));
    ADD_RES(resources[1433], 2440, 3, newImage(0x2750u, 0xf0u));
    ADD_RES(resources[1434], 2441, 3, newImage(0x2750u, 0xf1u));
    ADD_RES(resources[1435], 2442, 3, newImage(0x2750u, 0xf2u));
    ADD_RES(resources[1436], 2443, 3, newImage(0x2750u, 0xf3u));
    ADD_RES(resources[1437], 2444, 3, newImage(0x2750u, 0xf4u));
    ADD_RES(resources[1438], 2445, 3, newImage(0x2750u, 0xf5u));
    ADD_RES(resources[1439], 2446, 3, newImage(0x2750u, 0xf6u));
    ADD_RES(resources[1440], 2447, 3, newImage(0x2750u, 0xf7u));
    ADD_RES(resources[1441], 2448, 3, newImage(0x2750u, 0xf8u));
    ADD_RES(resources[1442], 2449, 3, newImage(0x2750u, 0xf9u));
    ADD_RES(resources[1443], 2450, 3, newImage(0x2750u, 0xfau));
    ADD_RES(resources[1444], 2451, 3, newImage(0x2750u, 0xfbu));
    ADD_RES(resources[1445], 2452, 3, newImage(0x2750u, 0xfcu));
    ADD_RES(resources[1446], 2453, 3, newImage(0x2750u, 0xfdu));
    ADD_RES(resources[1447], 2454, 3, newImage(0x2750u, 0xfeu));
    ADD_RES(resources[1448], 2455, 3, newImage(0x2750u, 0xffu));
    ADD_RES(resources[1449], 2456, 3, newImage(0x2750u, 0x100u));
    ADD_RES(resources[1450], 2457, 3, newImage(0x2750u, 0x101u));
    ADD_RES(resources[1451], 2458, 3, newImage(0x2750u, 0x102u));
    ADD_RES(resources[1452], 2459, 3, newImage(0x2750u, 0x103u));
    ADD_RES(resources[1453], 2460, 3, newImage(0x2750u, 0x104u));
    ADD_RES(resources[1454], 2461, 3, newImage(0x2750u, 0x105u));
    ADD_RES(resources[1455], 2462, 3, newImage(0x2750u, 0x106u));
    ADD_RES(resources[1456], 2463, 3, newImage(0x2750u, 0x107u));
    ADD_RES(resources[1457], 2464, 3, newImage(0x2750u, 0x108u));
    ADD_RES(resources[1458], 2465, 3, newImage(0x2750u, 0x109u));
    ADD_RES(resources[1459], 2466, 3, newImage(0x2750u, 0x10au));
    ADD_RES(resources[1460], 2467, 3, newImage(0x2750u, 0x10bu));
    ADD_RES(resources[1461], 2468, 3, newImage(0x2750u, 0x10cu));
    ADD_RES(resources[1462], 2469, 3, newImage(0x2750u, 0x10du));
    ADD_RES(resources[1463], 2470, 3, newImage(0x2750u, 0x10eu));
    ADD_RES(resources[1464], 2471, 3, newImage(0x2750u, 0x10fu));
    ADD_RES(resources[1465], 4000, 3, newImage(0x2d70u, 0x0u));
    ADD_RES(resources[1466], 4001, 3, newImage(0x2d70u, 0x1u));
    ADD_RES(resources[1467], 4002, 3, newImage(0x2d70u, 0x2u));
    ADD_RES(resources[1468], 4003, 3, newImage(0x2d70u, 0x3u));
    ADD_RES(resources[1469], 4004, 3, newImage(0x2d70u, 0x4u));
    ADD_RES(resources[1470], 4005, 3, newImage(0x2d70u, 0x5u));
    ADD_RES(resources[1471], 4006, 3, newImage(0x2d70u, 0x6u));
    ADD_RES(resources[1472], 4007, 3, newImage(0x2d70u, 0x7u));
    ADD_RES(resources[1473], 4008, 3, newImage(0x2d70u, 0x8u));
    ADD_RES(resources[1474], 4009, 3, newImage(0x2d70u, 0x9u));
    ADD_RES(resources[1475], 4010, 3, newImage(0x2d70u, 0xau));
    ADD_RES(resources[1476], 4011, 3, newImage(0x2d70u, 0xbu));
    ADD_RES(resources[1477], 4012, 3, newImage(0x2d70u, 0xcu));
    ADD_RES(resources[1478], 4013, 3, newImage(0x2d70u, 0xdu));
    ADD_RES(resources[1479], 4014, 3, newImage(0x2d70u, 0xeu));
    ADD_RES(resources[1480], 4015, 3, newImage(0x2d70u, 0xfu));
    ADD_RES(resources[1481], 4016, 3, newImage(0x2d70u, 0x10u));
    ADD_RES(resources[1482], 4017, 3, newImage(0x2d70u, 0x11u));
    ADD_RES(resources[1483], 4018, 3, newImage(0x2d70u, 0x12u));
    ADD_RES(resources[1484], 4019, 3, newImage(0x2d70u, 0x13u));
    ADD_RES(resources[1485], 4020, 3, newImage(0x2d70u, 0x14u));
    ADD_RES(resources[1486], 4021, 3, newImage(0x2d70u, 0x15u));
    ADD_RES(resources[1487], 4022, 3, newImage(0x2d70u, 0x16u));
    ADD_RES(resources[1488], 4023, 3, newImage(0x2d70u, 0x17u));
    ADD_RES(resources[1489], 4024, 3, newImage(0x2d70u, 0x18u));
    ADD_RES(resources[1490], 4025, 3, newImage(0x2d70u, 0x19u));
    ADD_RES(resources[1491], 4026, 3, newImage(0x2d70u, 0x1au));
    ADD_RES(resources[1492], 4027, 3, newImage(0x2d70u, 0x1bu));
    ADD_RES(resources[1493], 4028, 3, newImage(0x2d70u, 0x1cu));
    ADD_RES(resources[1494], 4029, 3, newImage(0x2d70u, 0x1du));
    ADD_RES(resources[1495], 4030, 3, newImage(0x2d70u, 0x1eu));
    ADD_RES(resources[1496], 4031, 3, newImage(0x2d70u, 0x1fu));
    ADD_RES(resources[1497], 4032, 3, newImage(0x2d70u, 0x20u));
    ADD_RES(resources[1498], 4033, 3, newImage(0x2d70u, 0x21u));
    ADD_RES(resources[1499], 4034, 3, newImage(0x2d70u, 0x22u));
    ADD_RES(resources[1500], 4035, 3, newImage(0x2d70u, 0x23u));
    ADD_RES(resources[1501], 4036, 3, newImage(0x2d70u, 0x24u));
    ADD_RES(resources[1502], 4037, 3, newImage(0x2d70u, 0x25u));
    ADD_RES(resources[1503], 4038, 3, newImage(0x2d70u, 0x26u));
    ADD_RES(resources[1504], 4039, 3, newImage(0x2d70u, 0x27u));
    ADD_RES(resources[1505], 4040, 3, newImage(0x2d70u, 0x28u));
    ADD_RES(resources[1506], 4041, 3, newImage(0x2d70u, 0x29u));
    ADD_RES(resources[1507], 4042, 3, newImage(0x2d70u, 0x2au));
    ADD_RES(resources[1508], 4043, 3, newImage(0x2d70u, 0x2bu));
    ADD_RES(resources[1509], 4044, 3, newImage(0x2d70u, 0x2cu));
    ADD_RES(resources[1510], 4045, 3, newImage(0x2d70u, 0x2du));
    ADD_RES(resources[1511], 4046, 3, newImage(0x2d70u, 0x2eu));
    ADD_RES(resources[1512], 4047, 3, newImage(0x2d70u, 0x2fu));
    ADD_RES(resources[1513], 4048, 3, newImage(0x2d70u, 0x30u));
    ADD_RES(resources[1514], 4049, 3, newImage(0x2d70u, 0x31u));
    ADD_RES(resources[1515], 4050, 3, newImage(0x2d70u, 0x32u));
    ADD_RES(resources[1516], 4051, 3, newImage(0x2d70u, 0x33u));
    ADD_RES(resources[1517], 4052, 3, newImage(0x2d70u, 0x34u));
    ADD_RES(resources[1518], 4053, 3, newImage(0x2d70u, 0x35u));
    ADD_RES(resources[1519], 4054, 3, newImage(0x2d70u, 0x36u));
    ADD_RES(resources[1520], 4055, 3, newImage(0x2d70u, 0x37u));
    ADD_RES(resources[1521], 4056, 3, newImage(0x2d70u, 0x38u));
    ADD_RES(resources[1522], 2472, 3, newImage(0x2d70u, 0x39u));
    ADD_RES(resources[1523], 2473, 3, newImage(0x2d70u, 0x3au));
    ADD_RES(resources[1524], 2474, 3, newImage(0x2d70u, 0x3bu));
    ADD_RES(resources[1525], 2475, 3, newImage(0x2d70u, 0x3cu));
    ADD_RES(resources[1526], 2476, 3, newImage(0x2d70u, 0x3du));
    ADD_RES(resources[1527], 2477, 3, newImage(0x2d70u, 0x3eu));
    ADD_RES(resources[1528], 2478, 3, newImage(0x2d70u, 0x3fu));
    ADD_RES(resources[1529], 2479, 3, newImage(0x2d70u, 0x40u));
    ADD_RES(resources[1530], 2480, 3, newImage(0x2d70u, 0x41u));
    ADD_RES(resources[1531], 5500, 3, newImage(0x2750u, 0x0u));
    ADD_RES(resources[1532], 5501, 3, newImage(0x2750u, 0x1u));
    ADD_RES(resources[1533], 5502, 3, newImage(0x2750u, 0x2u));
    ADD_RES(resources[1534], 7000, 3, newImage(0x2712u, 0x0u));
    ADD_RES(resources[1535], 7001, 3, newImage(0x2712u, 0x1u));
    ADD_RES(resources[1536], 7002, 3, newImage(0x2712u, 0x2u));
    ADD_RES(resources[1537], 7003, 3, newImage(0x2712u, 0x3u));
    ADD_RES(resources[1538], 7004, 3, newImage(0x2712u, 0x4u));
    ADD_RES(resources[1539], 6000, 3, newImage(0x6acdu, 0x0u));
    ADD_RES(resources[1540], 6001, 3, newImage(0x6acdu, 0x1u));
    ADD_RES(resources[1541], 6002, 3, newImage(0x6acdu, 0x2u));
    ADD_RES(resources[1542], 6003, 3, newImage(0x6acdu, 0x3u));
    ADD_RES(resources[1543], 6004, 3, newImage(0x6acdu, 0x4u));
    ADD_RES(resources[1544], 6005, 3, newImage(0x6acdu, 0x5u));
    ADD_RES(resources[1545], 6006, 3, newImage(0x6acdu, 0x6u));
    ADD_RES(resources[1546], 6007, 3, newImage(0x6acdu, 0x7u));
    ADD_RES(resources[1547], 6008, 3, newImage(0x6acdu, 0x8u));
    ADD_RES(resources[1548], 6009, 3, newImage(0x6acdu, 0x9u));
    ADD_RES(resources[1549], 6010, 3, newImage(0x6acdu, 0xau));
    ADD_RES(resources[1550], 6600, 3, newImage(0x733cu, 0x0u));
    ADD_RES(resources[1551], 6601, 3, newImage(0x733cu, 0x1u));
    ADD_RES(resources[1552], 6602, 3, newImage(0x733cu, 0x2u));
    ADD_RES(resources[1553], 6603, 3, newImage(0x733cu, 0x3u));
    ADD_RES(resources[1554], 6604, 3, newImage(0x733cu, 0x4u));
    ADD_RES(resources[1555], 6605, 3, newImage(0x733cu, 0x5u));
    ADD_RES(resources[1556], 6606, 3, newImage(0x733cu, 0x6u));
    ADD_RES(resources[1557], 6607, 3, newImage(0x733cu, 0x7u));
    ADD_RES(resources[1558], 6650, 3, newImage(0x733du, 0x0u));
    ADD_RES(resources[1559], 
        14379, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_midorian.aem", 24071,
                                                false));
    ADD_RES(resources[1560], 
        14310, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_midorian_add.aem", 20069,
                                                false));
    ADD_RES(resources[1561], 
        14264, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_midorian_alpha.aem", 20106,
                                                false));
    ADD_RES(resources[1562], 
        14377, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_vossk.aem", 24074, false));
    ADD_RES(resources[1563], 
        14304, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_vossk_anim_add.aem", 20071,
                                                false));
    ADD_RES(resources[1564], 
        14378, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_nivelian.aem", 24073,
                                                false));
    ADD_RES(resources[1565], 
        14307, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_nivelian_alpha.aem", 20074,
                                                false));
    ADD_RES(resources[1566], 
        14308, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_nivelian_add.aem", 20075,
                                                false));
    ADD_RES(resources[1567], 
        14376, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran.aem", 24070, false));
    ADD_RES(resources[1568], 
        14301, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_alpha.aem", 20077,
                                                false));
    ADD_RES(resources[1569], 
        14302, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_add.aem", 20078,
                                                false));
    ADD_RES(resources[1570], 
        14354, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/hangars/v_hangar_battlestation.aem",
                                                20157, false));
    ADD_RES(resources[1571], 14355, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/hangars/v_hangar_battlestation_emissive.aem", 27138,
                                  false));
    ADD_RES(resources[1572], 14356, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/hangars/v_hangar_battlestation_alpha.aem", 27139,
                                  false));
    ADD_RES(resources[1573], 
        14357, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/hangars/v_hangar_battlestation_add.aem",
                                                27140, false));
    ADD_RES(resources[1574], 14358, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/hangars/v_hangar_deep_science_emissive.aem", 27141,
                                  false));
    ADD_RES(resources[1575], 
        14359, 4, new AbyssEngine::ResourceMesh(
            "data/assets/valkyrie/3d/meshes/hangars/v_hangar_deep_science_alpha.aem", 27142, false));
    ADD_RES(resources[1576], 14360, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/hangars/v_hangar_deep_science_anim_add.aem", 27143,
                                  false));
    ADD_RES(resources[1577], 
        14362, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/hangars/v_hangar_deep_science.aem",
                                                27144, false));
    ADD_RES(resources[1578], 
        14265, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_bot_a_anim.aem",
                                                24070, false));
    ADD_RES(resources[1579], 
        14266, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_bot_b_anim.aem",
                                                24070, false));
    ADD_RES(resources[1580], 
        14267, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_bot_c_anim.aem",
                                                24070, false));
    ADD_RES(resources[1581], 
        14268, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_bot_d_anim.aem",
                                                24070, false));
    ADD_RES(resources[1582], 
        14269, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_bot_a_anim_add.aem",
                                                20078, false));
    ADD_RES(resources[1583], 
        14270, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_bot_b_anim_add.aem",
                                                20078, false));
    ADD_RES(resources[1584], 
        14271, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_bot_c_anim_add.aem",
                                                20078, false));
    ADD_RES(resources[1585], 
        14272, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_bot_d_anim_add.aem",
                                                20078, false));
    ADD_RES(resources[1586], 
        14273, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_nivelian_bot_a_anim.aem",
                                                24073, false));
    ADD_RES(resources[1587], 
        14274, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_nivelian_bot_b_anim.aem",
                                                24073, false));
    ADD_RES(resources[1588], 
        14275, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_nivelian_bot_c_anim.aem",
                                                24073, false));
    ADD_RES(resources[1589], 
        14276, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_nivelian_bot_a_anim_add.aem",
                                                20075, false));
    ADD_RES(resources[1590], 
        14277, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_nivelian_bot_b_anim_add.aem",
                                                20075, false));
    ADD_RES(resources[1591], 
        14278, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_nivelian_bot_c_anim_add.aem",
                                                20075, false));
    ADD_RES(resources[1592], 
        14380, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_x1.aem", 24070, false));
    ADD_RES(resources[1593], 
        14381, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_x1_shadow_alpha.aem",
                                                20077, false));
    ADD_RES(resources[1594], 
        14382, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_x2.aem", 24070, false));
    ADD_RES(resources[1595], 
        14383, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_x2_shadow_alpha.aem",
                                                20077, false));
    ADD_RES(resources[1596], 
        14384, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_x3.aem", 24070, false));
    ADD_RES(resources[1597], 
        14385, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_x3_shadow_alpha.aem",
                                                20077, false));
    ADD_RES(resources[1598], 
        14386, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_x4.aem", 24070, false));
    ADD_RES(resources[1599], 
        14387, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_x4_shadow_alpha.aem",
                                                20077, false));
    ADD_RES(resources[1600], 
        14388, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_x5.aem", 24070, false));
    ADD_RES(resources[1601], 
        14389, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_x5_shadow_alpha.aem",
                                                20077, false));
    ADD_RES(resources[1602], 
        14390, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_terran_x6.aem", 24070, false));
    ADD_RES(resources[1603], 
        14391, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_midorian_x1.aem", 24071,
                                                false));
    ADD_RES(resources[1604], 
        14392, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/hangars/hangar_midorian_x1_shadow_alpha.aem", 20106, false));
    ADD_RES(resources[1605], 
        14393, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_midorian_x2.aem", 24071,
                                                false));
    ADD_RES(resources[1606], 
        14394, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/hangars/hangar_midorian_x2_shadow_alpha.aem", 20106, false));
    ADD_RES(resources[1607], 
        14395, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_midorian_x3.aem", 24071,
                                                false));
    ADD_RES(resources[1608], 
        14396, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/hangars/hangar_midorian_x3_shadow_alpha.aem", 20106, false));
    ADD_RES(resources[1609], 
        14397, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_midorian_x4.aem", 24071,
                                                false));
    ADD_RES(resources[1610], 
        14398, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/hangars/hangar_midorian_x4_shadow_alpha.aem", 20106, false));
    ADD_RES(resources[1611], 
        14399, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_midorian_x5.aem", 24071,
                                                false));
    ADD_RES(resources[1612], 
        14400, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/hangars/hangar_midorian_x5_shadow_alpha.aem", 20106, false));
    ADD_RES(resources[1613], 
        14401, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_midorian_x5.aem", 24071,
                                                false));
    ADD_RES(resources[1614], 
        14402, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/hangars/hangar_midorian_x5_shadow_alpha.aem", 20106, false));
    ADD_RES(resources[1615], 
        14403, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_nivelian_x1.aem", 24073,
                                                false));
    ADD_RES(resources[1616], 
        14404, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/hangars/hangar_nivelian_x1_shadow_alpha.aem", 20074, false));
    ADD_RES(resources[1617], 
        14405, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_nivelian_x2.aem", 24073,
                                                false));
    ADD_RES(resources[1618], 
        14406, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/hangars/hangar_nivelian_x2_shadow_alpha.aem", 20074, false));
    ADD_RES(resources[1619], 
        14407, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_nivelian_x3.aem", 24073,
                                                false));
    ADD_RES(resources[1620], 
        14408, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/hangars/hangar_nivelian_x3_shadow_alpha.aem", 20074, false));
    ADD_RES(resources[1621], 
        14409, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_nivelian_x4.aem", 24073,
                                                false));
    ADD_RES(resources[1622], 
        14410, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/hangars/hangar_nivelian_x4_shadow_alpha.aem", 20074, false));
    ADD_RES(resources[1623], 
        14411, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/hangars/hangar_nivelian_x5.aem", 24073,
                                                false));
    ADD_RES(resources[1624], 
        14412, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/hangars/hangar_nivelian_x5_shadow_alpha.aem", 20074, false));
    ADD_RES(resources[1625], 
        14025, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_terran.aem", 20056, false));
    ADD_RES(resources[1626], 
        14026, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_terran_add.aem", 20057, false));
    ADD_RES(resources[1627], 
        14027, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_terran_alpha.aem", 20058, false));
    ADD_RES(resources[1628], 
        14280, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_terran_bot_anim.aem", 20056, false));
    ADD_RES(resources[1629], 
        14279, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_terran_bot_anim_add.aem", 20057,
                                                false));
    ADD_RES(resources[1630], 
        14028, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_vossk.aem", 20059, false));
    ADD_RES(resources[1631], 
        14029, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_vossk_anim_add.aem", 20060,
                                                false));
    ADD_RES(resources[1632], 
        14031, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_nivelian.aem", 20062, false));
    ADD_RES(resources[1633], 
        14032, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_nivelian_anim_add.aem", 20063,
                                                false));
    ADD_RES(resources[1634], 
        14033, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_nivelian_alpha.aem", 20064,
                                                false));
    ADD_RES(resources[1635], 
        14034, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_midorian_anim.aem", 20065, false));
    ADD_RES(resources[1636], 
        14035, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_midorian_add.aem", 20066, false));
    ADD_RES(resources[1637], 
        14036, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_midorian_alpha.aem", 20067,
                                                false));
    ADD_RES(resources[1638], 
        14038, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_midorian_alpha_anim.aem", 20067,
                                                false));
    ADD_RES(resources[1639], 
        14725, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_visitor_terran_m.aem", 20119,
                                                false));
    ADD_RES(resources[1640], 
        14724, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_visitor_terran_f.aem", 20118,
                                                false));
    ADD_RES(resources[1641], 
        14726, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_visitor_vossk.aem", 20120, false));
    ADD_RES(resources[1642], 
        14723, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_visitor_nivelian.aem", 20117,
                                                false));
    ADD_RES(resources[1643], 
        14722, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_visitor_multipod.aem", 20116,
                                                false));
    ADD_RES(resources[1644], 
        14720, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_visitor_bobolan.aem", 20114, false));
    ADD_RES(resources[1645], 
        14721, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_visitor_grey.aem", 20115, false));
    ADD_RES(resources[1646], 
        14281, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_visitor_glow.aem", 65535, false));
    ADD_RES(resources[1647], 
        14282, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_visitor_glow.aem", 65535, false));
    ADD_RES(resources[1648], 
        14283, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_visitor_glow.aem", 65535, false));
    ADD_RES(resources[1649], 
        14284, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_visitor_glow.aem", 65535, false));
    ADD_RES(resources[1650], 
        14348, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/bars/bar_visitor_shadow.aem", 65535,
                                                false));
    ADD_RES(resources[1651], 
        17800, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_000.aem", 20009, false));
    ADD_RES(resources[1652], 
        17801, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_001.aem", 20010, false));
    ADD_RES(resources[1653], 
        17802, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_002.aem", 20011, false));
    ADD_RES(resources[1654], 
        17803, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_003.aem", 20012, false));
    ADD_RES(resources[1655], 
        17804, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_004.aem", 20013, false));
    ADD_RES(resources[1656], 
        17805, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_005.aem", 20014, false));
    ADD_RES(resources[1657], 
        17806, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_006.aem", 20015, false));
    ADD_RES(resources[1658], 
        17807, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_007.aem", 20016, false));
    ADD_RES(resources[1659], 
        17808, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_008.aem", 20017, false));
    ADD_RES(resources[1660], 
        17809, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_009.aem", 20018, false));
    ADD_RES(resources[1661], 
        17810, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_010.aem", 20019, false));
    ADD_RES(resources[1662], 
        17811, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/skyboxes/v_skybox_011.aem", 20130,
                                                false));
    ADD_RES(resources[1663], 
        17812, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/skyboxes/v_skybox_012.aem", 20131,
                                                false));
    ADD_RES(resources[1664], 
        17813, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/skyboxes/v_skybox_013.aem", 20132,
                                                false));
    ADD_RES(resources[1665], 
        17814, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/skyboxes/v_skybox_013.aem", 27149,
                                                false));
    ADD_RES(resources[1666], 
        17850, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_stars.aem", 20009, false));
    ADD_RES(resources[1667], 
        17851, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_stars.aem", 20010, false));
    ADD_RES(resources[1668], 
        17852, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_stars.aem", 20011, false));
    ADD_RES(resources[1669], 
        6768, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 65535, false));
    ADD_RES(resources[1670], 
        16916, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/container_005_void.aem", 65535,
                                                false));
    ADD_RES(resources[1671], 
        16805, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/explosion_emp_anim_lookat_add.aem",
                                                27300, false));
    ADD_RES(resources[1672], 
        16820, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/explosion_anim_lookat_add.aem", 27252,
                                                false));
    ADD_RES(resources[1673], 
        16821, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/explosion_anim_lookat_alpha.aem", 27253,
                                                false));
    ADD_RES(resources[1674], 16806, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/fx/v_scattergun_000_explosion_lookat_anim_add.aem",
                                  20151, false));
    ADD_RES(resources[1675], 16807, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/fx/v_scattergun_000_explosion_lookat_anim_add.aem",
                                  20152, false));
    ADD_RES(resources[1676], 16808, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/fx/v_scattergun_000_explosion_lookat_anim_add.aem",
                                  20153, false));
    ADD_RES(resources[1677], 
        6754, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_000_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1678], 
        6755, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_001_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1679], 
        6756, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_002_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1680], 
        6760, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_003_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1681], 
        6761, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_004_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1682], 
        6762, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_005_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1683], 
        6763, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_006_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1684], 
        6764, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_007_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1685], 
        6765, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_008_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1686], 
        14297, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/fx/v_projectile_183_anim_add.aem",
                                                24096, false));
    ADD_RES(resources[1687], 
        6788, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_012_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1688], 
        6789, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_013_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1689], 
        6790, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_014_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1690], 
        6791, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_015_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1691], 
        6792, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_016_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1692], 
        6793, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_017_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1693], 
        6794, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_018_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1694], 
        6795, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_019_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1695], 
        6796, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_020_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1696], 
        6797, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_021_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1697], 
        6798, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_022_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1698], 
        6799, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_023_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1699], 
        6800, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_024_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1701], 
        6802, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_026_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1702], 
        6803, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_027_anim_add.aem", 24201,
                                               false));
    ADD_RES(resources[1703], 
        14232, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_068_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1704], 
        14233, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_069_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1705], 
        14234, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_070_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1706], 
        14229, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_009_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1707], 
        14230, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_010_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1708], 
        14231, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_011_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1709], 
        14236, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_028_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1710], 
        14237, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_029_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1711], 
        14238, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/projectile_030_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1712], 
        6900, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/fx/v_projectile_176_anim_add.aem", 24096,
                                               false));
    ADD_RES(resources[1713], 
        6901, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/fx/v_projectile_177_anim_add.aem", 24096,
                                               false));
    ADD_RES(resources[1714], 
        6902, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/fx/v_projectile_178_anim_add.aem", 24096,
                                               false));
    ADD_RES(resources[1715], 
        6905, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/fx/v_projectile_180_anim_add.aem", 24096,
                                               false));
    ADD_RES(resources[1716], 
        6906, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/fx/v_projectile_181_anim_add.aem", 24096,
                                               false));
    ADD_RES(resources[1717], 
        14239, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/fx/v_projectile_193_anim_add.aem",
                                                24096, false));
    ADD_RES(resources[1718], 
        14235, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/fx/v_projectile_194_anim_add.aem",
                                                24096, false));
    ADD_RES(resources[1719], 
        14247, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/rocket_explosive.aem", 65535, false));
    ADD_RES(resources[1720], 
        14248, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/rocket_explosive_add.aem", 0, false));
    ADD_RES(resources[1721], 
        14249, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/rocket_emp.aem", 65535, false));
    ADD_RES(resources[1722], 
        14250, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/rocket_emp_add.aem", 0, false));
    ADD_RES(resources[1723], 
        14680, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/bomb_explosive_a.aem", 65535, false));
    ADD_RES(resources[1724], 
        14681, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/bomb_explosive_a_add.aem", 0, false));
    ADD_RES(resources[1725], 
        14682, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/bomb_explosive_b.aem", 65535, false));
    ADD_RES(resources[1726], 
        14683, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/bomb_explosive_b_add.aem", 0, false));
    ADD_RES(resources[1727], 
        14684, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/bomb_emp_a.aem", 65535, false));
    ADD_RES(resources[1728], 
        14685, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/bomb_emp_a_add.aem", 0, false));
    ADD_RES(resources[1729], 
        14293, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/misc/v_guided_missile_anim.aem", 20125,
                                                false));
    ADD_RES(resources[1730], 
        14294, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/misc/v_guided_missile_add.aem", 20126,
                                                false));
    ADD_RES(resources[1731], 
        15000, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_terran.aem", 20004,
                                                false));
    ADD_RES(resources[1732], 
        15016, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_terran_lod_1.aem", 20004,
                                                false));
    ADD_RES(resources[1733], 
        15020, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_terran_lod_2.aem", 20004,
                                                false));
    ADD_RES(resources[1734], 
        15002, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[1735], 
        15001, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_terran_anim_add.aem", 3,
                                                false));
    ADD_RES(resources[1736], 
        15003, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/jumpgates/jumpgate_terran_jump_anim_add.aem", 3, false));
    ADD_RES(resources[1737], 
        15004, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_vossk_anim.aem", 20032,
                                                false));
    ADD_RES(resources[1738], 
        15017, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_vossk_lod_1_anim.aem",
                                                20032, false));
    ADD_RES(resources[1739], 
        15021, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_vossk_lod_2_anim.aem",
                                                20032, false));
    ADD_RES(resources[1740], 
        15006, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_vossk_emissive.aem",
                                                20031, false));
    ADD_RES(resources[1741], 
        15024, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/jumpgates/jumpgate_vossk_lod_1_emissive.aem", 20031, false));
    ADD_RES(resources[1742], 
        15025, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/jumpgates/jumpgate_vossk_lod_2_emissive.aem", 20031, false));
    ADD_RES(resources[1743], 
        15005, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_vossk_anim_add.aem",
                                                20033, false));
    ADD_RES(resources[1744], 
        15007, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_vossk_jump_anim_add.aem",
                                                20033, false));
    ADD_RES(resources[1745], 
        15008, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[1746], 
        15018, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_nivelian_lod_1.aem",
                                                20005, false));
    ADD_RES(resources[1747], 
        15022, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_nivelian_lod_2.aem",
                                                20005, false));
    ADD_RES(resources[1748], 
        15010, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[1749], 
        15009, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_nivelian_anim_add.aem",
                                                34810, false));
    ADD_RES(resources[1750], 15011, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/jumpgates/jumpgate_nivelian_jump_anim_add.aem", 34810,
                                  false));
    ADD_RES(resources[1751], 
        15012, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_midorian.aem", 20007,
                                                false));
    ADD_RES(resources[1752], 
        15019, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_midorian_lod_1.aem",
                                                20007, false));
    ADD_RES(resources[1753], 
        15023, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_midorian_lod_2.aem",
                                                20007, false));
    ADD_RES(resources[1754], 
        15014, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_midorian_emissive.aem",
                                                20008, false));
    ADD_RES(resources[1755], 
        15013, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/jumpgates/jumpgate_midorian_anim_add.aem",
                                                34600, false));
    ADD_RES(resources[1756], 15015, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/jumpgates/jumpgate_midorian_jump_anim_add.aem", 34600,
                                  false));
    ADD_RES(resources[1757], 
        15026, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/khador_jump.aem", 27260, false));
    ADD_RES(resources[1758], 
        15027, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/hyper_drive.aem", 27262, false));
    ADD_RES(resources[1759], 
        16917, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/space_junk_001.aem", 20094, false));
    ADD_RES(resources[1760], 
        16918, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/space_junk_002.aem", 20094, false));
    ADD_RES(resources[1761], 
        16919, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/space_junk_003.aem", 20094, false));
    ADD_RES(resources[1762], 
        16920, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/space_junk_004.aem", 20094, false));
    ADD_RES(resources[1763], 
        16926, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/asteroid_01_junk.aem", 20023, false));
    ADD_RES(resources[1764], 
        16927, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/asteroid_void_junk.aem", 20103,
                                                false));
    ADD_RES(resources[1765], 
        16992, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/container_003_terran.aem", 27254,
                                                false));
    ADD_RES(resources[1766], 
        16990, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/container_001_midorian.aem", 27257,
                                                false));
    ADD_RES(resources[1767], 
        16991, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/container_004_vossk.aem", 27255, false));
    ADD_RES(resources[1768], 
        16993, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/container_002_nivelian.aem", 27256,
                                                false));
    ADD_RES(resources[1769], 
        16994, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/wormhole_anim_add.aem", 20093, false));
    ADD_RES(resources[1770], 
        16921, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/asteroid_void.aem", 20103, false));
    ADD_RES(resources[1771], 
        16922, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/asteroid_void_lod_1.aem", 20103, false));
    ADD_RES(resources[1772], 
        16923, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/asteroid_void_lod_2.aem", 20103, false));
    ADD_RES(resources[1773], 
        16924, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/asteroid_void_lod_3.aem", 20103, false));
    ADD_RES(resources[1774], 
        16925, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/asteroid_void_explosion_anim.aem",
                                                20103, false));
    ADD_RES(resources[1775], 
        14311, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/battleship_terran.aem", 0, false));
    ADD_RES(resources[1776], 
        14312, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/battleship_terran_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[1777], 
        14313, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/battleship_terran_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[1778], 
        14315, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/battleship_terran_lights_add.aem",
                                                65535, false));
    ADD_RES(resources[1779], 
        14316, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/battleship_terran_lights_emissive.aem", 65535, false));
    ADD_RES(resources[1780], 
        18304, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/battleship_terran_explosion_anim.aem",
                                                0, false));
    ADD_RES(resources[1781], 
        17060, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_002_nivelian.aem", 34811, false));
    ADD_RES(resources[1782], 
        17062, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_002_nivelian_lod_1.aem", 34811,
                                                false));
    ADD_RES(resources[1783], 
        17063, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_002_nivelian_lod_2.aem", 34811,
                                                false));
    ADD_RES(resources[1784], 
        17064, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_002_nivelian_engine_add.aem",
                                                34810, false));
    ADD_RES(resources[1785], 
        17061, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_002_nivelian_lights_add.aem",
                                                65535, false));
    ADD_RES(resources[1786], 
        18301, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/cargo_002_nivelian_explosion_anim.aem", 34811, false));
    ADD_RES(resources[1787], 
        17065, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_003_terran.aem", 34811, false));
    ADD_RES(resources[1788], 
        17066, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_003_terran_lod_1.aem", 34811,
                                                false));
    ADD_RES(resources[1789], 
        17067, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_003_terran_lod_2.aem", 34811,
                                                false));
    ADD_RES(resources[1790], 
        17069, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_003_terran_engine_add.aem", 3,
                                                false));
    ADD_RES(resources[1791], 
        17074, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_003_terran_lights_add.aem", 0,
                                                false));
    ADD_RES(resources[1792], 
        17070, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_003_terran_lights_emissive.aem",
                                                0, false));
    ADD_RES(resources[1793], 
        18302, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_003_terran_explosion_anim.aem",
                                                34811, false));
    ADD_RES(resources[1794], 
        17049, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_001_midorian.aem", 0, false));
    ADD_RES(resources[1795], 
        17050, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_001_midorian_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[1796], 
        17051, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_001_midorian_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[1797], 
        17052, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_001_midorian_container.aem", 0,
                                                false));
    ADD_RES(resources[1798], 
        17053, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/cargo_001_midorian_container_lod_1.aem", 0, false));
    ADD_RES(resources[1799], 
        17054, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_001_midorian_engine_add.aem",
                                                65535, false));
    ADD_RES(resources[1800], 17055, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/ships/cargo_001_midorian_lights_emissive.aem", 65535,
                                  false));
    ADD_RES(resources[1801], 
        18300, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/cargo_001_midorian_explosion_anim.aem", 0, false));
    ADD_RES(resources[1802], 
        17038, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_004_vossk_engine_add.aem",
                                                65535, false));
    ADD_RES(resources[1803], 
        18303, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_004_vossk_explosion_anim.aem",
                                                0, false));
    ADD_RES(resources[1804], 
        17139, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_004_vossk_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[1805], 
        17140, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_004_vossk_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[1806], 
        18713, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_004_vossk_lights_emissive.aem",
                                                65535, false));
    ADD_RES(resources[1807], 
        17013, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/cargo_004_vossk.aem", 0, false));
    ADD_RES(resources[1808], 14363, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_battlestation_turret.aem", 20135,
                                  false));
    ADD_RES(resources[1809], 14364, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_battlestation_turret_gun.aem",
                                  20135, false));
    ADD_RES(resources[1810], 14365, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_battlestation_shield.aem", 20135,
                                  false));
    ADD_RES(resources[1811], 14366, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_battlestation_laser_anim_add.aem",
                                  20158, false));
    ADD_RES(resources[1812], 
        6770, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/turrets/turret_001.aem", 24075, false));
    ADD_RES(resources[1813], 
        6771, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/turrets/turret_001_gun.aem", 24075, false));
    ADD_RES(resources[1814], 
        6772, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/turrets/turret_002.aem", 24076, false));
    ADD_RES(resources[1815], 
        6773, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/turrets/turret_002_gun.aem", 24076, false));
    ADD_RES(resources[1816], 
        6774, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/turrets/turret_003.aem", 24077, false));
    ADD_RES(resources[1817], 
        6775, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/turrets/turret_003_gun.aem", 24077, false));
    ADD_RES(resources[1818], 
        18842, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/turrets/sn_turret_004.aem", 24085,
                                                false));
    ADD_RES(resources[1819], 
        18843, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/turrets/sn_turret_004_gun.aem", 24085,
                                                false));
    ADD_RES(resources[1820], 
        18844, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/turrets/sn_turret_004_add.aem", 24086,
                                                false));
    ADD_RES(resources[1821], 
        18845, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/turrets/sn_turret_004_gun_add.aem",
                                                24086, false));
    ADD_RES(resources[1822], 
        6805, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/turrets/v_autoturret_001_anim.aem",
                                               24081, false));
    ADD_RES(resources[1823], 
        6806, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/turrets/v_autoturret_001_gun_anim.aem",
                                               24081, false));
    ADD_RES(resources[1824], 
        6807, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/turrets/v_autoturret_002.aem", 24082,
                                               false));
    ADD_RES(resources[1825], 
        6808, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/turrets/v_autoturret_002_gun.aem", 24082,
                                               false));
    ADD_RES(resources[1826], 
        6809, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/turrets/v_autoturret_003.aem", 24083,
                                               false));
    ADD_RES(resources[1827], 
        6810, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/turrets/v_autoturret_003_gun.aem", 24083,
                                               false));
    ADD_RES(resources[1828], 
        14050, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/misc/v_mine_001.aem", 20121, false));
    ADD_RES(resources[1829], 
        14051, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/misc/v_mine_001_add.aem", 20154,
                                                false));
    ADD_RES(resources[1830], 
        14054, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/misc/v_mine_002.aem", 20122, false));
    ADD_RES(resources[1831], 
        14055, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/misc/v_mine_002_add.aem", 20155,
                                                false));
    ADD_RES(resources[1832], 
        14052, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/misc/v_mine_003.aem", 20123, false));
    ADD_RES(resources[1833], 
        14053, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/misc/v_mine_003_add.aem", 20156,
                                                false));
    ADD_RES(resources[1834], 
        6779, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_orbit.aem", 20029, false));
    ADD_RES(resources[1835], 
        14288, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/beer.aem", 20111, false));
    ADD_RES(resources[1836], 
        14289, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/bra.aem", 20111, false));
    ADD_RES(resources[1837], 
        14290, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/scanner_probe.aem", 20113, false));
    ADD_RES(resources[1838], 
        14291, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/explosion_debris_anim_alpha.aem", 27253,
                                                false));
    ADD_RES(resources[1839], 
        14292, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/explosion_debris_anim_add.aem", 27252,
                                                false));
    ADD_RES(resources[1840], 
        14600, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/impact_000_lookat_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1841], 
        14601, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/impact_001_lookat_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1842], 
        14602, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/impact_002_lookat_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1843], 
        14603, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/impact_003_lookat_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1844], 
        14604, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/impact_004_lookat_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1845], 
        14605, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/impact_005_lookat_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1846], 
        14606, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/impact_006_lookat_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1847], 
        14500, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/muzzle_flash_000_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1848], 
        14501, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/muzzle_flash_001_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1849], 
        14502, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/muzzle_flash_002_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1850], 
        14503, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/muzzle_flash_003_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1851], 
        14504, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/muzzle_flash_004_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1852], 
        14505, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/muzzle_flash_005_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1853], 
        14506, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/muzzle_flash_006_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1854], 
        14507, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/muzzle_flash_007_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1855], 
        14508, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/muzzle_flash_008_anim_add.aem", 24201,
                                                false));
    ADD_RES(resources[1856], 
        14509, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/fx/v_muzzle_flash_009_anim_add.aem",
                                                24096, false));
    ADD_RES(resources[1857], 
        14510, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/fx/v_muzzle_flash_010_anim_add.aem",
                                                24096, false));
    ADD_RES(resources[1858], 
        18070, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27280, false));
    ADD_RES(resources[1859], 
        18071, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27281, false));
    ADD_RES(resources[1860], 
        18072, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27282, false));
    ADD_RES(resources[1861], 
        18073, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27283, false));
    ADD_RES(resources[1862], 
        18074, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27284, false));
    ADD_RES(resources[1863], 
        18075, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27285, false));
    ADD_RES(resources[1864], 
        18076, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27286, false));
    ADD_RES(resources[1865], 
        18077, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27287, false));
    ADD_RES(resources[1866], 
        18078, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27288, false));
    ADD_RES(resources[1867], 
        18079, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27289, false));
    ADD_RES(resources[1868], 
        18080, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27290, false));
    ADD_RES(resources[1869], 
        27281, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27281, false));
    ADD_RES(resources[1870], 
        18082, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27288, false));
    ADD_RES(resources[1872], 
        18084, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27280, false));
    ADD_RES(resources[1873], 
        18085, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27285, false));
    ADD_RES(resources[1875], 
        18087, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27282, false));
    ADD_RES(resources[1876], 
        27286, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/plane.aem", 27286, false));
    ADD_RES(resources[1878], 
        18181, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_001.aem", 20027, false));
    ADD_RES(resources[1879], 
        18182, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_002.aem", 20027, false));
    ADD_RES(resources[1880], 
        20027, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_003.aem", 20027, false));
    ADD_RES(resources[1881], 
        18184, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_004.aem", 20027, false));
    ADD_RES(resources[1882], 
        20027, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_005.aem", 20027, false));
    ADD_RES(resources[1884], 
        18187, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_007.aem", 20027, false));
    ADD_RES(resources[1885], 
        20027, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_008.aem", 20027, false));
    ADD_RES(resources[1886], 
        18189, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_009.aem", 20027, false));
    ADD_RES(resources[1888], 
        18191, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_011.aem", 20027, false));
    ADD_RES(resources[1889], 
        18192, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_012.aem", 20027, false));
    ADD_RES(resources[1891], 
        18194, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_014.aem", 20027, false));
    ADD_RES(resources[1892], 
        18195, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_015.aem", 20027, false));
    ADD_RES(resources[1893], 
        18196, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_016.aem", 20027, false));
    ADD_RES(resources[1894], 
        18197, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_017.aem", 20027, false));
    ADD_RES(resources[1895], 
        18198, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_018.aem", 20027, false));
    ADD_RES(resources[1896], 
        18199, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_019.aem", 20027, false));
    ADD_RES(resources[1897], 
        18200, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/galaxymap/v_map_planet_020.aem", 27305,
                                                false));
    ADD_RES(resources[1898], 
        18201, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/galaxymap/v_map_planet_021.aem", 27305,
                                                false));
    ADD_RES(resources[1899], 
        18202, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/galaxymap/v_map_planet_022.aem", 27305,
                                                false));
    ADD_RES(resources[1900], 
        18204, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/galaxymap/sn_map_planet_024.aem",
                                                27306, false));
    ADD_RES(resources[1901], 
        18205, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/galaxymap/sn_map_planet_025.aem",
                                                27306, false));
    ADD_RES(resources[1902], 
        18206, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/galaxymap/sn_map_planet_026.aem",
                                                27306, false));
    ADD_RES(resources[1903], 
        17900, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/ship_000_midorian_engine_glow_add.aem", 34811, false));
    ADD_RES(resources[1904], 
        17901, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_001_terran_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1905], 
        17902, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_002_pirates_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1906], 
        17903, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/ship_003_midorian_engine_glow_add.aem", 34811, false));
    ADD_RES(resources[1907], 
        17904, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/ship_004_nivelian_engine_glow_add.aem", 34811, false));
    ADD_RES(resources[1908], 
        17905, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_005_terran_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1909], 
        17906, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/ship_006_midorian_engine_glow_add.aem", 34811, false));
    ADD_RES(resources[1910], 
        17907, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_007_terran_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1911], 
        17908, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_008_void_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1912], 
        17909, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_009_vossk_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1913], 
        17910, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_010_terran_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1914], 
        17911, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_011_pirates_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1915], 
        17912, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/ship_012_nivelian_engine_glow_add.aem", 34811, false));
    ADD_RES(resources[1916], 
        17916, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/ship_016_nivelian_engine_glow_add.aem", 34811, false));
    ADD_RES(resources[1917], 
        17917, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_017_terran_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1918], 
        17918, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/ship_018_nivelian_engine_glow_add.aem", 34811, false));
    ADD_RES(resources[1919], 
        17919, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/ship_019_midorian_engine_glow_add.aem", 34811, false));
    ADD_RES(resources[1920], 
        17920, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/ship_020_midorian_engine_glow_add.aem", 34811, false));
    ADD_RES(resources[1921], 
        17921, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/ship_021_nivelian_engine_glow_add.aem", 34811, false));
    ADD_RES(resources[1922], 
        17922, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_022_terran_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1923], 
        17923, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_023_pirates_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1924], 
        17924, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_024_pirates_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1925], 
        17925, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_025_pirates_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1926], 
        17926, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_026_terran_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1927], 
        17927, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_027_terran_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1928], 
        17928, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_028_terran_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1929], 
        17929, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_029_pirates_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1930], 
        17930, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/ship_030_midorian_engine_glow_add.aem", 34811, false));
    ADD_RES(resources[1931], 
        17931, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/ship_031_nivelian_engine_glow_add.aem", 34811, false));
    ADD_RES(resources[1932], 
        17932, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_032_pirates_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1933], 
        17933, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_033_terran_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1934], 
        17934, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_034_terran_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1935], 
        17935, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/ships/ship_035_nivelian_engine_glow_add.aem", 34811, false));
    ADD_RES(resources[1936], 
        17936, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_036_terran_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1937], 17937, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/ships/v_ship_037_deep_science_engine_glow_add.aem", 0,
                                  false));
    ADD_RES(resources[1938], 17938, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/ships/v_ship_038_deep_science_engine_glow_add.aem", 0,
                                  false));
    ADD_RES(resources[1939], 17940, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/ships/v_ship_040_deep_science_engine_glow_add.aem", 0,
                                  false));
    ADD_RES(resources[1940], 
        17942, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_042_retro_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1941], 
        17943, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_043_retro_engine_glow_add.aem",
                                                34811, false));
    ADD_RES(resources[1942], 17944, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_044_elite_nivelian_engine_glow_add.aem",
                                  32544, false));
    ADD_RES(resources[1943], 17945, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_045_most_wanted_engine_glow_add.aem",
                                  32545, false));
    ADD_RES(resources[1944], 17946, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_046_most_wanted_engine_glow_add.aem",
                                  32546, false));
    ADD_RES(resources[1945], 17947, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_047_most_wanted_engine_glow_add.aem",
                                  32547, false));
    ADD_RES(resources[1946], 17948, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_048_most_wanted_engine_glow_add.aem",
                                  32548, false));
    ADD_RES(resources[1947], 17949, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_049_boss_nivelian_engine_glow_add.aem",
                                  32549, false));
    ADD_RES(resources[1948], 17951, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_051_dropship_terran_engine_glow_add.aem",
                                  32551, false));
    ADD_RES(resources[1949], 17952, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_052_retro_engine_glow_add.aem", 32552,
                                  false));
    ADD_RES(resources[1950], 17954, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_054_vossk_engine_glow_add.aem", 32554,
                                  false));
    ADD_RES(resources[1951], 17955, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_055_modified_engine_glow_add.aem",
                                  34811, false));
    ADD_RES(resources[1952], 17956, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_056_modified_engine_glow_add.aem",
                                  34811, false));
    ADD_RES(resources[1953], 17957, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_057_modified_engine_glow_add.aem",
                                  34811, false));
    ADD_RES(resources[1954], 17958, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_058_modified_engine_glow_add.aem",
                                  34811, false));
    ADD_RES(resources[1955], 17959, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_059_modified_engine_glow_add.aem",
                                  34811, false));
    ADD_RES(resources[1956], 17960, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/ships/sn_ship_060_modified_engine_glow_add.aem",
                                  34811, false));
    ADD_RES(resources[1957], 17961, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/ships/ship_061_elite_nivelian_prototype_engine_glow_add.aem",
                                  32561, false));
    ADD_RES(resources[1958], 17962, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/ships/ship_062_prototype_engine_glow_add.aem", 32562,
                                  false));
    ADD_RES(resources[1959], 17963, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/ships/ship_063_vossk_prototype_engine_glow_add.aem",
                                  32563, false));
    ADD_RES(resources[1960], 
        17000, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_000_midorian.aem", 0, false));
    ADD_RES(resources[1961], 
        17100, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_000_midorian_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[1962], 
        17101, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_000_midorian_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[1963], 
        18000, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_000_midorian_engine_add.aem",
                                                34600, false));
    ADD_RES(resources[1964], 
        18700, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_000_midorian_lights_add.aem",
                                                34600, false));
    ADD_RES(resources[1965], 
        17001, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_001_terran.aem", 0, false));
    ADD_RES(resources[1966], 
        17103, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_001_terran_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[1967], 
        17104, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_001_terran_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[1968], 
        18001, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_001_terran_engine_add.aem", 3,
                                                false));
    ADD_RES(resources[1969], 
        18701, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_001_terran_lights_add.aem", 3,
                                                false));
    ADD_RES(resources[1970], 
        17002, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_002_pirates.aem", 0, false));
    ADD_RES(resources[1971], 
        17106, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_002_pirates_lod_1.aem", 0, false));
    ADD_RES(resources[1972], 
        17107, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_002_pirates_lod_2.aem", 0, false));
    ADD_RES(resources[1973], 
        18002, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_002_pirates_engine_add.aem",
                                                34811, false));
    ADD_RES(resources[1974], 
        18702, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_002_pirates_lights_add.aem",
                                                34811, false));
    ADD_RES(resources[1975], 
        17003, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_003_midorian.aem", 0, false));
    ADD_RES(resources[1976], 
        17109, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_003_midorian_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[1977], 
        17110, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_003_midorian_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[1978], 
        18003, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_003_midorian_engine_add.aem",
                                                34600, false));
    ADD_RES(resources[1979], 
        18703, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_003_midorian_lights_add.aem",
                                                34600, false));
    ADD_RES(resources[1980], 
        17004, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_004_nivelian.aem", 0, false));
    ADD_RES(resources[1981], 
        17112, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_004_nivelian_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[1982], 
        17113, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_004_nivelian_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[1983], 
        18004, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_004_nivelian_engine_add.aem",
                                                34810, false));
    ADD_RES(resources[1984], 
        18704, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_004_nivelian_lights_add.aem",
                                                34810, false));
    ADD_RES(resources[1985], 
        17005, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_005_terran.aem", 0, false));
    ADD_RES(resources[1986], 
        17115, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_005_terran_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[1987], 
        17116, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_005_terran_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[1988], 
        18005, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_005_terran_engine_add.aem", 3,
                                                false));
    ADD_RES(resources[1989], 
        18705, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_005_terran_lights_add.aem", 3,
                                                false));
    ADD_RES(resources[1990], 
        17006, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_006_midorian.aem", 0, false));
    ADD_RES(resources[1991], 
        17118, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_006_midorian_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[1992], 
        17119, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_006_midorian_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[1993], 
        18006, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_006_midorian_engine_add.aem",
                                                34600, false));
    ADD_RES(resources[1994], 
        18706, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_006_midorian_lights_add.aem",
                                                34600, false));
    ADD_RES(resources[1995], 
        17007, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_007_terran.aem", 0, false));
    ADD_RES(resources[1996], 
        17121, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_007_terran_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[1997], 
        17122, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_007_terran_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[1998], 
        18007, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_007_terran_engine_add.aem", 3,
                                                false));
    ADD_RES(resources[1999], 
        18707, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_007_terran_lights_add.aem", 3,
                                                false));
    ADD_RES(resources[2000], 
        17008, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_008_void.aem", 0, false));
    ADD_RES(resources[2001], 
        17124, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_008_void_lod_1.aem", 0, false));
    ADD_RES(resources[2002], 
        17125, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_008_void_lod_2.aem", 0, false));
    ADD_RES(resources[2003], 
        18008, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_008_void_engine_add.aem", 0,
                                                false));
    ADD_RES(resources[2004], 
        18708, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_008_void_lights_add.aem", 0,
                                                false));
    ADD_RES(resources[2005], 
        17009, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_009_vossk.aem", 0, false));
    ADD_RES(resources[2006], 
        17127, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_009_vossk_lod_1.aem", 0, false));
    ADD_RES(resources[2007], 
        17128, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_009_vossk_lod_2.aem", 0, false));
    ADD_RES(resources[2008], 
        18009, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_009_vossk_engine_add.aem", 65535,
                                                false));
    ADD_RES(resources[2009], 
        18709, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_009_vossk_lights_add.aem", 65535,
                                                false));
    ADD_RES(resources[2010], 
        17010, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_010_terran.aem", 0, false));
    ADD_RES(resources[2011], 
        17130, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_010_terran_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2012], 
        17131, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_010_terran_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2013], 
        18010, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_010_terran_engine_add.aem", 3,
                                                false));
    ADD_RES(resources[2014], 
        18710, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_010_terran_lights_add.aem", 3,
                                                false));
    ADD_RES(resources[2015], 
        17011, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_011_pirates.aem", 0, false));
    ADD_RES(resources[2016], 
        17133, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_011_pirates_lod_1.aem", 0, false));
    ADD_RES(resources[2017], 
        17134, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_011_pirates_lod_2.aem", 0, false));
    ADD_RES(resources[2018], 
        18011, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_011_pirates_engine_add.aem",
                                                34811, false));
    ADD_RES(resources[2019], 
        18711, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_011_pirates_lights_add.aem",
                                                34811, false));
    ADD_RES(resources[2020], 
        17012, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_012_nivelian.aem", 0, false));
    ADD_RES(resources[2021], 
        17136, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_012_nivelian_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2022], 
        17137, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_012_nivelian_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2023], 
        18012, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_012_nivelian_engine_add.aem",
                                                34810, false));
    ADD_RES(resources[2024], 
        18712, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_012_nivelian_lights_add.aem",
                                                34810, false));
    ADD_RES(resources[2025], 
        17016, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_016_nivelian.aem", 0, false));
    ADD_RES(resources[2026], 
        17148, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_016_nivelian_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2027], 
        17149, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_016_nivelian_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2028], 
        18016, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_016_nivelian_engine_add.aem",
                                                34810, false));
    ADD_RES(resources[2029], 
        18716, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_016_nivelian_lights_add.aem",
                                                34810, false));
    ADD_RES(resources[2030], 
        17017, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_017_terran.aem", 0, false));
    ADD_RES(resources[2031], 
        17151, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_017_terran_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2032], 
        17152, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_017_terran_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2033], 
        18017, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_017_terran_engine_add.aem", 3,
                                                false));
    ADD_RES(resources[2034], 
        18717, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_017_terran_lights_add.aem", 3,
                                                false));
    ADD_RES(resources[2035], 
        17018, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_018_nivelian.aem", 0, false));
    ADD_RES(resources[2036], 
        17154, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_018_nivelian_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2037], 
        17155, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_018_nivelian_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2038], 
        18018, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_018_nivelian_engine_add.aem",
                                                34810, false));
    ADD_RES(resources[2039], 
        18718, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_018_nivelian_lights_add.aem",
                                                34810, false));
    ADD_RES(resources[2040], 
        17019, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_019_midorian.aem", 0, false));
    ADD_RES(resources[2041], 
        17157, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_019_midorian_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2042], 
        17158, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_019_midorian_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2043], 
        18019, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_019_midorian_engine_add.aem",
                                                34600, false));
    ADD_RES(resources[2044], 
        18719, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_019_midorian_lights_add.aem",
                                                34600, false));
    ADD_RES(resources[2045], 
        17020, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_020_midorian.aem", 0, false));
    ADD_RES(resources[2046], 
        17160, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_020_midorian_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2047], 
        17161, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_020_midorian_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2048], 
        18020, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_020_midorian_engine_add.aem",
                                                34600, false));
    ADD_RES(resources[2049], 
        18720, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_020_midorian_lights_add.aem",
                                                34600, false));
    ADD_RES(resources[2050], 
        17021, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_021_nivelian.aem", 0, false));
    ADD_RES(resources[2051], 
        17163, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_021_nivelian_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2052], 
        17164, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_021_nivelian_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2053], 
        18021, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_021_nivelian_engine_add.aem",
                                                34810, false));
    ADD_RES(resources[2054], 
        18721, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_021_nivelian_lights_add.aem",
                                                34810, false));
    ADD_RES(resources[2055], 
        17022, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_022_terran.aem", 0, false));
    ADD_RES(resources[2056], 
        17166, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_022_terran_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2057], 
        17167, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_022_terran_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2058], 
        18022, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_022_terran_engine_add.aem", 3,
                                                false));
    ADD_RES(resources[2059], 
        18722, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_022_terran_lights_add.aem", 3,
                                                false));
    ADD_RES(resources[2060], 
        17023, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_023_pirates.aem", 0, false));
    ADD_RES(resources[2061], 
        17169, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_023_pirates_lod_1.aem", 0, false));
    ADD_RES(resources[2062], 
        17170, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_023_pirates_lod_2.aem", 0, false));
    ADD_RES(resources[2063], 
        18023, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_023_pirates_engine_add.aem",
                                                34811, false));
    ADD_RES(resources[2064], 
        18723, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_023_pirates_lights_add.aem",
                                                34811, false));
    ADD_RES(resources[2065], 
        17024, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_024_pirates.aem", 0, false));
    ADD_RES(resources[2066], 
        17172, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_024_pirates_lod_1.aem", 0, false));
    ADD_RES(resources[2067], 
        17173, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_024_pirates_lod_2.aem", 0, false));
    ADD_RES(resources[2068], 
        18024, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_024_pirates_engine_add.aem",
                                                34811, false));
    ADD_RES(resources[2069], 
        18724, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_024_pirates_lights_add.aem",
                                                34811, false));
    ADD_RES(resources[2070], 
        17025, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_025_pirates.aem", 0, false));
    ADD_RES(resources[2071], 
        17175, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_025_pirates_lod_1.aem", 0, false));
    ADD_RES(resources[2072], 
        17176, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_025_pirates_lod_2.aem", 0, false));
    ADD_RES(resources[2073], 
        18025, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_025_pirates_engine_add.aem",
                                                34811, false));
    ADD_RES(resources[2074], 
        18725, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_025_pirates_lights_add.aem",
                                                34811, false));
    ADD_RES(resources[2075], 
        17026, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_026_terran.aem", 0, false));
    ADD_RES(resources[2076], 
        17178, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_026_terran_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2077], 
        17179, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_026_terran_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2078], 
        18026, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_026_terran_engine_add.aem", 3,
                                                false));
    ADD_RES(resources[2079], 
        18726, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_026_terran_lights_add.aem", 3,
                                                false));
    ADD_RES(resources[2080], 
        17027, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_027_terran.aem", 0, false));
    ADD_RES(resources[2081], 
        17181, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_027_terran_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2082], 
        17182, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_027_terran_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2083], 
        18027, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_027_terran_engine_add.aem", 3,
                                                false));
    ADD_RES(resources[2084], 
        18727, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_027_terran_lights_add.aem", 3,
                                                false));
    ADD_RES(resources[2085], 
        17028, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_028_terran.aem", 0, false));
    ADD_RES(resources[2086], 
        17184, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_028_terran_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2087], 
        17185, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_028_terran_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2088], 
        18028, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_028_terran_engine_add.aem", 3,
                                                false));
    ADD_RES(resources[2089], 
        18728, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_028_terran_lights_add.aem", 3,
                                                false));
    ADD_RES(resources[2090], 
        17029, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_029_pirates.aem", 0, false));
    ADD_RES(resources[2091], 
        17187, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_029_pirates_lod_1.aem", 0, false));
    ADD_RES(resources[2092], 
        17188, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_029_pirates_lod_2.aem", 0, false));
    ADD_RES(resources[2093], 
        18029, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_029_pirates_engine_add.aem",
                                                34811, false));
    ADD_RES(resources[2094], 
        18729, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_029_pirates_lights_add.aem",
                                                34811, false));
    ADD_RES(resources[2095], 
        17030, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_030_midorian.aem", 0, false));
    ADD_RES(resources[2096], 
        17190, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_030_midorian_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2097], 
        17191, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_030_midorian_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2098], 
        18030, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_030_midorian_engine_add.aem",
                                                34600, false));
    ADD_RES(resources[2099], 
        18730, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_030_midorian_lights_add.aem",
                                                34600, false));
    ADD_RES(resources[2100], 
        17031, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_031_nivelian.aem", 0, false));
    ADD_RES(resources[2101], 
        17193, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_031_nivelian_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2102], 
        17194, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_031_nivelian_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2103], 
        18031, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_031_nivelian_engine_add.aem",
                                                34810, false));
    ADD_RES(resources[2104], 
        18731, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_031_nivelian_lights_add.aem",
                                                34810, false));
    ADD_RES(resources[2105], 
        17032, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_032_pirates.aem", 0, false));
    ADD_RES(resources[2106], 
        17196, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_032_pirates_lod_1.aem", 0, false));
    ADD_RES(resources[2107], 
        17197, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_032_pirates_lod_2.aem", 0, false));
    ADD_RES(resources[2108], 
        18032, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_032_pirates_engine_add.aem",
                                                34811, false));
    ADD_RES(resources[2109], 
        18732, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_032_pirates_lights_add.aem",
                                                34811, false));
    ADD_RES(resources[2110], 
        17033, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_033_terran.aem", 0, false));
    ADD_RES(resources[2111], 
        17199, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_033_terran_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2112], 
        17200, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_033_terran_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2113], 
        18033, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_033_terran_engine_add.aem", 3,
                                                false));
    ADD_RES(resources[2114], 
        18733, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_033_terran_lights_add.aem", 3,
                                                false));
    ADD_RES(resources[2115], 
        17034, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_034_terran.aem", 0, false));
    ADD_RES(resources[2116], 
        17202, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_034_terran_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2117], 
        17203, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_034_terran_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2118], 
        18034, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_034_terran_engine_add.aem", 3,
                                                false));
    ADD_RES(resources[2119], 
        18734, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_034_terran_lights_add.aem", 3,
                                                false));
    ADD_RES(resources[2120], 
        17035, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_035_nivelian.aem", 0, false));
    ADD_RES(resources[2121], 
        17205, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_035_nivelian_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2122], 
        17206, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_035_nivelian_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2123], 
        18035, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_035_nivelian_engine_add.aem",
                                                34810, false));
    ADD_RES(resources[2124], 
        18735, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_035_nivelian_lights_add.aem",
                                                34810, false));
    ADD_RES(resources[2125], 
        17036, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_036_terran.aem", 0, false));
    ADD_RES(resources[2126], 
        17208, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_036_terran_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2127], 
        17209, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_036_terran_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2128], 
        18036, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_036_terran_engine_add.aem", 3,
                                                false));
    ADD_RES(resources[2129], 
        18736, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_036_terran_lights_add.aem", 3,
                                                false));
    ADD_RES(resources[2130], 
        17211, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/ships/v_ship_037_deep_science.aem",
                                                65535, false));
    ADD_RES(resources[2131], 17215, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/ships/v_ship_037_deep_science_lights_anim_add.aem",
                                  32537, false));
    ADD_RES(resources[2132], 18037, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/ships/v_ship_037_deep_science_engine_add.aem", 32537,
                                  false));
    ADD_RES(resources[2133], 
        17216, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/ships/v_ship_038_deep_science.aem",
                                                65535, false));
    ADD_RES(resources[2134], 18738, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/ships/v_ship_038_deep_science_lights_anim_add.aem",
                                  32538, false));
    ADD_RES(resources[2135], 18038, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/ships/v_ship_038_deep_science_engine_add.aem", 32538,
                                  false));
    ADD_RES(resources[2136], 
        17221, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/ships/v_ship_039_vossk.aem", 0, false));
    ADD_RES(resources[2137], 
        17222, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/ships/v_ship_039_vossk_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2138], 
        17223, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/ships/v_ship_039_vossk_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2139], 
        18739, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/ships/v_ship_039_vossk_lights_add.aem",
                                                32539, false));
    ADD_RES(resources[2140], 
        18039, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/ships/v_ship_039_vossk_engine_add.aem",
                                                32539, false));
    ADD_RES(resources[2141], 
        17227, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/ships/v_ship_040_deep_science.aem",
                                                65535, false));
    ADD_RES(resources[2142], 17231, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/ships/v_ship_040_deep_science_lights_add.aem", 32540,
                                  false));
    ADD_RES(resources[2143], 18040, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/ships/v_ship_040_deep_science_engine_add.aem", 32540,
                                  false));
    ADD_RES(resources[2144], 
        17233, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/ships/v_ship_041_vossk.aem", 0, false));
    ADD_RES(resources[2145], 
        17234, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/ships/v_ship_041_vossk_lod_1.aem", 0,
                                                false));
    ADD_RES(resources[2146], 
        17235, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/ships/v_ship_041_vossk_lod_2.aem", 0,
                                                false));
    ADD_RES(resources[2147], 
        18741, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/ships/v_ship_041_vossk_lights_add.aem",
                                                32541, false));
    ADD_RES(resources[2148], 
        18041, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/ships/v_ship_041_vossk_engine_add.aem",
                                                32541, false));
    ADD_RES(resources[2149], 
        17239, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_042_retro.aem", 0, false));
    ADD_RES(resources[2150], 
        17240, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_042_retro_lod_1.aem", 0, false));
    ADD_RES(resources[2151], 
        17241, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_042_retro_lod_2.aem", 0, false));
    ADD_RES(resources[2152], 
        17243, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_042_retro_lights_add.aem", 32542,
                                                false));
    ADD_RES(resources[2153], 
        17245, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_043_retro.aem", 0, false));
    ADD_RES(resources[2154], 
        17246, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_043_retro_lod_1.aem", 0, false));
    ADD_RES(resources[2155], 
        17247, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_043_retro_lod_2.aem", 0, false));
    ADD_RES(resources[2156], 
        17249, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/ships/ship_043_retro_lights_add.aem", 32543,
                                                false));
    ADD_RES(resources[2157], 16800, 4, new AbyssEngine::ResourceMesh("data/meshes/col_box.aem", 20087, false));
    ADD_RES(resources[2158], 16801, 4, new AbyssEngine::ResourceMesh("data/meshes/col_sphere.aem", 20087, false));
    ADD_RES(resources[2159], 
        14243, 4,
        new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_pirates.aem", 20104, false));
    ADD_RES(resources[2160], 
        14244, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_pirates_emissive.aem",
                                                20105, false));
    ADD_RES(resources[2161], 
        14245, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_pirates_lights_add.aem",
                                                34811, false));
    ADD_RES(resources[2162], 
        14246, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/stations/station_pirates_explosion_anim.aem", 20104, false));
    ADD_RES(resources[2163], 
        16851, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_fog_layer_0.aem", 20025,
                                                false));
    ADD_RES(resources[2164], 
        16852, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_fog_layer_1.aem", 20026,
                                                false));
    ADD_RES(resources[2165], 
        16850, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_bg_plane.aem", 20024, false));
    ADD_RES(resources[2166], 
        16853, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/galaxymap/map_planet_ring.aem", 20030,
                                                false));
    ADD_RES(resources[2167], 
        16904, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/misc/v_asteroid_ice.aem", 20134,
                                                false));
    ADD_RES(resources[2168], 
        16905, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/misc/v_asteroid_ice_lod_1.aem", 20134,
                                                false));
    ADD_RES(resources[2169], 
        16906, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/misc/v_asteroid_ice_lod_2.aem", 20134,
                                                false));
    ADD_RES(resources[2170], 
        16907, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/misc/v_asteroid_ice_lod_3.aem", 20134,
                                                false));
    ADD_RES(resources[2171], 
        16908, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/misc/v_asteroid_ice_explosion_anim.aem",
                                                20134, false));
    ADD_RES(resources[2172], 16909, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/misc/v_asteroid_ice_explosion_anim_alpha.aem", 20133,
                                  false));
    ADD_RES(resources[2173], 
        16900, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/asteroid_01.aem", 20023, false));
    ADD_RES(resources[2174], 
        16901, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/asteroid_01_lod_1.aem", 20023, false));
    ADD_RES(resources[2175], 
        16902, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/asteroid_01_lod_2.aem", 20023, false));
    ADD_RES(resources[2176], 
        16903, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/asteroid_01_lod_3.aem", 20023, false));
    ADD_RES(resources[2177], 
        16913, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/asteroid_01_explosion_anim.aem", 20023,
                                                false));
    ADD_RES(resources[2178], 
        16915, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/misc/asteroid_explosion_anim_alpha.aem",
                                                20051, false));
    ADD_RES(resources[2179], 
        18836, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_asteroid_magma.aem", 27312,
                                                false));
    ADD_RES(resources[2180], 
        18837, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_asteroid_magma_lod_1.aem",
                                                27312, false));
    ADD_RES(resources[2181], 
        18838, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_asteroid_magma_lod_2.aem",
                                                27312, false));
    ADD_RES(resources[2182], 
        18839, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/misc/sn_asteroid_magma_lod_3.aem",
                                                27312, false));
    ADD_RES(resources[2183], 18840, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/misc/sn_asteroid_magma_explosion_anim.aem", 27312,
                                  false));
    ADD_RES(resources[2184], 18841, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/misc/sn_asteroid_magma_explosion_anim_alpha.aem",
                                  27313, false));
    ADD_RES(resources[2185], 16928, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_battlestation_anim.aem", 20135,
                                  false));
    ADD_RES(resources[2186], 16929, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_battlestation_anim_emissive.aem",
                                  20136, false));
    ADD_RES(resources[2187], 16930, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_battlestation_anim_add.aem", 20158,
                                  false));
    ADD_RES(resources[2188], 
        16931, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/stations/v_station_deep_science.aem",
                                                27145, false));
    ADD_RES(resources[2189], 16932, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_deep_science_emissive.aem", 27146,
                                  false));
    ADD_RES(resources[2190], 
        16933, 4, new AbyssEngine::ResourceMesh(
            "data/assets/valkyrie/3d/meshes/stations/v_station_deep_science_add.aem", 27147, false));
    ADD_RES(resources[2191], 16936, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_deep_science_emitters_anim_add.aem",
                                  0, false));
    ADD_RES(resources[2192], 16934, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_deep_science_lod_1.aem", 27145,
                                  false));
    ADD_RES(resources[2193], 16935, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_deep_science_lod_1_emissive.aem",
                                  27146, false));
    ADD_RES(resources[2194], 14367, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_deep_science_explosion_anim.aem",
                                  27145, false));
    ADD_RES(resources[2195], 14368, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_deep_science_explosion_emissive.aem",
                                  27146, false));
    ADD_RES(resources[2196], 14369, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_deep_science_explosion_anim_add.aem",
                                  27147, false));
    ADD_RES(resources[2197], 14370, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_deep_science_explosion_laser_anim_add.aem",
                                  27147, false));
    ADD_RES(resources[2198], 16937, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_deep_science_explosion_emitters_anim_add.aem",
                                  0, false));
    ADD_RES(resources[2199], 14371, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_deep_science_damaged.aem", 27145,
                                  false));
    ADD_RES(resources[2200], 14372, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_deep_science_damaged_emissive.aem",
                                  27146, false));
    ADD_RES(resources[2201], 14373, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_deep_science_damaged_add.aem",
                                  27147, false));
    ADD_RES(resources[2202], 16938, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_deep_science_damaged_emitters_anim_add.aem",
                                  0, false));
    ADD_RES(resources[2203], 
        14374, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/fx/v_shield.aem", 27150, false));
    ADD_RES(resources[2204], 
        14375, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/skyboxes/skybox_asteroid_belt_alpha.aem",
                                                27160, false));
    ADD_RES(resources[2205], 
        21108, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_kaamo_club.aem", 20007,
                                                false));
    ADD_RES(resources[2206], 
        21908, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_kaamo_club_emissive.aem",
                                                20008, false));
    ADD_RES(resources[2207], 
        22900, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_kaamo_club_fx_anim.aem",
                                                27162, false));
    ADD_RES(resources[2208], 22901, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_kaamo_club_fx_anim_emissive.aem", 27163,
                                  false));
    ADD_RES(resources[2209], 
        22902, 4, new AbyssEngine::ResourceMesh(
            "data/assets/main/3d/meshes/stations/station_kaamo_club_fx_anim_add.aem", 27164, false));
    ADD_RES(resources[2210], 22903, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_kaamo_club_fx_anim_alpha.aem", 27165,
                                  false));
    ADD_RES(resources[2211], 19080, 4, new AbyssEngine::ResourceMesh("data/meshes/test_dock.aem", 20004, false));
    ADD_RES(resources[2212], 18830, 4, new AbyssEngine::ResourceMesh("data/meshes/carrier.aem", 20034, false));
    ADD_RES(resources[2213], 
        18834, 4, new AbyssEngine::ResourceMesh("data/meshes/gas_cloud_a_anim_lookat_add.aem", 27311, false));
    ADD_RES(resources[2214], 
        18835, 4, new AbyssEngine::ResourceMesh("data/meshes/gas_cloud_b_anim_lookat_add.aem", 27311, false));
    ADD_RES(resources[2215], 
        21000, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_000_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2216], 
        21001, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_001_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2217], 
        21002, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_002_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2218], 
        21003, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_003_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2219], 
        21004, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_004_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2220], 
        21005, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_005_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2221], 
        21006, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_006_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2222], 
        21007, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_007_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2223], 
        21008, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_008_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2224], 
        21009, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_009_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2225], 
        21010, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_010_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2226], 
        21020, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_020_midorian.aem", 20007,
                                                false));
    ADD_RES(resources[2227], 
        21021, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_021_midorian.aem", 20007,
                                                false));
    ADD_RES(resources[2228], 
        21022, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_022_midorian.aem", 20007,
                                                false));
    ADD_RES(resources[2229], 
        21023, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_023_midorian.aem", 20007,
                                                false));
    ADD_RES(resources[2230], 
        21024, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_024_midorian.aem", 20007,
                                                false));
    ADD_RES(resources[2231], 
        21030, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_030_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2232], 
        21031, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_031_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2233], 
        21032, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_032_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2234], 
        21033, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_033_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2235], 
        21035, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_035_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2236], 
        21036, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_036_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2237], 
        21037, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_037_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2238], 
        21038, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_038_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2239], 
        21039, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_039_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2240], 
        21040, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_040_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2241], 
        21041, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_041_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2242], 
        21042, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_042_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2243], 
        21043, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_043_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2244], 
        21044, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_044_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2245], 
        21045, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_045_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2246], 
        21046, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_046_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2247], 
        21047, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_047_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2248], 
        21048, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_048_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2249], 
        21049, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_049_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2250], 
        21055, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_055_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2251], 
        21056, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_056_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2252], 
        21057, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_057_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2253], 
        21065, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_065_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2254], 
        21066, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_066_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2255], 
        21067, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_067_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2256], 
        21068, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_068_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2257], 
        21069, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_069_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2258], 
        21070, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_070_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2259], 
        21071, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_071_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2260], 
        21072, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_072_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2261], 
        21073, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_073_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2262], 
        21074, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_074_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2263], 
        21075, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_075_midorian.aem", 20007,
                                                false));
    ADD_RES(resources[2264], 
        21076, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_076_midorian.aem", 20007,
                                                false));
    ADD_RES(resources[2265], 
        21077, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_077_midorian.aem", 20007,
                                                false));
    ADD_RES(resources[2266], 
        21078, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_078_midorian.aem", 20007,
                                                false));
    ADD_RES(resources[2267], 
        21079, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_079_midorian.aem", 20007,
                                                false));
    ADD_RES(resources[2268], 
        21080, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_080_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2269], 
        21081, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_081_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2270], 
        21082, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_082_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2271], 
        21083, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_083_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2272], 
        21084, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_084_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2273], 
        21085, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_085_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2274], 
        21086, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_086_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2275], 
        21087, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_087_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2276], 
        21088, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_088_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2277], 
        21089, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_089_nivelian.aem", 20005,
                                                false));
    ADD_RES(resources[2278], 
        21090, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_090_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2279], 
        21091, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_091_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2280], 
        21092, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_092_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2281], 
        21093, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_093_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2282], 
        21094, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_094_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2283], 
        21095, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_095_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2284], 
        21096, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_096_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2285], 
        21097, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_097_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2286], 
        21098, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_098_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2287], 
        21099, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_099_terran.aem", 20004,
                                                false));
    ADD_RES(resources[2288], 
        21105, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/stations/v_station_105_loma.aem", 20007,
                                                false));
    ADD_RES(resources[2289], 
        21106, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/stations/v_station_106_loma.aem", 20007,
                                                false));
    ADD_RES(resources[2290], 
        21107, 4, new AbyssEngine::ResourceMesh("data/assets/valkyrie/3d/meshes/stations/v_station_107_loma.aem", 20007,
                                                false));
    ADD_RES(resources[2291], 
        21112, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/stations/sn_station_112_midorian.aem",
                                                20007, false));
    ADD_RES(resources[2292], 
        21113, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/stations/sn_station_113_midorian.aem",
                                                20007, false));
    ADD_RES(resources[2293], 
        21114, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/stations/sn_station_114_midorian.aem",
                                                20007, false));
    ADD_RES(resources[2294], 
        21115, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/stations/sn_station_115_midorian.aem",
                                                20007, false));
    ADD_RES(resources[2295], 
        21116, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/stations/sn_station_116_midorian.aem",
                                                20007, false));
    ADD_RES(resources[2296], 
        21117, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/stations/sn_station_117_midorian.aem",
                                                20007, false));
    ADD_RES(resources[2297], 
        21118, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/stations/sn_station_118_midorian.aem",
                                                20007, false));
    ADD_RES(resources[2298], 
        21119, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/stations/sn_station_119_nivelian.aem",
                                                20005, false));
    ADD_RES(resources[2299], 
        21120, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/stations/sn_station_120_nivelian.aem",
                                                20005, false));
    ADD_RES(resources[2300], 
        21121, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/stations/sn_station_121_nivelian.aem",
                                                20005, false));
    ADD_RES(resources[2301], 
        21122, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/stations/sn_station_122_nivelian.aem",
                                                20005, false));
    ADD_RES(resources[2302], 
        21123, 4, new AbyssEngine::ResourceMesh("data/assets/supernova/3d/meshes/stations/sn_station_123_nivelian.aem",
                                                20005, false));
    ADD_RES(resources[2303], 
        21800, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_000_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2304], 
        21801, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_001_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2305], 
        21802, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_002_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2306], 
        21803, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_003_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2307], 
        21804, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_004_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2308], 
        21805, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_005_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2309], 
        21806, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_006_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2310], 
        21807, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_007_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2311], 
        21808, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_008_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2312], 
        21809, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_009_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2313], 
        21810, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_010_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2314], 
        21820, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_020_midorian_emissive.aem",
                                                20008, false));
    ADD_RES(resources[2315], 
        21821, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_021_midorian_emissive.aem",
                                                20008, false));
    ADD_RES(resources[2316], 
        21822, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_022_midorian_emissive.aem",
                                                20008, false));
    ADD_RES(resources[2317], 
        21823, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_023_midorian_emissive.aem",
                                                20008, false));
    ADD_RES(resources[2318], 
        21824, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_024_midorian_emissive.aem",
                                                20008, false));
    ADD_RES(resources[2319], 
        21830, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_030_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2320], 
        21831, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_031_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2321], 
        21832, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_032_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2322], 
        21833, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_033_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2323], 
        21835, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_035_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2324], 
        21836, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_036_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2325], 
        21837, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_037_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2326], 
        21838, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_038_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2327], 
        21839, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_039_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2328], 
        21840, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_040_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2329], 
        21841, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_041_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2330], 
        21842, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_042_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2331], 
        21843, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_043_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2332], 
        21844, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_044_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2333], 
        21845, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_045_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2334], 
        21846, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_046_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2335], 
        21847, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_047_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2336], 
        21848, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_048_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2337], 
        21849, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_049_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2338], 
        21855, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_055_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2339], 
        21856, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_056_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2340], 
        21857, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_057_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2341], 
        21865, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_065_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2342], 
        21866, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_066_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2343], 
        21867, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_067_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2344], 
        21868, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_068_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2345], 
        21869, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_069_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2346], 
        21870, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_070_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2347], 
        21871, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_071_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2348], 
        21872, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_072_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2349], 
        21873, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_073_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2350], 
        21874, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_074_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2351], 
        21875, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_075_midorian_emissive.aem",
                                                20008, false));
    ADD_RES(resources[2352], 
        21876, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_076_midorian_emissive.aem",
                                                20008, false));
    ADD_RES(resources[2353], 
        21877, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_077_midorian_emissive.aem",
                                                20008, false));
    ADD_RES(resources[2354], 
        21878, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_078_midorian_emissive.aem",
                                                20008, false));
    ADD_RES(resources[2355], 
        21879, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_079_midorian_emissive.aem",
                                                20008, false));
    ADD_RES(resources[2356], 
        21880, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_080_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2357], 
        21881, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_081_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2358], 
        21882, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_082_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2359], 
        21883, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_083_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2360], 
        21884, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_084_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2361], 
        21885, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_085_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2362], 
        21886, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_086_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2363], 
        21887, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_087_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2364], 
        21888, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_088_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2365], 
        21889, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_089_nivelian_emissive.aem",
                                                20006, false));
    ADD_RES(resources[2366], 
        21890, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_090_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2367], 
        21891, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_091_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2368], 
        21892, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_092_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2369], 
        21893, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_093_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2370], 
        21894, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_094_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2371], 
        21895, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_095_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2372], 
        21896, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_096_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2373], 
        21897, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_097_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2374], 
        21898, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_098_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2375], 
        21899, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_099_terran_emissive.aem",
                                                20003, false));
    ADD_RES(resources[2376], 21905, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_105_loma_emissive.aem", 20008,
                                  false));
    ADD_RES(resources[2377], 21906, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_106_loma_emissive.aem", 20008,
                                  false));
    ADD_RES(resources[2378], 21907, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_107_loma_emissive.aem", 20008,
                                  false));
    ADD_RES(resources[2379], 21912, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_112_midorian_emissive.aem",
                                  20008, false));
    ADD_RES(resources[2380], 21913, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_113_midorian_emissive.aem",
                                  20008, false));
    ADD_RES(resources[2381], 21914, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_114_midorian_emissive.aem",
                                  20008, false));
    ADD_RES(resources[2382], 21915, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_115_midorian_emissive.aem",
                                  20008, false));
    ADD_RES(resources[2383], 21916, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_116_midorian_emissive.aem",
                                  20008, false));
    ADD_RES(resources[2384], 21917, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_117_midorian_emissive.aem",
                                  20008, false));
    ADD_RES(resources[2385], 21918, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_118_midorian_emissive.aem",
                                  20008, false));
    ADD_RES(resources[2386], 21919, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_119_nivelian_emissive.aem",
                                  20006, false));
    ADD_RES(resources[2387], 21920, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_120_nivelian_emissive.aem",
                                  20006, false));
    ADD_RES(resources[2388], 21921, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_121_nivelian_emissive.aem",
                                  20006, false));
    ADD_RES(resources[2389], 21922, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_122_nivelian_emissive.aem",
                                  20006, false));
    ADD_RES(resources[2390], 21923, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_123_nivelian_emissive.aem",
                                  20006, false));
    ADD_RES(resources[2391], 22000, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_000_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2392], 22001, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_001_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2393], 22002, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_002_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2394], 22003, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_003_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2395], 22004, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_004_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2396], 
        22005, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_005_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2397], 
        22006, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_006_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2398], 
        22007, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_007_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2399], 
        22008, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_008_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2400], 
        22009, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_009_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2401], 
        22010, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_010_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2402], 22020, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_020_midorian_lights_add.aem", 34600,
                                  false));
    ADD_RES(resources[2403], 22021, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_021_midorian_lights_add.aem", 34600,
                                  false));
    ADD_RES(resources[2404], 22022, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_022_midorian_lights_add.aem", 34600,
                                  false));
    ADD_RES(resources[2405], 22023, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_023_midorian_lights_add.aem", 34600,
                                  false));
    ADD_RES(resources[2406], 22024, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_024_midorian_lights_add.aem", 34600,
                                  false));
    ADD_RES(resources[2407], 22030, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_030_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2408], 22031, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_031_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2409], 22032, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_032_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2410], 22033, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_033_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2411], 
        22035, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_035_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2412], 
        22036, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_036_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2413], 
        22037, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_037_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2414], 
        22038, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_038_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2415], 
        22039, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_039_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2416], 
        22040, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_040_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2417], 
        22041, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_041_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2418], 
        22042, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_042_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2419], 
        22043, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_043_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2420], 
        22044, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_044_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2421], 22045, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_045_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2422], 22046, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_046_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2423], 22047, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_047_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2424], 22048, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_048_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2425], 22049, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_049_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2426], 
        22055, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_055_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2427], 
        22056, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_056_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2428], 
        22057, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_057_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2429], 
        22065, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_065_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2430], 
        22066, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_066_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2431], 
        22067, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_067_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2432], 
        22068, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_068_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2433], 
        22069, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_069_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2434], 
        22070, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_070_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2435], 
        22071, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_071_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2436], 
        22072, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_072_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2437], 
        22073, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_073_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2438], 
        22074, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_074_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2439], 22075, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_075_midorian_lights_add.aem", 34600,
                                  false));
    ADD_RES(resources[2440], 22076, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_076_midorian_lights_add.aem", 34600,
                                  false));
    ADD_RES(resources[2441], 22077, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_077_midorian_lights_add.aem", 34600,
                                  false));
    ADD_RES(resources[2442], 22078, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_078_midorian_lights_add.aem", 34600,
                                  false));
    ADD_RES(resources[2443], 22079, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_079_midorian_lights_add.aem", 34600,
                                  false));
    ADD_RES(resources[2444], 
        22080, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_080_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2445], 
        22081, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_081_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2446], 
        22082, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_082_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2447], 
        22083, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_083_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2448], 
        22084, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_084_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2449], 22085, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_085_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2450], 22086, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_086_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2451], 22087, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_087_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2452], 22088, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_088_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2453], 22089, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/main/3d/meshes/stations/station_089_nivelian_lights_add.aem", 34810,
                                  false));
    ADD_RES(resources[2454], 
        22090, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_090_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2455], 
        22091, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_091_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2456], 
        22092, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_092_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2457], 
        22093, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_093_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2458], 
        22094, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_094_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2459], 
        22095, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_095_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2460], 
        22096, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_096_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2461], 
        22097, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_097_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2462], 
        22098, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_098_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2463], 
        22099, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_099_terran_lights_add.aem",
                                                3, false));
    ADD_RES(resources[2464], 22105, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_105_loma_lights_add.aem", 34600,
                                  false));
    ADD_RES(resources[2465], 22106, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_106_loma_lights_add.aem", 34600,
                                  false));
    ADD_RES(resources[2466], 22107, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/valkyrie/3d/meshes/stations/v_station_107_loma_lights_add.aem", 34600,
                                  false));
    ADD_RES(resources[2467], 22112, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_112_midorian_add.aem", 34600,
                                  false));
    ADD_RES(resources[2468], 22113, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_113_midorian_add.aem", 34600,
                                  false));
    ADD_RES(resources[2469], 22114, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_114_midorian_add.aem", 34600,
                                  false));
    ADD_RES(resources[2470], 22115, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_115_midorian_add.aem", 34600,
                                  false));
    ADD_RES(resources[2471], 22116, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_116_midorian_add.aem", 34600,
                                  false));
    ADD_RES(resources[2472], 22117, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_117_midorian_add.aem", 34600,
                                  false));
    ADD_RES(resources[2473], 22118, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_118_midorian_add.aem", 34600,
                                  false));
    ADD_RES(resources[2474], 22119, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_119_nivelian_add.aem", 34810,
                                  false));
    ADD_RES(resources[2475], 22120, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_120_nivelian_add.aem", 34810,
                                  false));
    ADD_RES(resources[2476], 22121, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_121_nivelian_add.aem", 34810,
                                  false));
    ADD_RES(resources[2477], 22122, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_122_nivelian_add.aem", 34810,
                                  false));
    ADD_RES(resources[2478], 22123, 4, new AbyssEngine::ResourceMesh(
                                  "data/assets/supernova/3d/meshes/stations/sn_station_123_nivelian_add.aem", 34810,
                                  false));
    ADD_RES(resources[2479], 
        16436, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_vossk.aem", 20032, false));
    ADD_RES(resources[2480], 
        16439, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_vossk_emissive.aem", 20031,
                                                false));
    ADD_RES(resources[2481], 
        16442, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_vossk_lights_add.aem",
                                                20033, false));
    ADD_RES(resources[2482], 
        16443, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_void.aem", 20045, false));
    ADD_RES(resources[2483], 
        16446, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_void_emissive.aem", 20048,
                                                false));
    ADD_RES(resources[2484], 
        16449, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/stations/station_void_add.aem", 20047,
                                                false));
    ADD_RES(resources[2485], 
        14285, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/explosion_void_station_add.aem", 27301,
                                                false));
    ADD_RES(resources[2486], 
        14286, 4, new AbyssEngine::ResourceMesh("data/assets/main/3d/meshes/fx/explosion_void_station_add_lookat.aem",
                                                27301, false));
    canvas->SetResourceList(resources, 2488);

    for (int i = 0; i < 152; ++i)
        canvas->AddResource(makeRes((unsigned short) (i + 5000), 3,
                                    newImage((unsigned short) (i + 10200), 0)));

    canvas->AddResource(
        makeRes(11635, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_font_ar_1440.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11637, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_font_cht_1440.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11639, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_font_chs_1440.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11643, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_font_kr_1440.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11645, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_font_ja_1440.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11641, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_font_langselect_1440.aei", 0.0f)));
    canvas->AddResource(
        makeRes(10062, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_interface_ipad_1440.aei", 0.0f)));
    canvas->AddResource(makeRes(10002, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_logos_1440.aei", 0.0f)));
    canvas->AddResource(
        makeRes(27340, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_credits_1440.aei", 0.0f)));
    canvas->AddResource(makeRes(
        27341, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_challenge_interface_ipad_1440.aei", 0.0f)));
    canvas->AddResource(makeRes(
        27342, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_campaign_select_ipad_1440.aei", 0.0f)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(
        makeRes(10089, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_interface3_ipad_large.aei", 0.0f)));
    canvas->AddResource(
        makeRes(10063, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_interface2_ipad_large.aei", 0.0f)));
    canvas->AddResource(
        makeRes(10064, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_items_ipad_large.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11632, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_items_ipad_2_large.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11630, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_dlc_interface_ipad_large.aei", 0.0f)));
    canvas->AddResource(makeRes(
        27341, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_challenge_interface_ipad_large.aei", 0.0f)));
    canvas->AddResource(makeRes(
        27342, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_campaign_select_ipad_large.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11635, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_font_ar_iphone4.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11637, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_font_cht_iphone4.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11639, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_font_chs_iphone4.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11643, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_font_kr_iphone4.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11645, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_font_ja_iphone4.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11641, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_font_langselect_iphone4.aei", 0.0f)));
    canvas->AddResource(
        makeRes(10062, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_interface_iphone4.aei", 0.0f)));
    canvas->AddResource(
        makeRes(10002, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_logos_iphone4.aei", 0.0f)));
    canvas->AddResource(
        makeRes(27340, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_credits_iphone4.aei", 0.0f)));
    canvas->AddResource(
        makeRes(10064, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_items_iphone4.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11632, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_items_iphone4_2.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11630, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_dlc_interface_iphone4.aei", 0.0f)));
    canvas->AddResource(
        makeRes(10089, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_interface3_iphone4.aei", 0.0f)));
    canvas->AddResource(makeRes(
        27341, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_challenge_interface_iphone4.aei", 0.0f)));
    canvas->AddResource(
        makeRes(27342, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_campaign_select_iphone4.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11630, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_dlc_interface.aei", 0.0f)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(
        makeRes(10089, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_interface3_1440.aei", 0.0f)));
    canvas->AddResource(
        makeRes(10063, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_interface2_ipad_1440.aei", 0.0f)));
    canvas->AddResource(
        makeRes(10064, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_items_ipad_1440.aei", 0.0f)));
    canvas->AddResource(
        makeRes(11632, 2, new AbyssEngine::ResourceTexture("data/textures/gof2_items_ipad_2_1440.aei", 0.0f)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(3001, 3, newImage(0x2d6eu, 0x0u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(3002, 3, newImage(0x2d6eu, 0x2u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(3104, 3, newImage(0x2d6eu, 0x4u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(3106, 3, newImage(0x2d6eu, 0x6u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(3006, 3, newImage(0x2d6eu, 0x8u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(3008, 3, newImage(0x2d6eu, 0xau)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(3010, 3, newImage(0x2d6eu, 0xcu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(3012, 3, newImage(0x2d6eu, 0xeu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(3014, 3, newImage(0x2d6eu, 0x10u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(3016, 3, newImage(0x2d6eu, 0x12u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1701, 3, newImage(0x274fu, 0x1u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1703, 3, newImage(0x274fu, 0x3u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1705, 3, newImage(0x274fu, 0x5u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1312, 3, newImage(0x274fu, 0x7u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1222, 3, newImage(0x274fu, 0x9u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1204, 3, newImage(0x274fu, 0xbu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1257, 3, newImage(0x274fu, 0xdu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1214, 3, newImage(0x274fu, 0xfu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1212, 3, newImage(0x274fu, 0x11u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1210, 3, newImage(0x274fu, 0x13u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1208, 3, newImage(0x274fu, 0x15u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1199, 3, newImage(0x274fu, 0x17u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1192, 3, newImage(0x274fu, 0x19u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1207, 3, newImage(0x274fu, 0x1bu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1217, 3, newImage(0x274fu, 0x1du)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1202, 3, newImage(0x274fu, 0x1fu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1200, 3, newImage(0x274fu, 0x21u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1708, 3, newImage(0x274fu, 0x23u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1273, 3, newImage(0x274fu, 0x25u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1275, 3, newImage(0x274fu, 0x27u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1271, 3, newImage(0x274fu, 0x29u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1709, 3, newImage(0x274fu, 0x2bu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1711, 3, newImage(0x274fu, 0x2du)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1713, 3, newImage(0x274fu, 0x2fu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1715, 3, newImage(0x274fu, 0x31u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1717, 3, newImage(0x274fu, 0x33u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1719, 3, newImage(0x274fu, 0x35u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1721, 3, newImage(0x274fu, 0x37u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1723, 3, newImage(0x274fu, 0x39u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1316, 3, newImage(0x274fu, 0x3bu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1318, 3, newImage(0x274fu, 0x3du)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1320, 3, newImage(0x274fu, 0x3fu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1322, 3, newImage(0x274fu, 0x41u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1324, 3, newImage(0x274fu, 0x43u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1104, 3, newImage(0x274fu, 0x45u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1100, 3, newImage(0x274fu, 0x47u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1105, 3, newImage(0x274fu, 0x49u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1264, 3, newImage(0x274fu, 0x4bu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1225, 3, newImage(0x274fu, 0x4du)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1227, 3, newImage(0x274fu, 0x4fu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1262, 3, newImage(0x274fu, 0x51u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1234, 3, newImage(0x274fu, 0x53u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1229, 3, newImage(0x274fu, 0x55u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1224, 3, newImage(0x274fu, 0x57u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1239, 3, newImage(0x274fu, 0x59u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1241, 3, newImage(0x274fu, 0x5bu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1238, 3, newImage(0x274fu, 0x5du)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1236, 3, newImage(0x274fu, 0x5fu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1325, 3, newImage(0x274fu, 0x61u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1327, 3, newImage(0x274fu, 0x63u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1329, 3, newImage(0x274fu, 0x65u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1331, 3, newImage(0x274fu, 0x67u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1332, 3, newImage(0x274eu, 0xe6u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1343, 3, newImage(0x274fu, 0x6bu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1345, 3, newImage(0x274fu, 0x6du)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1347, 3, newImage(0x274fu, 0x6fu)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(1350, 3, newImage(0x274fu, 0x71u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(7005, 3, newImage(0x2712u, 0x5u)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(
        10011, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_000_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10012, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_001_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10013, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_002_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10014, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_003_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10015, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_004_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10016, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_005_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10017, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_006_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10018, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_007_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10019, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_008_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10020, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_009_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10021, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_010_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10022, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_011_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10023, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_012_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10024, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_013_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10025, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_014_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10026, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_015_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10027, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_016_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10028, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_017_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10029, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_018_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        10030, 2, new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/planets/planet_019_small.aei",
                                                   0.0f)));
    canvas->AddResource(makeRes(
        11621, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/planets/v_planet_020_small.aei", 0.0f)));
    canvas->AddResource(makeRes(
        11622, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/planets/v_planet_021_small.aei", 0.0f)));
    canvas->AddResource(makeRes(
        11623, 2, new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/planets/v_planet_022_small.aei", 0.0f)));
    canvas->AddResource(makeRes(
        11784, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/planets/sn_planet_024_small.aei", 0.0f)));
    canvas->AddResource(makeRes(
        11785, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/planets/sn_planet_025_small.aei", 0.0f)));
    canvas->AddResource(makeRes(
        11786, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/planets/sn_planet_026_small.aei", 0.0f)));
    canvas->AddResource(makeRes(
        29080, 2, new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/planets/sn_planet_ring.aei", 0.0f)));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(0, 0, nullptr));
    canvas->AddResource(makeRes(9510, 3, newImage(0x6acfu, 0x5u)));

    loadPortraits(engine);
    loadLowTexturesAndMaterials(engine);
}
