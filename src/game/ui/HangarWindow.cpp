#include "game/ui/HangarWindow.h"
#include <algorithm>
#include "game/world/SolarSystem.h"
#include "engine/render/PaintCanvas.h"
#include "game/ui/ChoiceWindow.h"
#include "engine/audio/FModSound.h"
#include "game/core/HangarList.h"
#include "game/mission/Item.h"
#include "game/ui/ListItemWindow.h"
#include "game/mission/BluePrint.h"
#include "engine/render/ImageFactory.h"
#include "game/mission/RecordHandler.h"
#include "game/world/Station.h"
#include "game/core/Globals.h"
#include "game/mission/Status.h"
#include "game/ui/ListItem.h"
#include "game/mission/PendingProduct.h"
#include "game/ship/Ship.h"
#include "engine/core/NFC.h"
#include "engine/core/GameText.h"
#include "engine/core/GameData.h"
#include "game/menu/ModStation.h"

// PaintCanvas::gCanvas is declared in engine/render/PaintCanvas.h (included above).

static const String g_HangarWindow_emptyDialogText;

void Status_replaceHash(...);

static inline int IDIV(int a, int b) { return a / b; }
static inline unsigned int UDIV(unsigned int a, unsigned int b) { return a / b; }

// Array<TouchButton*> at HangarWindow+0x20. The allocation order and all
// action destinations are confirmed by the Android-HD initialize/render/input
// bodies. Keep the numeric layout stable: ARM code owns 24 button slots.
enum HangarButtonSlot : unsigned int {
    kHangarButtonInspect = 0,
    kHangarButtonSelectShip = 1,
    kHangarButtonSelectItemFirst = 2,
    kHangarButtonSelectItemSecond = 3,
    kHangarButtonSelectItemThird = 4,
    kHangarButtonSellListEntry = 5,
    kHangarButtonMoveToCargoEntry = 6,
    kHangarButtonSelectBlueprint = 7,
    kHangarButtonCurrentAmount = 8,
    kHangarButtonStationAmount = 9,
    kHangarButtonSellShip = 10,
    kHangarButtonCredits = 11,
    kHangarButtonPaidCreditsFirst = 12,
    kHangarButtonPaidCreditsLast = 16,
    kHangarButtonCreditsMore = 17,
    kHangarButtonFreeCreditsFirst = 18,
    kHangarButtonFreeCreditsLast = 22,
    kHangarButtonBlueprintAutoComplete = 23,
    kHangarButtonCount = 24,
};

void *AppManager_GetApplicationData();

void *AppManager_GetApplicationModule(unsigned int id);

void *ApplicationManager_GetApplicationData();

void Status_changeCredits(void *globals);

// File-local globals. Defined once here as file-statics; only referenced
// within this translation unit.
static void **g_hw_globals;
static Layout **g_hw_layout;
static int *g_hw_screenWidth;
static int *g_hw_screenHeight;
static void **g_hw_font;
static RecordHandler **g_hw_recordHandler;
static uint8_t *g_hw_optionFlags;
static unsigned int *g_hw_dlcModuleId;
static uint8_t *g_hw_blackMarketHintFlag;
static uint8_t *g_hw_introHintFlag;
static int *g_hw_headerTextId;
static int *g_hw_shipNameBase;
static int *g_hw_itemNameBase;
static float g_hw_lineScale;
static unsigned int *g_hw_modStationId;
static int *g_hw_bpDoneTextId;
static int *g_hw_notEnoughTextId;
static int *g_hw_helpTextId;
static int *g_hw_sellShipTextId;
static int *g_hw_baseTextId;
static FModSound **g_hw_sound;
static int *g_hw_bpTextId;
static int *g_hw_buyTextId;
static int *g_hw_buyTextId2;
static char *g_hw_buyFlag;
static int *g_hw_buyWidth;
static int *g_hw_buyHeight;
static void **g_hw_itemFlags;
static int *g_hw_sellTextId1;
static int *g_hw_sellTextId2;
static int *g_hw_routesTextId;
static int *g_hw_bpStations;
static int *g_hw_unsaleableTextId;
static int *g_hw_slotMsgTextId;
static int *g_hw_buyBaseTextId;
static int *g_hw_sellMsgTextId1;
static int *g_hw_sellMsgTextId2;
static int *g_hw_equipTextId;
static int *g_hw_freeCreditsTextId;
static void **g_hw_posXArray;
static void **g_hw_posYArray;
static void **g_hw_imageCountSlot;
static uint8_t *g_hw_specialModeFlag;
static uint8_t *g_hw_listModeFlag;
static float g_hw_posScale;
static int *g_hw_openCounter;

// File-local string/data symbols.
static const char hw_rnd_empty[1] = "", hw_rnd_a[1] = "", hw_rnd_b[1] = "",
        hw_rnd_c[1] = "", hw_rnd_d[1] = "", hw_rnd_e[1] = "", hw_rnd_x[1] = "";
static const char hw_buy_yes[1] = "", hw_buy_no[1] = "", hw_buy_icon[1] = "";
static const char hw_buy2_yes[1] = "", hw_buy2_no[1] = "", hw_buy2_icon[1] = "";
static float hw_buy_heightScale;
static const char hw_init_buy[1] = "", hw_init_sell[1] = "", hw_init_lbl[1] = "",
        hw_init_more[1] = "", hw_init_back[1] = "", hw_init_help[1] = "";
// Android 0x218269..0x21826b are bytes 0x1d..0x1f of Globals::hints.
// RecordHandler saves them with the game record; GameRecord::load restores
// them as part of the 0x18..0x27 hint block.
static uint8_t &g_hangarStoreBuyHintShown = Globals::hints[0x1d];
static uint8_t &g_hangarStoreSellHintShown = Globals::hints[0x1e];
static uint8_t &g_hangarSlotHintShown = Globals::hints[0x1f];
// Android dword_202844 at 0x202844. Slot zero is the record-store action;
// slots 1..4 are the four visible social-credit offers; slot 5 is retained
// in the original table even though the touch loop stops before it.
static constexpr int kHangarSocialCreditReward[6] = {0, 7500, 5000, 5000, 5000, 7500};

// Android addresses 0x218230..0x218236 belong to Globals::options at
// 0x2181e8. RecordHandler loads and saves these exact bytes. Slot zero of the
// offer flags is reserved; slots 1..5 are the five social offers. The viewed
// byte also gates the first hangar introduction dialog.
static uint8_t &g_hangarCreditOfferShown = Globals::options[0x4e];
static uint8_t *const g_hangarSocialCreditClaimed = Globals::options + 0x48;

bool HangarWindow::isInSpecialMode() {
    if (this->specialMode != 0) return true;
    return this->dialogActive != 0;
}



void HangarWindow::refreshCurrentContentHeight() {
    Array<ListItem *> *items = ((HangarList *) this->hangarList)->getCurrentTabItems();
    if (items != 0) {
        int n = (int) items->size();
        Layout *layout = static_cast<Layout *>(Globals::layout);
        int rowH = layout->field_0x70;
        this->currentContentHeight = this->rowLayoutMetrics.rowGap * (n - 1) + n * rowH;
    }
}

bool HangarWindow::listMode() {
    HangarWindow *self = this;
    if (self->viewMode == 0 && self->specialMode == 0) {
        return self->dialogActive == 0;
    }
    return false;
}

bool HangarWindow::readyToClose() {
    HangarWindow *self = this;
    int tab = self->hangarList->getCurrentTab();
    if (tab == 4 && self->buyMode != 0 && self->bluePrintBuyCount > 0 &&
        self->dialogActive == 0 && self->bluePrint->isEmpty() == 0) {
        int si = self->bluePrint->getStationIndex();
        Station *st = Status::gStatus->getStation();
        if (si != st->getIndex()) {
            return false;
        }
    }
    return self->dialogActive == 0;
}

void HangarWindow::render3D() {
    if (this->viewMode == 1) {
        this->listItemWindow->render();
    }
}

bool HangarWindow::currentItemIsHighlighted() {
    ListItem *item = (ListItem *) this->hangarList->getCurrentItem();
    if (item == 0) return false;
    return item == this->selectedItem;
}

HangarWindow::~HangarWindow() {
    delete this->hangarList;
    this->hangarList = nullptr;
    delete this->listItemWindow;
    this->listItemWindow = nullptr;
    delete this->choiceWindow;
    this->choiceWindow = nullptr;
    delete this->dialog;
    this->dialog = nullptr;
    if (this->tabButtons != nullptr) {
        ArrayReleaseClasses(*this->tabButtons);
        delete this->tabButtons;
    }
    this->tabButtons = nullptr;
    if (this->buttons != nullptr) {
        ArrayReleaseClasses(*this->buttons);
        delete this->buttons;
    }
    void *icons = this->tabIcons;
    this->active = 0;
    this->buttons = nullptr;
    if (icons != nullptr)
        ::operator delete[](icons);
    this->tabIcons = nullptr;
}

float HangarWindow::getRelativeScrollStartPos() {
    int range = this->scrollOffset;
    float position;
    if (range <= 0)
        position = (float) -(float) range / (float) this->currentContentHeight;
    else
        position = 0.0f;
    return position;
}

void HangarWindow::hideMessage() {
    this->dialogActive = 0;
}
















// Android 0x1490d8: snapshots, repeated slot reads and String cleanup boundaries
// follow the native call order. Button/payload arrays belong to initialize().
void HangarWindow::render() {
    Layout *&layout = Globals::layout;
    PaintCanvas *&canvas = Globals::Canvas;
    Status *&status = Globals::status;
    GameText *&gameText = Globals::gameText;

    const unsigned int &font = Globals::font;

    if (layout == nullptr || canvas == nullptr || status == nullptr || gameText == nullptr) {
        return;
    }

    auto textFor = [&gameText](int id) __attribute__((always_inline))->const String & {
        return *gameText->getText(id);
    };
    auto buttonAt = [this](unsigned int index) __attribute__((always_inline))->TouchButton * {
        return (*this->buttons)[index];
    };
    auto itemTypeAt = [](int index) __attribute__((always_inline))->int {
        Array<Item *> *itemTable = static_cast<Array<Item *> *>(Globals::items);
        return (*itemTable)[index]->getType();
    };

    canvas->SetColor(0xffffffffu);
    ModStation *stationModule = static_cast<ModStation *>(
        static_cast<ApplicationManager *>(Globals::appManager)->GetApplicationModule(5));

    if (stationModule == nullptr || stationModule->closeHangarAfterCredits == 0) {
        int tab2 = this->viewMode;
        if (tab2 == 0) {
            layout->drawBG();
            unsigned int tab = this->hangarList->getCurrentTab();
            Array<ListItem *> *items = ((HangarList *)this->hangarList)->getCurrentTabItems();
            if (items != 0) {
                float startPos = this->getRelativeScrollStartPos();
                float visH = (float)this->visibleHeight;
                float scrollH = this->getRelativeScrollHeight();
                int scrollPx = (int)(scrollH * visH);
                int startPx = (int)(startPos * visH);

                int topY = Globals::w;
                if (scrollPx > 0)
                    topY = (topY - layout->field_0x48) - layout->field_0x2c;

                int rowGap;
                if (tab == 0)
                    rowGap = layout->field_0x4c << 1;
                else if (tab == 4 || tab == 1)
                    rowGap = layout->field_0x50;
                else
                    rowGap = 0;

                topY += (layout->field_0x28 + this->hintOffsetX) * -2;
                int baseY = layout->field_0x2cc;
                int colW = layout->field_0x4c;

                if (Globals::iPad != 0 && Globals::iPadAssetsWithLowerRes == 0 && this->hintImage != 0) {
                    int iw = canvas->GetImage2DWidth(this->hintImage);
                    int ih = canvas->GetImage2DHeight(this->hintImage);
                    int rows = ih == 0 ? 0 : IDIV(Globals::h, ih);
                    int y = 0;
                    for (int r = 0; r <= rows; r++) {
                        canvas->DrawImage2D((unsigned)this->hintImage,
                                            (layout->field_0x28 - iw) + this->hintOffsetX, y,
                                            (unsigned char)1);
                        int off = (scrollPx < 1) ? 0 : (layout->field_0x48 + layout->field_0x2c);
                        canvas->DrawImage2D((unsigned)this->hintImage,
                                            this->hintOffsetX + layout->field_0x28 + topY + off, y,
                                            (unsigned char)0);
                        y += ih;
                    }
                }

                int contentBase = colW + baseY + rowGap;

                for (unsigned int i = 0; i != kHangarButtonCount; i++) {
                    if (this->dragging == 0) {
                        TouchButton *btn = buttonAt(i);
                        if (btn != nullptr)
                            btn->setVisible(false);
                    }
                }

                int boxW = rowGap - 2;

                for (unsigned int i = 0; i < items->size(); i++) {
                    int y = (layout->field_0x70 + this->rowLayoutMetrics.rowGap) * (int)i +
                            this->scrollOffset + layout->field_0x20 + layout->field_0xc;
                    if (y < 0 || y > Globals::h)
                        continue;

                    ListItem *li = items->data()[i];
                    if (li->isSelectable() != 0) {
                        if (this->selectedItem == li && li->isTextButton() == 0) {
                            if (tab == 0 && li->inTabIndex >= 0) {
                                layout->drawBox(10, this->hintOffsetX + layout->field_0x28, y, topY,
                                                layout->field_0x70, String("", false));
                            } else {
                                layout->drawBox(4, this->hintOffsetX + layout->field_0x28, y, topY,
                                                layout->field_0x70, String("", false));
                            }
                        } else if (tab != 0 || li->inTabIndex < 0) {
                            layout->drawBox(3, this->hintOffsetX + layout->field_0x28, y, topY,
                                            layout->field_0x70, String("", false));
                        } else {
                            layout->drawBox(9, this->hintOffsetX + layout->field_0x28, y, topY,
                                            layout->field_0x70, String("", false));
                        }
                    }

                    canvas->SetColor(0xffffffffu);
                    String label;
                    int occupiedRightWidth = 0;

                    if (!li->isItem()) {
                        if (li->isShip()) {
                            GameText *labelText = gameText;
                            const int shipIndex = li->ship->getIndex();
                            label = *labelText->getText(shipIndex + 913);
                            canvas->SetColor(0x777777ffu);
                            if (tab == 1 && this->upgradeMode != 0) {
                                PaintCanvas *priceCanvas = canvas;
                                const unsigned int priceFont = font;
                                String price = layout->formatCredits(li->ship->getPrice());
                                priceCanvas->DrawString(
                                    priceFont, price, (this->hintOffsetX + layout->field_0x28 + contentBase),
                                    (y + layout->field_0x70 / 2 + 1), false);
                            } else if (tab == 1 || i >= 2) {
                                const int difference = li->ship->getPrice() - status->getShip()->getPrice();
                                canvas->SetColor(difference <= status->getCredits() ? 0x7aa35bffu
                                                                                    : 0xa35b5bffu);
                                PaintCanvas *priceCanvas = canvas;
                                const unsigned int priceFont = font;
                                String price = layout->formatCredits(li->ship->getPrice() -
                                                                     status->getShip()->getPrice());
                                priceCanvas->DrawString(
                                    priceFont, price, (this->hintOffsetX + layout->field_0x28 + contentBase),
                                    (y + layout->field_0x70 / 2 + 1), false);
                            } else {
                                PaintCanvas *priceCanvas = canvas;
                                const unsigned int priceFont = font;
                                String price = layout->formatCredits(li->ship->getPrice());
                                priceCanvas->DrawString(
                                    priceFont, price, (this->hintOffsetX + layout->field_0x28 + contentBase),
                                    (y + layout->field_0x70 / 2 + 1), false);
                            }

                            ImageFactory *factory = static_cast<ImageFactory *>(Globals::imageFactory);
                            const int imageIndex = li->ship->getIndex();
                            factory->drawShip(imageIndex, this->hintOffsetX + layout->field_0x28 + rowGap,
                                              this->iconOffsetY + y);

                            canvas->SetColor(0xffffffffu);
                        } else if (li->isSlot()) {
                            label = textFor(174);
                            if (tab == 4 && i + 1 == items->size()) {

                                (*this->buttons)[kHangarButtonBlueprintAutoComplete]->setPosition(
                                    this->hintOffsetX + layout->field_0x28 + topY / 2,
                                    this->rowActionOffsetY + y, 0x14);
                                (*this->buttons)[kHangarButtonBlueprintAutoComplete]->setVisible(true);
                                (*this->buttons)[kHangarButtonBlueprintAutoComplete]->draw();

                                label = String("");
                            }
                        } else if (li->isBluePrint()) {
                            GameText *labelText = gameText;
                            const int blueprintIndex = li->bluePrint->getIndex();
                            label = *labelText->getText(blueprintIndex + 1274);
                            if (li->bluePrint->getCompletionRate() > 0.0f) {
                                const float completion = li->bluePrint->getCompletionRate();
                                canvas->DrawImage2D(
                                    this->progressBarBgImage,
                                    (this->hintOffsetX + layout->field_0x28 + contentBase + 2),
                                    (y + layout->field_0x70 / 2 + 1));
                                const int fillWidth = static_cast<int>(completion * this->progressBarWidth);
                                canvas->DrawRegion2D(
                                    this->progressBarFillImage, 0, 0, fillWidth, this->progressBarHeight,
                                    0.0f, 0, 0,
                                    (this->hintOffsetX + layout->field_0x28 + contentBase + 2) + 1,
                                    (y + layout->field_0x70 / 2 + 1) + 1);
                                canvas->DrawImage2D(
                                    this->progressBarBorderImage,
                                    (this->hintOffsetX + layout->field_0x28 + contentBase + 2) + 3 +
                                        static_cast<int>(completion * (this->progressBarWidth - 4)),
                                    y + layout->field_0x70 / 2 + 2, 0x11, 0x14);
                                canvas->SetColor(0x777777ffu);
                                String progressLabel = String(static_cast<int>(completion * 100.0f)) +
                                                       String("%  (", false) + li->bluePrint->stationName +
                                                       String(")", false);
                                canvas->DrawString(
                                    font, progressLabel,
                                    (this->hintOffsetX + layout->field_0x28 + contentBase + 2) +
                                        this->progressBarWidth + layout->field_0x2c,
                                    y + layout->field_0x70 / 2 + layout->field_0x260 + this->rowActionOffsetY,
                                    false);
                                canvas->SetColor(0xffffffffu);
                            }

                            ImageFactory *factory = static_cast<ImageFactory *>(Globals::imageFactory);
                            const int imageIndex = li->bluePrint->getIndex();
                            const int imageType = itemTypeAt(li->bluePrint->getIndex());
                            factory->drawItem(imageIndex, imageType,
                                              this->hintOffsetX + layout->field_0x28 + rowGap,
                                              this->iconOffsetY + y);

                            if (li->craftable != 0) {
                                canvas->SetColor(0x00ed00ffu);
                            }
                        } else if (li->isPendingProduct()) {
                            label = (li->pendingProduct->quantity >= 2
                                         ? String(li->pendingProduct->quantity) + String("x ", false)
                                         : String("", false)) +
                                    textFor(li->pendingProduct->blueprintIndex + 1274);
                            canvas->SetColor(0x777777ffu);
                            PaintCanvas *subtitleCanvas = canvas;
                            const unsigned int subtitleFont = font;
                            subtitleCanvas->DrawString(subtitleFont,
                                                       *gameText->getText(275) + String(" ", false) +
                                                           li->pendingProduct->stationName,
                                                       this->hintOffsetX + layout->field_0x28 + contentBase,
                                                       y + layout->field_0x70 / 2 + 1, false);
                            canvas->SetColor(0xffffffffu);

                            const int imageIndex = li->pendingProduct->blueprintIndex;
                            ImageFactory *factory = static_cast<ImageFactory *>(Globals::imageFactory);
                            const int imageType = itemTypeAt(imageIndex);
                            factory->drawItem(imageIndex, imageType,
                                              this->hintOffsetX + layout->field_0x28 + rowGap,
                                              this->iconOffsetY + y);

                        } else if (li->isMoveToCargoButton()) {

                            (*this->buttons)[kHangarButtonMoveToCargoEntry]->setPosition(
                                this->hintOffsetX + layout->field_0x28, y, 0x11);
                            (*this->buttons)[kHangarButtonMoveToCargoEntry]->setVisible(true);
                            (*this->buttons)[kHangarButtonMoveToCargoEntry]->draw();
                            continue;

                        } else if (li->isSellButton()) {

                            (*this->buttons)[kHangarButtonSellListEntry]->setPosition(
                                this->hintOffsetX + layout->field_0x28, y, 0x11);
                            (*this->buttons)[kHangarButtonSellListEntry]->setVisible(true);
                            (*this->buttons)[kHangarButtonSellListEntry]->draw();
                            continue;

                        } else {
                            layout->drawBox(0, this->hintOffsetX + layout->field_0x28,
                                            y + layout->field_0x70 - layout->field_0x1c, topY,
                                            layout->field_0x1c, String(*li->name, false));
                            continue;
                        }
                    } else {
                        GameText *labelText = gameText;
                        const int itemIndex = li->item->getIndex();
                        label = *labelText->getText(itemIndex + 1274);
                        if (tab != 4 && tab != 1)
                            goto itemCommon;
                        {
                            int leftAmount;
                            int rightAmount;
                            if (li == this->selectedItem) {
                                if (this->hangarList->getCurrentTab() == 1 ||
                                    (this->hangarList->getCurrentTab() == 4 && li->isItem())) {
                                    const int stationAmount =
                                        tab == 1 ? li->item->getStationAmount() : li->item->getAmount();
                                    const int currentAmount =
                                        tab == 1 ? li->item->getAmount()
                                                 : this->bluePrint->getCurrentAmount(li->item->getIndex());

                                    if (currentAmount >= 1) {
                                        (*this->buttons)[kHangarButtonCurrentAmount]->setPosition(
                                            this->hintOffsetX + layout->field_0x28,
                                            this->rowActionOffsetY + y, 0x11);
                                        (*this->buttons)[kHangarButtonCurrentAmount]->setVisible(true);
                                        (*this->buttons)[kHangarButtonCurrentAmount]->draw();
                                    }

                                    if (stationAmount >= 1) {
                                        (*this->buttons)[kHangarButtonStationAmount]->setPosition(
                                            (this->hintOffsetX + layout->field_0x28 + topY),
                                            this->rowActionOffsetY + y, 0x12);
                                        (*this->buttons)[kHangarButtonStationAmount]->setVisible(true);
                                        (*this->buttons)[kHangarButtonStationAmount]->draw();
                                    }

                                    occupiedRightWidth =
                                        (*this->buttons)[kHangarButtonStationAmount]->getWidth() +
                                        layout->field_0x2c;
                                }

                                canvas->SetColor(0xffffffffu);
                                if (tab == 1) {
                                    PaintCanvas *leftCaptionCanvas = canvas;
                                    const unsigned int leftCaptionFont = font;
                                    const String *leftWidthText = gameText->getText(136);
                                    leftCaptionCanvas->DrawString(
                                        leftCaptionFont, *leftWidthText,
                                        this->hintOffsetX + layout->field_0x28 +
                                            this->rowLayoutMetrics.field_0x00 -
                                            canvas->GetTextWidth(font, *gameText->getText(136)) / 2,
                                        y + 2, false);
                                    PaintCanvas *rightCaptionCanvas = canvas;
                                    const unsigned int rightCaptionFont = font;
                                    const String *rightWidthText = gameText->getText(183);
                                    rightCaptionCanvas->DrawString(
                                        rightCaptionFont, *rightWidthText,
                                        topY + this->hintOffsetX - this->rowLayoutMetrics.field_0x00 -
                                            canvas->GetTextWidth(font, *gameText->getText(183)) / 2,
                                        y + 2, false);
                                } else {
                                    PaintCanvas *leftCaptionCanvas = canvas;
                                    const unsigned int leftCaptionFont = font;
                                    const String *leftWidthText = gameText->getText(183);
                                    leftCaptionCanvas->DrawString(
                                        leftCaptionFont, *leftWidthText,
                                        this->hintOffsetX + layout->field_0x28 +
                                            this->rowLayoutMetrics.field_0x00 - layout->field_0x254 -
                                            canvas->GetTextWidth(font, *gameText->getText(183)) / 2,
                                        y + 2, false);
                                    PaintCanvas *rightCaptionCanvas = canvas;
                                    const unsigned int rightCaptionFont = font;
                                    const String *rightWidthText = gameText->getText(271);
                                    rightCaptionCanvas->DrawString(
                                        rightCaptionFont, *rightWidthText,
                                        topY + this->hintOffsetX - this->rowLayoutMetrics.field_0x00 +
                                            layout->field_0x258 -
                                            canvas->GetTextWidth(font, *gameText->getText(271)) / 2,
                                        y + 2, false);
                                }
                                if (tab != 1)
                                    goto blueprintAmounts;
                            } else {
                                occupiedRightWidth = 0;
                                if (tab != 1)
                                    goto blueprintAmounts;
                            }
                            leftAmount = li->item->getStationAmount();
                            rightAmount = li->item->getAmount();
                            goto amountsReady;
                        blueprintAmounts : {
                            leftAmount = li->item->getAmount();
                            const int currentAmount = this->bluePrint->getCurrentAmount(li->item->getIndex());
                            rightAmount = currentAmount + li->item->getBlueprintAmount();
                        }
                        amountsReady:

                            String leftText = String(leftAmount) + String("t", false);
                            String rightText =
                                (tab == 1
                                     ? String(rightAmount)
                                     : String(rightAmount) + String("/", false) +
                                           String(this->bluePrint->getTotalAmount(li->item->getIndex()))) +
                                String("t", false);

                            canvas->SetColor(0xffffffffu);
                            if (leftAmount >= 1) {
                                const int leftX = this->hintOffsetX + layout->field_0x28 +
                                                  this->rowLayoutMetrics.field_0x00 +
                                                  this->rowLayoutMetrics.field_0x08;
                                PaintCanvas *amountCanvas = canvas;
                                const unsigned int amountFont = font;
                                const int textWidth = canvas->GetTextWidth(font, leftText);
                                amountCanvas->DrawString(amountFont, leftText, leftX - textWidth,
                                                         (y + layout->field_0x70 / 2 + 1), false);
                            }
                            if (tab == 4 || rightAmount >= 1) {
                                const int blueprintOffset = tab == 4 ? layout->field_0x25c : 0;
                                const int rightX = topY + blueprintOffset + this->hintOffsetX -
                                                   this->rowLayoutMetrics.field_0x00 +
                                                   this->rowLayoutMetrics.field_0x04;
                                PaintCanvas *amountCanvas = canvas;
                                const unsigned int amountFont = font;
                                const int textWidth = canvas->GetTextWidth(font, rightText);
                                amountCanvas->DrawString(amountFont, rightText, rightX - textWidth,
                                                         (y + layout->field_0x70 / 2 + 1), false);
                            }
                        }
                    itemCommon:
                        canvas->SetColor(tab == 1 ? (li->item->getSinglePrice() <= status->getCredits()
                                                         ? 0x7aa35bffu
                                                         : 0xa35b5bffu)
                                                  : 0x777777ffu);
                        if (this->upgradeMode == 0) {
                            PaintCanvas *priceCanvas = canvas;
                            const unsigned int priceFont = font;
                            String price = Globals::layout->formatCredits(li->item->getSinglePrice());
                            priceCanvas->DrawString(priceFont, price,
                                                    (this->hintOffsetX + layout->field_0x28 + contentBase),
                                                    (y + layout->field_0x70 / 2 + 1), false);
                        }

                        ImageFactory *factory = static_cast<ImageFactory *>(Globals::imageFactory);
                        const int imageIndex = li->item->getIndex();
                        const int imageType = li->item->getType();
                        factory->drawItem(imageIndex, imageType,
                                          this->hintOffsetX + layout->field_0x28 + rowGap,
                                          this->iconOffsetY + y);

                        if (tab == 1 && !status->field_54->data()[li->item->getIndex()]) {
                            canvas->DrawImage2D(this->pendingIconImage,
                                                this->hintOffsetX + layout->field_0x28 + boxW +
                                                    layout->field_0x2cc,
                                                y + this->iconOffsetY + layout->field_0x2d0 - 2, 0x11, 0x22);
                            goto rowLabel;
                        }
                        if (li->inTabIndex > -1 ||
                            (tab == 1 && status->getShip()->hasEquipment(li->item->getIndex(), 1))) {
                            canvas->DrawImage2D(this->blueprintIconImage,
                                                this->hintOffsetX + layout->field_0x28 + boxW +
                                                    layout->field_0x2cc,
                                                y + this->iconOffsetY + layout->field_0x2d0 - 2, 0x11, 0x22);
                            goto rowLabel;
                        } else if (tab == 4) {
                            if (Item::isInList(li->item, status->getStation()->getItems())) {
                                canvas->DrawImage2D(
                                    this->blueprintIconImage,
                                    this->hintOffsetX + layout->field_0x28 + boxW + layout->field_0x2cc,
                                    y + this->iconOffsetY + layout->field_0x2d0 - 2, 0x11, 0x22);
                            }
                            goto rowLabel;
                        }

                        if ((tab == 0 || tab == 3) && li->item->getAmount() >= 2) {
                            String open(" (");
                            String amount(li->item->getAmount());
                            String partial = open + amount;
                            String close(")");
                            String suffix = partial + close;
                            label += suffix;
                        }
                    }

                rowLabel:
                    canvas->DrawString(font, label, (this->hintOffsetX + layout->field_0x28 + contentBase),
                                       y + 2, false);

                    if (li != this->selectedItem) {
                        continue;
                    }

                    if (li->isShip()) {
                        if (tab != 0 && (tab != 3 || li->ship != status->getShip())) {
                            (*this->buttons)[kHangarButtonSelectShip]->setPosition(
                                (this->hintOffsetX + layout->field_0x28 + topY), this->rowActionOffsetY + y,
                                0x12);
                            (*this->buttons)[kHangarButtonSelectShip]->setVisible(true);
                            (*this->buttons)[kHangarButtonSelectShip]->draw();
                            occupiedRightWidth =
                                (*this->buttons)[kHangarButtonSelectShip]->getWidth() + layout->field_0x2c;
                        }
                        if (tab == 1 && this->upgradeMode != 0) {
                            (*this->buttons)[kHangarButtonSelectItemFirst]->setVisible(true);
                            (*this->buttons)[kHangarButtonSellShip]->setPosition(
                                this->hintOffsetX + layout->field_0x28, this->rowActionOffsetY + y, 0x11);
                            (*this->buttons)[kHangarButtonSellShip]->setVisible(true);
                            (*this->buttons)[kHangarButtonSellShip]->draw();
                        }
                    } else if (tab == 2 && li->isBluePrint()) {
                        (*this->buttons)[kHangarButtonSelectBlueprint]->setPosition(
                            (this->hintOffsetX + layout->field_0x28 + topY), this->rowActionOffsetY + y,
                            0x12);
                        (*this->buttons)[kHangarButtonSelectBlueprint]->setVisible(true);
                        (*this->buttons)[kHangarButtonSelectBlueprint]->draw();
                        occupiedRightWidth =
                            (*this->buttons)[kHangarButtonSelectBlueprint]->getWidth() + layout->field_0x2c;
                    } else if (tab == 0 && li->isItem()) {
                        Ship *ship = status->getShip();
                        Item *installed = ship->getFirstEquipmentOfSort(li->item->getSort());
                        bool singleInstall = false;
                        bool sameTypeThree = false;
                        if (installed != nullptr) {
                            singleInstall = !li->item->canBeInstalledMultipleTimes();
                            if (installed->getType() == 3) {
                                sameTypeThree = installed->getIndex() == li->item->getIndex();
                            }
                        }
                        bool blocked = false;
                        if (li->inTabIndex <= -1) {
                            const int freeSlots = status->getShip()->getFreeSlots(li->item->getType());
                            blocked = (!singleInstall && freeSlots == 0) ||
                                      (sameTypeThree && !li->item->canBeInstalledMultipleTimes());
                        }
                        if (!blocked) {
                            const unsigned int actionIndex = li->inTabIndex < 0
                                                                 ? kHangarButtonSelectItemFirst
                                                                 : kHangarButtonSelectItemSecond;
                            (*this->buttons)[actionIndex]->setPosition(
                                (this->hintOffsetX + layout->field_0x28 + topY) - layout->field_0x2c * 2,
                                this->rowActionOffsetY + y, 0x12);
                            (*this->buttons)[actionIndex]->setVisible(true);
                            (*this->buttons)[actionIndex]->draw();
                            occupiedRightWidth =
                                (*this->buttons)[actionIndex]->getWidth() + layout->field_0x2c * 2;
                        }
                    }

                    if (li->isItem() || li->isShip() || li->isBluePrint() || li->isPendingProduct()) {
                        (*this->buttons)[kHangarButtonInspect]->setPosition(
                            (this->hintOffsetX + layout->field_0x28 + topY) - occupiedRightWidth,
                            this->rowActionOffsetY + y, 0x12);
                        (*this->buttons)[kHangarButtonInspect]->setVisible(true);
                        (*this->buttons)[kHangarButtonInspect]->draw();
                    }
                }

                if (scrollPx > 0 || startPx > 0) {
                    layout->drawScrollBar(
                        Globals::w - layout->field_0x48 - layout->field_0x28 - this->hintOffsetX,
                        layout->field_0x20 + layout->field_0xc, this->visibleHeight, startPx, scrollPx);
                }
            }

            layout->drawHeader(String(*gameText->getText(167), false));

            for (unsigned int i = 0; i < this->tabButtons->size(); i++) {
                TouchButton *tabButton = this->tabButtons->data()[i];
                tabButton->draw();
            }
        }

        if (this->viewMode == 1) {
            this->viewMode = 0;
            this->render();
            this->viewMode = 1;

            this->listItemWindow->draw();
        }
    }

    layout->drawFooter();

    (*this->buttons)[kHangarButtonCredits]->setVisible(true);
    (*this->buttons)[kHangarButtonCredits]->setAlwaysPressed(g_hangarCreditOfferShown == 0);
    (*this->buttons)[kHangarButtonCredits]->setText(Globals::layout->formatCredits(status->getCredits()));
    (*this->buttons)[kHangarButtonCredits]->draw();

    if (this->dialogActive == 0)
        return;

    this->dialog->draw();

    if (this->buyCreditsActive == 0) {
        if (this->freeCreditsActive != 0) {
            int visibleRow = 0;
            const int buttonX = this->dialog->x + layout->field_0x28;
            const int firstButtonY = this->dialog->y + layout->field_0x8 + 2 * layout->field_0x2c;
            for (unsigned int offerIndex = 0; offerIndex <= 4; ++offerIndex) {
                TouchButton *button = buttonAt(kHangarButtonFreeCreditsFirst + offerIndex);
                const bool visible = offerIndex != 0 &&
                                     (offerIndex != 1 || g_hangarSocialCreditClaimed[1] == 0) &&
                                     (offerIndex != 2 || g_hangarSocialCreditClaimed[2] == 0) &&
                                     (offerIndex != 3 || g_hangarSocialCreditClaimed[3] == 0) &&
                                     (offerIndex != 4 || g_hangarSocialCreditClaimed[4] == 0);
                button->setVisible(visible);
                const int buttonY = firstButtonY + visibleRow * (layout->field_0x30 + layout->field_0x34);
                button->setPosition(buttonX, buttonY);
                button->draw();

                if (visible) {
                    const int textX = buttonX + button->getWidth() + layout->field_0x2c;
                    const int textY = static_cast<int>(buttonY + button->getHeight() * 0.2f);
                    canvas->SetColor(0xffffffffu);
                    PaintCanvas *descriptionCanvas = canvas;
                    const unsigned int descriptionFont = font;
                    descriptionCanvas->DrawString(
                        descriptionFont, *gameText->getText(offerIndex == 0 ? 3400 : 112 + offerIndex), textX,
                        textY, false);
                    canvas->SetColor(0x777777ffu);
                    PaintCanvas *rewardCanvas = canvas;
                    const unsigned int rewardFont = font;
                    if (offerIndex != 0) {
                        rewardCanvas->DrawString(
                            rewardFont,
                            String("(+", false) +
                                layout->formatCredits(kHangarSocialCreditReward[offerIndex]) +
                                String(")", false),
                            textX, textY + layout->field_0x4, false);
                    } else {
                        String empty("");
                        rewardCanvas->DrawString(rewardFont, empty, textX, textY + layout->field_0x4, false);
                    }
                    ++visibleRow;
                }
            }
        }
    } else {
        void *applicationData =
            static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->GetApplicationData();

        if (this->listModeFlag != 0) {
            for (unsigned int productIndex = 0; productIndex <= 4; ++productIndex) {
                String label("");
                String split("");
                (*this->buttons)[kHangarButtonPaidCreditsFirst + productIndex]->setVisible(true);
                switch (productIndex & 0x7fffffffu) {
                case 0: {
                    {
                        String value(Globals::cItemListDescription_00);
                        label = value;
                    }
                    String price(Globals::cItemListPrice_00);
                    split = price;
                    break;
                }
                case 1: {
                    {
                        String value(Globals::cItemListDescription_01);
                        label = value;
                    }
                    String price(Globals::cItemListPrice_01);
                    split = price;
                    break;
                }
                case 2: {
                    {
                        String value(Globals::cItemListDescription_02);
                        label = value;
                    }
                    String price(Globals::cItemListPrice_02);
                    split = price;
                    break;
                }
                case 3: {
                    {
                        String value(Globals::cItemListDescription_03);
                        label = value;
                    }
                    String price(Globals::cItemListPrice_03);
                    split = price;
                    break;
                }
                case 4: {
                    {
                        String value(Globals::cItemListDescription_04);
                        label = value;
                    }
                    String price(Globals::cItemListPrice_04);
                    split = price;
                    break;
                }
                default:
                    label = String("", false);
                    split = String("", false);
                    break;
                }
                const int row = static_cast<int>(productIndex / 3);
                const int column = static_cast<int>(productIndex % 3);
                const int x = Globals::w / 2 - this->buttonWidth - this->gridSpacingX +
                              column * (this->buttonWidth + this->gridSpacingX);
                const int y = static_cast<int>(-3 * layout->field_0x20 + Globals::h / 2 -
                                               this->gridButtonHeight / 2 - this->gridSpacingY * 0.5f +
                                               row * (this->gridButtonHeight + this->gridSpacingY));
                (*this->buttons)[kHangarButtonPaidCreditsFirst + productIndex]->setPosition(x, y, 0x44);
                (*this->buttons)[kHangarButtonPaidCreditsFirst + productIndex]->replaceTextKeepSize(label);
                (*this->buttons)[kHangarButtonPaidCreditsFirst + productIndex]->setSplitText(split);
                (*this->buttons)[kHangarButtonPaidCreditsFirst + productIndex]->draw();

                canvas->DrawImage2D(static_cast<const unsigned int *>(this->tabIcons)[productIndex], x,
                                    y - layout->field_0x2c, 0x11, 0x44);

                if (productIndex == 4 && Globals::showBestDeal != 0) {
                    const float badgeOffset = Globals::iPad != 0 ? 0.05f : 0.13f;
                    canvas->DrawImage2D(this->scrollHintImageA,
                                        static_cast<int>(x + badgeOffset * this->buttonWidth),
                                        static_cast<int>(y - this->gridButtonHeight * 0.1f), 0x11, 0x11);
                }
            }
        } else {
            const int startY = this->dialog->y + layout->field_0x8 + 5 * layout->field_0x2c;
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 0]->setVisible(true);
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 0]->setText(
                String(Globals::cItemListDescription_00, false));
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 0]->setSplitText(
                String(Globals::cItemListPrice_00, false));
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 0]->setPosition(Globals::w / 2, startY, 0x14);
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 0]->draw();
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 1]->setVisible(true);
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 1]->setText(
                String(Globals::cItemListDescription_01, false));
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 1]->setSplitText(
                String(Globals::cItemListPrice_01, false));
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 1]->setPosition(
                Globals::w / 2, startY + 1 * (layout->field_0x30 + layout->field_0x34), 0x14);
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 1]->draw();
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 2]->setVisible(true);
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 2]->setText(
                String(Globals::cItemListDescription_02, false));
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 2]->setSplitText(
                String(Globals::cItemListPrice_02, false));
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 2]->setPosition(
                Globals::w / 2, startY + 2 * (layout->field_0x30 + layout->field_0x34), 0x14);
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 2]->draw();
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 3]->setVisible(true);
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 3]->setText(
                String(Globals::cItemListDescription_03, false));
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 3]->setSplitText(
                String(Globals::cItemListPrice_03, false));
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 3]->setPosition(
                Globals::w / 2, startY + 3 * (layout->field_0x30 + layout->field_0x34), 0x14);
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 3]->draw();
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 4]->setVisible(true);
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 4]->setText(
                String(Globals::cItemListDescription_04, false));
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 4]->setSplitText(
                String(Globals::cItemListPrice_04, false));
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 4]->setPosition(
                Globals::w / 2, startY + 4 * (layout->field_0x30 + layout->field_0x34), 0x14);
            (*this->buttons)[kHangarButtonPaidCreditsFirst + 4]->draw();
        }

        const bool showMore = g_hangarSocialCreditClaimed[3] == 0 || g_hangarSocialCreditClaimed[2] == 0 ||
                              g_hangarSocialCreditClaimed[1] == 0 || g_hangarSocialCreditClaimed[5] == 0 ||
                              g_hangarSocialCreditClaimed[4] == 0 ||
                              *(static_cast<uint8_t *>(applicationData) + 21) != 0;
        unsigned int buttonCount = 5;
        if (showMore) {

            (*this->buttons)[kHangarButtonCreditsMore]->setVisible(true);
            if (this->listModeFlag != 0) {
                const int x = Globals::w / 2 - this->buttonWidth - this->gridSpacingX +
                              2 * (this->buttonWidth + this->gridSpacingX);
                const int y =
                    static_cast<int>(-3 * layout->field_0x20 + Globals::h / 2 - this->gridButtonHeight / 2 -
                                     this->gridSpacingY * 0.5f + this->gridButtonHeight + this->gridSpacingY);
                (*this->buttons)[kHangarButtonCreditsMore]->setPosition(x, y, 0x44);
                (*this->buttons)[kHangarButtonCreditsMore]->replaceTextKeepSize(*gameText->getText(118));
                (*this->buttons)[kHangarButtonCreditsMore]->draw();
                canvas->DrawImage2D(static_cast<const unsigned int *>(this->tabIcons)[5], x,
                                    y - layout->field_0x2c, 0x11, 0x44);
                const float badgeOffset = Globals::iPad != 0 ? 0.05f : 0.13f;
                const int badgeX = Globals::w / 2 - this->buttonWidth - this->gridSpacingX +
                                   2 * (this->buttonWidth + this->gridSpacingX);
                const int badgeY =
                    static_cast<int>(Globals::h / 2 - this->gridButtonHeight / 2 - 3 * layout->field_0x20 -
                                     this->gridSpacingY * 0.5f + this->gridButtonHeight + this->gridSpacingY);
                canvas->DrawImage2D(this->scrollHintImageB,
                                    static_cast<int>(badgeX + badgeOffset * this->buttonWidth),
                                    static_cast<int>(badgeY - this->gridButtonHeight * 0.1f), 0x11, 0x11);
            } else {
                const int stepY = layout->field_0x30 + layout->field_0x34;
                (*this->buttons)[kHangarButtonCreditsMore]->setText(*gameText->getText(118));
                (*this->buttons)[kHangarButtonCreditsMore]->setPosition(
                    Globals::w / 2, this->field_0x48 + stepY * buttonCount, 0x14);
                (*this->buttons)[kHangarButtonCreditsMore]->draw();
            }
            (*this->buttons)[kHangarButtonCreditsMore]->setSplitText(*gameText->getText(119));
            ++buttonCount;
        }
        if (this->listModeFlag == 0) {
            const int stepY = layout->field_0x30 + layout->field_0x34;
            this->dialog->setHeight(stepY * (buttonCount + 1) + canvas->GetTextHeight(font) * 2);
        }
    }
}

// Superseded untyped first-pass body. Keep it out of the build while the
// recovered state router below is compared against the ARM control flow.
#if 0
void HangarWindow::OnTouchEnd(int touch, int coord) {
    HangarWindow *self = this;
    Globals *globals = (Globals *) *g_hw_globals;
    self->holdTime = 0;
    self->repeatTimer = 0;
    self->dragging = 0;
    if (self->suppressTouchEnd != 0) {
        self->suppressTouchEnd = 0;
        return;
    }

    if (self->dialogActive == 0) {
        if (self->viewMode == 1)
            self->listItemWindow->OnTouchEnd(touch, coord);

        Layout *layout = *g_hw_layout;
        int handled = ((Layout *) (layout))->OnTouchEnd(touch, coord);
        if (handled == 0) {
            int delta = self->scrollDelta;
            int newScroll = self->scrollOffset + delta;
            float vel = (float) delta;
            int absd = delta < 0 ? -delta : delta;
            self->velocity = (absd > 3) ? (int) vel : 0;
            self->damping = 0;
            self->scrollOffset = newScroll;
            self->scrollOffsetBackup = newScroll;

            Array<TouchButton *> *tabs = self->tabButtons;
            for (unsigned int i = 0; i < tabs->size(); i++) {
                if (((TouchButton *) (tabs->data()[i]))->OnTouchEnd(touch, coord) != 0) {
                    self->setSellMode(true);
                    self->selectedItem = 0;
                    self->hangarList->setCurrentTab(0, i != 0);
                    if (i == 2)
                        self->refreshCargoAvailabilityForBlueprints();
                    self->refreshCurrentContentHeight();
                    self->scrollOffset = 0;
                    self->scrollOffsetBackup = 0;
                    self->hangarList->setCurrentItemIndex(-1);
                }
            }

            if (layout->field_0xc < coord) {
                int row = IDIV(
                    (((coord - layout->field_0xc) - layout->field_0x20) - self->rowLayoutMetrics.rowGap) -
                    self->scrollOffset,
                    layout->field_0x70 + self->rowLayoutMetrics.rowGap);
                if (row < self->hangarList->getCurrentLength()) {
                    self->hangarList->setCurrentItemIndex(row);
                    if (self->currentItemIsHighlighted() != 0 &&
                        self->sellConfirmPending != 0) {
                        self->setSellMode(true);
                    }
                }
            }
            if (self->sellConfirmPending != 0) {
                self->sellConfirmPending = 0;
                return;
            }

            if ((*self->buttons)[(0x5c) >> 2]->OnTouchEnd(touch, coord) != 0) {
                self->bluePrint->getAutoCompletionPrice();
                String line, priceStr, fmt, msg;
                priceStr = Globals::layout->formatCredits(0);
                Status_replaceHash(&msg, globals, &line, &priceStr);
                self->dialog->set(*(String *) &msg, true);
                self->autoCompletePending = 1;
                self->dialogActive = 1;
            }

            if (self->currentItemIsHighlighted() != 0) {
                Array<TouchButton *> *btns = self->buttons;
                for (unsigned int i = 0; i < btns->size(); i++) {
                    if (((TouchButton *) (btns->data()[i]))->OnTouchEnd(touch, coord) != 0) {
                        if ((i & 0x7fffffff) < 0xc)
                            return;
                    }
                }
            }

            if (((Layout *) (layout))->helpPressed() != 0) {
                String help;
                if (self->viewMode == 1) {
                    ((Layout *) (layout))->initHelpWindow(help);
                } else {
                    unsigned int t = self->hangarList->getCurrentTab();
                    if (t <= 4) {
                        ((Layout *) (layout))->initHelpWindow(help);
                    }
                }
            }

            if ((*self->buttons)[(0x2c) >> 2]->OnTouchEnd(touch, coord) != 0) {
                g_hw_optionFlags[0x4e] = 1;
                (*g_hw_recordHandler)->saveOptions();
                self->showCreditsBuyWindow();
            }
            return;
        }

        if (self->viewMode == 1) {
            ((Layout *) (layout))->resetWindowDimensions();
            self->viewMode = 0;
            return;
        }

        unsigned int tab = self->hangarList->getCurrentTab();
        if (tab == 4) {
            self->setSellMode(true);
            self->selectedItem = 0;
            self->hangarList->setCurrentTab(0, true);
            self->refreshCargoAvailabilityForBlueprints();
            self->refreshCurrentContentHeight();
            self->scrollOffset = 0;
            self->scrollOffsetBackup = 0;
            self->hangarList->setCurrentItemIndex(-1);
            return;
        }
        if (self->hangarList->getCurrentTab() == 3) {
            self->hangarList->setCurrentTab(0, false);
            self->refreshCargoAvailabilityForBlueprints();
            self->refreshCurrentContentHeight();
        } else if (self->readyToClose() != 0) {
            self->setSellMode(true);
            self->selectedItem = 0;
            self->hangarList->setCurrentItemIndex(-1);
        }
        return;
    }

    if (self->autoCompletePending != 0) {
        int r = self->dialog->OnTouchEnd(touch, coord);
        if (r == 1) {
            self->dialogActive = 0;
        } else if (r == 0) {
            int price = self->bluePrint->getAutoCompletionPrice();
            if (Status::gStatus->getCredits() < price) {
                String line, priceStr, fmt, msg, suffix, combined;
                priceStr = Globals::layout->formatCredits(Status::gStatus->getCredits());
                Status_replaceHash(&msg, globals, &line, &priceStr);
                GameText::gGameText->getText(*g_hw_notEnoughTextId);
                combined = suffix + suffix;
                *((String *) &msg) += combined;
                self->dialog->set(*(String *) &msg, true);
                self->notEnoughCredits = 1;
            } else {
                self->dialogActive = 0;
                if (self->bluePrint != 0) {
                    if (self->bluePrint->isEmpty() != 0) {
                        ((BluePrint *) self->bluePrint)->stationIndex = Status::gStatus->getStation()->getIndex();
                        Status::gStatus->getStation()->getName();
                    }
                    self->bluePrint->complete();
                    self->highlightItem(self->hangarList->getCurrentItemAt(1));
                    self->buyMode = 1;
                    self->setSellMode(true);
                    Status_changeCredits(globals);
                }
            }
        }
        self->autoCompletePending = 0;
        return;
    }

    if (self->replaceEquipPending != 0) {
        int r = self->dialog->OnTouchEnd(touch, coord);
        if (r == 1) {
            self->dialogActive = 0;
            self->replaceEquipPending = 0;
            self->scrollOffset = self->savedScrollOffset;
        } else if (r == 0 && self->pendingMountItem != 0 && self->pendingDemountItem != 0) {
            self->demountItem(self->pendingDemountItem, -1);
            self->savedScrollOffset = self->scrollOffset;
            self->mountItem(self->pendingMountItem);
            self->dialogActive = 0;
            self->replaceEquipPending = 0;
        }
    } else if (self->notEnoughCredits != 0) {
        int r = self->dialog->OnTouchEnd(touch, coord);
        if (r == 1) {
            self->notEnoughCredits = 0;
            self->dialogActive = 0;
        } else if (r == 0) {
            g_hw_optionFlags[0x4e] = 1;
            (*g_hw_recordHandler)->saveOptions();
            self->showCreditsBuyWindow();
        }
    } else if (self->buyCreditsActive == 0) {
        if (self->freeCreditsActive != 0) {
            int r = self->dialog->OnTouchEnd(touch, coord);
            if (r == 0) {
                for (int i = 0x12; i != 0x17; i++)
                    (*self->buttons)[(i * 4) >> 2]->setVisible(false);
                self->freeCreditsActive = 0;
                self->showCreditsBuyWindow();
            }

            GameData *appData = static_cast<GameData *>(AppManager_GetApplicationData());
            RecordHandler *rh = *g_hw_recordHandler;
            for (unsigned int i = 0; i != 5; i++) {
                if ((*self->buttons)[(i * 4 + 0x48) >> 2]->OnTouchEnd(touch, coord) != 0) {
                    switch (i) {
                        case 0:
                            ((RecordHandler *) (rh))->recordStoreWrite(0);
                            ((RecordHandler *) (rh))->recordStoreWritePreview(0);
                            break;
                        case 1:
                            appData->flag_0xa0 = 1;
                            NFC().free_credits_likeGOF2OnFacebook();
                            Status_changeCredits(globals);
                            g_hw_optionFlags[0x49] = 1;
                            break;
                        case 2:
                            appData->flag_0xa1 = 1;
                            NFC().free_credits_likeFishlabsOnFacebook();
                            Status_changeCredits(globals);
                            g_hw_optionFlags[0x4a] = 1;
                            break;
                        case 3:
                            appData->flag_0xa2 = 1;
                            NFC().free_credits_subscribeToYoutubeChannel();
                            Status_changeCredits(globals);
                            g_hw_optionFlags[0x4b] = 1;
                            break;
                        case 4:
                            appData->flag_0xa3 = 1;
                            NFC().free_credits_followOnTwitter();
                            Status_changeCredits(globals);
                            g_hw_optionFlags[0x4c] = 1;
                            break;
                        case 5:
                            appData->rateGameFlagA = 1;
                            NFC().free_credits_rateGame();
                            Status_changeCredits(globals);
                            g_hw_optionFlags[0x4d] = 1;
                            break;
                    }
                }
            }
            return;
        }

        if (self->bluePrintPurchasePending != 0) {
            int r = self->dialog->OnTouchEnd(touch, coord);
            int cost = ((Item *) ((ListItem *) self->bluePrintItem)->item)->getBlueprintAmount() * 200;
            bool revert = true;
            if (r == 0 && Status::gStatus->getCredits() >= cost && self->localBluePrint == 0) {
                Status_changeCredits(globals);
                self->setSellMode(true);
                self->selectedItem = 0;
                self->hangarList->setCurrentItemIndex(-1);
                self->localBluePrint = 0;
                self->bluePrintPurchasePending = 0;
                revert = false;
            } else if (r == 1 && Status::gStatus->getCredits() >= cost && self->localBluePrint == 0) {
                revert = false;
            }
            if (revert) {
                ((Item *) ((ListItem *) self->bluePrintItem)->item)->setStationAmount(self->savedStationAmount);
                ((Item *) ((ListItem *) self->bluePrintItem)->item)->setAmount(self->savedAmount);
                ((Item *) ((ListItem *) self->bluePrintItem)->item)->setBlueprintAmount(self->savedBlueprintAmount);
                ((Status *) (globals))->setCredits(self->savedCredits);
                self->savedStationAmount = 0;
                self->bluePrintBuyCount = 0;
                self->selectedItem = 0;
                self->savedAmount = 0;
                self->savedBlueprintAmount = 0;
                self->currentLoad = self->savedLoad;
                self->hangarList->setCurrentItemIndex(-1);
                self->dialogActive = 0;
                self->bluePrintPurchasePending = 0;
                self->buyMode = 0;
                if (Status::gStatus->getCredits() < cost && self->localBluePrint == 0) {
                    String line, priceStr, fmt, msg;
                    priceStr = Globals::layout->formatCredits(Status::gStatus->getCredits());
                    Status_replaceHash(&msg, globals, &line, &priceStr);
                    self->dialog->set(g_HangarWindow_emptyDialogText);
                    self->dialogActive = 1;
                }
                self->localBluePrint = 0;
            }
            self->refreshCurrentContentHeight();
            return;
        }

        if (self->sellShipPending != 0) {
            int r = self->dialog->OnTouchEnd(touch, coord);
            if (r == 1) {
                self->sellShipPending = 0;
                self->dialogActive = 0;
            } else if (r == 0) {
                self->sellShipPending = 0;
                self->dialogActive = 0;
                ((ListItem *) (self->selectedItem))->getPrice();
                Status_changeCredits(globals);
                Status::gStatus->getStation()->removeShip((Ship *) Status::gStatus->getShip());
                self->hangarList->initShopTab((Array<Item *> *) (self->itemList), Status::gStatus->getStation()->getShips());
                self->refreshCurrentContentHeight();
            }
            return;
        }

        int r = self->dialog->OnTouchEnd(touch, coord);
        bool special = globals->field_0x114 == 3 && self->upgradeMode == 0;
        if (special && self->dlcMenuPending != 0 && r == 1) {
            int idx = globals->field_0x14c;
            Status::gStatus->getShip()->getIndex();
            if (((Station *) ((void *) (uintptr_t) idx))->hasShip(Status::gStatus->getShip()->getIndex()) == 0) {
                int price = ((ListItem *) self->selectedItem)->ship->getPrice();
                if (Status::gStatus->getCredits() < price) {
                    String line, priceStr, fmt, msg, suffix, combined;
                    ((ListItem *) self->selectedItem)->ship->getPrice();
                    priceStr = Globals::layout->formatCredits(Status::gStatus->getCredits());
                    Status_replaceHash(&msg, globals, &line, &priceStr);
                    GameText::gGameText->getText(*g_hw_sellShipTextId);
                    combined = suffix + suffix;
                    *((String *) &msg) += combined;
                    self->dialog->set(*(String *) &msg, true);
                    self->dialogActive = 1;
                    self->notEnoughCredits = 1;
                    self->shipSwapPending = 0;
                    return;
                }
            }
        }

        {
            ChoiceWindow *swapDialog = self->dialog;

            int swapResult = swapDialog->OnTouchEnd(touch, coord);
            if (swapResult == 1) {
                self->dialogActive = 0;
                self->replaceEquipPending = 0;
                self->scrollOffset = self->savedScrollOffset;
            } else if (swapResult == 0 && self->pendingMountItem != 0 &&
                       self->pendingDemountItem != 0) {
                self->demountItem(self->pendingDemountItem, -1);
                self->savedScrollOffset = self->scrollOffset;
                self->mountItem(self->pendingMountItem);
                self->dialogActive = 0;
                self->replaceEquipPending = 0;
            }

            uint8_t buying = self->buyMode;
            int buyResult = swapDialog->OnTouchEnd(touch, coord);
            if (buying != 0) {
                if (buyResult == 1) {
                    self->buyMode = 0;
                    self->dialogActive = 0;
                } else {
                    if (buyResult != 0)
                        return;
                    if (self->routeWarningPending == 0) {
                        self->dialogActive = 0;
                        if (self->autoEquipped == 0) {
                            self->buyMode = 1;
                        } else {
                            Array<TouchButton *> *tabs = self->tabButtons;
                            for (unsigned int i = 0; i < tabs->size(); i++)
                                ((TouchButton *) (tabs->data()[i]))->resetTouch();
                        }
                        return;
                    }
                    self->dialogActive = 0;
                    self->routeWarningPending = 0;
                    self->buyMode = 0;
                }
                self->selectedItem = 0;
                self->hangarList->setCurrentItemIndex(-1);
            }
        }
        return;
    } else {
        int r = self->dialog->OnTouchEnd(touch, coord);
        if (r != 0) {
            Array<TouchButton *> *btns = self->buttons;
            for (unsigned int i = 0; i < 5; i++) {
                if ((*btns)[(i * 4 + 0x30) >> 2]->OnTouchEnd(touch, coord) != 0) {
                    switch (i) {
                        case 0: NFC().iap_buy_credits_300_000();
                            break;
                        case 1: NFC().iap_buy_credits_1_000_000();
                            break;
                        case 2: NFC().iap_buy_credits_3_000_000();
                            break;
                        case 3: NFC().iap_buy_credits_10_000_000();
                            break;
                        default: NFC().iap_buy_credits_100_000();
                            break;
                    }
                }
            }
            if ((*btns)[(0x44) >> 2]->OnTouchEnd(touch, coord) != 0) {
                bool show = true;
                if (g_hw_optionFlags[0x4a] && g_hw_optionFlags[0x49] &&
                    g_hw_optionFlags[0x4d] && g_hw_optionFlags[0x4c])
                    show = (g_hw_optionFlags[0x4b] == 0);
                GameData *appData = static_cast<GameData *>(AppManager_GetApplicationData());
                if (show || appData->flag_0x15 == 0) {
                    for (int i = 0xc; i != 0x11; i++)
                        (*btns)[(i * 4) >> 2]->setVisible(false);
                    self->showFreeCreditsWindow();
                }
            }
            return;
        }
        self->buyCreditsActive = 0;
        self->dialogActive = 0;
        for (int i = 0xc; i != 0x11; i++)
            (*self->buttons)[(i * 4) >> 2]->setVisible(false);
        (*self->buttons)[(0x44) >> 2]->setVisible(false);
        GameData *appData = static_cast<GameData *>(AppManager_GetApplicationData());
        appData->dlcStoreReadyFlag = 0;
        void *mod = AppManager_GetApplicationModule(*g_hw_modStationId);
        if (mod != 0 && (*(uint8_t *) ((char *) (mod) + (0x18))) != 0)
            (*(uint8_t *) ((char *) (mod) + (0x18))) = 0;
        return;
    }

    uint8_t buying = self->buyMode;
    int r2 = self->dialog->OnTouchEnd(touch, coord);
    if (buying != 0) {
        if (r2 == 1) {
            self->buyMode = 0;
            self->dialogActive = 0;
        } else if (r2 == 0) {
            if (self->routeWarningPending == 0) {
                self->dialogActive = 0;
                if (self->autoEquipped == 0) {
                    self->buyMode = 1;
                } else {
                    Array<TouchButton *> *tabs = self->tabButtons;
                    for (unsigned int i = 0; i < tabs->size(); i++)
                        ((TouchButton *) (tabs->data()[i]))->resetTouch();
                }
                return;
            }
            self->dialogActive = 0;
            self->routeWarningPending = 0;
            self->buyMode = 0;
        } else {
            return;
        }
        self->selectedItem = 0;
        self->hangarList->setCurrentItemIndex(-1);
        return;
    }
    if (r2 != 0)
        return;
    self->dialogActive = 0;
    void *mod = AppManager_GetApplicationModule(*g_hw_dlcModuleId);
    if ((*(uint8_t *) ((char *) (mod) + (0x18))) != 0)
        (*(uint8_t *) ((char *) (mod) + (0x18))) = 0;
}

#endif

// Android 0x14c740; iOS sub_F0CE4 corroborates modal and ship-transfer lifetimes.
int HangarWindow::OnTouchEnd(int touch, int coord) {
    this->holdTime = 0;
    this->repeatTimer = 0;
    this->dragging = 0;
    if (this->suppressTouchEnd != 0) {
        this->suppressTouchEnd = 0;
        return 0;
    }

    auto buttonAt = [this](unsigned int index) -> TouchButton * { return (*this->buttons)[index]; };
    if (this->dialogActive != 0) {
        if (this->autoCompletePending != 0) {
            const int result = this->dialog->OnTouchEnd(touch, coord);
            if (result == 1) {
                this->dialogActive = 0;
            } else if (result == 0) {
                const int price = this->bluePrint->getAutoCompletionPrice();
                Status *&status = Globals::status;
                if (price <= status->getCredits()) {
                    this->dialogActive = 0;
                    if (this->bluePrint != nullptr) {
                        if (this->bluePrint->isEmpty()) {
                            this->bluePrint->stationIndex = status->getStation()->getIndex();
                            this->bluePrint->stationName = status->getStation()->getName();
                        }
                        this->bluePrint->complete();
                        this->highlightItem(this->hangarList->getCurrentItemAt(1));
                        this->buyMode = 1;
                        this->setSellMode(false);
                        status->changeCredits(-price);
                    }
                } else {
                    GameText *&gameText = Globals::gameText;
                    String message = status->replaceHash(
                        String(*gameText->getText(203), false),
                        String(Globals::layout->formatCredits(price - status->getCredits()), false),
                        String("#C"));
                    {
                        String newline("\n\n", false);
                        String suffix = newline + *gameText->getText(124);
                        message += suffix;
                    }
                    this->dialog->set(message, true);
                    this->notEnoughCredits = 1;
                }
            }
            this->autoCompletePending = 0;
            return 0;
        }

        Status *&status = Globals::status;
        GameText *&gameText = Globals::gameText;
        if (this->replaceEquipPending != 0) {
            const int result = this->dialog->OnTouchEnd(touch, coord);
            if (result == 1) {
                this->dialogActive = 0;
                this->replaceEquipPending = 0;
                this->scrollOffset = this->savedScrollOffset;
            } else if (result == 0 && this->pendingMountItem != nullptr &&
                       this->pendingDemountItem != nullptr) {
                this->demountItem(this->pendingDemountItem, -1);
                this->savedScrollOffset = this->scrollOffset;
                this->mountItem(this->pendingMountItem);
                this->dialogActive = 0;
                this->replaceEquipPending = 0;
            }
            goto common_choice_tail;
        }

        if (this->notEnoughCredits != 0) {
            const int result = this->dialog->OnTouchEnd(touch, coord);
            if (result == 1) {
                this->notEnoughCredits = 0;
                this->dialogActive = 0;
            } else if (result == 0) {
                g_hangarCreditOfferShown = 1;
                RecordHandler *recordHandler = static_cast<RecordHandler *>(Globals::recordHandler);

                recordHandler->saveOptions();

                this->showCreditsBuyWindow();
            }
            goto common_choice_tail;
        }

        if (this->buyCreditsActive != 0) {
            const int result = this->dialog->OnTouchEnd(touch, coord);
            if (result == 0) {
                this->buyCreditsActive = 0;
                this->dialogActive = 0;
                for (unsigned int i = kHangarButtonPaidCreditsFirst; i != kHangarButtonCreditsMore; ++i) {
                    (*this->buttons)[i]->setVisible(false);
                }
                (*this->buttons)[kHangarButtonCreditsMore]->setVisible(false);
                GameData *appData = static_cast<GameData *>(
                    static_cast<ApplicationManager *>(Globals::appManager)->GetApplicationData());

                appData->dlcStoreReadyFlag = 0;

                ModStation *module = static_cast<ModStation *>(
                    static_cast<ApplicationManager *>(Globals::appManager)->GetApplicationModule(5));
                if (module != nullptr && module->closeHangarAfterCredits != 0) {
                    module->closeHangarAfterCredits = 0;
                    return 1;
                }
            }

            for (int productIndex = 0; productIndex <= 4; ++productIndex) {
                TouchButton *button = buttonAt(kHangarButtonPaidCreditsFirst + productIndex);
                if (button->OnTouchEnd(touch, coord) == 0)
                    continue;
                switch (productIndex & 0x7fffffff) {
                case 0:
                    NFC::iap_buy_credits_100_000();
                    break;
                case 1:
                    NFC::iap_buy_credits_300_000();
                    break;
                case 2:
                    NFC::iap_buy_credits_1_000_000();
                    break;
                case 3:
                    NFC::iap_buy_credits_3_000_000();
                    break;
                case 4:
                    NFC::iap_buy_credits_10_000_000();
                    break;
                default:
                    break;
                }
                break;
            }
            TouchButton *more = buttonAt(kHangarButtonCreditsMore);
            if (more->OnTouchEnd(touch, coord) != 0) {
                const bool allSocialOffersClaimed =
                    g_hangarSocialCreditClaimed[1] != 0 && g_hangarSocialCreditClaimed[2] != 0 &&
                    g_hangarSocialCreditClaimed[3] != 0 && g_hangarSocialCreditClaimed[4] != 0 &&
                    g_hangarSocialCreditClaimed[5] != 0;
                GameData *appData = static_cast<GameData *>(
                    static_cast<ApplicationManager *>(Globals::appManager)->GetApplicationData());
                if (!allSocialOffersClaimed || appData->flag_0x15 == 0) {
                    for (unsigned int i = kHangarButtonPaidCreditsFirst; i != kHangarButtonCreditsMore; ++i) {
                        (*this->buttons)[i]->setVisible(false);
                    }
                    this->showFreeCreditsWindow();
                }
            }
            goto common_choice_tail;
        }

        if (this->freeCreditsActive != 0) {
            const int result = this->dialog->OnTouchEnd(touch, coord);
            if (result == 0) {
                for (unsigned int i = kHangarButtonFreeCreditsFirst; i != kHangarButtonBlueprintAutoComplete;
                     ++i) {
                    (*this->buttons)[i]->setVisible(false);
                }
                this->freeCreditsActive = 0;
                this->showCreditsBuyWindow();
            }

            GameData *appData = static_cast<GameData *>(
                static_cast<ApplicationManager *>(Globals::appManager)->GetApplicationData());
            for (int offerIndex = 0; offerIndex != 5; ++offerIndex) {
                TouchButton *button = buttonAt(kHangarButtonFreeCreditsFirst + offerIndex);
                if (button->OnTouchEnd(touch, coord) == 0) {
                    continue;
                }
                RecordHandler *recordHandler = static_cast<RecordHandler *>(Globals::recordHandler);
                switch (offerIndex & 0x7fffffff) {
                case 0:

                    recordHandler->recordStoreWrite(0);
                    recordHandler->recordStoreWritePreview(0);

                    break;
                case 1:
                    appData->flag_0xa0 = 1;
                    NFC::free_credits_likeGOF2OnFacebook();
                    status->changeCredits(kHangarSocialCreditReward[offerIndex]);
                    g_hangarSocialCreditClaimed[1] = 1;
                    break;
                case 2:
                    appData->flag_0xa1 = 1;
                    NFC::free_credits_likeFishlabsOnFacebook();
                    status->changeCredits(kHangarSocialCreditReward[offerIndex]);
                    g_hangarSocialCreditClaimed[2] = 1;
                    break;
                case 3:
                    appData->flag_0xa2 = 1;
                    NFC::free_credits_subscribeToYoutubeChannel();
                    status->changeCredits(kHangarSocialCreditReward[offerIndex]);
                    g_hangarSocialCreditClaimed[3] = 1;
                    break;
                case 4:
                    appData->flag_0xa3 = 1;
                    NFC::free_credits_followOnTwitter();
                    status->changeCredits(kHangarSocialCreditReward[offerIndex]);
                    g_hangarSocialCreditClaimed[4] = 1;
                    break;
                default:
                    break;
                }
                continue;
            }
            goto common_choice_tail;
        }

        if (this->bluePrintPurchasePending != 0) {
            const int result = this->dialog->OnTouchEnd(touch, coord);
            const int cost = this->bluePrintItem->item->getBlueprintAmount() * 200;
            if (result == 0 && cost <= status->getCredits() && this->localBluePrint == 0) {
                status->changeCredits(-cost);
                this->setSellMode(false);
                this->selectedItem = nullptr;
                this->hangarList->setCurrentItemIndex(-1);
                this->localBluePrint = 0;
                this->bluePrintPurchasePending = 0;
            } else if (result == 1 || cost > status->getCredits() ||
                       (result == 0 && this->localBluePrint != 0)) {
                this->bluePrintItem->item->setStationAmount(this->savedStationAmount);
                this->bluePrintItem->item->setAmount(this->savedAmount);
                this->bluePrintItem->item->setBlueprintAmount(this->savedBlueprintAmount);
                status->setCredits(this->savedCredits);
                this->savedStationAmount = 0;
                this->bluePrintBuyCount = 0;
                this->savedAmount = 0;
                this->savedBlueprintAmount = 0;
                this->currentLoad = this->savedLoad;
                this->selectedItem = nullptr;
                this->hangarList->setCurrentItemIndex(-1);
                this->dialogActive = 0;
                this->bluePrintPurchasePending = 0;
                this->buyMode = 0;
                if (cost > status->getCredits() && this->localBluePrint == 0) {
                    this->dialog->set(status->replaceHash(
                        String(*gameText->getText(203), false),
                        String(Globals::layout->formatCredits(cost - status->getCredits()), false),
                        String("#C", false)));
                    this->dialogActive = 1;
                }
                this->localBluePrint = 0;
            }
            this->refreshCurrentContentHeight();
            return 0;
        }

        if (this->sellShipPending != 0) {
            const int result = this->dialog->OnTouchEnd(touch, coord);
            if (result == 1) {
                this->sellShipPending = 0;
                this->dialogActive = 0;
            } else if (result == 0) {
                this->sellShipPending = 0;
                this->dialogActive = 0;
                status->changeCredits(this->selectedItem->getPrice());
                status->getStation()->removeShip(this->selectedItem->ship);
                this->hangarList->initShopTab(this->itemList, status->getStation()->getShips());
                this->refreshCurrentContentHeight();
            }
            goto common_choice_tail;
        }

        if (this->shipSwapPending != 0) {
            const int result = this->dialog->OnTouchEnd(touch, coord);

            bool purchaseWithoutTradeIn = false;
            // Android compares Status+0x114 with 3 before entering its two-step
            // ship purchase confirmation. The field has no safe semantic name yet.
            if (status->field_114 == 3 && this->upgradeMode == 0) {
                if (this->swapConfirmFlag != 0) {
                    if (result == 1) {
                        if (status->voidStation->hasShip(status->getShip()->getIndex()) != 0) {
                            this->dialog->set(*gameText->getText(328));
                            this->dialogActive = 1;
                            // Android LABEL_254 clears the two ship-swap bytes
                            // before exposing the already-owned-ship notice.
                            this->shipSwapPending = 0;
                            this->swapConfirmFlag = 0;
                            return 0;
                        }
                        if (this->selectedItem->ship->getPrice() > status->getCredits()) {
                            String message = status->replaceHash(
                                String(*gameText->getText(203), false),
                                String(Globals::layout->formatCredits(this->selectedItem->ship->getPrice() -
                                                                      status->getCredits()),
                                       false),
                                String("#C", false));
                            {
                                String newline("\n\n", false);
                                String suffix = newline + *gameText->getText(124);
                                message += suffix;
                            }
                            this->dialog->set(message, true);
                            this->dialogActive = 1;
                            this->notEnoughCredits = 1;
                            this->shipSwapPending = 0;
                            this->swapConfirmFlag = 0;
                            return 0;
                        }
                        purchaseWithoutTradeIn = true;
                    } else if (result != 0) {
                        goto common_choice_tail;
                    }
                } else {
                    if (result == 0) {
                        String empty("");
                        this->dialog->set(empty, *gameText->getText(327), true, *gameText->getText(330),
                                          *gameText->getText(331), *gameText->getText(330), -1, -1);
                        this->swapConfirmFlag = 1;
                        return 0;
                    }
                    if (result == 1) {
                        this->shipSwapPending = 0;
                        this->dialogActive = 0;
                        return 0;
                    }
                    goto common_choice_tail;
                }
            } else if (status->field_114 != 3 && this->upgradeMode == 0) {
                if (this->dlcMenuPending == 0) {
                    if (result == 0) {
                        this->dialog->set(String("", false), *gameText->getText(189), true);
                        this->dlcMenuPending = 1;
                        return 0;
                    }
                    if (result != 1)
                        goto common_choice_tail;
                    this->dialogActive = 0;
                    this->shipSwapPending = 0;
                    this->swapConfirmFlag = 0;
                    return 0;
                }
                if (result == 0) {
                    ModStation *module = static_cast<ModStation *>(
                        static_cast<ApplicationManager *>(Globals::appManager)->GetApplicationModule(5));
                    if (module != nullptr) {
                        module->showDlcMenu();
                    }
                    this->shipSwapPending = 0;
                    this->swapConfirmFlag = 0;
                    this->dlcMenuPending = 0;
                    this->dialogActive = 0;
                    return 0;
                }
                if (result != 1) {
                    goto common_choice_tail;
                }
                this->dlcMenuPending = 0;
            } else {
                if (result == 1) {
                    this->shipSwapPending = 0;
                    this->swapConfirmFlag = 0;
                    this->dialogActive = 0;
                    return 0;
                }
                if (result != 0) {
                    goto common_choice_tail;
                }
            }

            status->getShip()->setCargo(Item::extractItems(this->itemList, true));
            if (purchaseWithoutTradeIn) {
                status->changeCredits(-this->selectedItem->ship->getPrice());
            } else if (this->upgradeMode == 0) {
                status->changeCredits(status->getShip()->getPrice() - this->selectedItem->ship->getPrice());
            }

            Ship *oldShip = status->getShip();
            Array<Item *> *equipment = oldShip->getEquipment();
            Array<Item *> *cargo = oldShip->getCargo();
            Array<Ship *> *shipTemplates = static_cast<Array<Ship *> *>(Globals::ships);
            Ship *newShip = (*shipTemplates)[this->selectedItem->ship->getIndex()]->makeShip(-1);
            newShip->setRace(this->selectedItem->ship->getRace());
            newShip->adjustPrice();
            if (cargo != nullptr) {
                for (unsigned int i = 0; i < cargo->size(); ++i) {
                    if (cargo->data()[i] != nullptr) {
                        newShip->addCargo(cargo->data()[i]->clone());
                    }
                }
            }
            if (equipment != nullptr) {
                for (unsigned int i = 0; i < equipment->size(); ++i) {
                    if (equipment->data()[i] != nullptr) {
                        Item *copy = equipment->data()[i]->clone();
                        if (!newShip->addEquipment(copy)) {
                            newShip->addCargo(copy);
                        }
                    }
                }
            }
            if (this->selectedItem->ship->getMods() != nullptr) {
                for (unsigned int i = 0; i < this->selectedItem->ship->getMods()->size(); ++i) {
                    newShip->addMod(this->selectedItem->ship->getMods()->data()[i]);
                }
            }

            Array<Ship *> *stationShips = status->getStation()->getShips();
            int shopSlot = -1;
            for (unsigned int i = 0; i < stationShips->size(); ++i) {
                if ((*stationShips)[i]->getIndex() == this->selectedItem->ship->getIndex()) {
                    shopSlot = static_cast<int>(i);
                    break;
                }
            }
            if (purchaseWithoutTradeIn) {
                if (shopSlot >= 0) {
                    ArrayRemove((*stationShips)[shopSlot], *stationShips);
                }
                Ship *storedShip =
                    (*static_cast<Array<Ship *> *>(Globals::ships))[oldShip->getIndex()]->makeShip(-1);
                status->voidStation->addShip(storedShip);
                if (oldShip->getMods() != nullptr) {
                    for (unsigned int i = 0; i < oldShip->getMods()->size(); ++i) {
                        Array<Ship *> *storedShips = status->voidStation->getShips();
                        (*storedShips)[status->voidStation->getShips()->size() - 1]->addMod(
                            oldShip->getMods()->data()[i]);
                    }
                }
            } else if (shopSlot >= 0) {
                delete (*stationShips)[shopSlot];
                (*stationShips)[shopSlot] = nullptr;
                Ship *storedShip =
                    (*static_cast<Array<Ship *> *>(Globals::ships))[oldShip->getIndex()]->makeShip(-1);
                (*stationShips)[shopSlot] = storedShip;
                (*stationShips)[shopSlot]->adjustPrice();
                (*stationShips)[shopSlot]->setRace(oldShip->getRace());
            }
            Array<int> *oldMods = oldShip->getMods();
            if (shopSlot >= 0 && oldMods != nullptr && !purchaseWithoutTradeIn) {
                for (unsigned int i = 0; i < oldShip->getMods()->size(); ++i) {
                    if (this->upgradeMode != 0) {
                        (*status->voidStation->getShips())[shopSlot]->addMod(oldShip->getMods()->data()[i]);
                    }
                    (*stationShips)[shopSlot]->addMod(oldShip->getMods()->data()[i]);
                }
            }
            if (this->upgradeMode != 0) {
                status->voidStation->setShips(status->getStation()->getShips(), true);
            }
            status->setShip(newShip);
            status->getShip()->refreshValue();
            this->hangarList->initShipTab(status->getShip());
            if (this->itemList != nullptr) {
                ArrayReleaseClasses(*this->itemList);
                delete this->itemList;
            }
            this->itemList = nullptr;
            this->itemList = Item::mixItems(status->getShip()->getCargo(), status->getStation()->getItems());
            this->hangarList->initShopTab(this->itemList, status->getStation()->getShips());
            this->hangarList->setCurrentTab(0, true);
            this->refreshCurrentContentHeight();
            this->shipSwapPending = 0;
            this->swapConfirmFlag = 0;
            this->dialogActive = 0;

            if (this->upgradeMode == 0) {
                String message(*gameText->getText(303), false);
                message = status->replaceHash(String(message, false),
                                              String(*gameText->getText(newShip->getIndex() + 913), false),
                                              String("#N", false));
                this->dialog->set(message);
                this->dialogActive = 1;
            }
            this->field_0x0 = 1;
            return 0;
        }

    common_choice_tail:
        // Android LABEL_117: specialised modal branches converge here and invoke
        // ChoiceWindow again before processing the shared buy/owner-close route.
        const uint8_t wasBuying = this->buyMode;
        const int result = this->dialog->OnTouchEnd(touch, coord);
        if (wasBuying != 0) {
            if (result == 1) {
                this->buyMode = 0;
                this->dialogActive = 0;
            } else if (result == 0) {
                this->dialogActive = 0;
                if (this->routeWarningPending != 0) {
                    this->routeWarningPending = 0;
                    this->buyMode = 0;
                } else if (this->autoEquipped == 0) {
                    this->buyMode = 1;
                    return 0;
                } else {

                    for (unsigned int i = 0; i < this->tabButtons->size(); ++i) {
                        TouchButton *tabButton = this->tabButtons->data()[i];
                        tabButton->resetTouch();
                    }

                    return 0;
                }
            } else {
                return 0;
            }
            this->selectedItem = nullptr;
            this->hangarList->setCurrentItemIndex(-1);
            return 0;
        }

        if (result == 0) {
            this->dialogActive = 0;
            ModStation *module = static_cast<ModStation *>(
                static_cast<ApplicationManager *>(Globals::appManager)->GetApplicationModule(5));
            if (module != nullptr && module->closeHangarAfterCredits != 0) {
                module->closeHangarAfterCredits = 0;
                return 1;
            }
        }
        return 0;
    }

    Layout *&layout = Globals::layout;
    Status *&status = Globals::status;
    GameText *&gameText = Globals::gameText;
    if (this->viewMode == 1) {
        this->listItemWindow->OnTouchEnd(touch, coord);
    }

    if (layout->OnTouchEnd(touch, coord) == 0) {
        const int delta = this->scrollDelta;
        const int newScroll = this->scrollOffset + delta;
        this->damping = 0.9f;
        this->velocity = delta < -3 || delta > 3 ? static_cast<float>(delta) : 0.0f;
        this->scrollOffset = newScroll;
        this->scrollOffsetBackup = newScroll;

        for (unsigned int i = 0; i < this->tabButtons->size(); ++i) {
            TouchButton *tab = this->tabButtons->data()[i];
            if (tab->OnTouchEnd(touch, coord) != 0) {
                // This three-step reset is emitted by the ARM body before every tab change.
                this->setSellMode(false);
                this->setSellMode(true);
                this->setSellMode(false);
                this->selectedItem = nullptr;
                this->hangarList->setCurrentTab(static_cast<int>(i), true);
                if (i == 2) {
                    this->refreshCargoAvailabilityForBlueprints();
                }
                this->refreshCurrentContentHeight();
                this->scrollOffset = 0;
                this->scrollOffsetBackup = 0;
                this->hangarList->setCurrentItemIndex(-1);
            }
        }

        int row = -1;
        if (layout->field_0xc < coord) {
            row = IDIV(coord - layout->field_0xc - layout->field_0x20 - this->rowLayoutMetrics.rowGap -
                           this->scrollOffset,
                       layout->field_0x70 + this->rowLayoutMetrics.rowGap);
            if (row < this->hangarList->getCurrentLength()) {
                this->hangarList->setCurrentItemIndex(row);
                if (this->currentItemIsHighlighted() != 0 && this->sellConfirmPending != 0) {
                    this->setSellMode(false);
                    this->setSellMode(true);
                }
            }
        }
        if (this->sellConfirmPending != 0) {
            this->sellConfirmPending = 0;
            return 0;
        }

        TouchButton *autoComplete = buttonAt(kHangarButtonBlueprintAutoComplete);
        if (autoComplete->OnTouchEnd(touch, coord) != 0) {
            const int price = this->bluePrint->getAutoCompletionPrice();
            this->dialog->set(status->replaceHash(String(*gameText->getText(195), false),
                                                  Globals::layout->formatCredits(price), String("#C", false)),
                              true);
            this->autoCompletePending = 1;
            this->dialogActive = 1;
        }

        if (this->currentItemIsHighlighted() != 0) {
            for (unsigned int i = 0; i < this->buttons->size(); ++i) {
                TouchButton *button = this->buttons->data()[i];
                if (button->OnTouchEnd(touch, coord) == 0) {
                    continue;
                }

                const int actionRow = (coord - layout->field_0xc - layout->field_0x20 -
                                       this->rowLayoutMetrics.rowGap - this->scrollOffset) /
                                      (layout->field_0x70 + this->rowLayoutMetrics.rowGap);
                switch (i & 0x7fffffff) {
                case kHangarButtonInspect:
                    this->hangarList->setCurrentItemIndex(actionRow);

                    this->listItemWindow->set(this->hangarList->getCurrentItem(), 0, 0, 0, 0, 1);
                    this->viewMode = 1;

                    Globals::sound->play(0x61, nullptr, nullptr, 0.0f);

                    return 0;
                case kHangarButtonSelectShip:
                case kHangarButtonSelectItemFirst:
                case kHangarButtonSelectItemSecond:
                case kHangarButtonSelectItemThird:
                case kHangarButtonSellListEntry:
                case kHangarButtonMoveToCargoEntry:
                case kHangarButtonSelectBlueprint:
                    this->hangarList->setCurrentItemIndex(actionRow);
                    this->selectItem(this->selectedItem);
                    // Android keeps scanning the action array after these
                    // state changes. TouchButton resets its own latch, so a
                    // second action cannot be consumed by this same touch.
                    continue;
                case kHangarButtonCurrentAmount:
                    this->hangarList->setCurrentItemIndex(actionRow);
                    this->transaction(false);

                    Globals::sound->play(0x64, nullptr, nullptr, 0.0f);

                    continue;
                case kHangarButtonStationAmount: {
                    this->hangarList->setCurrentItemIndex(actionRow);
                    this->transaction(true);

                    Globals::sound->play(0x65, nullptr, nullptr, 0.0f);

                    ListItem *current = this->hangarList->getCurrentItem();
                    if (current->isItem() && this->hangarList->getCurrentItem()->item->getType() == 1) {
                        this->autoEquipPending = 1;
                        this->autoEquipIndex = this->hangarList->getCurrentItemIndex();
                    }
                    continue;
                }
                case kHangarButtonSellShip:

                    this->dialog->set(*gameText->getText(334), true);
                    this->sellShipPending = 1;
                    this->dialogActive = 1;

                    continue;
                case kHangarButtonCredits: {
                    g_hangarCreditOfferShown = 1;
                    RecordHandler *recordHandler = static_cast<RecordHandler *>(Globals::recordHandler);

                    recordHandler->saveOptions();

                    this->showCreditsBuyWindow();
                    continue;
                }
                default:
                    break;
                }
            }
        }

        if (layout->helpPressed() != 0) {
            if (this->viewMode == 1) {
                layout->initHelpWindow(String(*gameText->getText(643), false));
            } else {
                // The Android body has a separate GameText/String lifetime
                // for every tab instead of an inferred indexed table.
                switch (this->hangarList->getCurrentTab()) {
                case 0: {
                    layout->initHelpWindow(String(*gameText->getText(623), false));
                    break;
                }
                case 1: {
                    layout->initHelpWindow(String(*gameText->getText(622), false));
                    break;
                }
                case 2: {
                    layout->initHelpWindow(String(*gameText->getText(625), false));
                    break;
                }
                case 3: {
                    layout->initHelpWindow(String(*gameText->getText(624), false));
                    break;
                }
                case 4: {
                    layout->initHelpWindow(String(*gameText->getText(626), false));
                    break;
                }
                default:
                    break;
                }
            }
        }

        TouchButton *credits = buttonAt(kHangarButtonCredits);
        if (credits->OnTouchEnd(touch, coord) != 0) {
            g_hangarCreditOfferShown = 1;
            RecordHandler *recordHandler = static_cast<RecordHandler *>(Globals::recordHandler);

            recordHandler->saveOptions();

            this->showCreditsBuyWindow();
        }
        return 0;
    }

    if (this->viewMode == 1) {
        layout->resetWindowDimensions();
        this->viewMode = 0;
        return 0;
    }
    if (this->hangarList->getCurrentTab() == 4) {
        this->setSellMode(false);
        this->selectedItem = nullptr;
        this->hangarList->setCurrentTab(2, true);
        this->refreshCargoAvailabilityForBlueprints();
        this->refreshCurrentContentHeight();
        this->scrollOffset = 0;
        this->scrollOffsetBackup = 0;
        this->hangarList->setCurrentItemIndex(-1);
        return 0;
    }
    if (this->hangarList->getCurrentTab() == 3) {
        this->hangarList->setCurrentTab(0, true);
        this->refreshCargoAvailabilityForBlueprints();
        this->refreshCurrentContentHeight();
    } else if (this->readyToClose()) {
        this->setSellMode(false);
        this->selectedItem = nullptr;
        this->hangarList->setCurrentItemIndex(-1);
        return 1;
    }
    return 0;
}

void HangarWindow::update(int delta) {
    if (this->active == 0)
        return;
    this->lastDelta = delta;

    if (this->viewMode == 1) {
        this->listItemWindow->update(delta);
        return;
    }

    unsigned int tab = this->hangarList->getCurrentTab();
    for (unsigned int i = 0; i < this->tabButtons->size(); i++) {
        bool pressed = true;
        if (i != tab && !(tab == 3 && i == 0))
            pressed = (i == 2 && tab == 4);
        this->tabButtons->data()[i]->setAlwaysPressed(pressed);
    }

    if (this->dragging == 0) {
        float v = this->damping * this->velocity;
        this->velocity = v;
        float mag = v > 0.0f ? v : -v;
        if (mag > 1.0f) {
            float pos = static_cast<float>(this->scrollOffset);
            this->scrollOffset = (int) (v + pos);
        }
    }

    if (this->scrollOffset > 0) {
        float f = static_cast<float>(-this->scrollOffset);
        this->damping = 1.0f;
        this->velocity = f * 0.5f;
    }

    if (((HangarList *) this->hangarList)->getCurrentTabItems() != 0) {
        int diff = this->visibleHeight - this->currentContentHeight;
        if (diff < 0) {
            if (this->scrollOffset < diff) {
                float f = static_cast<float>(diff - this->scrollOffset);
                this->damping = 1.0f;
                this->velocity = f * 0.5f;
            }
        } else {
            this->velocity = 0;
            this->scrollOffset = 0;
        }
    }

    if (this->buyMode != 0) {
        if ((*this->buttons)[8]->isTouched() != 0 || (*this->buttons)[9]->isTouched() != 0) {
            int t6c = this->holdTime + delta;
            int t70 = this->repeatTimer + delta;
            this->holdTime = t6c;
            this->repeatTimer = t70;
            int threshold = (t6c > 0x5dc) ? 0x1e : 200;
            if (t70 > threshold && (this->repeatTimer = 0,
                                    this->buyMode != 0 || this->specialMode != 0)) {
                if ((*this->buttons)[9]->isTouched() != 0 && (*this->buttons)[9]->isVisible() != 0) {
                    int n = (this->holdTime > 4000) ? 5 : 1;
                    for (; n != 0; n--)
                        this->transaction(true);
                } else if ((*this->buttons)[8]->isTouched() != 0 &&
                           (*this->buttons)[8]->isVisible() != 0) {
                    int n = (this->holdTime > 4000) ? 5 : 1;
                    for (; n != 0; n--)
                        this->transaction(false);
                }
            }
        }
    }
}



int HangarWindow::highlightItem(ListItem *item) {
    if (item != nullptr && item->isSelectable() != 0) {
        Globals::sound->play(0x7c, nullptr, nullptr, 0.0f);
        unsigned flag = 0;
        if (this->selectedItem != item) {
            flag = item->isTextButton() ^ 1;
        }
        this->selectedItem = item;
        this->sellConfirmPending = flag;
        if (item->isShip() != 0) {
            this->selectedItem->ship->adjustPrice();
        }
    }
    return 0;
}



void HangarWindow::demountItem(Item *item, int slot) {
    const int amount = item->getType() == 1 ? item->getAmount() : 1;
    Item *made = item->makeItem(amount);
    Globals::status->getShip()->addCargo(made);

    Ship *ship = Globals::status->getShip();
    if (slot >= 0)
        ship->freeSlot(item, slot);
    else
        ship->freeSlot(item);

    bool merged = false;
    for (unsigned int i = 0; i < this->itemList->size(); i++) {
        Item *cur = this->itemList->data()[i];
        if (cur->getIndex() == made->getIndex()) {
            cur->changeAmount(made->getAmount());
            merged = true;
            break;
        }
    }
    if (!merged)
        ArrayAdd(made, *(this->itemList));

    Globals::status->getShip()->setCargo(Item::extractItems(this->itemList, true));

    if (this->itemList != 0) {
        ArrayReleaseClasses(*this->itemList);
        delete this->itemList;
    }
    this->itemList = 0;

    this->itemList = Item::mixItems(Globals::status->getShip()->getCargo(), Globals::status->getStation()->getItems());
    this->hangarList->initShipTab(Globals::status->getShip());

    ItemArray *items = Item::mixItems(Globals::status->getShip()->getCargo(), Globals::status->getStation()->getItems());
    this->hangarList->initShopTab(items, Globals::status->getStation()->getShips());
    this->hangarList->setCurrentTab(0, true);

    refreshCurrentContentHeight();
    this->scrollOffset = this->savedScrollOffset;
    Globals::sound->play(0x60, nullptr, nullptr, 0.0f);
}






int HangarWindow::OnTouchBegin(int touch, int coord) {
    this->holdTime = 0;
    this->repeatTimer = 0;
    int handled = Globals::layout->OnTouchBegin(touch, coord);

    if (this->dialogActive != 0) {
        if (this->buyCreditsActive != 0) {
            for (int i = kHangarButtonPaidCreditsFirst;; ++i) {
                if (i == kHangarButtonCreditsMore) {
                    break;
                }
                (*this->buttons)[i]->OnTouchBegin(touch, coord);
            }
            (*this->buttons)[kHangarButtonCreditsMore]->OnTouchBegin(touch, coord);
        } else if (this->freeCreditsActive != 0) {
            for (int i = kHangarButtonFreeCreditsFirst;
                 i != kHangarButtonBlueprintAutoComplete; ++i) {
                (*this->buttons)[i]->OnTouchBegin(touch, coord);
            }
        }
        this->dialog->OnTouchBegin(touch, coord);
    } else {
        *reinterpret_cast<volatile int *>(&this->touchStartY) = coord;
        *reinterpret_cast<volatile int *>(&this->lastTouchY) = coord;
        this->scrollDelta = 0;
        this->dragging = 1;

        if (this->viewMode == 1) {
            this->listItemWindow->OnTouchBegin(touch, coord);
        } else {
            int outsideList;
            int row;
            if (Globals::layout->field_0xc >= coord ||
                Globals::h - Globals::layout->field_0x10 <= coord ||
                (row = IDIV(coord - Globals::layout->field_0xc - Globals::layout->field_0x20 -
                                this->rowLayoutMetrics.rowGap - this->scrollOffset,
                            Globals::layout->field_0x70 + this->rowLayoutMetrics.rowGap),
                 row >= this->hangarList->getCurrentLength())) {
                outsideList = 1;
            } else {
                this->hangarList->setCurrentItemIndex(row);
                this->highlightItem(this->hangarList->getCurrentItem());
                if (this->upgradeMode != 0 && this->hangarList->getCurrentItem()->isShip() &&
                    this->hangarList->getCurrentTab() == 1) {
                    Ship *ship = Globals::status->getShip();
                    ship->setCargo(Item::extractItems(this->itemList, true));
                    Globals::status->getStation()->setItems(
                        Item::extractItems(this->itemList, false), false);
                }
                outsideList = 0;
            }

            if (this->hangarList->getCurrentTab() != 4 || this->buyMode == 0 ||
                ((~outsideList & (this->selectedItem == this->bluePrintItem)) != 0) ||
                this->bluePrintBuyCount < 1 || this->dialogActive != 0 ||
                this->bluePrint->isEmpty() ||
                this->bluePrint->getStationIndex() == Globals::status->getStation()->getIndex()) {
                for (unsigned int i = 0;; ++i) {
                    Array<TouchButton *> *buttons = this->tabButtons;
                    if (i >= buttons->size()) {
                        break;
                    }
                    handled |= (*buttons)[i]->OnTouchBegin(touch, coord);
                }
                for (unsigned int i = 0;; ++i) {
                    Array<TouchButton *> *buttons = this->buttons;
                    if (i >= buttons->size()) {
                        break;
                    }
                    TouchButton *button = (*buttons)[i];
                    if (button != nullptr) {
                        button->OnTouchBegin(touch, coord);
                    }
                }

                if (this->autoEquipPending != 0) {
                    if (this->hangarList->getCurrentTab() == 1) {
                        int index = this->autoEquipIndex;
                        if (index >= 0 &&
                            ((~handled & (index == this->hangarList->getCurrentItemIndex())) == 0)) {
                            this->autoEquipPending = 0;
                            this->autoEquipSecondaryWeapons(this->autoEquipIndex);
                        }
                    }
                }
            } else {
                this->localBluePrint = this->bluePrintItem->item->getIndex() == 209 ||
                                       this->bluePrintItem->item->getIndex() == 204;

                String message;
                GameText *gameText = Globals::gameText;
                const String *text;
                if (this->localBluePrint != 0) {
                    text = gameText->getText(289);
                } else {
                    text = gameText->getText(288);
                }
                message = String(*text, false);

                bool confirm;
                if (this->localBluePrint != 0) {
                    confirm = false;
                } else {
                    message = Globals::status->replaceHash(
                        String(message, false), String(this->bluePrint->getStationName(), false),
                        String("#S"));
                    message = Globals::status->replaceHash(
                        String(message, false),
                        String(Globals::layout->formatCredits(
                                   this->bluePrintItem->item->getBlueprintAmount() * 200),
                               false),
                        String("#C"));
                    confirm = this->localBluePrint == 0;
                }

                this->dialog->set(message, confirm);
                *reinterpret_cast<volatile uint8_t *>(&this->dialogActive) = 1;
                *reinterpret_cast<volatile uint8_t *>(&this->bluePrintPurchasePending) = 1;
                *reinterpret_cast<volatile uint8_t *>(&this->suppressTouchEnd) = 1;
            }
        }
    }
    return 0;
}








void HangarWindow::showCreditsBuyWindow() {
    static_cast<GameData *>(static_cast<ApplicationManager *>(Globals::appManager)->GetApplicationData())
        ->dlcMenuAckFlag = 0;
    static_cast<GameData *>(static_cast<ApplicationManager *>(Globals::appManager)->GetApplicationData())
        ->dlcMenuRequestFlag = 1;

    ChoiceWindow *dialog = this->dialog;
    if (this->listModeFlag != 0) {
        {
            String title("");
            String message("");
            String left("");
            String right("");
            const String *text = Globals::gameText->getText(170);
            dialog->set(title, message, false, left, right, *text, -1, -1);
        }
        if (Globals::iPad != 0) {
            this->dialog->setWidth(this->buttonWidth * 3);
            this->dialog->setHeight(static_cast<int>(static_cast<float>(this->gridButtonHeight) * 2.3f));
        } else {
            this->dialog->setWidth(Globals::w);
            this->dialog->setHeight(Globals::h);
        }
    } else {
        {
            String title("");
            String message("\n\n\n\n\n\n\n\n");
            String left("");
            String right("");
            const String *text = Globals::gameText->getText(170);
            dialog->set(title, message, false, left, right, *text, -1, -1);
        }
    }

    *reinterpret_cast<volatile uint8_t *>(&this->dialogActive) = 1;
    *reinterpret_cast<volatile uint16_t *>(&this->buyCreditsActive) = 1;
}

int HangarWindow::getCurrentTab() {
    return this->hangarList->getCurrentTab();
}

void HangarWindow::refreshCargoAvailabilityForBlueprints() {
    Array<Array<ListItem *> *> *items = this->hangarList->getItems();
    Array<ListItem *> *arr = (*items)[2];
    if (arr == nullptr) return;
    for (uint32_t i = 0; i < arr->size(); i++) {
        ListItem *it = arr->data()[i];

        it->craftable = 0;
        if (it != nullptr && it->isBluePrint() != 0) {
            BluePrint *bp = it->bluePrint;
            Array<Item *> *cargo = Status::gStatus->getShip()->getCargo();
            Array<int> *ingr = bp->getIngredientList();
            if (cargo != nullptr) {
                Array<int> *counters = bp->ingredientCounters;
                for (uint32_t j = 0; j < ingr->size(); j++) {
                      if (counters->data()[j] > 0) {
                        for (uint32_t k = 0; k < cargo->size(); k++) {
                            if (cargo->data()[k]->getIndex() == ingr->data()[j]) {
                                it->craftable = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}







void HangarWindow::setSellMode(bool buy) {
    const bool requestedMode = buy;
    ListItem *item = this->selectedItem;
    if (this->buyMode != 0) {
        if (item == nullptr) {
            this->buyMode = 0;
            return;
        }
    } else if (item == nullptr || !buy) {
        this->buyMode = 0;
        return;
    }
    if (this->selectedItem->isShip() || item->isSlot() || item->isTextButton() ||
        !item->isSelectable() || item->isBluePrint()) {
        this->buyMode = 0;
        return;
    }

    this->buyMode = requestedMode;
    if (this->hangarList->getCurrentTab() == 1) {
        if (this->buyMode != 0) {
            if (g_hangarStoreBuyHintShown == 0) {
                this->dialog->set(*Globals::gameText->getText(588));
                g_hangarStoreBuyHintShown = 1;
                this->dialogActive = 1;
            }
            this->savedStationAmount = item->item->getStationAmount();
            this->savedAmount = item->item->getAmount();
            this->savedCredits = Globals::status->getCredits();
            this->savedLoad = this->currentLoad;
        } else {
            if (item->isItem() && item->item->getType() != 4 &&
                g_hangarStoreSellHintShown == 0) {
                this->dialog->set(*Globals::gameText->getText(589));
                g_hangarStoreSellHintShown = 1;
                this->dialogActive = 1;
            }
            this->autoEquipPending = 1;
            this->autoEquipIndex = this->hangarList->getCurrentItemIndex();
        }

        Globals::status->getShip()->setCargo(Item::extractItems(this->itemList, true));
        Globals::status->getStation()->setItems(
            Item::extractItems(this->itemList, false), false);
        if (this->itemList != nullptr) {
            ArrayReleaseClasses(*this->itemList);
            delete this->itemList;
        }
        this->itemList = nullptr;
        this->itemList = Item::mixItems(
            Globals::status->getShip()->getCargo(), Globals::status->getStation()->getItems());
        this->hangarList->initShopTab(
            this->itemList, Globals::status->getStation()->getShips());
        this->hangarList->initShipTab(Globals::status->getShip());

        this->selectedItem = this->hangarList->getCurrentItem();
        if (this->selectedItem != nullptr && this->selectedItem->isShip()) {
            this->selectedItem->ship->adjustPrice();
        }
        this->refreshCurrentContentHeight();
    } else if (this->hangarList->getCurrentTab() == 4) {
        if (this->buyMode == 0) {
            if (this->bluePrintItem != nullptr) {
                BluePrint *bluePrint = this->bluePrint;
                if (bluePrint != nullptr) {
                    Item *ingredient = this->bluePrintItem->item;
                    bluePrint->addItem(
                        ingredient, ingredient->getBlueprintAmount(),
                        Globals::status->getStation()->getIndex());
                }
            }

            bool completed;
            if (this->bluePrint->isCompleted()) {
                if (this->bluePrint->getStationIndex() ==
                    Globals::status->getStation()->getIndex()) {
                    String message(*Globals::gameText->getText(211), false);
                    message = Globals::status->replaceHash(
                        message,
                        String(
                            *Globals::gameText->getText(this->bluePrint->getIndex() + 1274),
                            false),
                        String("#N"));
                    this->dialog->set(message);

                    Array<Item *> *items = static_cast<Array<Item *> *>(Globals::items);
                    Item *product = (*items)[this->bluePrint->getIndex()]->makeItem(
                        this->bluePrint->getQuantity());
                    Globals::status->getShip()->addCargo(product);
                    ArrayAdd(product, *this->itemList);
                    this->hangarList->setCurrentTab(1, true);
                    this->refreshCurrentContentHeight();
                } else {
                    String message(*Globals::gameText->getText(210), false);
                    message = Globals::status->replaceHash(
                        message,
                        String(
                            *Globals::gameText->getText(this->bluePrint->getIndex() + 1274),
                            false),
                        String("#N"));
                    message = Globals::status->replaceHash(
                        message, String(this->bluePrint->getStationName(), false), String("#S"));
                    this->dialog->set(message);
                    Globals::status->addPendingProduct(this->bluePrint);
                    this->hangarList->setCurrentTab(2, true);
                    this->refreshCargoAvailabilityForBlueprints();
                }
                this->bluePrint->reset();
                completed = true;
            } else {
                completed = false;
            }

            Globals::status->getShip()->setCargo(
                Item::extractItems(Globals::status->getShip()->getCargo(), true));
            this->hangarList->initShopTab(
                Item::mixItems(
                    Globals::status->getShip()->getCargo(),
                    Globals::status->getStation()->getItems()),
                Globals::status->getStation()->getShips());
            this->hangarList->initBlueprintTab(Globals::status->getBluePrints());
            this->itemList = Item::mixItems(
                Globals::status->getShip()->getCargo(), Globals::status->getStation()->getItems());
            this->dialogActive = completed;

            if (completed) {
                Array<ListItem *> *items = this->hangarList->getCurrentTabItems();
                for (unsigned int i = 0; i < items->size(); ++i) {
                    ListItem *candidate = (*items)[i];
                    if (candidate != nullptr && candidate->isItem() &&
                        candidate->item->getIndex() == this->bluePrint->getIndex() &&
                        Globals::status->getShip()->hasEquipment(
                            candidate->item->getIndex(), 1)) {
                        this->autoEquipIndex = i;
                        this->autoEquipPending = 1;
                        this->autoEquipSecondaryWeapons(static_cast<int>(i));
                        this->autoEquipPending = 0;
                        return;
                    }
                }
            }
            return;
        }

        this->bluePrintBuyCount = 0;
        if (this->bluePrint->isEmpty() && item->item->getAmount() >= 1) {
            if ((this->bluePrint->getIndex() == 210 || this->bluePrint->getIndex() == 223) &&
                Globals::status->getSystem()->getRoutes() == nullptr) {
                this->routeWarningPending = 1;
                this->dialog->set(*Globals::gameText->getText(528), false);
            } else {
                this->dialog->set(*Globals::gameText->getText(212), true);
            }
            this->dialogActive = 1;
        }

        this->savedBlueprintAmount = item->item->getBlueprintAmount();
        this->savedAmount = item->item->getAmount();
        this->savedCredits = Globals::status->getCredits();
        this->savedLoad = this->currentLoad;
        this->bluePrintItem = this->selectedItem;
    }

    this->refreshCurrentContentHeight();
}










int HangarWindow::selectItem(ListItem *item) {
    this->selectedItem = item;
    if (item != nullptr && item->isShip()) {
        this->selectedItem->ship->adjustPrice();
    }

    const int tab = this->hangarList->getCurrentTab();

    if (tab == 2) {
        if (item->isSelectable() && !item->isPendingProduct()) {
            this->bluePrint = item->bluePrint;
            this->hangarList->fillIngredientsList(this->bluePrint, false);
            this->hangarList->setCurrentTab(4, true);
            this->refreshCurrentContentHeight();
            if (this->specialMode) {
                this->specialMode = 0;
            }
        }
    } else if (tab == 1) {
        if (!item->isSelectable()) {
            return 0;
        }

        if (item->isShip()) {
            const int price = item->ship->getPrice();
            const int credits = Globals::status->getCredits();
            Ship *activeShip = Globals::status->getShip();
            if (price > activeShip->getPrice() + credits && !this->upgradeMode) {
                String message = [&]() __attribute__((always_inline)) -> String {
                    String missing = Globals::layout->formatCredits(
                        item->getPrice() - Globals::status->getCredits() -
                        Globals::status->getShip()->getPrice());
                    return Globals::status->replaceHash(
                        String(*Globals::gameText->getText(203)), missing, String("#C"));
                }();
                {
                    String newline("\n\n");
                    String suffix = newline + *Globals::gameText->getText(124);
                    message += suffix;
                }
                this->dialog->set(message, true);
                this->dialogActive = 1;
                this->notEnoughCredits = 1;
                return 0;
            }

            if (Globals::status->passengers < 1) {
                if (Globals::status->getCurrentCampaignMission() == 77 &&
                    Globals::status->getShip()->getIndex() == 37) {
                    this->dialog->set(*Globals::gameText->getText(325));
                } else if (Globals::status->getShip()->getIndex() != item->ship->getIndex()) {
                    this->shipSwapPending = 1;
                    this->dialog->set(
                        *Globals::gameText->getText(this->upgradeMode ? 333 : 304), true);
                    this->dialogActive = 1;
                    return 0;
                } else {
                    this->dialog->set(*Globals::gameText->getText(329));
                }
            } else {
                this->dialog->set(*Globals::gameText->getText(336));
            }
            this->dialogActive = 1;
            return 0;
        }

        if (!item->item->isUnsaleable()) {
            const uint8_t previousMode = this->buyMode;
            this->buyMode = previousMode ^ 1;
            if (previousMode) {
                if (item->isItem() && item->item->getType() != 4 && !g_hangarStoreSellHintShown) {
                    this->dialog->set(*Globals::gameText->getText(589));
                    g_hangarStoreSellHintShown = 1;
                    this->dialogActive = 1;
                }
                if (item->getIndex() >= 132 && item->getIndex() <= 153) {
                    (*Globals::status->field_ac)[item->getIndex() - 132] = true;
                }
                this->autoEquipPending = 1;
                this->autoEquipIndex = this->hangarList->getCurrentItemIndex();

                Globals::status->getShip()->setCargo(Item::extractItems(this->itemList, true));
                Globals::status->getStation()->setItems(
                    Item::extractItems(this->itemList, false), false);
                if (this->itemList != nullptr) {
                    ArrayReleaseClasses(*this->itemList);
                    delete this->itemList;
                }
                this->itemList = nullptr;
                this->itemList = Item::mixItems(
                    Globals::status->getShip()->getCargo(), Globals::status->getStation()->getItems());
                this->hangarList->initShopTab(
                    this->itemList, Globals::status->getStation()->getShips());
                this->hangarList->initShipTab(Globals::status->getShip());
            } else {
                if (!g_hangarStoreBuyHintShown) {
                    this->dialog->set(*Globals::gameText->getText(588));
                    g_hangarStoreBuyHintShown = 1;
                    this->dialogActive = 1;
                }
                this->savedStationAmount = item->item->getStationAmount();
                this->savedAmount = item->item->getAmount();
                this->savedCredits = Globals::status->getCredits();
                this->savedLoad = this->currentLoad;
            }
        }
    } else if (tab == 0 && item->isSelectable()) {
        this->scrollOffsetBackup = 0;
    this->savedScrollOffset = this->scrollOffset;
    this->scrollOffset = 0;

    if (!g_hangarSlotHintShown && item->isSlot()) {
        this->dialog->set(*Globals::gameText->getText(587));
        g_hangarSlotHintShown = 1;
        this->dialogActive = 1;
    }

    if (!item->isSelectable()) {
        return 0;
    }

    Item *currentItem = this->hangarList->getCurrentItem()->item;
    if (currentItem != nullptr) {
        if (currentItem->isUnsaleable()) {
            this->dialog->set(*Globals::gameText->getText(323));
            this->dialogActive = 1;
            return 0;
        }
        if (currentItem->getSort() == 20) {
            const int passengers = Globals::status->passengers;
            if (passengers >= 1 && item->inTabIndex >= 0 &&
                Globals::status->getShip()->getMaxPassengers() - currentItem->getAttribute(34) <
                    passengers) {
                this->dialog->set(*Globals::gameText->getText(323));
                this->dialogActive = 1;
                return 0;
            }
        }
    }

    if (item->inTabIndex >= 0) {
        this->demountItem(currentItem, this->hangarList->getCurrentItem()->subTabIndex);
        return 0;
    }

    Ship *ship = Globals::status->getShip();
    Item *existing = ship->getFirstEquipmentOfSort(item->item->getSort());
    bool canMount = false;
    if (item->item->getSort() == 21) {
        if (Globals::status->getShip()->getIndex() == 44) {
            canMount = true;
        } else {
            const int shipIndex = Globals::status->getShip()->getIndex();
            if (existing == nullptr) {
                canMount = true;
            } else {
                canMount = shipIndex == 49;
            }
        }
    } else if (existing == nullptr) {
        canMount = true;
    }
    if (canMount || item->item->canBeInstalledMultipleTimes()) {
        this->mountItem(item->item);
        return 0;
    }

    String message(*Globals::gameText->getText(287));
    message = Globals::status->replaceHash(
        Globals::status->replaceHash(
            message, *Globals::gameText->getText(existing->getIndex() + 1274), String("#ITEM1")),
        *Globals::gameText->getText(item->item->getIndex() + 1274), String("#ITEM2"));
    this->dialog->set(message, true);
    this->replaceEquipPending = 1;
    this->dialogActive = 1;
    this->pendingMountItem = item->item;
        this->pendingDemountItem = existing;
    }
    return 0;
}

float HangarWindow::getRelativeScrollHeight() {
    int contentHeight = this->currentContentHeight;
    int visibleHeight = this->visibleHeight;
    if (visibleHeight > contentHeight) {
        return 0.0f;
    }

    int scroll = this->scrollOffset;
    int visiblePart;
    float contentHeightFloat;
    if (scroll >= 1) {
        contentHeightFloat = (float) contentHeight;
#if !defined(__aarch64__)
        __asm__ volatile("" : "+t"(contentHeightFloat));
#endif
        visiblePart = visibleHeight - scroll;
    } else if (scroll >= visibleHeight - contentHeight) {
        __asm__ volatile("" ::: "memory");
        contentHeightFloat = (float) contentHeight;
        visiblePart = visibleHeight;
    } else {
        contentHeightFloat = (float) contentHeight;
#if !defined(__aarch64__)
        __asm__ volatile("" : "+t"(contentHeightFloat));
#endif
        visiblePart = scroll + contentHeight;
    }
    return (float) visiblePart / contentHeightFloat;
}





void HangarWindow::transaction(bool buy) {
    const unsigned int tab = this->hangarList->getCurrentTab();
    if (tab <= 1) {
        if (this->selectedItem->item->isUnsaleable()) {
            this->dialog->set(*Globals::gameText->getText(323));
            *reinterpret_cast<volatile uint8_t *>(&this->buyMode) = 0;
            *reinterpret_cast<volatile uint8_t *>(&this->dialogActive) = 1;
            return;
        }
        const int result = this->selectedItem->item->transaction(
            buy, this->currentLoad, this->upgradeMode);
        const unsigned int itemIndex = this->selectedItem->item->getIndex();
        Array<bool> *seenItems = Globals::status->field_54;
        if (itemIndex < seenItems->size()) {
            (*seenItems)[this->selectedItem->item->getIndex()] = true;
        }

        if (result <= -1 && buy) {
            ++this->currentLoad;
            Globals::status->getShip()->changeLoad(1);
            if (this->selectedItem->getIndex() >= 132 && this->selectedItem->getIndex() <= 153) {
                (*Globals::status->field_ac)[this->selectedItem->getIndex() - 132] = true;
            }
        } else if (result != 0 || !buy) {
            if (result >= 1 && !buy) {
                --this->currentLoad;
                Globals::status->getShip()->changeLoad(-1);
            }
        } else if (Globals::status->getCredits() < this->selectedItem->item->getSinglePrice()) {
            if (this->upgradeMode) {
                return;
            }
            String message = [&]() __attribute__((always_inline)) -> String {
                String missing = Globals::layout->formatCredits(
                    this->selectedItem->item->getSinglePrice() - Globals::status->getCredits());
                return Globals::status->replaceHash(
                    String(*Globals::gameText->getText(203)), missing, String("#C"));
            }();
            {
                String newline("\n\n");
                String suffix = newline + *Globals::gameText->getText(124);
                message += suffix;
            }
            this->dialog->set(message, true);
            this->dialogActive = 1;
            this->notEnoughCredits = 1;
            (*this->buttons)[8]->resetTouch();
            (*this->buttons)[9]->resetTouch();
        }

        if (!this->upgradeMode) {
            Globals::status->changeCredits(result);
        }
    } else if (tab == 4) {
        if (buy) {
            const int blueprintAmount = this->selectedItem->item->getBlueprintAmount();
            BluePrint *bluePrint = this->bluePrint;
            const int itemIndex = this->selectedItem->item->getIndex();
            if (blueprintAmount < bluePrint->getRemainingAmount(itemIndex)) {
                const int result = this->selectedItem->item->transactionBlueprint(
                    false, this->currentLoad);
                if (result <= -1) {
                    ++this->currentLoad;
                } else if (result != 0) {
                    ++this->bluePrintBuyCount;
                    Globals::status->getShip()->changeLoad(-1);
                }
            }
        }

        Array<Item *> *cargo = Globals::status->getShip()->getCargo();
        if (cargo != nullptr) {
            for (unsigned int i = 0; i < cargo->size(); ++i) {
                Item *cargoItem = cargo->data()[i];
                if (cargoItem->getIndex() == this->selectedItem->item->getIndex()) {
                    cargoItem->setAmount(this->selectedItem->item->getAmount());
                    cargoItem->setBlueprintAmount(this->selectedItem->item->getBlueprintAmount());
                }
            }
        }
    }
}



void HangarWindow::mountItem(Item *item) {
    int type = item->getType();
    int amount = 1;
    if (type == 1)
        amount = item->getAmount();

    Item *made = item->makeItem(amount);
    Ship *ship = Globals::status->getShip();
    ship->addEquipment(made);
    Globals::status->getShip()->removeCargo(made->getIndex(), type == 1 ? made->getAmount() : 1);

    Array<Item *> *cargo = this->itemList;
    if (cargo != nullptr) {
        for (unsigned int i = 0; i < cargo->size(); i++) {
            Item *cur = cargo->data()[i];
            if (cur->getIndex() == item->getIndex()) {
                int change;
                if (cur->getStationAmount() == 0) {
                    if (type == 1 || item->getAmount() == 1) {
                        ArrayRemove(cur, *this->itemList);
                        break;
                    }
                    change = -1;
                } else if (type == 1) {
                    change = -item->getAmount();
                } else {
                    change = -1;
                }
                cur->changeAmount(change);
                break;
            }
            cargo = this->itemList;
        }
    }

    Globals::status->getShip()->setCargo(Item::extractItems(this->itemList, true));
    this->hangarList->initShipTab(Globals::status->getShip());

    ItemArray *items = Item::mixItems(Globals::status->getShip()->getCargo(), Globals::status->getStation()->getItems());
    this->hangarList->initShopTab(items, Globals::status->getStation()->getShips());
    this->hangarList->setCurrentTab(0, true);

    refreshCurrentContentHeight();
    this->scrollOffset = this->savedScrollOffset;
    Globals::sound->play(0x62, nullptr, nullptr, 0.0f);
}




int HangarWindow::OnTouchMove(int touch, int coord) {
    static_cast<Layout *>(Globals::layout)->OnTouchMove(touch, coord);

    if (this->dialogActive != 0) {
        if (this->buyCreditsActive != 0) {
            for (unsigned int i = kHangarButtonPaidCreditsFirst;
                 i <= kHangarButtonPaidCreditsLast; ++i) {
                (*this->buttons)[i]->OnTouchMove(touch, coord);
            }
            (*this->buttons)[kHangarButtonCreditsMore]->OnTouchMove(touch, coord);
        } else if (this->freeCreditsActive != 0) {
            for (unsigned int i = kHangarButtonFreeCreditsFirst;
                 i <= kHangarButtonFreeCreditsLast; ++i) {
                (*this->buttons)[i]->OnTouchMove(touch, coord);
            }
        }
        this->dialog->OnTouchMove(touch, coord);
    } else if (this->viewMode == 1) {
        this->listItemWindow->OnTouchMove(touch, coord);
    } else {
        if (static_cast<Layout *>(Globals::layout)->field_0xc < coord &&
            coord < Globals::h - static_cast<Layout *>(Globals::layout)->field_0x10) {
            const int delta = coord - this->lastTouchY;
            this->scrollDelta = delta;
            this->damping = 1.0f;
            this->scrollOffset += delta;
            this->lastTouchY = coord;

            int isTouched;
            if ((*this->buttons)[kHangarButtonCurrentAmount]->isTouched()) {
                isTouched = 1;
            } else {
                isTouched = (*this->buttons)[kHangarButtonStationAmount]->isTouched();
            }
            int touchDistance = coord - this->touchStartY;
            if (touchDistance < 0) {
                touchDistance = -touchDistance;
            }

            if (isTouched != 0 || touchDistance < 6) {
                // Keep the current touch; the native body skips the reset path.
            } else {
                this->holdTime = 0;
                this->repeatTimer = 0;
                for (unsigned int i = 0; i < this->buttons->size(); ++i) {
                    (*this->buttons)[i]->OnTouchMove(touch, coord);
                }
                this->setSellMode(false);
                this->sellConfirmPending = 0;
                this->selectedItem = nullptr;
                (*this->buttons)[kHangarButtonCurrentAmount]->resetTouch();
                (*this->buttons)[kHangarButtonStationAmount]->resetTouch();
            }
        }

        for (unsigned int i = 0; i < this->tabButtons->size(); ++i) {
            (*this->tabButtons)[i]->OnTouchMove(touch, coord);
        }
    }
    return 0;
}




void HangarWindow::autoEquipSecondaryWeapons(int row) {
    ListItem *listItem = this->hangarList->getCurrentItemAt(row);
    if (listItem != nullptr) {
        Item *candidate = listItem->item;
        if (candidate != nullptr && candidate->getType() == 1 && listItem->isItem() &&
            listItem->item->getType() == 1 && listItem->item->getAmount() >= 1) {
            Array<Item *> *secondaryWeapons = Globals::status->getShip()->getEquipment(1);
            if (secondaryWeapons != nullptr) {
                for (unsigned int slot = 0; slot < secondaryWeapons->size(); ++slot) {
                    Item *equipped = (*secondaryWeapons)[slot];
                    if (equipped != nullptr &&
                        (*secondaryWeapons)[slot]->getIndex() == listItem->item->getIndex()) {
                        Item *merged = listItem->item->makeItem(
                            equipped->getAmount() + listItem->item->getAmount());
                        if (this->itemList != nullptr) {
                            for (unsigned int i = 0; i < this->itemList->size(); ++i) {
                                if ((*this->itemList)[i]->getIndex() == merged->getIndex()) {
                                    (*this->itemList)[i]->setAmount(0);
                                }
                            }
                        }

                        Globals::status->getShip()->setEquipment(merged, static_cast<int>(slot));
                        Globals::status->getShip()->removeCargo(
                            merged->getIndex(), merged->getAmount());
                        this->hangarList->initShipTab(Globals::status->getShip());

                        String message(*Globals::gameText->getText(208));
                        message = Globals::status->replaceHash(
                            message,
                            String(*Globals::gameText->getText(merged->getIndex() + 1274)),
                            String("#N"));
                        this->dialog->set(message);
                        this->autoEquipped = 1;
                        this->dialogActive = 1;
                        return;
                    }
                }
            }
        }
    }
}




void HangarWindow::showFreeCreditsWindow() {
    static_cast<GameData *>(static_cast<ApplicationManager *>(Globals::appManager)
                                ->GetApplicationData())->dlcMenuAckFlag = 0;
    static_cast<GameData *>(static_cast<ApplicationManager *>(Globals::appManager)
                                ->GetApplicationData())->dlcMenuRequestFlag = 1;

    ChoiceWindow *dialog = this->dialog;
    {
        String title("");
        String message("");
        String left("");
        String right("");
        const String *text = Globals::gameText->getText(170);
        dialog->set(title, message, false, left, right, *text, -1, -1);
    }

    this->dialog->setHeight((*this->buttons)[18]->getHeight() * 5);

    int maxTextWidth = 0;
    for (int i = 0; i < 5; ++i) {
        int textId = 112 + i;
        const unsigned int font = Globals::font;
        PaintCanvas *canvas = Globals::Canvas;
        if (i == 0)
            textId = 3400;
        const String *line = Globals::gameText->getText(textId);
        int textWidth = canvas->GetTextWidth(font, *line);
        if (textWidth > maxTextWidth) {
            maxTextWidth = textWidth;
        }
    }
    int buttonWidth = (*this->buttons)[18]->getWidth();
    Layout *layout = static_cast<Layout *>(Globals::layout);
    this->dialog->setWidth(layout->field_0x2c + buttonWidth + maxTextWidth +
                           layout->field_0x28 * 4);

    this->freeCreditsActive = 1;
    this->dialogActive = 1;
    *reinterpret_cast<uint16_t *>(&this->buyCreditsActive) = 0;
}
















void HangarWindow::initialize() {
    HangarWindow *self = this;

    uint8_t special = Globals::status->getStation()->getIndex() == 0x6c && Globals::status->field_114 == 3;
    self->upgradeMode = special;
    Globals::status->calcCargoPrices();

    self->hangarList = new HangarList();
    Array<Item *> *initialCargo = Globals::status->getShip()->getCargo();
    Array<Item *> *initialStationItems = Globals::status->getStation()->getItems();
    self->itemList = Item::mixItems(initialCargo, initialStationItems);
    HangarList *list = self->hangarList;
    Ship *initialShip = Globals::status->getShip();
    Array<Item *> *initialItems = self->itemList;
    Array<Ship *> *initialStationShips = Globals::status->getStation()->getShips();
    Array<BluePrint *> *initialBlueprints = Globals::status->getBluePrints();
    list->init(initialShip, initialItems, initialStationShips, initialBlueprints);

    self->tabButtons = new Array<TouchButton *>();
    ArraySetLength(3, *(self->tabButtons));

    TouchButton *b0 = new TouchButton(*Globals::gameText->getText(272), 3,
                                      Globals::w - Globals::layout->getHelpButtonOffset(), 0, 0x12);
    (*self->tabButtons)[(8) >> 2] = b0;

    int tab1TextId = self->upgradeMode != 0 ? 186 : 185;
    const String *tab1Text = Globals::gameText->getText(tab1TextId);
    int tab1ScreenWidth = Globals::w;
    int tab1HelpOffset = Globals::layout->getHelpButtonOffset();
    int w0 = (*self->tabButtons)[2]->getWidth();
    TouchButton *b1 = new TouchButton(
        *tab1Text, 3, (tab1ScreenWidth - tab1HelpOffset - w0) + Globals::layout->field_0x38, 0, 0x12);
    (*self->tabButtons)[(4) >> 2] = b1;

    const String *tab2Text = Globals::gameText->getText(183);
    int tab2ScreenWidth = Globals::w;
    int tab2HelpOffset = Globals::layout->getHelpButtonOffset();
    int w0b = (*self->tabButtons)[2]->getWidth();
    int w1b = (*self->tabButtons)[1]->getWidth();
    TouchButton *b2 = new TouchButton(
        *tab2Text, 3, (tab2ScreenWidth - tab2HelpOffset - w0b - w1b) + Globals::layout->field_0x38 * 2, 0,
        0x12);
    (*self->tabButtons)[(0) >> 2] = b2;
    self->listModeFlag = Globals::showNewCreditsMenu;

    void *icons = ::operator new[](0x18);
    self->tabIcons = icons;
    for (int i = 0; i != 6; i++)

        Globals::Canvas->Image2DCreate((unsigned short)(i + 0x232a),
                                       static_cast<unsigned int *>(self->tabIcons)[i]);

    for (unsigned int i = 0; i < self->tabButtons->size(); i++) {
        if (i < 10) {
            {
                Vector position = self->tabButtons->data()[i]->getPosition();
                Globals::sub_menu_buttons_x[i] = static_cast<int>(position.x);
            }
            {
                Vector position = self->tabButtons->data()[i]->getPosition();
                Globals::sub_menu_buttons_y[i] = static_cast<int>(position.y);
            }
        }
    }

    Globals::sub_menu_button_count = (int)self->tabButtons->size();
    Globals::Canvas->Image2DCreate((unsigned short)(0x52e), self->blueprintIconImage);
    Globals::Canvas->Image2DCreate((unsigned short)(0x544), self->pendingIconImage);

    self->buttons = new Array<TouchButton *>();
    ArraySetLength(kHangarButtonCount, *(self->buttons));

    unsigned int img;
    img = 0xffffffff;
    Globals::Canvas->Image2DCreate((unsigned short)(0x470), img);
    TouchButton *e0 = new TouchButton(img, 7, 0, 0, Globals::layout->field_0x60, 0x11, 4);
    (*self->buttons)[kHangarButtonInspect] = e0;

    bool deepScienceCampaign = Globals::status->getCurrentCampaignMission() == 0x4d &&
                               Globals::status->getStation()->getIndex() == 100;

    TouchButton *e1 = new TouchButton(
        *Globals::gameText->getText((self->upgradeMode != 0 || deepScienceCampaign) ? 332 : 301), 7, 0, 0,
        0x11);
    (*self->buttons)[kHangarButtonSelectShip] = e1;
    TouchButton *e2 = new TouchButton(*Globals::gameText->getText(302), 7, 0, 0, 0x11);
    (*self->buttons)[kHangarButtonSelectItemFirst] = e2;

    img = 0xffffffff;
    Globals::Canvas->Image2DCreate((unsigned short)(0x533), img);
    TouchButton *e3 = new TouchButton(img, 7, 0, 0, Globals::layout->field_0x64, 0x11, 4);
    (*self->buttons)[kHangarButtonSelectItemSecond] = e3;

    img = 0xffffffff;
    Globals::Canvas->Image2DCreate((unsigned short)(0x532), img);
    TouchButton *e4 = new TouchButton(img, 7, 0, 0, Globals::layout->field_0x64, 0x11, 4);
    (*self->buttons)[kHangarButtonSelectItemThird] = e4;

    TouchButton *e5 =
        new TouchButton(*Globals::gameText->getText(279), 7, 0, 0, self->listEntryActionWidth, 0x11, 1);
    (*self->buttons)[kHangarButtonSellListEntry] = e5;
    TouchButton *e6 =
        new TouchButton(*Globals::gameText->getText(282), 7, 0, 0, self->listEntryActionWidth, 0x11, 1);
    (*self->buttons)[kHangarButtonMoveToCargoEntry] = e6;
    TouchButton *e7 = new TouchButton(*Globals::gameText->getText(283), 7, 0, 0, 0x11);
    (*self->buttons)[kHangarButtonSelectBlueprint] = e7;

    {
        String lbl("", false);
        TouchButton *e8 = new TouchButton(lbl, 8, 0, 0, Globals::layout->field_0x50, 0x11, 4);
        (*self->buttons)[kHangarButtonCurrentAmount] = e8;
    }
    {
        String lbl("", false);
        TouchButton *e9 = new TouchButton(lbl, 9, 0, 0, Globals::layout->field_0x50, 0x11, 4);
        (*self->buttons)[kHangarButtonStationAmount] = e9;
    }
    {
        TouchButton *e10 =
            new TouchButton(*Globals::gameText->getText(330), 7, 0, 0, Globals::layout->field_0x50, 0x11, 4);
        (*self->buttons)[kHangarButtonSellShip] = e10;
    }
    {
        String credits = Globals::layout->formatCredits(Globals::status->getCredits());
        TouchButton *e11 = new TouchButton(credits, 0xb, Globals::w, Globals::h,
                                           Globals::layout->getFooterTransitionWidth(), 0x22, 4);
        (*self->buttons)[kHangarButtonCredits] = e11;
    }

    int row = 0;
    for (unsigned int slot = kHangarButtonPaidCreditsFirst; slot <= kHangarButtonPaidCreditsLast; slot++) {
        uint8_t listMode = self->listModeFlag;
        int visIdx;
        {
            String lbl("", false);
            TouchButton *btn;
            if (listMode == 0) {
                btn = new TouchButton(lbl, 0, 0, 0, Globals::layout->field_0x264, 0x11, 1);
                visIdx = row + kHangarButtonPaidCreditsFirst;
            } else {
                btn = new TouchButton(lbl, 10, 0, 0, 1);
                visIdx = slot;
            }
            (*self->buttons)[slot] = btn;
        }
        (*self->buttons)[visIdx]->setVisible(false);
        row++;
    }
    {
        uint8_t listMode = self->listModeFlag;
        {
            String lbl("", false);
            TouchButton *btn;
            if (listMode == 0)
                btn = new TouchButton(lbl, 0, 0, 0, Globals::layout->field_0x264, 0x11, 1);
            else
                btn = new TouchButton(lbl, 10, 0, 0, 1);
            (*self->buttons)[kHangarButtonCreditsMore] = btn;
        }
        (*self->buttons)[kHangarButtonCreditsMore]->setVisible(false);
    }

    Globals::Canvas->Image2DCreate((unsigned short)(0x233e), self->scrollHintImageA);
    Globals::Canvas->Image2DCreate((unsigned short)(0x233f), self->scrollHintImageB);
    {
        String lbl("Autocomplete");
        TouchButton *btn = new TouchButton(lbl, 7, 0, 0, 0x11);
        (*self->buttons)[kHangarButtonBlueprintAutoComplete] = btn;
    }

    // Android initialize seeds both handles before the social-offer loop.
    unsigned int imgA = 0xffffffffu, imgB = 0xffffffffu;
    for (unsigned int i = kHangarButtonFreeCreditsFirst; i <= kHangarButtonFreeCreditsLast; i++) {
        if (i == kHangarButtonFreeCreditsFirst) {
            Globals::Canvas->Image2DCreate((unsigned short)(0x233c), imgA);
            Globals::Canvas->Image2DCreate((unsigned short)(0x233d), imgB);
        } else {
            short s = (short)(i - kHangarButtonFreeCreditsFirst);
            Globals::Canvas->Image2DCreate((unsigned short)(s * 2 + 0x2330), imgA);
            Globals::Canvas->Image2DCreate((unsigned short)(s * 2 + 0x2331), imgB);
        }
        TouchButton *btn = new TouchButton(imgA, imgB, 0x13, 0, 0, 1);
        (*self->buttons)[i] = btn;
        (*self->buttons)[i]->setVisible(false);
    }

    self->buttonWidth = (*self->buttons)[kHangarButtonPaidCreditsFirst]->getWidth();
    int h = (*self->buttons)[kHangarButtonPaidCreditsFirst]->getHeight();
    self->gridButtonHeight = h;
    self->gridSpacingX = (int)((float)(-self->buttonWidth) * 0.1f);
    self->gridSpacingY = (int)((float)(-h) * 0.1f);

    Globals::Canvas->Image2DCreate((unsigned short)(0x475),
                                   *reinterpret_cast<unsigned int *>(&self->progressBarBgImage));
    Globals::Canvas->Image2DCreate((unsigned short)(0x476), self->progressBarFillImage);
    Globals::Canvas->Image2DCreate((unsigned short)(0x477), self->progressBarBorderImage);
    self->progressBarWidth = Globals::Canvas->GetImage2DWidth(self->progressBarFillImage);
    self->progressBarHeight = Globals::Canvas->GetImage2DHeight(self->progressBarFillImage);

    if (self->itemList != 0 && Globals::status->inBlackMarketSystem() == 0 && self->upgradeMode == 0) {
        Array<Item *> *equip = Globals::status->getShip()->getEquipment();
        for (unsigned int i = 0;; i++) {
            Array<Item *> *cargo = self->itemList;
            unsigned int cargoCount = cargo->size();
            unsigned int equipCount = equip ? equip->size() : 0;
            if (i >= cargoCount + equipCount)
                break;
            Item *item;
            if (i < cargoCount)
                item = cargo->data()[i];
            else
                item = equip->data()[i - cargoCount];
            if (item != 0) {
                int price = item->getSinglePrice();
                int idx = item->getIndex();
                Status *priceStatus = Globals::status;
                int *buyTbl = priceStatus->field_0x40->data();
                int oldBuyPrice = buyTbl[idx];
                if (price > oldBuyPrice || oldBuyPrice == 0) {
                    buyTbl[idx] = price;
                    int sysIdx = priceStatus->getSystem()->getIndex();
                    priceStatus = Globals::status;
                    priceStatus->field_0x48->data()[idx] = sysIdx;
                }
                int *sellTbl = priceStatus->field_0x3c->data();
                int oldSellPrice = sellTbl[idx];
                if (price < oldSellPrice || oldSellPrice == 0) {
                    sellTbl[idx] = price;
                    int sysIdx = priceStatus->getSystem()->getIndex();
                    Globals::status->field_0x44->data()[idx] = sysIdx;
                }
            }
        }
    }

    Globals::status->getShip()->adjustPrice();

    self->listItemWindow = new ListItemWindow();
    self->viewMode = 0;
    self->choiceWindow = new ChoiceWindow();
    self->dialogActive = 0;
    self->dialog = new ChoiceWindow();

    self->replaceEquipPending = 0;
    self->holdTime = 0;
    self->repeatTimer = 0;
    self->pendingMountItem = 0;
    self->pendingDemountItem = 0;
    self->routeWarningPending = 0;
    self->suppressTouchEnd = 0;
    self->autoCompletePending = 0;
    self->autoEquipped = 0;

    int scrH = Globals::h;
    int contentW = Globals::w - 10;
    self->field_0x40 = 0x10;
    self->field_0x44 = 5;
    self->field_0x48 = 5;
    self->contentWidth = contentW;
    self->contentHeight = scrH - 10;

    int *cols = (int *)::operator new[](0xc);
    self->columnWidths = cols;
    int third = IDIV(contentW, 3) - 2;
    cols[0] = third;
    cols[1] = third;
    cols[2] = (Globals::w - 0xe) + third * -2;

    self->hangarList->setCurrentTab(HangarWindow::lastTab, false);
    self->refreshCurrentContentHeight();

    self->currentLoad = Globals::status->getShip()->getCurrentLoad();
    Layout *lay2 = static_cast<Layout *>(Globals::layout);
    self->visibleHeight =
        ((Globals::h - lay2->field_0x10 - lay2->field_0xc) - lay2->field_0x20) - lay2->field_0x24;

    int extra = 0;
    if (Globals::iPad != 0 && Globals::iPadAssetsWithLowerRes == 0) {
        Globals::Canvas->Image2DCreate((unsigned short)(0x6a4),
                                       *reinterpret_cast<unsigned int *>(&self->hintImage));
        extra = (int)((float)Globals::w * 0.2f);
    }
    self->selectedItem = 0;
    self->hintOffsetX = extra;
    self->buyMode = 0;
    self->bluePrintPurchasePending = 0;
    self->shipSwapPending = 0;
    self->swapConfirmFlag = 0;

    self->damping = 0.0f;
    self->velocity = 0.0f;
    self->touchStartY = 0;
    self->dragging = 0;
    self->suppressTouchEnd = 0;
    self->sellConfirmPending = 0;
    self->savedScrollOffset = 0;
    self->field_0x0 = 0;
    self->active = 1;
    self->autoEquipPending = 0;
    self->autoEquipIndex = 0xffffffff;
    self->scrollOffset = 0;
    self->lastTouchY = 0;
    self->scrollOffsetBackup = 0;
    self->scrollDelta = 0;

    if (Globals::status->getCurrentCampaignMission() >= 14 && g_hangarCreditOfferShown == 0) {
        self->dialog->set(*Globals::gameText->getText(109));
        g_hangarCreditOfferShown = 1;
        static_cast<RecordHandler *>(Globals::recordHandler)->saveOptions();
        self->dialogActive = 1;
    }
}

HangarWindow::HangarWindow() {
#if __SIZEOF_POINTER__ == 4
    // Android 0x00147d20 only clears the state which is live before
    // initialize(). Keep the host-wide defensive initialization below out of
    // the matching object so it does not invent stores in the ARM body.
    this->bluePrint = nullptr;
    this->bluePrintItem = nullptr;
    this->dialogActive = 0;
    this->bluePrintPurchasePending = 0;
    this->hangarList = nullptr;
    this->listItemWindow = nullptr;
    this->choiceWindow = nullptr;
    this->dialog = nullptr;
    this->localBluePrint = 0;
    HangarWindow::lastTab = 1;
    this->routeWarningPending = 0;
    this->dragging = 0;
    this->tabButtons = nullptr;
    this->buttons = nullptr;
    *reinterpret_cast<int *>(&this->shipSwapPending) = 0;
    *reinterpret_cast<int *>(&this->buyCreditsActive) = 0;

    Layout *layout = static_cast<Layout *>(Globals::layout);
    this->rowLayoutMetrics = layout->hangarRowMetrics;
    this->listEntryActionWidth = layout->hangarListEntryFixedWidth;
    this->rowActionOffsetY = layout->hangarSelectedRowActionOffsetY;
    this->iconOffsetY = layout->hangarRowIconOffsetY;
#else
    // Android constructor at 0x00147d20: all interaction state starts empty,
    // then the four row-layout values are copied from Layout. The host build
    // additionally clears fields which Android initializes on initialize().
    this->field_0x0 = 0;
    this->tabButtons = nullptr;
    this->lastDelta = 0;
    this->active = 0;
    this->itemList = nullptr;
    this->hangarList = nullptr;
    this->listItemWindow = nullptr;
    this->choiceWindow = nullptr;
    this->dialog = nullptr;
    this->buttons = nullptr;
    this->pendingMountItem = nullptr;
    this->pendingDemountItem = nullptr;
    this->tabIcons = nullptr;
    this->scrollHintImageA = 0;
    this->scrollHintImageB = 0;
    this->dialogActive = 0;
    this->field_0x40 = 0;
    this->field_0x44 = 0;
    this->field_0x48 = 0;
    this->contentWidth = 0;
    this->contentHeight = 0;
    this->columnWidths = nullptr;
    this->viewMode = 0;
    this->field_0x5c = 0;
    this->field_0x60 = 0;
    this->field_0x64 = 0;
    this->selectedItem = nullptr;
    this->holdTime = 0;
    this->repeatTimer = 0;
    this->progressBarBorderImage = 0;
    this->progressBarBgImage = 0;
    this->progressBarFillImage = 0;
    this->bluePrint = nullptr;
    this->bluePrintItem = nullptr;
    this->buyMode = 0;
    this->specialMode = 0;
    this->field_0x8a = 0;
    this->field_0x8b = 0;
    this->savedStationAmount = 0;
    this->shipSwapPending = 0;
    this->dlcMenuPending = 0;
    this->swapConfirmFlag = 0;
    this->sellShipPending = 0;
    this->bluePrintBuyCount = 0;
    this->savedCredits = 0;
    this->savedLoad = 0;
    this->savedAmount = 0;
    this->savedBlueprintAmount = 0;
    this->currentLoad = 0;
    this->bluePrintPurchasePending = 0;
    this->autoEquipped = 0;
    this->buyCreditsActive = 0;
    this->notEnoughCredits = 0;
    this->freeCreditsActive = 0;
    this->autoCompletePending = 0;
    this->field_0xb2 = 0;
    this->field_0xb3 = 0;
    this->scrollOffset = 0;
    this->lastTouchY = 0;
    this->scrollOffsetBackup = 0;
    this->scrollDelta = 0;
    this->damping = 0.0f;
    this->velocity = 0.0f;
    this->touchStartY = 0;
    this->dragging = 0;
    this->suppressTouchEnd = 0;
    this->sellConfirmPending = 0;
    this->field_0xd3 = 0;
    this->currentContentHeight = 0;
    this->visibleHeight = 0;
    this->progressBarWidth = 0;
    this->progressBarHeight = 0;
    this->savedScrollOffset = 0;
    this->blueprintIconImage = 0;
    this->pendingIconImage = 0;
    this->hintImage = 0;
    this->hintOffsetX = 0;
    this->autoEquipPending = 0;
    this->autoEquipIndex = 0;
    this->replaceEquipPending = 0;
    this->upgradeMode = 0;
    this->localBluePrint = 0;
    this->listModeFlag = 0;
    this->buttonWidth = 0;
    this->gridButtonHeight = 0;
    this->gridSpacingX = 0;
    this->gridSpacingY = 0;
    this->routeWarningPending = 0;

    HangarWindow::lastTab = 1;
    Layout *layout = static_cast<Layout *>(Globals::layout);
    if (layout != nullptr) {
        this->rowLayoutMetrics = layout->hangarRowMetrics;
        this->listEntryActionWidth = layout->hangarListEntryFixedWidth;
        this->rowActionOffsetY = layout->hangarSelectedRowActionOffsetY;
        this->iconOffsetY = layout->hangarRowIconOffsetY;
    }
#endif
}

int HangarWindow::isInitialized() {
    return this->active;
}

ListItem *HangarWindow::getCurrentItem() {
    return this->selectedItem;
}

// Static data members present in the original binary (defined for symbol parity).
int HangarWindow::lastTab;
