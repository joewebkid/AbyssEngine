#include "engine/render/ResourceMaterial.h"
#include "engine/render/ResourceTexture.h"
#include "engine/render/Engine.h"
#include "engine/core/ApplicationManager.h"

#ifndef GOF2_ENUM_BlendMode
#define GOF2_ENUM_BlendMode
#endif
#include "engine/render/PaintCanvas.h"
#include "game/core/String.h"
#include "game/core/Globals.h"

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

void loadPortraits(AbyssEngine::Engine *engine) {
    AbyssEngine::PaintCanvas *canvas = engine->appManager->paintCanvas;

    const char *infix;
    // Android ARM 0x112d60-0x112dba: five GOT-backed Globals bytes.
    if (Globals::iPadLarge != 0)
        infix = "_ipad_large";
    else if (Globals::iPadHD != 0)
        infix = "_ipad_1440";
    else if ((Globals::retinaDisplay | Globals::iPad | Globals::n9) != 0)
        infix = "_ipad";
    else
        infix = "";
    AbyssEngine::String suffix(infix, false);

    // ARM 0x112de8-0x112e6c: allocate resource/payload before names; keep
    // all four String temporaries alive through AddResource's full expression.
    canvas->AddResource(new AbyssEngine::Resource{0x27d8, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_0_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27d9, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_0_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27da, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_0_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27db, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_0_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27dc, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_0_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27dd, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_0_5", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27de, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_0_6", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27df, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_0_7", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27e0, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_0_8", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27e1, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_0_9", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27e2, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_0_10", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27e3, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_1_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27e4, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_1_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27e5, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_1_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27e6, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_1_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27e7, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_1_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27e8, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_1_5", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27e9, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_1_6", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27ea, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_1_7", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27eb, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_1_8", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27ec, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_1_9", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27ed, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_1_10", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27ee, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_2_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27ef, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_2_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27f0, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_2_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27f1, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_2_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27f2, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_2_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27f3, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_2_5", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27f4, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_2_6", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27f5, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_2_7", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27f6, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_2_8", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27f7, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_2_9", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27f8, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_2_10", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27f9, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_3_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27fa, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_3_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27fb, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_3_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27fc, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_3_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27fd, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_3_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27fe, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_3_5", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x27ff, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_3_6", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2800, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_3_7", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2801, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_3_8", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2802, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_3_9", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2803, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/0_3_10", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2804, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_0_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2805, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_0_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2806, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_0_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2807, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_0_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2808, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_1_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2809, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_1_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x280a, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_1_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x280b, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_1_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x280c, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_1_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x280d, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_2_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x280e, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_2_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x280f, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_2_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2810, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_2_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2811, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_2_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2812, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_2_5", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2813, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_3_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2814, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_3_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2815, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_3_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2816, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_3_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2817, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_3_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2818, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_3_5", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2819, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_3_6", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x281a, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_3_7", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x281b, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/1_3_8", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x281c, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_0_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x281d, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_0_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x281e, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_0_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x281f, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_0_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2820, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_0_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2821, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_1_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2822, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_1_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2823, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_1_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2824, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_1_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2825, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_1_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2826, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_2_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2827, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_2_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2828, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_2_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2829, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_2_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x282a, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_2_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x282b, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_3_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x282c, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_3_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x282d, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_3_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x282e, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_3_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x282f, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/2_3_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2830, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_0_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2831, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_0_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2832, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_0_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2833, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_1_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2834, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_1_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2835, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_1_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2836, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_2_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2837, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_2_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2838, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_2_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2839, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_2_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x283a, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_2_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x283b, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_3_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x283c, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_3_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x283d, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_3_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x283e, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/4_3_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x283f, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/6_0_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2840, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/6_0_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2841, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/6_1_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2842, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/6_1_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2843, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/6_1_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2844, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/6_2_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2845, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/6_2_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2846, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/6_2_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2847, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/6_2_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2848, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/6_2_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2849, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/7_0_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x284a, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/7_0_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x284b, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/7_1_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x284c, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/7_1_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x284d, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/7_2_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x284e, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/7_2_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x284f, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/7_2_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2850, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/7_3_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2851, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/7_3_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2852, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/9_0_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2853, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/9_1_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2854, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/9_2_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2855, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/9_3_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2856, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_0_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2857, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_0_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2858, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_0_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2859, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_0_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x285a, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_1_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x285b, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_1_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x285c, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_1_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x285d, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_1_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x285e, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_2_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x285f, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_2_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2860, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_2_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2861, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_2_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2862, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_2_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2863, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_3_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2864, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_3_1", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2865, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_3_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2866, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_3_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2867, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_3_4", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2868, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_3_5", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x2869, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/10_3_6", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x286a, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/11_3_2", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x286b, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/11_3_3", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x286c, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/12_0_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x286d, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/12_1_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x286e, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/12_2_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
    canvas->AddResource(new AbyssEngine::Resource{0x286f, 2, -1,
        new AbyssEngine::ResourceTexture(AbyssEngine::String("data/textures/12_3_0", false) + suffix +
            AbyssEngine::String(".aei", false), 0.0f)});
}
