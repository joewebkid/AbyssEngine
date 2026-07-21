#include "game/ui/ListItemWindow.h"
#include "game/core/Globals.h"
#include "game/ship/Ship.h"
#include "game/mission/BluePrint.h"
#include "game/mission/PendingProduct.h"
#include "engine/core/ApplicationManager.h"
#include "engine/render/AEGeometry.h"
#include "engine/render/Engine.h"
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/ui/ScrollTouchWindow.h"
#include "game/world/Galaxy.h"
#include "game/world/SolarSystem.h"
#include "engine/core/GameText.h"
#include "engine/render/ImageFactory.h"
#include "game/ui/Layout.h"
#include "game/ui/ListItem.h"
#include "engine/render/PaintCanvas.h"

namespace {

constexpr int kMissingItemAttribute = -979797979;

// Android HD rodata: 0x00202034. The two trailing zero words are part of the
// original 11-entry scan and make attribute zero remain hidden.
constexpr int kHiddenItemAttributes[11] = {
    0, 1, 4, 5, 6, 7, 8, 60, 61, 0, 0,
};

// Android HD rodata: 0x00202060, one GameText ID per item attribute.
constexpr int kItemAttributeTextIds[62] = {
    320, 320, 320, 133, 132, 132, 132, 132, 132, 146, 138, 149,
    151, 156, 139, 158, 153, 164, 148, 149, 165, 140, 157, 153,
    141, 157, 149, 152, 157, 141, 142, 143, 164, 144, 145, 157,
    149, 149, 150, 147, 146, 157, 157, 149, 151, 159, 157, 151,
    145, 156, 139, 151, 160, 151, 157, 161, 157, 162, 159, 163,
    174, 174,
};

// Android HD exported data: LISTITEMWINDOW_UNITS at 0x00215e28.
constexpr const char *kItemAttributeUnits[62] = {
    "", "", "", "", "", "", "", "", "", "", "", "ms",
    "m", "km/h", "m", "", "", "", "", "ms", "", "%", "%", "",
    "ms", "%", "ms", "ms", "%", "ms", "", "", "%", "%", "", "ms",
    "ms", "ms", "", "%", "%", "ms", "ms", "ms", "m", "", "%", "m",
    "t", "km/h", "%", "m", "%", "m", "%", "", "%", "", "", "t", "", "",
};

void liw_add_string(Array<String *> *rows, const String &value) {
    ArrayAdd(new String(value), *rows);
}

void liw_add_row(ListItemWindow *self, const String &label, const String &value) {
    liw_add_string(self->labels, label);
    liw_add_string(self->values, value);
}

String liw_text(int textId) {
    return String(*GameText::gGameText->getText(textId), false);
}

String liw_with_unit(int value, int attribute) {
    String unit(kItemAttributeUnits[attribute], false);
    return unit.size() == 0 ? String(value) : value + unit;
}

String liw_signed_with_unit(int value, int attribute) {
    String prefix(value > 0 ? "+" : "", false);
    return prefix + liw_with_unit(value, attribute);
}

bool liw_is_hidden_item_attribute(int itemIndex, int attribute) {
    if (((itemIndex >= 9 && itemIndex < 12) || itemIndex == 228) && attribute == 13)
        return true;

    for (int hidden : kHiddenItemAttributes) {
        if (hidden == attribute)
            return true;
    }
    return false;
}

bool liw_is_boolean_item_attribute(int attribute) {
    if (attribute >= 0 && attribute <= 31)
        return (0xc0818000u & (1u << attribute)) != 0;
    return attribute == 57 || attribute == 58;
}

String liw_format_item_attribute(ListItem *listItem, Item *item, int attribute, int value, int &labelId) {
    if (attribute == 2)
        return liw_text(value + 221);

    if (liw_is_boolean_item_attribute(attribute)) {
        if (attribute == 23) {
            if (value == 0)
                return liw_text(135);
            return liw_text(value == 1 ? 154 : 155);
        }
        return liw_text(value == 0 ? 135 : 134);
    }

    if (attribute == 39 || attribute == 40)
        return liw_signed_with_unit(value, attribute);

    switch (attribute) {
    case 12:
        if (item->getSort() == 11) {
            labelId = 157;
            return String(value / 1000) + String("s", false);
        }
        {
            const int converted = static_cast<int>((static_cast<float>(value) / 3600.0f) *
                                                   static_cast<float>(250 * item->getAttribute(13)));
            int rounded = converted + converted % 100;
            if (rounded % 100)
                rounded = converted - converted % 100;
            return liw_with_unit(rounded, attribute);
        }
    case 13:
        return liw_with_unit(value * 250, attribute);
    case 14:
        return liw_with_unit(static_cast<int>(static_cast<float>(value) * 0.5f), attribute);
    case 49:
        return liw_with_unit(static_cast<int>(static_cast<float>(value) * 50.0f), attribute);
    case 51:
    case 53:
        return liw_with_unit(static_cast<int>(static_cast<float>(value) * 0.1f), attribute);
    case 38:
        if (listItem->getIndex() == 85 && Status::gStatus->hardCoreMode())
            return String(value * 2);
        break;
    default:
        break;
    }

    return liw_with_unit(value, attribute);
}

void liw_add_ship_stat(ListItemWindow *self, int labelId, const String &displayed, int current, int previous) {
    liw_add_row(self, liw_text(labelId), displayed);
    ArrayAdd(current, *self->statsCur);
    ArrayAdd(previous, *self->statsPrev);
}

void liw_fill_ship_rows(ListItemWindow *self, ListItem *listItem) {
    Ship *ship = listItem->ship;
    Ship *currentShip = Status::gStatus->getShip();

    const int baseHp = ship->getBaseHP();
    const int maxHp = ship->getMaxHP();
    const String hp = maxHp > baseHp
        ? String(maxHp) + String(" (+)", false)
        : String(baseHp);
    liw_add_ship_stat(self, 165, hp, maxHp > baseHp ? maxHp : baseHp, currentShip->getBaseHP());

    const int baseLoad = ship->getBaseLoad();
    const int moddedLoad = ship->getModdedLoad();
    const String load = moddedLoad > baseLoad
        ? String(moddedLoad) + String(" (+)", false)
        : String(baseLoad);
    liw_add_ship_stat(self, 166, load, moddedLoad > baseLoad ? moddedLoad : baseLoad, currentShip->getBaseLoad());

    liw_add_ship_stat(self, 265, String(ship->getSlots(0)), ship->getSlots(0), currentShip->getSlots(0));
    liw_add_ship_stat(self, 266, String(ship->getSlots(1)), ship->getSlots(1), currentShip->getSlots(1));
    liw_add_ship_stat(self, 267, String(ship->getSlots(2)), ship->getSlots(2), currentShip->getSlots(2));

    const int devices = ship->getSlots(3);
    const bool extraDevice = ship->getNumAddedDeviceSlots() > 0;
    const String deviceText = extraDevice
        ? String(devices) + String(" (+)", false)
        : String(devices);
    liw_add_ship_stat(self, 269, deviceText, devices, currentShip->getSlots(3));

    const float shopHandling = ship->getHandlingForShop();
    const float baseHandling = static_cast<float>(ship->getUnmoddedHandling());
    const bool extraHandling = shopHandling > baseHandling;
    const int shownHandling = static_cast<int>((extraHandling ? ship->getHandling() : shopHandling) * 100.0f);
    liw_add_ship_stat(
        self,
        164,
        extraHandling ? String(shownHandling) + String(" (+)", false) : String(shownHandling),
        static_cast<int>(shopHandling * 100.0f),
        static_cast<int>(currentShip->getUnmoddedHandling() * 100.0f));

    const String price = Layout::formatCredits(listItem->getPrice());
    liw_add_ship_stat(self, 132, price, listItem->getPrice(), currentShip->getPrice());

    String empty("", false);
    self->scrollWindow->setText(empty, liw_text(ship->getIndex() + 977));
}

void liw_add_item_rating_row(ListItemWindow *self, Item *item) {
    if (item->getType() != 0 && item->getType() != 2 && item->getSort() != 39)
        return;

    int numerator = item->getAttribute(9);
    if (numerator == 0)
        numerator = item->getAttribute(10);
    const int denominator = item->getAttribute(11);
    if (denominator == 0)
        return;

    const float rating = (static_cast<float>(numerator) / static_cast<float>(denominator)) * 1000.0f;
    const int whole = static_cast<int>(rating);
    const int fraction = static_cast<int>((rating - static_cast<float>(whole)) * 100.0f);
    String value = String(whole) + String(".", false) + String(fraction);
    liw_add_row(self, liw_text(188), value);
}

void liw_fill_item_rows(ListItemWindow *self, ListItem *listItem, bool includePrice) {
    Item *item = listItem->item;
    if (!listItem->isItem()) {
        const int itemIndex = listItem->isBluePrint()
            ? listItem->bluePrint->getIndex()
            : listItem->pendingProduct->blueprintIndex;
        item = (*Item::g_items)[itemIndex];
    }

    const int itemIndex = item->getIndex();
    if (Status::gStatus->field_54 != nullptr && itemIndex >= 0 &&
        static_cast<unsigned int>(itemIndex) < Status::gStatus->field_54->size()) {
        Status::gStatus->field_54->data()[itemIndex] = true;
    }

    for (int attribute = 0; attribute < 62; ++attribute) {
        if (liw_is_hidden_item_attribute(itemIndex, attribute))
            continue;

        const int rawValue = item->getAttribute(attribute);
        if (rawValue == kMissingItemAttribute)
            continue;

        int labelId = kItemAttributeTextIds[attribute];
        const String value = liw_format_item_attribute(listItem, item, attribute, rawValue, labelId);
        liw_add_row(self, liw_text(labelId), value);
        if (attribute == 11)
            liw_add_item_rating_row(self, item);
    }

    if (includePrice && !listItem->isBluePrint() && !listItem->isPendingProduct()) {
        const String newline("\n", false);
        const String label = newline + liw_text(132);
        const String value = newline + Layout::formatCredits(item->getSinglePrice());
        liw_add_row(self, label, value);
    }

    String empty("", false);
    self->scrollWindow->setText(empty, liw_text(listItem->getIndex() + 1041));
}

void liw_fill_rows(ListItemWindow *self, ListItem *item, bool includePrice) {
    if (item->isShip()) {
        liw_fill_ship_rows(self, item);
        return;
    }

    if (item->isItem() || item->isBluePrint() || item->isPendingProduct())
        liw_fill_item_rows(self, item, includePrice);
}

constexpr unsigned short kShipPreviewMeshes[64] = {
    0x4268, 0x4269, 0x426a, 0x426b, 0x426c, 0x426d, 0x426e, 0x426f,
    0x4270, 0x4271, 0x4272, 0x4273, 0x4274, 0x4275, 0x4276, 0x4277,
    0x4278, 0x4279, 0x427a, 0x427b, 0x427c, 0x427d, 0x427e, 0x427f,
    0x4280, 0x4281, 0x4282, 0x4283, 0x4284, 0x4285, 0x4286, 0x4287,
    0x4288, 0x4289, 0x428a, 0x428b, 0x428c, 0x433b, 0x4340, 0x4345,
    0x434b, 0x4351, 0x4357, 0x435d, 0x4363, 0x4369, 0x436f, 0x4375,
    0x437b, 0x4381, 0x4387, 0x438d, 0x4393, 0x4399, 0x439f, 0x43a5,
    0x43ab, 0x43b1, 0x43b7, 0x43bd, 0x43c3, 0x43c9, 0x43cf, 0x43d5,
};

constexpr unsigned short kShipPreviewSecondaryMeshes[64] = {
    0xffff, 0xffff, 0xffff, 0xffff, 0x4910, 0xffff, 0xffff, 0xffff,
    0x4914, 0x4915, 0xffff, 0xffff, 0x4918, 0xffff, 0xffff, 0xffff,
    0x491c, 0xffff, 0x491e, 0xffff, 0xffff, 0x4921, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x492b,
    0xffff, 0xffff, 0xffff, 0x492f, 0xffff, 0xffff, 0xffff, 0x4933,
    0xffff, 0x4935, 0x4936, 0x4937, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0x4386, 0x438c, 0xffff, 0xffff, 0x439e, 0x43a4, 0x4982,
    0x4983, 0x4984, 0x4985, 0x4986, 0x4987, 0xffff, 0xffff, 0x43da,
};

constexpr float kShipPreviewScales[64] = {
    1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f,
    1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f,
    1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f,
    1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f,
    1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f,
    1.5f, 1.5f, 1.5f, 1.5f, 1.4f, 1.3f, 1.3f, 1.3f,
    1.4f, 1.3f, 1.5f, 1.3f, 1.4f, 1.5f, 1.5f, 1.5f,
    1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f,
};

PaintCanvas *liw_canvas() {
    return static_cast<PaintCanvas *>(Globals::Canvas);
}

unsigned int liw_font() {
    return static_cast<unsigned int>(reinterpret_cast<uintptr_t>(Globals::font));
}

unsigned short liw_preview_light_texture(int race) {
    switch (race) {
    case 0: return 0x276f;
    case 1: return 0x2771;
    case 2: return 0x2772;
    case 3: return 0x2770;
    default: return 0x2773;
    }
}

void liw_build_ship_preview(ListItemWindow *self, ListItem *item) {
    PaintCanvas *canvas = liw_canvas();
    Ship *ship = item->ship;
    const int shipIndex = ship->getIndex();
    const int race = ship->getRace();

    self->previewGeometry = Globals::gGlobals->getShipGroup(shipIndex, race, true);
    canvas->TransformCreate(self->previewTransformId);
    canvas->TransformAddMesh(self->previewTransformId, kShipPreviewMeshes[shipIndex], false);

    self->previewSecondaryTransformId = 0xffffffffu;
    const unsigned short secondaryMesh = kShipPreviewSecondaryMeshes[shipIndex];
    if (secondaryMesh != 0xffff) {
        canvas->TransformCreate(self->previewSecondaryTransformId);
        canvas->TransformAddMesh(self->previewSecondaryTransformId, secondaryMesh, false);
    }

    canvas->TextureCreate(liw_preview_light_texture(race), self->previewLightingTexture, false);
    canvas->CameraCreate(self->previewCameraId);
    canvas->CameraSetPerspective(self->previewCameraId, 0.9203f, 200.0f, 30000.0f);
    canvas->CameraSetCurrent(self->previewCameraId);

    Matrix camera = *static_cast<Matrix *>(canvas->CameraGetLocal(self->previewCameraId));
    const bool specialShip = shipIndex == 51;
    const float itemX = specialShip ? 700.0f : 0.0f;
    const float itemY = specialShip ? 800.0f : 0.0f;
    const float itemZ = specialShip ? -2000.0f : 0.0f;
    float pitch = 0.3927f;

    if (Globals::iPadHD) {
        AbyssEngine::AEMath::MatrixSetTranslation(
            camera,
            1900.0f - (static_cast<float>(Globals::w - 1024) * 0.6f),
            1736.0f - ((static_cast<float>(2048 - Globals::w) * 0.6f) * 0.6f),
            -5531.0f - (static_cast<float>(Globals::h - 1536) * 0.6f));
    } else if (Globals::iPadLarge) {
        AbyssEngine::AEMath::MatrixSetTranslation(
            camera,
            1900.0f - (static_cast<float>(Globals::w - 1024) * 0.6f),
            1736.0f - ((static_cast<float>(2048 - Globals::w) * 0.6f) * 0.6f),
            -5531.0f - (static_cast<float>(Globals::h - 1536) * 0.6f));
    } else if (!Globals::iPad) {
        AbyssEngine::AEMath::MatrixSetTranslation(
            camera, itemY + (Globals::n9 ? 1750.0f : 1380.0f), itemX + 1500.0f, itemZ - 3500.0f);
        pitch = 0.5736f;
    } else {
        AbyssEngine::AEMath::MatrixSetTranslation(
            camera,
            itemY + (1274.0f - (static_cast<float>(Globals::w - 1024) * 0.6f)),
            itemX + (1736.0f - ((static_cast<float>(1024 - Globals::w) * 0.6f) * 0.6f)),
            itemZ + (-5531.0f - (static_cast<float>(Globals::h - 768) * 0.6f)));
    }
    AbyssEngine::AEMath::MatrixSetRotation(camera, pitch, 3.1416f, -0.1f);
    canvas->CameraSetLocal(self->previewCameraId, camera);

    AbyssEngine::Engine *engine = static_cast<AbyssEngine::Engine *>(
        static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->GetEngine());
    engine->LightSetLightDirection(-5.0f, 1.0f, -5.0f, 0x4000);
    engine->LightSetLightColorDiffuse(1.0f, 1.0f, 1.0f, 0x4000);
    engine->LightSetLightColorAmbient(0.25f, 0.25f, 0.25f, 0x4000);
    engine->LightSetLightColorSpecular(0.5f, 0.5f, 0.5f, 0x4000);

    canvas->Image2DCreate(0x50b, self->scrollThumbImage);
    self->scrollBarOffsetX = canvas->GetImage2DWidth(self->scrollThumbImage);
    self->scrollBarTrackLength = canvas->GetImage2DHeight(self->scrollThumbImage);
    self->scrollBarX = self->x + (self->width >> 1) + Globals::gLayout->field_0x2c +
        (((self->width >> 1) - Globals::gLayout->field_0x2c - Globals::gLayout->buttonInsetX) >> 1);
    self->scrollBarY = Globals::gLayout->field_0x20 + Globals::gLayout->field_0xc + self->y +
        (self->previewHeight >> 1);
    canvas->Image2DCreate(0x512, self->arrowDownImage);
    canvas->Image2DCreate(0x513, self->arrowUpImage);
    canvas->Image2DCreate(0x514, self->arrowEqualImage);
    self->arrowSeparator = canvas->GetImage2DWidth(self->arrowDownImage);
}

} // namespace

void ListItemWindow::OnTouchBegin(int x, int y) {
    this->scrollWindow->OnTouchBegin(x, y);
    if (this->shows3DShipFlag &&
        this->x + (this->width >> 1) < x) {
        Layout *obj = Globals::gLayout;
        if (y < this->y + obj->field_0xc + obj->field_0x20 + this->previewHeight) {
            this->dragLastX = x;
            this->dragStartX = x;
            this->dragDelta = 0;
            this->dragging = 1;
        }
    }
}

uint8_t ListItemWindow::shows3DShip() {
    return this->shows3DShipFlag;
}

void ListItemWindow::OnTouchMove(int x, int y) {
    this->scrollWindow->OnTouchMove(x, y);
    if (this->shows3DShipFlag && this->dragging) {
        int d = x - this->dragLastX;
        this->dragDelta = d;
        this->spinDamping = 1.0f;
        this->dragAccum = this->dragAccum + d;
        this->dragLastX = x;
    }
}

void ListItemWindow::OnTouchEnd(int x, int y) {
    this->scrollWindow->OnTouchEnd(x, y);
    if (this->shows3DShipFlag && this->dragging) {
        int dv = this->dragDelta;
        int sum = this->dragAccum + dv;
        float vel = (float) dv;
        int a = dv < 0 ? -dv : dv;
        float v = 0.0f;
        if (a > 3) v = vel;
        this->spinDamping = 0.9f;
        this->dragging = 0;
        this->dragAccum = sum;
        this->dragSettled = sum;
        this->spinVelocity = v;
    }
}


void ListItemWindow::render() {
    if (!this->shows3DShipFlag)
        return;

    PaintCanvas *canvas = liw_canvas();
    canvas->Begin3d();

    Layout *obj = Globals::gLayout;
    int s = obj->field_0x128;
    int h = this->previewHeight - s * 2;
    canvas->EnableClip(
        this->x + s + (this->width >> 1) + obj->field_0x2c,
        this->y + s + obj->field_0xc + obj->field_0x20,
        ((this->width >> 1) - (obj->field_0x2c + s * 2)) - obj->buttonInsetX,
        h);
    canvas->SetColor(0xffffffffu);
    this->previewCameraLocal = *static_cast<Matrix *>(canvas->CameraGetLocal(this->previewCameraId));
    this->previewGeometry->render();
    canvas->End3d();
    canvas->DisableClip();
}

void ListItemWindow::set(ListItem *item, unsigned p2, unsigned p3,
                         unsigned p4, unsigned p5, bool p6) {
    this->item = item;
    this->param2 = p2;
    this->param3 = p3;
    this->param4 = p4;
    this->param5 = p5;

    Layout *layout = Globals::gLayout;

    int w, h, x, y;
    this->previewScaleBias = 0.0f;
    if (!Globals::iPad) {
        this->x = 0;
        this->y = 0;
        w = Globals::w;
        h = Globals::h;
        this->width = w;
        this->height = h;
        x = 0;
        y = 0;
    } else {
        if (Globals::iPadHD) {
            w = 914;
            h = 562;
            this->previewScaleBias = -0.3f;
        } else {
            w = Globals::iPadLarge ? 1300 : 650;
            h = Globals::iPadLarge ? 800 : 400;
            this->previewScaleBias = Globals::retinaDisplay ? -0.4f : -0.2f;
        }
        x = (Globals::w >> 1) - (w >> 1);
        y = (Globals::h >> 1) - (h >> 1);
        this->x = x;
        this->y = y;
        this->width = w;
        this->height = h;
    }
    layout->setWindowDimensions(x, y, w, h);

    if (this->labels != 0) {
        ArrayReleaseClasses(*this->labels); ArrayRemoveAll(*(this->labels));
        delete this->labels;
    }
    this->labels = 0;

    if (this->values != 0) {
        ArrayReleaseClasses(*this->values); ArrayRemoveAll(*(this->values));
        delete this->values;
    }
    this->values = 0;

    this->labels = new Array<String *>();
    this->values = new Array<String *>();

    int isShip = this->item->isShip();
    if (isShip == 0) {
        this->previewHeight = 0;
        this->shows3DShipFlag = 0;
    } else {
        this->previewHeight =
                ((((this->height - layout->field_0xc) - layout->field_0x10) - layout->field_0x20) - layout->field_0x24)
                / 2
                - layout->field_0x2c;
        this->shows3DShipFlag = 1;
        liw_build_ship_preview(this, item);

        this->statsCur = new Array<int>();
        this->statsPrev = new Array<int>();
    }

    {
        int progH = this->previewHeight;

        int sel = (progH > 0) ? layout->field_0x1c : layout->field_0x5c;
        int rowH = layout->field_0x2c;
        int sx = this->x + rowH + (this->width >> 1);
        int sy = layout->field_0x20 + this->y + rowH + layout->field_0xc + progH + sel;
        int sw = (this->width >> 1) - layout->buttonInsetX;
        int sh = ((this->height -
                   (layout->field_0xc + rowH * 2 + layout->field_0x20 + progH + sel))
                  - layout->field_0x10) - layout->field_0x24;
        this->scrollWindow = new ScrollTouchWindow(sx, sy, sw, sh, false);
    }

    liw_fill_rows(this, item, p6);

    this->dragging = 0;
    this->previewAngle = 0.0f;
    this->dragAccum = 0x104;
    this->dragLastX = 0;
    this->dragSettled = 0;
    this->dragDelta = 0;
    this->spinDamping = 0.0f;
    this->spinVelocity = 0.0f;
    this->dragStartX = 0;
}

ListItemWindow::~ListItemWindow() {
    if (this->labels) {
        ArrayReleaseClasses(*this->labels); ArrayRemoveAll(*(this->labels));
        delete this->labels;
        this->labels = 0;
    }
    if (this->values) {
        ArrayReleaseClasses(*this->values); ArrayRemoveAll(*(this->values));
        delete this->values;
        this->values = 0;
    }
    if (this->statsCur) {
        ArrayRemoveAll(*(this->statsCur));
        delete this->statsCur;
        this->statsCur = 0;
    }
    if (this->statsPrev) {
        ArrayRemoveAll(*(this->statsPrev));
        delete this->statsPrev;
        this->statsPrev = 0;
    }
    delete this->scrollWindow;
    this->scrollWindow = 0;
}


void ListItemWindow::draw() {
    Layout *layout = Globals::gLayout;
    PaintCanvas *canvas = liw_canvas();
    const bool masked = Globals::iPad != 0;

    if (masked)
        layout->drawMask();

    canvas->SetColor(0xffu);
    canvas->FillRectangle(this->x, this->y, this->width, this->height);

    String empty("", false);
    layout->drawBox(2, this->x, this->y, this->width, this->height, empty, 0);
    if (masked) {
        layout->drawBox(7, this->x, this->y, this->width, this->height, empty, 0);
    }

    String title(*GameText::gGameText->getText(390), false);
    layout->drawHeader(title);

    ListItem *li = this->item;
    const bool isShip = li->isShip();
    canvas->SetColor(0xffffffffu);
    const int iconX = this->x + layout->buttonInsetX + layout->field_0x2c;
    const int iconY = this->y + layout->field_0x124 + layout->field_0xc + layout->field_0x20 +
        layout->field_0x5c / 2 - layout->field_0x2c8 / 2;
    ImageFactory *imageFactory = static_cast<ImageFactory *>(Globals::imageFactory);

    if (li->isItem() || li->isBluePrint() || li->isPendingProduct()) {
        String itemTitle(*GameText::gGameText->getText(li->getIndex() + 1274), false);
        layout->drawBox(1, this->x + layout->buttonInsetX, this->y + layout->field_0xc + layout->field_0x20,
                        (this->width >> 1) - (layout->field_0x2c + layout->buttonInsetX), layout->field_0x5c,
                        itemTitle, 2);

        Item *itemPtr = li->item;
        if (!li->isItem()) {
            const int itemIndex = li->isBluePrint() ? li->bluePrint->getIndex() : li->pendingProduct->blueprintIndex;
            itemPtr = (*Item::g_items)[itemIndex];
        }
        imageFactory->drawItem(itemPtr->getIndex(), itemPtr->getType(), iconX, iconY);
    } else if (isShip) {
        String shipTitle(*GameText::gGameText->getText(li->ship->getIndex() + 913), false);
        layout->drawBox(1, this->x + layout->buttonInsetX, this->y + layout->field_0xc + layout->field_0x20,
                        (this->width >> 1) - (layout->field_0x2c + layout->buttonInsetX), layout->field_0x5c,
                        shipTitle, 2);
        imageFactory->drawShip(li->ship->getIndex(), iconX, iconY);
    }

    Array<String *> *rows = this->labels;
    if (rows) {
        const unsigned int n = rows->size();
        int rowH = layout->field_0x2c;
        int yTop = layout->field_0x5c + layout->field_0xc + this->y + layout->field_0x20 + rowH;
        if (yTop + (layout->field_0x1c + rowH) * static_cast<int>(n) > Globals::h - layout->field_0x10)
            rowH = 2;
        int ycur = yTop;
        for (unsigned int i = 0; i < n; ++i) {
            canvas->SetColor(0x255d8dffu);
            String s(*rows->data()[i], false);
            layout->drawBox(6, layout->buttonInsetX + this->x, ycur,
                            (this->width >> 1) - (layout->field_0x2c + layout->buttonInsetX), layout->field_0x1c, s, 0);
            canvas->SetColor(0xffffffffu);

            String *value = this->values->data()[i];
            int textX;
            if (isShip && i < this->statsCur->size()) {
                const unsigned int cur = this->statsCur->size();
                if (i < cur) {
                    if (i < cur - 1) {
                        const int current = this->statsCur->data()[i];
                        const int previous = this->statsPrev->data()[i];
                        unsigned int arrow = this->arrowEqualImage;
                        if (current < previous)
                            arrow = this->arrowDownImage;
                        else if (previous < current)
                            arrow = this->arrowUpImage;
                        canvas->DrawImage2D(arrow,
                                            this->x + (this->width >> 1) - layout->field_0x2c - this->arrowSeparator,
                                            ycur);
                    }
                    textX = this->x + (this->width >> 1) + 10 - layout->field_0x2c -
                        this->arrowSeparator - canvas->GetTextWidth(liw_font(), *value);
                } else {
                    textX = this->x + (this->width >> 1) - layout->field_0x2c * 2 -
                        canvas->GetTextWidth(liw_font(), *value);
                }
            } else {
                textX = this->x + (this->width >> 1) - layout->field_0x2c * 2 -
                    canvas->GetTextWidth(liw_font(), *value);
            }
            canvas->DrawString(liw_font(), *value, textX,
                               ycur + layout->field_0x1c / 2 - this->textHalfHeight, false);
            ycur = ycur + rowH + layout->field_0x1c;
        }
    }

    if (this->previewHeight < 1) {
        String s(*GameText::gGameText->getText(280), false);
        layout->drawBox(1, this->x + (this->width >> 1) + layout->field_0x2c,
                        this->y + layout->field_0xc + layout->field_0x20,
                        ((this->width >> 1) - layout->field_0x2c) - layout->buttonInsetX, layout->field_0x5c, s, 0);
    } else {
        canvas->SetColor(0x255d8dffu);
        layout->drawBox(8, this->x + (this->width >> 1) + layout->field_0x2c,
                        this->y + layout->field_0xc + layout->field_0x20,
                        ((this->width >> 1) - layout->field_0x2c) - layout->buttonInsetX, this->previewHeight, empty,
                        0);
        String previewTitle(*GameText::gGameText->getText(280), false);
        layout->drawBox(0, this->x + (this->width >> 1) + layout->field_0x2c,
                        this->y + layout->field_0xc + layout->field_0x20 + this->previewHeight + layout->field_0x2c,
                        ((this->width >> 1) - layout->field_0x2c) - layout->buttonInsetX, layout->field_0x1c,
                        previewTitle, 0);
        canvas->SetColor(0xffffffffu);
        const int scrollY = this->scrollBarY - this->scrollBarTrackLength / 3;
        canvas->DrawImage2D(this->scrollThumbImage, this->scrollBarX - this->scrollBarOffsetX, scrollY);
        canvas->DrawImage2D(this->scrollThumbImage, this->scrollBarX, scrollY, 1);
    }

    this->scrollWindow->drawTextBG();
    this->scrollWindow->draw();
}

void ListItemWindow::update(int frameTime) {
    this->scrollWindow->update(frameTime);

    if (this->shows3DShipFlag == 0)
        return;

    if (this->dragging == 0) {
        float spin = this->spinDamping * this->spinVelocity;
        float mag = spin > 0.0f ? spin : -spin;
        this->spinVelocity = spin;
        if (mag > 1.0f) {
            float angle = (float) this->dragAccum;
            this->dragAccum = (int) (spin + angle);
        }
    }

    int idx = this->item->ship->getIndex();

    float angle = static_cast<float>(this->dragAccum) / 120.0f;
    this->previewAngle = angle;

    PaintCanvas *canvas = liw_canvas();
    Matrix *loc = static_cast<Matrix *>(canvas->TransformGetLocal(this->previewTransformId));
    AbyssEngine::AEMath::MatrixSetRotation(*loc, 0.0f, angle, 0.0f);
    const float scale = kShipPreviewScales[idx] + this->previewScaleBias;
    loc = static_cast<Matrix *>(canvas->TransformGetLocal(this->previewTransformId));
    AbyssEngine::AEMath::MatrixSetScaling(*loc, scale, scale, scale);

    if (this->previewSecondaryTransformId != 0xffffffffu) {
        loc = static_cast<Matrix *>(canvas->TransformGetLocal(this->previewSecondaryTransformId));
        AbyssEngine::AEMath::MatrixSetRotation(*loc, 0.0f, angle, 0.0f);
        loc = static_cast<Matrix *>(canvas->TransformGetLocal(this->previewSecondaryTransformId));
        AbyssEngine::AEMath::MatrixSetScaling(*loc, scale, scale, scale);
    }

    this->previewGeometry->setRotation(0.0f, angle, 0.0f);
}

ListItemWindow::ListItemWindow() {
    this->labels = 0;
    this->values = 0;
    this->statsCur = 0;
    this->statsPrev = 0;
    this->previewGeometry = 0;
    this->item = 0;
    this->scrollWindow = 0;
    PaintCanvas *canvas = liw_canvas();
    int h = canvas->GetTextHeight(static_cast<unsigned int>(reinterpret_cast<uintptr_t>(Globals::font)));
    this->textHalfHeight = h / 2 - 1;
    this->previewHeight = 0;
}
