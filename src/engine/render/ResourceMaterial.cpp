#include "engine/render/ResourceMaterial.h"
#include "engine/render/ResourceTexture.h"
#include "engine/render/Engine.h"
#include "engine/core/ApplicationManager.h"

#ifndef GOF2_ENUM_BlendMode
#define GOF2_ENUM_BlendMode
#endif
#include "engine/render/PaintCanvas.h"
#include "game/core/String.h"

namespace AbyssEngine {
    ResourceMaterial::ResourceMaterial(uint16_t texId, uint16_t texId2, BlendMode blend) {
        this->blendMode = static_cast<int>(blend);
        this->field_14 = 0;
        this->field_18 = 0;
        this->field_1c = 0x3f800000;
        this->field_20 = 0;
        this->field_24 = 0;
        for (int i = 0; i != 8; ++i)
            this->texIndices[i] = 0xffff;
        this->texIndices[1] = texId2;
        this->texIndices[0] = texId;
    }

    ResourceMaterial::ResourceMaterial(uint16_t texId, BlendMode blend) {
        this->blendMode = static_cast<int>(blend);
        this->field_14 = 0;
        this->field_18 = 0;
        this->field_1c = 0x3f800000;
        this->field_20 = 0;
        this->field_24 = 0;
        for (int i = 0; i != 8; ++i)
            this->texIndices[i] = 0xffff;
        this->texIndices[0] = texId;
    }

    struct Resource {
        unsigned short id;
        int kind;
        int unused;
        void *payload;
    };
}

static char g_Portraits_zeroFlag = 0;
static char *g_Portraits_ipadLargeFlag = &g_Portraits_zeroFlag;
static char *g_Portraits_ipad1440Flag = &g_Portraits_zeroFlag;
static char *g_Portraits_ipadFlagA = &g_Portraits_zeroFlag;
static char *g_Portraits_ipadFlagB = &g_Portraits_zeroFlag;
static char *g_Portraits_ipadFlagC = &g_Portraits_zeroFlag;

void loadPortraits(AbyssEngine::Engine *engine) {
    AbyssEngine::PaintCanvas *canvas = engine->appManager->paintCanvas;

    const char *infix;
    if (*g_Portraits_ipadLargeFlag != 0)
        infix = "_ipad_large";
    else if (*g_Portraits_ipad1440Flag != 0)
        infix = "_ipad_1440";
    else if ((*g_Portraits_ipadFlagA | *g_Portraits_ipadFlagB | *g_Portraits_ipadFlagC) != 0)
        infix = "_ipad";
    else
        infix = "";
    AbyssEngine::String suffix(infix, false);

    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_0_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27d8;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_0_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27d9;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_0_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27da;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_0_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27db;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_0_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27dc;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_0_5", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27dd;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_0_6", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27de;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_0_7", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27df;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_0_8", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27e0;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_0_9", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27e1;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_0_10", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27e2;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_1_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27e3;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_1_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27e4;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_1_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27e5;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_1_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27e6;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_1_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27e7;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_1_5", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27e8;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_1_6", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27e9;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_1_7", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27ea;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_1_8", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27eb;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_1_9", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27ec;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_1_10", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27ed;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_2_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27ee;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_2_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27ef;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_2_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27f0;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_2_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27f1;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_2_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27f2;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_2_5", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27f3;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_2_6", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27f4;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_2_7", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27f5;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_2_8", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27f6;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_2_9", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27f7;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_2_10", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27f8;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_3_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27f9;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_3_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27fa;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_3_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27fb;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_3_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27fc;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_3_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27fd;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_3_5", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27fe;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_3_6", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x27ff;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_3_7", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2800;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_3_8", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2801;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_3_9", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2802;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/0_3_10", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2803;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_0_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2804;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_0_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2805;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_0_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2806;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_0_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2807;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_1_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2808;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_1_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2809;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_1_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x280a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_1_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x280b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_1_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x280c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_2_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x280d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_2_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x280e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_2_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x280f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_2_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2810;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_2_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2811;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_2_5", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2812;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_3_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2813;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_3_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2814;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_3_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2815;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_3_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2816;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_3_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2817;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_3_5", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2818;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_3_6", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2819;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_3_7", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x281a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/1_3_8", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x281b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_0_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x281c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_0_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x281d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_0_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x281e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_0_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x281f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_0_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2820;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_1_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2821;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_1_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2822;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_1_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2823;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_1_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2824;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_1_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2825;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_2_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2826;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_2_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2827;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_2_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2828;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_2_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2829;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_2_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x282a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_3_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x282b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_3_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x282c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_3_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x282d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_3_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x282e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/2_3_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x282f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_0_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2830;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_0_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2831;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_0_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2832;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_1_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2833;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_1_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2834;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_1_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2835;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_2_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2836;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_2_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2837;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_2_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2838;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_2_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2839;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_2_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x283a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_3_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x283b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_3_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x283c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_3_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x283d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/4_3_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x283e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/6_0_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x283f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/6_0_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2840;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/6_1_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2841;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/6_1_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2842;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/6_1_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2843;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/6_2_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2844;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/6_2_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2845;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/6_2_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2846;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/6_2_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2847;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/6_2_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2848;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/7_0_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2849;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/7_0_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x284a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/7_1_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x284b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/7_1_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x284c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/7_2_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x284d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/7_2_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x284e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/7_2_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x284f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/7_3_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2850;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/7_3_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2851;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/9_0_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2852;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/9_1_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2853;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/9_2_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2854;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/9_3_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2855;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_0_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2856;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_0_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2857;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_0_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2858;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_0_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2859;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_1_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x285a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_1_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x285b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_1_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x285c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_1_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x285d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_2_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x285e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_2_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x285f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_2_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2860;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_2_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2861;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_2_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2862;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_3_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2863;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_3_1", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2864;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_3_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2865;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_3_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2866;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_3_4", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2867;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_3_5", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2868;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/10_3_6", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x2869;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/11_3_2", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x286a;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/11_3_3", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x286b;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/12_0_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x286c;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/12_1_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x286d;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/12_2_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x286e;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
    {
        AbyssEngine::String fullName = AbyssEngine::String("data/textures/12_3_0", false) + suffix +
                                       AbyssEngine::String(".aei", false);
        AbyssEngine::Resource *res = new AbyssEngine::Resource;
        res->id = 0x286f;
        res->kind = 2;
        res->unused = -1;
        res->payload = new AbyssEngine::ResourceTexture(fullName, 0.0f);
        canvas->AddResource(res);
    }
}
