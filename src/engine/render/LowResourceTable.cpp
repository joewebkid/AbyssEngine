

#include "engine/render/ResourceTexture.h"
#include "engine/render/ResourceMaterial.h"

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
        void *payload;
    };
}

void loadLowTexturesAndMaterials(AbyssEngine::Engine *engine) {
    AbyssEngine::PaintCanvas *canvas = engine->appManager->paintCanvas;

    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d00;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_000_midorian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d01;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_001_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d02;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_002_pirates_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d03;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_003_midorian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d04;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_004_nivelian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d05;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_005_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d06;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_006_midorian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d07;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_007_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d08;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_008_void_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d09;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_009_vossk_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d0a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_010_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d0b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_011_pirates_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d0c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_012_nivelian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d10;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_016_nivelian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d11;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_017_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d12;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_018_nivelian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d13;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_019_midorian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d14;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_020_midorian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d15;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_021_nivelian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d16;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_022_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d17;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_023_pirates_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d18;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_024_pirates_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d19;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_025_pirates_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d1a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_026_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d1b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_027_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d1c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_028_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d1d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_029_pirates_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d1e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_030_midorian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d1f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_031_nivelian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d20;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_032_pirates_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d21;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_033_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d22;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_034_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d23;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_035_nivelian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d24;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_036_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d64;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_000_midorian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d65;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_001_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d66;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_002_pirates_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d67;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_003_midorian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d68;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_004_nivelian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d69;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_005_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d6a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_006_midorian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d6b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_007_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d6c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_008_void_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d6d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_009_vossk_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d6e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_010_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d6f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_011_pirates_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d70;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_012_nivelian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d74;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_016_nivelian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d75;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_017_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d76;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_018_nivelian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d77;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_019_midorian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d78;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_020_midorian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d79;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_021_nivelian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d7a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_022_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d7b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_023_pirates_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d7c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_024_pirates_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d7d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_025_pirates_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d7e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_026_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d7f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_027_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d80;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_028_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d81;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_029_pirates_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d82;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_030_midorian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d83;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_031_nivelian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d84;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_032_pirates_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d85;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_033_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d86;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_034_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d87;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_035_nivelian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7d88;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/ship_036_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dc8;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d00, 0x7d64, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dc9;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d01, 0x7d65, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dca;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d02, 0x7d66, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dcb;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d03, 0x7d67, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dcc;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d04, 0x7d68, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dcd;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d05, 0x7d69, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dce;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d06, 0x7d6a, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dcf;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d07, 0x7d6b, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dd0;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d08, 0x7d6c, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dd1;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d09, 0x7d6d, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dd2;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d0a, 0x7d6e, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dd3;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d0b, 0x7d6f, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dd4;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d0c, 0x7d70, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dd8;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d10, 0x7d74, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dd9;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d11, 0x7d75, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dda;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d12, 0x7d76, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7ddb;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d13, 0x7d77, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7ddc;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d14, 0x7d78, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7ddd;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d15, 0x7d79, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dde;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d16, 0x7d7a, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7ddf;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d17, 0x7d7b, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7de0;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d18, 0x7d7c, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7de1;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d19, 0x7d7d, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7de2;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d1a, 0x7d7e, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7de3;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d1b, 0x7d7f, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7de4;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d1c, 0x7d80, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7de5;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d1d, 0x7d81, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7de6;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d1e, 0x7d82, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7de7;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d1f, 0x7d83, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7de8;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d20, 0x7d84, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7de9;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d21, 0x7d85, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dea;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d22, 0x7d86, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7deb;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d23, 0x7d87, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7dec;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7d24, 0x7d88, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e96;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e97;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e94;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_pirates_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e95;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_pirates_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e92;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_nivelian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e93;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_nivelian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e90;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_midorian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e91;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_midorian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e9a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_vossk_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e9b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_vossk_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e98;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_void_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e99;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/stations/stations_void_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e9d;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7e96, 0x7e97, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e9c;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7e96, static_cast<AbyssEngine::BlendMode>(0x0));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e9e;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7e92, 0x7e93, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7e9f;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7e92, static_cast<AbyssEngine::BlendMode>(0x0));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7ea9;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7e94, 0x7e95, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7eaa;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7e94, static_cast<AbyssEngine::BlendMode>(0x0));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7ea0;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7e90, 0x7e91, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7ea1;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7e90, static_cast<AbyssEngine::BlendMode>(0x0));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7ea6;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x80f4, static_cast<AbyssEngine::BlendMode>(0x0));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7ea5;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7e98, 0x7e99, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7ea7;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7e98, static_cast<AbyssEngine::BlendMode>(0x2));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7eab;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7e98, static_cast<AbyssEngine::BlendMode>(0x1));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7ea3;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7e9a, 0x7e9b, static_cast<AbyssEngine::BlendMode>(0x1c));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7ea2;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7e9a, static_cast<AbyssEngine::BlendMode>(0x0));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x7ea4;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x7e9a, static_cast<AbyssEngine::BlendMode>(0x2));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x5e88;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/fx/projectiles.aei",
                                                        -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x5e1f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture("data/assets/valkyrie/3d/textures/low/etc/fx/v_projectiles.aei",
                                                        -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x5e89;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x5e88, static_cast<AbyssEngine::BlendMode>(0x2));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x5e20;
        res->kind = 6;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceMaterial(0x5e1f, static_cast<AbyssEngine::BlendMode>(0x3));
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2766;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/skyboxes/skybox_stars_000.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2767;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/skyboxes/skybox_stars_001.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2768;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/skyboxes/skybox_stars_002.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x273a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_000_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x273b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_001_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x273c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_002_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x273d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_003_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x273e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_004_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x273f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_005_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2740;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_006_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2741;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_007_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2742;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_008_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2743;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_009_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2744;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_010_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2745;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_011_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2746;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_012_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2747;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_013_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2748;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_014_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2749;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_015_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x274a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_016_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x274b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_017_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x274c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_018_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x274d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_019_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2d68;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/planets/v_planet_020_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2d69;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/planets/v_planet_021_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2d6a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/planets/v_planet_022_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2719;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/planets/planet_void_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2e14;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/planets/sn_planet_024_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2e15;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/planets/sn_planet_025_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2e16;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/planets/sn_planet_026_big.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2751;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/skyboxes/skybox_000.aei",
                                                        -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2752;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/skyboxes/skybox_001.aei",
                                                        -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2753;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/skyboxes/skybox_002.aei",
                                                        -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2754;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/skyboxes/skybox_003.aei",
                                                        -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2755;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/skyboxes/skybox_004.aei",
                                                        -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2756;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/skyboxes/skybox_005.aei",
                                                        -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2757;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/skyboxes/skybox_006.aei",
                                                        -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2758;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/skyboxes/skybox_007.aei",
                                                        -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2759;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/skyboxes/skybox_008.aei",
                                                        -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x275a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/skyboxes/skybox_009.aei",
                                                        -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x275b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture("data/assets/main/3d/textures/low/etc/skyboxes/skybox_010.aei",
                                                        -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x275c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/skyboxes/v_skybox_011.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x275d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/skyboxes/v_skybox_012.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x275e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/skyboxes/v_skybox_013.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x275f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/valkyrie/3d/textures/low/etc/skyboxes/v_skybox_014.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x80e8;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/asteroid_01_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x80e9;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/asteroid_01_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x80ea;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/asteroid_void_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x80eb;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/misc/asteroid_void_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2760;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/skyboxes/sn_skybox_015.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2764;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/skyboxes/sn_skybox_015_flares.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2765;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/skyboxes/sn_skybox_015_flares_nasty.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2761;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/skyboxes/sn_skybox_016.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2762;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/skyboxes/sn_skybox_017.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2763;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/supernova/3d/textures/low/etc/skyboxes/sn_skybox_018.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2798;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/bars/bar_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x279c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/bars/bar_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x279b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/bars/bar_midorian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x279f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/bars/bar_midorian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x279a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/bars/bar_nivelian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x279e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/bars/bar_nivelian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2799;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/bars/bar_vossk_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x279d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/bars/bar_vossk_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x816a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/hangars/hangar_midorian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x816b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/hangars/hangar_midorian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x816c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/hangars/hangar_vossk_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x816d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/hangars/hangar_vossk_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x816e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/hangars/hangar_nivelian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x816f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/hangars/hangar_nivelian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8170;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/hangars/hangar_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8171;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/hangars/hangar_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8160;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_midorian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8161;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_midorian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8162;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_nivelian_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8163;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_nivelian_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8164;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8165;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8166;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_vossk_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8167;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_vossk_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8168;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/battleship_terran_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8169;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/battleship_terran_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x823e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_midorian_dmg_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x823f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_midorian_dmg_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8240;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_nivelian_dmg_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8241;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_nivelian_dmg_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8242;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_terran_dmg_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8243;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_terran_dmg_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8244;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_vossk_dmg_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8245;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/cargo_vossk_dmg_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8246;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/battleship_terran_dmg_diffuse.aei", -1.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x8247;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(
            "data/assets/main/3d/textures/low/etc/ships/battleship_terran_dmg_normal_specular.aei", -1.0f);
        canvas->AddResource(res);
    }
}
