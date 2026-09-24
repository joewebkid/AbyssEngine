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

float VectorSignedToFloat(int v, int mode);

void TouchButton_ctor_text(void *btn, void *text, int a, int b, int c, char k);

void TouchButton_ctor_text2(void *btn, void *text, int a, int b, int c, int d, char k);

void TouchButton_ctor_img(void *btn, void *img, int a, int b, int c, int d, char k, char m);

void TouchButton_ctor_img2(void *btn, void *imgA, void *imgB, int a, int b, int c, char k);

void TouchButton_getPosition(void *btn, float *x, float *y);

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
static uint8_t g_hangarIntroShown = 0;
// Android keeps these one-shot Store explanations in standalone globals
// (HIBYTE(word_218268) and byte_21826A). Their owning options record has not
// been typed yet, but the flags must remain valid host storage rather than
// dereferencing the retired g_hw_itemFlags shim.
static uint8_t g_hangarStoreBuyHintShown = 0;
static uint8_t g_hangarStoreSellHintShown = 0;
static uint8_t g_hangarSlotHintShown = 0;
// Android dword_202844 at 0x202844. Slot zero is the record-store action;
// slots 1..4 are the four visible social-credit offers.
static constexpr int kHangarSocialCreditReward[5] = {0, 7500, 5000, 5000, 7500};

// The ARM renderer uses separate persisted bytes for these offers. Their
// RecordHandler slots are not typed yet, so retain the corresponding local
// state until that persistence object is recovered.
static uint8_t g_hangarCreditOfferShown = 0;
static uint8_t g_hangarSocialCreditClaimed[5] = {};

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
    this->active = 0;
    this->buttons = nullptr;
    delete[] (uint32_t *) this->tabIcons;
    this->tabIcons = nullptr;
}

float HangarWindow::getRelativeScrollStartPos() {
    int range = this->scrollOffset;
    if (range > 0) {
        return 0.0f;
    }
    return -(float) range / (float) this->currentContentHeight;
}

void HangarWindow::hideMessage() {
    this->dialogActive = 0;
}
















void HangarWindow::render() {
    Layout *layout = static_cast<Layout *>(Globals::layout);
    PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
    Status *status = Globals::status != nullptr ? Globals::status : Status::gStatus;
    GameText *gameText = static_cast<GameText *>(Globals::gameText);
    if (gameText == nullptr) {
        gameText = GameText::gGameText;
    }
    ImageFactory *imageFactory = static_cast<ImageFactory *>(Globals::imageFactory);
    Array<Item *> *itemTable = static_cast<Array<Item *> *>(Globals::items);
    const unsigned int font = Globals::font;

    if (layout == nullptr || canvas == nullptr || status == nullptr || gameText == nullptr) {
        return;
    }

    auto textFor = [gameText](int id) -> String {
        return *gameText->getText(id);
    };
    auto buttonAt = [this](unsigned int index) -> TouchButton * {
        if (this->buttons == nullptr || index >= this->buttons->size()) {
            return nullptr;
        }
        return this->buttons->data()[index];
    };
    auto itemTypeAt = [itemTable](int index) -> int {
        if (itemTable == nullptr || index < 0 || static_cast<unsigned int>(index) >= itemTable->size()) {
            return 0;
        }
        Item *item = itemTable->data()[index];
        return item == nullptr ? 0 : item->getType();
    };

    canvas->SetColor(0xffffffffu);
    ModStation *stationModule = nullptr;
    if (AbyssEngine::ApplicationManager::gAppManager != nullptr) {
        stationModule = static_cast<ModStation *>(
            AbyssEngine::ApplicationManager::gAppManager->GetApplicationModule(5));
    }

    if (stationModule == nullptr || stationModule->closeHangarAfterCredits == 0) {
        int tab2 = this->viewMode;
        if (tab2 == 0) {
            layout->drawBG();
            unsigned int tab = this->hangarList->getCurrentTab();
            Array<ListItem *> *items = ((HangarList *) this->hangarList)->getCurrentTabItems();
            if (items != 0) {
                float startPos = this->getRelativeScrollStartPos();
                float visH = (float) this->visibleHeight;
                float scrollH = this->getRelativeScrollHeight();
                int scrollPx = (int) (scrollH * visH);
                int startPx = (int) (startPos * visH);

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
                        canvas->DrawImage2D((unsigned) this->hintImage,
                                            (layout->field_0x28 - iw) + this->hintOffsetX, y,
                                            (unsigned char) 1);
                        int off = (scrollPx < 1) ? 0 : (layout->field_0x48 + layout->field_0x2c);
                        canvas->DrawImage2D((unsigned) this->hintImage,
                                            this->hintOffsetX + layout->field_0x28 + topY + off, y,
                                            (unsigned char) 0);
                        y += ih;
                    }
                }

                int contentBase = colW + baseY + rowGap;

                Array<TouchButton *> *btnArr = this->buttons;
                for (unsigned int i = 0; i != kHangarButtonCount; i++) {
                    if (this->dragging == 0) {
                        TouchButton *btn = buttonAt(i);
                        if (btn != nullptr)
                            btn->setVisible(false);
                    }
                }

                int boxW = rowGap - 2;

                for (unsigned int i = 0; i < items->size(); i++) {
                    int y = (layout->field_0x70 + this->rowLayoutMetrics.rowGap) * (int) i +
                            this->scrollOffset + layout->field_0x20 + layout->field_0xc;
                    if (y < 0 || y > Globals::h)
                        continue;

                    ListItem *li = items->data()[i];
                    if (li == nullptr || li->isSelectable() == 0)
                        continue;

                    if (this->selectedItem == li && li->isTextButton() == 0) {
                        String boxText;
                        if (tab == 0 && li->inTabIndex >= 0) {
                            layout->drawBox(10, this->hintOffsetX + layout->field_0x28, y, topY,
                                            layout->field_0x70, boxText);
                        } else {
                            layout->drawBox(4, this->hintOffsetX + layout->field_0x28, y, topY,
                                            layout->field_0x70, boxText);
                        }
                    } else if (tab != 0 || li->inTabIndex < 0) {
                        String boxText;
                        layout->drawBox(3, this->hintOffsetX + layout->field_0x28, y, topY,
                                        layout->field_0x70, boxText);
                    } else {
                        String boxText;
                        layout->drawBox(9, this->hintOffsetX + layout->field_0x28, y, topY,
                                        layout->field_0x70, boxText);
                    }

                    canvas->SetColor(0xffffffffu);
                    String label("");
                    const int titleX = this->hintOffsetX + layout->field_0x28 + contentBase;
                    const int priceY = y + layout->field_0x70 / 2 + 1;
                    const int iconX = this->hintOffsetX + layout->field_0x28 + rowGap;
                    const int iconY = this->iconOffsetY + y;

                    if (!li->isItem()) {
                        if (li->isShip() && li->ship != nullptr) {
                            const int shipIndex = li->ship->getIndex();
                            label = textFor(shipIndex + 913);
                            const int shipPrice = li->ship->getPrice();
                            if (tab == 1 && this->upgradeMode != 0) {
                                canvas->SetColor(0x777777ffu);
                            } else if (tab == 1 || i >= 2) {
                                Ship *currentShip = status->getShip();
                                const int tradeIn = currentShip == nullptr ? 0 : currentShip->getPrice();
                                canvas->SetColor(shipPrice - tradeIn <= status->getCredits()
                                                     ? 0x7aa35bffu
                                                     : 0xa35b5bffu);
                            } else {
                                canvas->SetColor(0x777777ffu);
                            }
                            String price = Globals::layout->formatCredits(shipPrice);
                            canvas->DrawString(font, price, titleX, priceY, false);
                            if (imageFactory != nullptr) {
                                imageFactory->drawShip(shipIndex, iconX, iconY);
                            }
                            canvas->SetColor(0xffffffffu);
                        } else if (li->isSlot()) {
                            label = textFor(174);
                            if (tab == 4 && i + 1 == items->size()) {
                                TouchButton *createButton = buttonAt(kHangarButtonBlueprintAutoComplete);
                                if (createButton != nullptr) {
                                    createButton->setPosition(this->hintOffsetX + layout->field_0x28 + topY / 2,
                                                              this->rowActionOffsetY + y, 0x14);
                                    createButton->setVisible(true);
                                    createButton->draw();
                                }
                            }
                        } else if (li->isBluePrint() && li->bluePrint != nullptr) {
                            const int blueprintIndex = li->bluePrint->getIndex();
                            label = textFor(blueprintIndex + 1274);
                            const float completion = li->bluePrint->getCompletionRate();
                            if (completion > 0.0f) {
                                const int progressX = titleX + 2;
                                canvas->DrawImage2D(this->progressBarBgImage, progressX, priceY);
                                const int fillWidth = static_cast<int>(completion * this->progressBarWidth);
                                canvas->DrawRegion2D(this->progressBarFillImage, 0, 0, fillWidth,
                                                     this->progressBarHeight, 0.0f, 0, 0, 0, progressX + 3);
                                canvas->DrawImage2D(this->progressBarBorderImage,
                                                    progressX + 5 + static_cast<int>(completion * (this->progressBarWidth - 4)),
                                                    y + layout->field_0x70 / 2 + 2, 0x11);
                                canvas->SetColor(0x777777ffu);
                                String progress(static_cast<int>(completion * 100.0f));
                                String percent("%  (");
                                progress += percent;
                                canvas->DrawString(font, progress,
                                                   progressX + this->progressBarWidth + layout->field_0x2c,
                                                   y + layout->field_0x70 / 2 + this->rowActionOffsetY, false);
                                canvas->SetColor(0xffffffffu);
                            }
                            if (imageFactory != nullptr) {
                                imageFactory->drawItem(blueprintIndex, itemTypeAt(blueprintIndex), iconX, iconY);
                            }
                            if (li->craftable != 0) {
                                canvas->SetColor(0x00ed00ffu);
                            }
                        } else if (li->isPendingProduct() && li->pendingProduct != nullptr) {
                            const int productIndex = li->pendingProduct->blueprintIndex;
                            label = textFor(productIndex + 1274);
                            if (li->pendingProduct->quantity >= 2) {
                                String count(li->pendingProduct->quantity);
                                String prefix("x ");
                                label = count + prefix + label;
                            }
                            if (imageFactory != nullptr) {
                                imageFactory->drawItem(productIndex, itemTypeAt(productIndex), iconX, iconY);
                            }
                        } else if (li->isMoveToCargoButton()) {
                            TouchButton *moveButton = buttonAt(kHangarButtonMoveToCargoEntry);
                            if (moveButton != nullptr) {
                                moveButton->setPosition(this->hintOffsetX + layout->field_0x28, y, 0x11);
                                moveButton->setVisible(true);
                                moveButton->draw();
                            }
                        } else if (li->isSellButton()) {
                            TouchButton *sellButton = buttonAt(kHangarButtonSellListEntry);
                            if (sellButton != nullptr) {
                                sellButton->setPosition(this->hintOffsetX + layout->field_0x28, y, 0x11);
                                sellButton->setVisible(true);
                                sellButton->draw();
                            }
                        } else if (li->name != nullptr) {
                            layout->drawBox(0, this->hintOffsetX + layout->field_0x28,
                                            y + layout->field_0x70 - layout->field_0x1c,
                                            topY, layout->field_0x1c, *li->name);
                        }
                    } else if (li->item != nullptr) {
                        const int itemIndex = li->item->getIndex();
                        label = textFor(itemIndex + 1274);
                        canvas->SetColor(tab == 1
                                             ? (li->item->getSinglePrice() <= status->getCredits()
                                                    ? 0x7aa35bffu
                                                    : 0xa35b5bffu)
                                             : 0x777777ffu);
                        if (this->upgradeMode == 0) {
                            String price = Globals::layout->formatCredits(li->item->getSinglePrice());
                            canvas->DrawString(font, price, titleX, priceY, false);
                        }
                        if (imageFactory != nullptr) {
                            imageFactory->drawItem(itemIndex, li->item->getType(), iconX, iconY);
                        }
                    }

                    canvas->DrawString(font, label, titleX, y + 2, false);

                    if (li != this->selectedItem) {
                        continue;
                    }

                    const int rightEdge = this->hintOffsetX + layout->field_0x28 + topY;
                    auto drawRightAction = [&](unsigned int buttonIndex, int xOffset) -> int {
                        TouchButton *button = buttonAt(buttonIndex);
                        if (button == nullptr) {
                            return 0;
                        }
                        button->setPosition(rightEdge - xOffset, this->rowActionOffsetY + y, 0x12);
                        button->setVisible(true);
                        button->draw();
                        return button->getWidth() + layout->field_0x2c;
                    };

                    int occupiedRightWidth = 0;
                    if (li->isItem() && li->item != nullptr && (tab == 1 || tab == 4)) {
                        const int stationAmount = tab == 1
                                                      ? li->item->getStationAmount()
                                                      : li->item->getAmount();
                        const int currentAmount = tab == 1
                                                      ? li->item->getAmount()
                                                      : (this->bluePrint == nullptr
                                                             ? 0
                                                             : this->bluePrint->getCurrentAmount(li->item->getIndex()));
                        TouchButton *cargoButton = buttonAt(kHangarButtonCurrentAmount);
                        if (currentAmount >= 1 && cargoButton != nullptr) {
                            cargoButton->setPosition(this->hintOffsetX + layout->field_0x28,
                                                     this->rowActionOffsetY + y, 0x11);
                            cargoButton->setVisible(true);
                            cargoButton->draw();
                        }
                        TouchButton *stationButton = buttonAt(kHangarButtonStationAmount);
                        if (stationAmount >= 1 && stationButton != nullptr) {
                            stationButton->setPosition(rightEdge, this->rowActionOffsetY + y, 0x12);
                            stationButton->setVisible(true);
                            stationButton->draw();
                            occupiedRightWidth = stationButton->getWidth() + layout->field_0x2c;
                        }
                    }

                    if (li->isShip() && li->ship != nullptr) {
                        const bool isCurrentShip = li->ship == status->getShip();
                        if (tab != 0 && !(tab == 3 && isCurrentShip)) {
                            occupiedRightWidth = drawRightAction(kHangarButtonSelectShip, 0);
                        }
                    } else if (tab == 2 && li->isBluePrint()) {
                        occupiedRightWidth = drawRightAction(kHangarButtonSelectBlueprint, 0);
                    } else if (tab == 0 && li->isItem() && li->item != nullptr) {
                        Ship *ship = status->getShip();
                        Item *installed = ship == nullptr ? nullptr :
                            ship->getFirstEquipmentOfSort(li->item->getSort());
                        const bool blockedBySlots = installed != nullptr &&
                            !li->item->canBeInstalledMultipleTimes() &&
                            ship->getFreeSlots(li->item->getType()) == 0;
                        if (!blockedBySlots) {
                            const unsigned int actionIndex = li->inTabIndex < 0
                                                                 ? kHangarButtonSelectItemFirst
                                                                 : kHangarButtonSelectItemSecond;
                            occupiedRightWidth = drawRightAction(actionIndex, layout->field_0x2c * 2);
                        }
                    }

                    if (li->isItem() || li->isShip() || li->isBluePrint() || li->isPendingProduct()) {
                        drawRightAction(kHangarButtonInspect, occupiedRightWidth);
                    }
                }

                if (scrollPx > 0 || startPx > 0) {
                    layout->drawScrollBar(Globals::w - layout->field_0x48 - layout->field_0x28 - this->hintOffsetX,
                                          layout->field_0x20 + layout->field_0xc, this->visibleHeight,
                                          startPx, scrollPx);
                }
            }

            layout->drawHeader(textFor(167));

            Array<TouchButton *> *tabs = this->tabButtons;
            if (tabs != nullptr) {
                for (unsigned int i = 0; i < tabs->size(); i++) {
                    TouchButton *tabButton = tabs->data()[i];
                    if (tabButton != nullptr) {
                        tabButton->draw();
                    }
                }
            }
        }

        if (this->viewMode == 1) {
            this->viewMode = 0;
            this->render();
            this->viewMode = 1;
            if (this->listItemWindow != nullptr) {
                this->listItemWindow->draw();
            }
        }
    }

    layout->drawFooter();
    TouchButton *creditsButton = buttonAt(kHangarButtonCredits);
    if (creditsButton != nullptr) {
        creditsButton->setVisible(true);
        creditsButton->setAlwaysPressed(g_hangarCreditOfferShown == 0);
        creditsButton->setText(Globals::layout->formatCredits(status->getCredits()));
        creditsButton->draw();
    }

    if (this->dialogActive == 0)
        return;

    if (this->dialog == nullptr) {
        return;
    }
    this->dialog->draw();

    if (this->buyCreditsActive == 0) {
        if (this->freeCreditsActive != 0) {
            for (unsigned int i = kHangarButtonFreeCreditsFirst;
                 i <= kHangarButtonFreeCreditsLast; i++) {
                const unsigned int offerIndex = i - kHangarButtonFreeCreditsFirst;
                TouchButton *button = buttonAt(i);
                if (button == nullptr) {
                    continue;
                }
                const bool visible = offerIndex != 0 && g_hangarSocialCreditClaimed[offerIndex] == 0;
                button->setVisible(visible);
                if (!visible) {
                    continue;
                }
                const int y = this->dialog->y + layout->field_0x8 + layout->field_0x2c * 2 +
                              static_cast<int>(offerIndex - 1) * (layout->field_0x30 + layout->field_0x34);
                button->setPosition(this->dialog->x + layout->field_0x28, y);
                button->draw();

                const int textId = offerIndex == 0 ? 3400 : 112 + static_cast<int>(offerIndex);
                String activity = textFor(textId);
                canvas->SetColor(0xffffffffu);
                canvas->DrawString(font, activity,
                                   this->dialog->x + layout->field_0x28 + button->getWidth() + layout->field_0x2c,
                                   y + button->getHeight() / 5, false);
            }
        }
    } else {
        const char *const descriptions[5] = {
            Globals::cItemListDescription_00, Globals::cItemListDescription_01,
            Globals::cItemListDescription_02, Globals::cItemListDescription_03,
            Globals::cItemListDescription_04
        };
        const char *const prices[5] = {
            Globals::cItemListPrice_00, Globals::cItemListPrice_01,
            Globals::cItemListPrice_02, Globals::cItemListPrice_03,
            Globals::cItemListPrice_04
        };
        const unsigned int *productIcons = static_cast<const unsigned int *>(this->tabIcons);

        if (this->listModeFlag != 0) {
            for (unsigned int i = kHangarButtonPaidCreditsFirst;
                 i <= kHangarButtonPaidCreditsLast; ++i) {
                const unsigned int productIndex = i - kHangarButtonPaidCreditsFirst;
                TouchButton *button = buttonAt(i);
                if (button == nullptr) {
                    continue;
                }
                const int row = static_cast<int>(productIndex / 3);
                const int column = static_cast<int>(productIndex % 3);
                const int x = Globals::w / 2 - this->buttonWidth - this->gridSpacingX +
                              column * (this->buttonWidth + this->gridSpacingX);
                const int y = static_cast<int>(-3 * layout->field_0x20 + Globals::h / 2 -
                                               this->gridButtonHeight / 2 - this->gridSpacingY * 0.5f) +
                              row * (this->gridButtonHeight + this->gridSpacingY);
                String label(descriptions[productIndex] == nullptr ? "" : descriptions[productIndex]);
                String split(prices[productIndex] == nullptr ? "" : prices[productIndex]);
                button->setVisible(true);
                button->setPosition(x, y, 0x44);
                button->replaceTextKeepSize(label);
                button->setSplitText(split);
                button->draw();
                if (productIcons != nullptr) {
                    canvas->DrawImage2D(productIcons[productIndex], x, y - layout->field_0x2c, 0x11, 0x44);
                }
            }
        } else {
            const int startY = this->dialog->y + layout->field_0x8 + 5 * layout->field_0x2c;
            const int stepY = layout->field_0x30 + layout->field_0x34;
            for (unsigned int i = kHangarButtonPaidCreditsFirst;
                 i <= kHangarButtonPaidCreditsLast; ++i) {
                const unsigned int productIndex = i - kHangarButtonPaidCreditsFirst;
                TouchButton *button = buttonAt(i);
                if (button == nullptr) {
                    continue;
                }
                String label(descriptions[productIndex] == nullptr ? "" : descriptions[productIndex]);
                String split(prices[productIndex] == nullptr ? "" : prices[productIndex]);
                button->setVisible(true);
                button->setText(label);
                button->setSplitText(split);
                button->setPosition(Globals::w / 2, startY + static_cast<int>(productIndex) * stepY, 0x14);
                button->draw();
            }
            this->dialog->setHeight(stepY * 6 + canvas->GetTextHeight(font) * 2);
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

            void *appData = AppManager_GetApplicationData();
            RecordHandler *rh = *g_hw_recordHandler;
            for (unsigned int i = 0; i != 5; i++) {
                if ((*self->buttons)[(i * 4 + 0x48) >> 2]->OnTouchEnd(touch, coord) != 0) {
                    switch (i) {
                        case 0:
                            ((RecordHandler *) (rh))->recordStoreWrite(0);
                            ((RecordHandler *) (rh))->recordStoreWritePreview(0);
                            break;
                        case 1:
                            (*(uint8_t *) ((char *) (appData) + (0xa0))) = 1;
                            NFC().free_credits_likeGOF2OnFacebook();
                            Status_changeCredits(globals);
                            g_hw_optionFlags[0x49] = 1;
                            break;
                        case 2:
                            (*(uint8_t *) ((char *) (appData) + (0xa1))) = 1;
                            NFC().free_credits_likeFishlabsOnFacebook();
                            Status_changeCredits(globals);
                            g_hw_optionFlags[0x4a] = 1;
                            break;
                        case 3:
                            (*(uint8_t *) ((char *) (appData) + (0xa2))) = 1;
                            NFC().free_credits_subscribeToYoutubeChannel();
                            Status_changeCredits(globals);
                            g_hw_optionFlags[0x4b] = 1;
                            break;
                        case 4:
                            (*(uint8_t *) ((char *) (appData) + (0xa3))) = 1;
                            NFC().free_credits_followOnTwitter();
                            Status_changeCredits(globals);
                            g_hw_optionFlags[0x4c] = 1;
                            break;
                        case 5:
                            (*(uint8_t *) ((char *) (appData) + (0xd))) = 1;
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
                void *appData = AppManager_GetApplicationData();
                if (show || (*(uint8_t *) ((char *) (appData) + (0x15))) == 0) {
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
        void *appData = AppManager_GetApplicationData();
        (*(uint8_t *) ((char *) (appData) + (0x40))) = 0;
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

int HangarWindow::OnTouchEnd(int touch, int coord) {
    Layout *layout = static_cast<Layout *>(Globals::layout);
    Status *status = Globals::status != nullptr ? Globals::status : Status::gStatus;
    GameText *gameText = static_cast<GameText *>(Globals::gameText);
    if (gameText == nullptr) {
        gameText = GameText::gGameText;
    }
    if (layout == nullptr || status == nullptr || gameText == nullptr || this->hangarList == nullptr) {
        return 0;
    }

    auto buttonAt = [this](unsigned int index) -> TouchButton * {
        if (this->buttons == nullptr || index >= this->buttons->size()) {
            return nullptr;
        }
        return this->buttons->data()[index];
    };
    auto setCreditButtonsVisible = [&buttonAt](unsigned int first, unsigned int last, bool visible) {
        for (unsigned int i = first; i < last; ++i) {
            TouchButton *button = buttonAt(i);
            if (button != nullptr) {
                button->setVisible(visible);
            }
        }
    };
    auto formatCredits = [status](int credits) {
        return Globals::layout->formatCredits(credits);
    };
    auto showNotEnoughCredits = [&]() {
        String message = *gameText->getText(203);
        message = status->replaceHash(message, formatCredits(status->getCredits()), String("#C"));
        message += String("\n\n");
        message += *gameText->getText(124);
        this->dialog->set(message, true);
        this->dialogActive = 1;
        this->notEnoughCredits = 1;
    };
    auto resetSelection = [&]() {
        this->selectedItem = nullptr;
        this->hangarList->setCurrentItemIndex(-1);
    };
    auto resetTabs = [&]() {
        if (this->tabButtons == nullptr) {
            return;
        }
        for (unsigned int i = 0; i < this->tabButtons->size(); ++i) {
            TouchButton *tab = this->tabButtons->data()[i];
            if (tab != nullptr) {
                tab->resetTouch();
            }
        }
    };
    auto stationModule = []() -> ModStation * {
        if (ApplicationManager::gAppManager == nullptr) {
            return nullptr;
        }
        return static_cast<ModStation *>(ApplicationManager::gAppManager->GetApplicationModule(5));
    };

    this->holdTime = 0;
    this->repeatTimer = 0;
    this->dragging = 0;
    if (this->suppressTouchEnd != 0) {
        this->suppressTouchEnd = 0;
        return 0;
    }

    if (this->dialogActive == 0) {
        if (this->viewMode == 1 && this->listItemWindow != nullptr) {
            this->listItemWindow->OnTouchEnd(touch, coord);
        }

        if (layout->OnTouchEnd(touch, coord) == 0) {
            const int delta = this->scrollDelta;
            const int newScroll = this->scrollOffset + delta;
            this->damping = 0.9f;
            this->velocity = delta < -3 || delta > 3 ? static_cast<float>(delta) : 0.0f;
            this->scrollOffset = newScroll;
            this->scrollOffsetBackup = newScroll;

            if (this->tabButtons != nullptr) {
                for (unsigned int i = 0; i < this->tabButtons->size(); ++i) {
                    TouchButton *tab = this->tabButtons->data()[i];
                    if (tab != nullptr && tab->OnTouchEnd(touch, coord) != 0) {
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
            }

            int row = -1;
            if (layout->field_0xc < coord) {
                row = IDIV(coord - layout->field_0xc - layout->field_0x20 - this->rowLayoutMetrics.rowGap -
                               this->scrollOffset,
                           layout->field_0x70 + this->rowLayoutMetrics.rowGap);
                if (row >= 0 && row < this->hangarList->getCurrentLength()) {
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
            if (autoComplete != nullptr && this->bluePrint != nullptr && this->dialog != nullptr &&
                autoComplete->OnTouchEnd(touch, coord) != 0) {
                String message = *gameText->getText(195);
                message = status->replaceHash(message, formatCredits(this->bluePrint->getAutoCompletionPrice()),
                                              String("#C"));
                this->dialog->set(message, true);
                this->autoCompletePending = 1;
                this->dialogActive = 1;
            }

            if (this->currentItemIsHighlighted() != 0 && this->buttons != nullptr) {
                for (unsigned int i = 0; i < this->buttons->size(); ++i) {
                    TouchButton *button = this->buttons->data()[i];
                    if (button == nullptr || button->OnTouchEnd(touch, coord) == 0) {
                        continue;
                    }

                    if (row >= 0 && row < this->hangarList->getCurrentLength()) {
                        this->hangarList->setCurrentItemIndex(row);
                    }
                    switch (i) {
                    case kHangarButtonInspect:
                        if (this->listItemWindow != nullptr) {
                            this->listItemWindow->set(this->hangarList->getCurrentItem(), 0, 0, 0, 0, 1);
                            this->viewMode = 1;
                            if (Globals::sound != nullptr) {
                                Globals::sound->play(0x61, nullptr, nullptr, 0.0f);
                            }
                        }
                        return 0;
                    case kHangarButtonSelectShip:
                    case kHangarButtonSelectItemFirst:
                    case kHangarButtonSelectItemSecond:
                    case kHangarButtonSelectItemThird:
                    case kHangarButtonSellListEntry:
                    case kHangarButtonMoveToCargoEntry:
                    case kHangarButtonSelectBlueprint:
                        this->selectItem(this->selectedItem);
                        // Android keeps scanning the action array after these
                        // state changes. TouchButton resets its own latch, so a
                        // second action cannot be consumed by this same touch.
                        continue;
                    case kHangarButtonCurrentAmount:
                        this->transaction(false);
                        if (Globals::sound != nullptr) {
                            Globals::sound->play(0x64, nullptr, nullptr, 0.0f);
                        }
                        continue;
                    case kHangarButtonStationAmount: {
                        this->transaction(true);
                        if (Globals::sound != nullptr) {
                            Globals::sound->play(0x65, nullptr, nullptr, 0.0f);
                        }
                        ListItem *current = this->hangarList->getCurrentItem();
                        if (current != nullptr && current->isItem() && current->item != nullptr &&
                            current->item->getType() == 1) {
                            this->autoEquipPending = 1;
                            this->autoEquipIndex = this->hangarList->getCurrentItemIndex();
                        }
                        continue;
                    }
                    case kHangarButtonSellShip:
                        if (this->dialog != nullptr) {
                            this->dialog->set(*gameText->getText(334), true);
                            this->sellShipPending = 1;
                            this->dialogActive = 1;
                        }
                        continue;
                    case kHangarButtonCredits: {
                        g_hangarCreditOfferShown = 1;
                        RecordHandler *recordHandler = static_cast<RecordHandler *>(Globals::recordHandler);
                        if (recordHandler != nullptr) {
                            recordHandler->saveOptions();
                        }
                        this->showCreditsBuyWindow();
                        continue;
                    }
                    default:
                        break;
                    }
                }
            }

            if (layout->helpPressed() != 0) {
                const unsigned int tab = this->hangarList->getCurrentTab();
                if (this->viewMode == 1) {
                    String helpText = *gameText->getText(643);
                    layout->initHelpWindow(helpText);
                } else {
                    // The Android body has a separate GameText/String lifetime
                    // for every tab instead of an inferred indexed table.
                    switch (tab) {
                    case 0: {
                        String helpText = *gameText->getText(623);
                        layout->initHelpWindow(helpText);
                        break;
                    }
                    case 1: {
                        String helpText = *gameText->getText(622);
                        layout->initHelpWindow(helpText);
                        break;
                    }
                    case 2: {
                        String helpText = *gameText->getText(625);
                        layout->initHelpWindow(helpText);
                        break;
                    }
                    case 3: {
                        String helpText = *gameText->getText(624);
                        layout->initHelpWindow(helpText);
                        break;
                    }
                    case 4: {
                        String helpText = *gameText->getText(626);
                        layout->initHelpWindow(helpText);
                        break;
                    }
                    default:
                        break;
                    }
                }
            }

            TouchButton *credits = buttonAt(kHangarButtonCredits);
            if (credits != nullptr && credits->OnTouchEnd(touch, coord) != 0) {
                g_hangarCreditOfferShown = 1;
                RecordHandler *recordHandler = static_cast<RecordHandler *>(Globals::recordHandler);
                if (recordHandler != nullptr) {
                    recordHandler->saveOptions();
                }
                this->showCreditsBuyWindow();
            }
            return 0;
        }

        if (this->viewMode == 1) {
            layout->resetWindowDimensions();
            this->viewMode = 0;
            return 0;
        }
        const unsigned int tab = this->hangarList->getCurrentTab();
        if (tab == 4) {
            this->setSellMode(false);
            resetSelection();
            this->hangarList->setCurrentTab(2, true);
            this->refreshCargoAvailabilityForBlueprints();
            this->refreshCurrentContentHeight();
            this->scrollOffset = 0;
            this->scrollOffsetBackup = 0;
            return 0;
        }
        if (tab == 3) {
            this->hangarList->setCurrentTab(0, true);
            this->refreshCargoAvailabilityForBlueprints();
            this->refreshCurrentContentHeight();
        } else if (this->readyToClose()) {
            this->setSellMode(false);
            resetSelection();
            return 1;
        }
        return 0;
    }

    if (this->dialog == nullptr) {
        this->dialogActive = 0;
        return 0;
    }

    if (this->autoCompletePending != 0) {
        const int result = this->dialog->OnTouchEnd(touch, coord);
        if (result == 1) {
            this->dialogActive = 0;
        } else if (result == 0) {
            const int price = this->bluePrint != nullptr ? this->bluePrint->getAutoCompletionPrice() : 0;
            if (this->bluePrint != nullptr && price <= status->getCredits()) {
                this->dialogActive = 0;
                if (this->bluePrint->isEmpty() && status->getStation() != nullptr) {
                    this->bluePrint->stationIndex = status->getStation()->getIndex();
                    this->bluePrint->stationName = status->getStation()->getName();
                }
                this->bluePrint->complete();
                this->highlightItem(this->hangarList->getCurrentItemAt(1));
                this->buyMode = 1;
                this->setSellMode(false);
                status->changeCredits(-price);
            } else {
                showNotEnoughCredits();
            }
        }
        this->autoCompletePending = 0;
        return 0;
    }

    if (this->replaceEquipPending != 0) {
        const int result = this->dialog->OnTouchEnd(touch, coord);
        if (result == 1) {
            this->dialogActive = 0;
            this->replaceEquipPending = 0;
            this->scrollOffset = this->savedScrollOffset;
        } else if (result == 0 && this->pendingMountItem != nullptr && this->pendingDemountItem != nullptr) {
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
            if (recordHandler != nullptr) {
                recordHandler->saveOptions();
            }
            this->showCreditsBuyWindow();
        }
        goto common_choice_tail;
    }

    if (this->buyCreditsActive != 0) {
        const int result = this->dialog->OnTouchEnd(touch, coord);
        if (result == 0) {
            this->buyCreditsActive = 0;
            this->dialogActive = 0;
            setCreditButtonsVisible(kHangarButtonPaidCreditsFirst, kHangarButtonFreeCreditsFirst, false);
            uint8_t *appData = ApplicationManager::gAppManager != nullptr
                                   ? static_cast<uint8_t *>(ApplicationManager::gAppManager->GetApplicationData())
                                   : nullptr;
            if (appData != nullptr) {
                appData[64] = 0;
            }
            ModStation *module = stationModule();
            if (module != nullptr && module->closeHangarAfterCredits != 0) {
                module->closeHangarAfterCredits = 0;
                return 1;
            }
        }

        const auto buyCredits = [](unsigned int product) {
            switch (product) {
            case 0: NFC().iap_buy_credits_100_000(); break;
            case 1: NFC().iap_buy_credits_300_000(); break;
            case 2: NFC().iap_buy_credits_1_000_000(); break;
            case 3: NFC().iap_buy_credits_3_000_000(); break;
            case 4: NFC().iap_buy_credits_10_000_000(); break;
            default: break;
            }
        };
        for (unsigned int i = kHangarButtonPaidCreditsFirst;
             i <= kHangarButtonPaidCreditsLast; ++i) {
            TouchButton *button = buttonAt(i);
            if (button != nullptr && button->OnTouchEnd(touch, coord) != 0) {
                buyCredits(i - kHangarButtonPaidCreditsFirst);
                break;
            }
        }
        TouchButton *more = buttonAt(kHangarButtonCreditsMore);
        if (more != nullptr && more->OnTouchEnd(touch, coord) != 0) {
            const bool allSocialOffersClaimed =
                g_hangarSocialCreditClaimed[1] != 0 &&
                g_hangarSocialCreditClaimed[2] != 0 &&
                g_hangarSocialCreditClaimed[3] != 0 &&
                g_hangarSocialCreditClaimed[4] != 0;
            uint8_t *appData = ApplicationManager::gAppManager != nullptr
                                   ? static_cast<uint8_t *>(ApplicationManager::gAppManager->GetApplicationData())
                                   : nullptr;
            if (!allSocialOffersClaimed || (appData != nullptr && appData[21] == 0)) {
                setCreditButtonsVisible(kHangarButtonPaidCreditsFirst, kHangarButtonCreditsMore, false);
                this->showFreeCreditsWindow();
            }
        }
        goto common_choice_tail;
    }

    if (this->freeCreditsActive != 0) {
        const int result = this->dialog->OnTouchEnd(touch, coord);
        if (result == 0) {
            setCreditButtonsVisible(kHangarButtonFreeCreditsFirst, kHangarButtonBlueprintAutoComplete, false);
            this->freeCreditsActive = 0;
            this->showCreditsBuyWindow();
        }

        uint8_t *appData = ApplicationManager::gAppManager != nullptr
                               ? static_cast<uint8_t *>(ApplicationManager::gAppManager->GetApplicationData())
                               : nullptr;
        for (unsigned int i = kHangarButtonFreeCreditsFirst;
             i <= kHangarButtonFreeCreditsLast; ++i) {
            const unsigned int offerIndex = i - kHangarButtonFreeCreditsFirst;
            TouchButton *button = buttonAt(i);
            if (button == nullptr || button->OnTouchEnd(touch, coord) == 0) {
                continue;
            }
            RecordHandler *recordHandler = static_cast<RecordHandler *>(Globals::recordHandler);
            switch (offerIndex) {
            case 0:
                if (recordHandler != nullptr) {
                    recordHandler->recordStoreWrite(0);
                    recordHandler->recordStoreWritePreview(0);
                }
                break;
            case 1:
                if (appData != nullptr) appData[160] = 1;
                NFC().free_credits_likeGOF2OnFacebook();
                status->changeCredits(kHangarSocialCreditReward[offerIndex]);
                g_hangarSocialCreditClaimed[offerIndex] = 1;
                break;
            case 2:
                if (appData != nullptr) appData[161] = 1;
                NFC().free_credits_likeFishlabsOnFacebook();
                status->changeCredits(kHangarSocialCreditReward[offerIndex]);
                g_hangarSocialCreditClaimed[offerIndex] = 1;
                break;
            case 3:
                if (appData != nullptr) appData[162] = 1;
                NFC().free_credits_subscribeToYoutubeChannel();
                status->changeCredits(kHangarSocialCreditReward[offerIndex]);
                g_hangarSocialCreditClaimed[offerIndex] = 1;
                break;
            case 4:
                if (appData != nullptr) appData[163] = 1;
                NFC().free_credits_followOnTwitter();
                status->changeCredits(kHangarSocialCreditReward[offerIndex]);
                g_hangarSocialCreditClaimed[offerIndex] = 1;
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
        Item *item = this->bluePrintItem != nullptr ? this->bluePrintItem->item : nullptr;
        const int cost = item != nullptr ? item->getBlueprintAmount() * 200 : 0;
        const bool canBuy = cost <= status->getCredits() && this->localBluePrint == 0;
        if (result == 0 && canBuy) {
            status->changeCredits(-cost);
            this->setSellMode(false);
            resetSelection();
            this->localBluePrint = 0;
            this->bluePrintPurchasePending = 0;
        } else if (result == 1 || cost > status->getCredits() ||
                   (result == 0 && this->localBluePrint != 0)) {
            if (item != nullptr) {
                item->setStationAmount(this->savedStationAmount);
                item->setAmount(this->savedAmount);
                item->setBlueprintAmount(this->savedBlueprintAmount);
            }
            status->setCredits(this->savedCredits);
            this->savedStationAmount = 0;
            this->bluePrintBuyCount = 0;
            this->savedAmount = 0;
            this->savedBlueprintAmount = 0;
            this->currentLoad = this->savedLoad;
            resetSelection();
            this->dialogActive = 0;
            this->bluePrintPurchasePending = 0;
            this->buyMode = 0;
            if (cost > status->getCredits() && this->localBluePrint == 0) {
                String message = *gameText->getText(203);
                message = status->replaceHash(message, formatCredits(status->getCredits()), String("#C"));
                this->dialog->set(message, true);
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
        } else if (result == 0 && this->selectedItem != nullptr && this->selectedItem->ship != nullptr &&
                   status->getStation() != nullptr) {
            status->changeCredits(this->selectedItem->getPrice());
            status->getStation()->removeShip(this->selectedItem->ship);
            this->sellShipPending = 0;
            this->dialogActive = 0;
            this->hangarList->initShopTab(this->itemList, status->getStation()->getShips());
            this->refreshCurrentContentHeight();
        }
        goto common_choice_tail;
    }

    if (this->shipSwapPending != 0) {
        const int result = this->dialog->OnTouchEnd(touch, coord);
        Ship *oldShip = status->getShip();
        Station *station = status->getStation();
        Ship *shopShip = this->selectedItem == nullptr ? nullptr : this->selectedItem->ship;
        if (oldShip == nullptr || station == nullptr || shopShip == nullptr) {
            return 0;
        }

        bool purchaseWithoutTradeIn = false;
        // Android compares Status+0x114 with 3 before entering its two-step
        // ship purchase confirmation. The field has no safe semantic name yet.
        if (status->field_114 == 3 && this->upgradeMode == 0) {
            if (this->swapConfirmFlag != 0) {
                if (result == 1) {
                    if (station->hasShip(oldShip->getIndex()) != 0) {
                        this->dialog->set(*gameText->getText(328));
                        this->dialogActive = 1;
                        // Android LABEL_254 clears the two ship-swap bytes
                        // before exposing the already-owned-ship notice.
                        this->shipSwapPending = 0;
                        this->swapConfirmFlag = 0;
                        return 0;
                    }
                    if (shopShip->getPrice() > status->getCredits()) {
                        String message = *gameText->getText(203);
                        message = status->replaceHash(message, Globals::layout->formatCredits(status->getCredits()),
                                                      String("#C"));
                        message += String("\n\n");
                        message += *gameText->getText(124);
                        this->dialog->set(message, true);
                        this->dialogActive = 1;
                        this->notEnoughCredits = 1;
                        this->shipSwapPending = 0;
                        this->swapConfirmFlag = 0;
                        return 0;
                    }
                    purchaseWithoutTradeIn = true;
                } else if (result != 0) {
                    return 0;
                }
            } else {
                if (result == 0) {
                    String empty("");
                    this->dialog->set(empty, *gameText->getText(327), true,
                                      *gameText->getText(330), *gameText->getText(331),
                                      *gameText->getText(330), -1, -1);
                    this->swapConfirmFlag = 1;
                    return 0;
                }
                if (result == 1) {
                    this->shipSwapPending = 0;
                    this->dialogActive = 0;
                    return 0;
                }
                return 0;
            }
        } else if (this->upgradeMode == 0 && this->dlcMenuPending != 0) {
            if (result == 0) {
                ModStation *module = stationModule();
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
                return 0;
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
                return 0;
            }
        }

        // The incoming ship receives the old loadout. The trade-in left at the
        // station is a fresh hull plus modifications only; Android does not
        // duplicate cargo or mounted equipment into the station inventory.
        Ship *newShip = shopShip->makeShip(-1);
        Ship *returnedShip = oldShip->makeShip(-1);
        if (newShip == nullptr || returnedShip == nullptr) {
            delete newShip;
            delete returnedShip;
            return 0;
        }
        newShip->setRace(shopShip->getRace());
        newShip->adjustPrice();
        Array<Item *> *cargo = oldShip->getCargo();
        if (cargo != nullptr) {
            for (unsigned int i = 0; i < cargo->size(); ++i) {
                if (cargo->data()[i] != nullptr) {
                    newShip->addCargo(cargo->data()[i]->clone());
                }
            }
        }
        Array<Item *> *equipment = oldShip->getEquipment();
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
        Array<int> *mods = oldShip->getMods();
        if (mods != nullptr) {
            for (unsigned int i = 0; i < mods->size(); ++i) {
                const int mod = mods->data()[i];
                newShip->addMod(mod);
                returnedShip->addMod(mod);
            }
        }

        if (purchaseWithoutTradeIn) {
            status->changeCredits(-shopShip->getPrice());
        } else if (this->upgradeMode == 0) {
            status->changeCredits(oldShip->getPrice() - shopShip->getPrice());
        }
        station->removeShip(shopShip);
        station->addShip(returnedShip);
        status->setShip(newShip);
        newShip->refreshValue();
        this->hangarList->initShipTab(newShip);
        if (this->itemList != nullptr) {
            ArrayReleaseClasses(*this->itemList);
            ArrayRemoveAll(*this->itemList);
            delete this->itemList;
        }
        this->itemList = Item::mixItems(newShip->getCargo(), station->getItems());
        this->hangarList->initShopTab(this->itemList, station->getShips());
        this->hangarList->setCurrentTab(0, true);
        this->refreshCurrentContentHeight();
        this->shipSwapPending = 0;
        this->swapConfirmFlag = 0;
        this->dialogActive = 0;

        if (this->upgradeMode == 0) {
            String messageTemplate = *gameText->getText(303);
            String shipName = *gameText->getText(newShip->getIndex() + 913);
            String message = status->replaceHash(messageTemplate, shipName, String("#N"));
            this->dialog->set(message);
            this->dialogActive = 1;
        }
        this->field_0x0 = 1;
        return 0;
    }

    if (this->dlcMenuPending != 0) {
        const int result = this->dialog->OnTouchEnd(touch, coord);
        if (result == 0) {
            ModStation *module = stationModule();
            if (module != nullptr) {
                module->showDlcMenu();
            }
        }
        if (result == 0 || result == 1) {
            this->dlcMenuPending = 0;
            this->dialogActive = 0;
        }
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
                resetTabs();
                return 0;
            }
        } else {
            return 0;
        }
        resetSelection();
        return 0;
    }

    if (result == 0) {
        this->dialogActive = 0;
        ModStation *module = stationModule();
        if (module != nullptr && module->closeHangarAfterCredits != 0) {
            module->closeHangarAfterCredits = 0;
            return 1;
        }
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
    if (slot < 0)
        ship->freeSlot(item);
    else
        ship->freeSlot(item, slot);

    bool merged = false;
    Array<Item *> *cargo = this->itemList;
    for (unsigned int i = 0; i < cargo->size(); i++) {
        Item *cur = cargo->data()[i];
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
        ArrayReleaseClasses(*this->itemList); ArrayRemoveAll(*(this->itemList));
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
    Layout *layout = static_cast<Layout *>(Globals::layout);
    Status *status = static_cast<Status *>(Globals::status);
    GameText *gameText = static_cast<GameText *>(Globals::gameText);

    this->holdTime = 0;
    this->repeatTimer = 0;
    int handled = layout->OnTouchBegin(touch, coord);

    if (this->dialogActive != 0) {
        if (this->buyCreditsActive != 0) {
            for (unsigned int i = kHangarButtonPaidCreditsFirst;
                 i != kHangarButtonCreditsMore; ++i) {
                (*this->buttons)[i]->OnTouchBegin(touch, coord);
            }
            (*this->buttons)[kHangarButtonCreditsMore]->OnTouchBegin(touch, coord);
        } else if (this->freeCreditsActive != 0) {
            for (unsigned int i = kHangarButtonFreeCreditsFirst;
                 i != kHangarButtonBlueprintAutoComplete; ++i) {
                (*this->buttons)[i]->OnTouchBegin(touch, coord);
            }
        }
        this->dialog->OnTouchBegin(touch, coord);
        return 0;
    }

    this->touchStartY = coord;
    this->lastTouchY = coord;
    this->scrollDelta = 0;
    this->dragging = 1;

    if (this->viewMode == 1) {
        this->listItemWindow->OnTouchBegin(touch, coord);
        return 0;
    }

    int outsideList = 1;
    int row = 0;
    if (layout->field_0xc < coord && coord < Globals::h - layout->field_0x10) {
        row = IDIV(coord - layout->field_0xc - layout->field_0x20 - this->rowLayoutMetrics.rowGap -
                       this->scrollOffset,
                   layout->field_0x70 + this->rowLayoutMetrics.rowGap);
        // Android's body has no lower-bound check because touch coordinates keep
        // this calculation non-negative. Preserve that invariant locally.
        if (row >= 0 && row < this->hangarList->getCurrentLength()) {
            this->hangarList->setCurrentItemIndex(row);
            ListItem *current = this->hangarList->getCurrentItem();
            this->highlightItem(current);
            outsideList = 0;

            if (this->upgradeMode != 0 && current->isShip() &&
                this->hangarList->getCurrentTab() == 1) {
                status->getShip()->setCargo(Item::extractItems(this->itemList, true));
                status->getStation()->setItems(Item::extractItems(this->itemList, false), false);
            }
        }
    }

    const bool keepInputRouting = this->hangarList->getCurrentTab() != 4 || this->buyMode == 0 ||
                                  (!outsideList && this->selectedItem == this->bluePrintItem) ||
                                  this->bluePrintBuyCount < 1 || this->dialogActive != 0 ||
                                  this->bluePrint->isEmpty() ||
                                  this->bluePrint->getStationIndex() == status->getStation()->getIndex();
    if (keepInputRouting) {
        for (unsigned int i = 0; i < this->tabButtons->size(); ++i) {
            handled |= (*this->tabButtons)[i]->OnTouchBegin(touch, coord);
        }
        for (unsigned int i = 0; i < this->buttons->size(); ++i) {
            TouchButton *button = (*this->buttons)[i];
            if (button != nullptr) {
                button->OnTouchBegin(touch, coord);
            }
        }

        if (this->autoEquipPending != 0 && this->hangarList->getCurrentTab() == 1) {
            const int index = static_cast<int>(this->autoEquipIndex);
            if (index >= 0 && ((~handled & (index == this->hangarList->getCurrentItemIndex())) == 0)) {
                this->autoEquipPending = 0;
                this->autoEquipSecondaryWeapons(index);
            }
        }
    } else {
        const int itemIndex = this->bluePrintItem->item->getIndex();
        this->localBluePrint = itemIndex == 209 || itemIndex == 204;
        String message = *gameText->getText(this->localBluePrint != 0 ? 289 : 288);
        if (this->localBluePrint == 0) {
            message = status->replaceHash(message, this->bluePrint->getStationName(), String("#S"));
            message = status->replaceHash(message,
                                          Globals::layout->formatCredits(this->bluePrintItem->item->getBlueprintAmount()),
                                          String("#C"));
        }
        this->dialog->set(message, this->localBluePrint == 0);
        this->dialogActive = 1;
        this->bluePrintPurchasePending = 1;
        this->suppressTouchEnd = 1;
    }
    return 0;
}








void HangarWindow::showCreditsBuyWindow() {
    ApplicationManager *appManager = static_cast<ApplicationManager *>(Globals::appManager);
    static_cast<uint8_t *>(appManager->GetApplicationData())[0x4c] = 0;
    static_cast<uint8_t *>(appManager->GetApplicationData())[0x3d] = 1;

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
            dialog->setWidth(this->buttonWidth * 3);
            dialog->setHeight(static_cast<int>(static_cast<float>(this->gridButtonHeight) * 2.3f));
        } else {
            dialog->setWidth(Globals::w);
            dialog->setHeight(Globals::h);
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

    this->dialogActive = 1;
    *reinterpret_cast<uint16_t *>(&this->buyCreditsActive) = 1;
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
                int *amts = counters->data();
                for (uint32_t j = 0; j < ingr->size(); j++) {
                    if (amts[j] > 0) {
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
    ListItem *item = this->selectedItem;
    if ((this->buyMode != 0 && item == nullptr) ||
        (this->buyMode == 0 && (item == nullptr || !buy)) ||
        (item != nullptr && (item->isShip() || item->isSlot() || item->isTextButton() ||
                             item->isSelectable() == 0 || item->isBluePrint()))) {
        this->buyMode = 0;
        return;
    }

    Status *status = Globals::status != nullptr ? Globals::status : Status::gStatus;
    GameText *gameText = Globals::gameText != nullptr
                             ? static_cast<GameText *>(Globals::gameText)
                             : GameText::gGameText;
    Ship *ship = status == nullptr ? nullptr : status->getShip();
    Station *station = status == nullptr ? nullptr : status->getStation();
    if (status == nullptr || gameText == nullptr || ship == nullptr || station == nullptr ||
        this->hangarList == nullptr) {
        this->buyMode = 0;
        return;
    }

    this->buyMode = buy;
    const int currentTab = this->hangarList->getCurrentTab();
    if (currentTab == 1) {
        Item *storeItem = item->item;
        if (storeItem == nullptr) {
            this->buyMode = 0;
            return;
        }

        if (this->buyMode != 0) {
            if (g_hangarStoreBuyHintShown == 0) {
                this->dialog->set(*gameText->getText(588));
                g_hangarStoreBuyHintShown = 1;
                this->dialogActive = 1;
            }
            this->savedStationAmount = storeItem->getStationAmount();
            this->savedAmount = storeItem->getAmount();
            this->savedCredits = status->getCredits();
            this->savedLoad = this->currentLoad;
        } else {
            if (item->isItem() && storeItem->getType() != 4 && g_hangarStoreSellHintShown == 0) {
                this->dialog->set(*gameText->getText(589));
                g_hangarStoreSellHintShown = 1;
                this->dialogActive = 1;
            }
            this->autoEquipPending = 1;
            this->autoEquipIndex = this->hangarList->getCurrentItemIndex();
        }

        if (this->itemList != nullptr) {
            ship->setCargo(Item::extractItems(this->itemList, true));
            station->setItems(Item::extractItems(this->itemList, false), false);
            ArrayReleaseClasses(*this->itemList);
            ArrayRemoveAll(*this->itemList);
            delete this->itemList;
        }
        this->itemList = Item::mixItems(ship->getCargo(), station->getItems());
        this->hangarList->initShopTab(this->itemList, station->getShips());
        this->hangarList->initShipTab(ship);

        this->selectedItem = this->hangarList->getCurrentItem();
        if (this->selectedItem != nullptr && this->selectedItem->isShip()) {
            this->selectedItem->ship->adjustPrice();
        }
        this->refreshCurrentContentHeight();
        return;
    }

    if (currentTab != 4) {
        return;
    }

    BluePrint *bluePrint = this->bluePrint;
    if (bluePrint == nullptr || item->item == nullptr) {
        this->buyMode = 0;
        return;
    }

    if (this->buyMode == 0) {
        if (this->bluePrintItem != nullptr && this->bluePrintItem->item != nullptr) {
            Item *ingredient = this->bluePrintItem->item;
            bluePrint->addItem(ingredient, ingredient->getBlueprintAmount(), station->getIndex());
        }

        const int blueprintIndex = bluePrint->getIndex();
        bool completed = bluePrint->isCompleted();
        if (completed) {
            if (bluePrint->getStationIndex() == station->getIndex()) {
                String message = *gameText->getText(211);
                message = status->replaceHash(message, *gameText->getText(blueprintIndex + 1274),
                                              String("#N"));
                this->dialog->set(message);

                Item *prototype = Item::g_items == nullptr || blueprintIndex < 0 ||
                                      static_cast<unsigned int>(blueprintIndex) >= Item::g_items->size()
                                      ? nullptr
                                      : (*Item::g_items)[blueprintIndex];
                if (prototype != nullptr) {
                    Item *product = prototype->makeItem(bluePrint->getQuantity());
                    ship->addCargo(product);
                    if (this->itemList != nullptr) {
                        ArrayAdd(product, *this->itemList);
                    }
                }
                this->hangarList->setCurrentTab(1, true);
                this->refreshCurrentContentHeight();
            } else {
                String message = *gameText->getText(210);
                message = status->replaceHash(message, *gameText->getText(blueprintIndex + 1274),
                                              String("#N"));
                message = status->replaceHash(message, bluePrint->getStationName(), String("#S"));
                this->dialog->set(message);
                status->addPendingProduct(bluePrint);
                this->hangarList->setCurrentTab(2, true);
                this->refreshCargoAvailabilityForBlueprints();
            }
            bluePrint->reset();
        }

        ship->setCargo(Item::extractItems(ship->getCargo(), true));
        Array<Item *> *shopItems = Item::mixItems(ship->getCargo(), station->getItems());
        this->hangarList->initShopTab(shopItems, station->getShips());
        this->hangarList->initBlueprintTab(status->getBluePrints());
        this->itemList = Item::mixItems(ship->getCargo(), station->getItems());
        this->dialogActive = completed;

        if (completed) {
            Array<ListItem *> *items = this->hangarList->getCurrentTabItems();
            if (items != nullptr) {
                for (unsigned int i = 0; i < items->size(); ++i) {
                    ListItem *candidate = items->data()[i];
                    if (candidate != nullptr && candidate->isItem() && candidate->item != nullptr &&
                        candidate->item->getIndex() == blueprintIndex &&
                        ship->hasEquipment(candidate->item->getIndex(), 1)) {
                        this->autoEquipIndex = i;
                        this->autoEquipPending = 1;
                        this->autoEquipSecondaryWeapons(static_cast<int>(i));
                        this->autoEquipPending = 0;
                        return;
                    }
                }
            }
        }
        return;
    }

    this->bluePrintBuyCount = 0;
    if (bluePrint->isEmpty() && item->item->getAmount() >= 1) {
        const int blueprintIndex = bluePrint->getIndex();
        const bool requiresRoute = blueprintIndex == 210 || blueprintIndex == 223;
        SolarSystem *system = status->getSystem();
        const bool routeUnavailable = requiresRoute && system != nullptr && system->getRoutes() == nullptr;
        if (routeUnavailable) {
            this->routeWarningPending = 1;
            this->dialog->set(*gameText->getText(528), false);
        } else {
            this->dialog->set(*gameText->getText(212), true);
        }
        this->dialogActive = 1;
    }

    this->savedBlueprintAmount = item->item->getBlueprintAmount();
    this->savedAmount = item->item->getAmount();
    this->savedCredits = status->getCredits();
    this->savedLoad = this->currentLoad;
    this->bluePrintItem = this->selectedItem;
    this->refreshCurrentContentHeight();
}










int HangarWindow::selectItem(ListItem *item) {
    this->selectedItem = item;
    if (item != nullptr && item->isShip()) {
        item->ship->adjustPrice();
    }

    Status *status = Globals::status;
    GameText *gameText = Globals::gameText;
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
        return 0;
    }

    if (tab == 1) {
        if (!item->isSelectable()) {
            return 0;
        }

        if (item->isShip()) {
            const int price = item->ship->getPrice();
            const int credits = status->getCredits();
            Ship *activeShip = status->getShip();
            if (price > activeShip->getPrice() + credits && !this->upgradeMode) {
                String message(*gameText->getText(203));
                String missing = Globals::layout->formatCredits(
                    item->getPrice() - status->getCredits() - status->getShip()->getPrice());
                message = status->replaceHash(message, missing, String("#C"));
                message += String("\n\n") + *gameText->getText(124);
                this->dialog->set(message, true);
                this->dialogActive = 1;
                this->notEnoughCredits = 1;
                return 0;
            }

            int textId;
            bool confirm = false;
            if (status->passengers < 1) {
                if (status->getCurrentCampaignMission() == 77 && status->getShip()->getIndex() == 37) {
                    textId = 325;
                } else if (status->getShip()->getIndex() != item->ship->getIndex()) {
                    this->shipSwapPending = 1;
                    textId = this->upgradeMode ? 333 : 304;
                    confirm = true;
                } else {
                    textId = 329;
                }
            } else {
                textId = 336;
            }
            if (confirm) {
                this->dialog->set(*gameText->getText(textId), true);
            } else {
                this->dialog->set(*gameText->getText(textId));
            }
            this->dialogActive = 1;
            return 0;
        }

        Item *storeItem = item->item;
        if (storeItem->isUnsaleable()) {
            return 0;
        }

        const uint8_t previousMode = this->buyMode;
        this->buyMode = previousMode ^ 1;
        if (previousMode) {
            if (item->isItem() && storeItem->getType() != 4 && !g_hangarStoreSellHintShown) {
                this->dialog->set(*gameText->getText(589));
                g_hangarStoreSellHintShown = 1;
                this->dialogActive = 1;
            }
            const int itemIndex = item->getIndex();
            if (itemIndex >= 132 && itemIndex <= 153) {
                (*status->field_ac)[itemIndex - 132] = true;
            }
            this->autoEquipPending = 1;
            this->autoEquipIndex = this->hangarList->getCurrentItemIndex();

            Ship *ship = status->getShip();
            Station *station = status->getStation();
            ship->setCargo(Item::extractItems(this->itemList, true));
            station->setItems(Item::extractItems(this->itemList, false), false);
            if (this->itemList != nullptr) {
                ArrayReleaseClasses(*this->itemList);
                ArrayRemoveAll(*this->itemList);
                delete this->itemList;
            }
            this->itemList = nullptr;
            this->itemList = Item::mixItems(ship->getCargo(), station->getItems());
            this->hangarList->initShopTab(this->itemList, station->getShips());
            this->hangarList->initShipTab(ship);
        } else {
            if (!g_hangarStoreBuyHintShown) {
                this->dialog->set(*gameText->getText(588));
                g_hangarStoreBuyHintShown = 1;
                this->dialogActive = 1;
            }
            this->savedStationAmount = storeItem->getStationAmount();
            this->savedAmount = storeItem->getAmount();
            this->savedCredits = status->getCredits();
            this->savedLoad = this->currentLoad;
        }
        return 0;
    }

    if (tab != 0 || !item->isSelectable()) {
        return 0;
    }

    this->scrollOffsetBackup = 0;
    this->savedScrollOffset = this->scrollOffset;
    this->scrollOffset = 0;

    if (!g_hangarSlotHintShown && item->isSlot()) {
        this->dialog->set(*gameText->getText(587));
        g_hangarSlotHintShown = 1;
        this->dialogActive = 1;
    }

    if (!item->isSelectable()) {
        return 0;
    }

    ListItem *current = this->hangarList->getCurrentItem();
    Item *currentItem = current->item;
    if (currentItem != nullptr) {
        if (currentItem->isUnsaleable()) {
            this->dialog->set(*gameText->getText(323));
            this->dialogActive = 1;
            return 0;
        }
        if (currentItem->getSort() == 20) {
            const int passengers = status->passengers;
            if ((passengers < 1 || item->inTabIndex < 0) &&
                status->getShip()->getMaxPassengers() - currentItem->getAttribute(34) < passengers) {
                this->dialog->set(*gameText->getText(323));
                this->dialogActive = 1;
                return 0;
            }
        }
    }

    if (item->inTabIndex >= 0) {
        this->demountItem(currentItem, current->subTabIndex);
        return 0;
    }

    Item *mount = item->item;
    Ship *ship = status->getShip();
    Item *existing = ship->getFirstEquipmentOfSort(mount->getSort());
    bool canMount = false;
    if (mount->getSort() == 21) {
        if (ship->getIndex() == 44 || existing == nullptr || ship->getIndex() == 49) {
            canMount = true;
        }
    } else if (existing == nullptr) {
        canMount = true;
    }
    if (canMount || mount->canBeInstalledMultipleTimes()) {
        this->mountItem(mount);
        return 0;
    }

    String message(*gameText->getText(287));
    message = status->replaceHash(message, *gameText->getText(existing->getIndex() + 1274), String("#ITEM1"));
    message = status->replaceHash(message, *gameText->getText(mount->getIndex() + 1274), String("#ITEM2"));
    this->dialog->set(message, true);
    this->replaceEquipPending = 1;
    this->dialogActive = 1;
    this->pendingMountItem = mount;
    this->pendingDemountItem = existing;
    return 0;
}

float HangarWindow::getRelativeScrollHeight() {
    int a = this->currentContentHeight;
    int b = this->visibleHeight;
    if (a < b) {
        return 0.0f;
    }
    int e = this->scrollOffset;
    int num;
    if (e >= 1) {
        num = b - e;
    } else if (e >= b - a) {
        num = b;
    } else {
        num = e + a;
    }
    return (float) num / (float) a;
}





void HangarWindow::transaction(bool buy) {
    const unsigned int tab = this->hangarList->getCurrentTab();
    Status *status = Globals::status;
    Item *item = this->selectedItem->item;

    if (tab > 1) {
        if (tab == 4) {
            if (buy) {
                const int blueprintAmount = item->getBlueprintAmount();
                if (blueprintAmount < this->bluePrint->getRemainingAmount(item->getIndex())) {
                    const int result = item->transactionBlueprint(false, this->currentLoad);
                    if (result <= -1) {
                        ++this->currentLoad;
                    } else if (result != 0) {
                        ++this->bluePrintBuyCount;
                        status->getShip()->changeLoad(-1);
                    }
                }
            }
            Array<Item *> *cargo = status->getShip()->getCargo();
            if (cargo != nullptr) {
                for (unsigned int i = 0; i < cargo->size(); ++i) {
                    Item *cargoItem = cargo->data()[i];
                    if (cargoItem->getIndex() == item->getIndex()) {
                        cargoItem->setAmount(item->getAmount());
                        cargoItem->setBlueprintAmount(item->getBlueprintAmount());
                    }
                }
            }
        }
        return;
    }

    if (item->isUnsaleable()) {
        this->dialog->set(*Globals::gameText->getText(323));
        this->buyMode = 0;
        this->dialogActive = 1;
        return;
    }

    const int result = item->transaction(buy, this->currentLoad, this->upgradeMode);
    const unsigned int itemIndex = item->getIndex();
    if (itemIndex < status->field_54->size()) {
        (*status->field_54)[item->getIndex()] = true;
    }

    if (result <= -1 && buy) {
        ++this->currentLoad;
        status->getShip()->changeLoad(1);
        if (this->selectedItem->getIndex() >= 132 && this->selectedItem->getIndex() <= 153) {
            (*status->field_ac)[this->selectedItem->getIndex() - 132] = true;
        }
    } else if (result != 0 || !buy) {
        if (result >= 1 && !buy) {
            --this->currentLoad;
            status->getShip()->changeLoad(-1);
        }
    } else if (status->getCredits() < item->getSinglePrice()) {
        if (this->upgradeMode) {
            return;
        }
        String message(*Globals::gameText->getText(203));
        String missing = Globals::layout->formatCredits(item->getSinglePrice() - status->getCredits());
        message = status->replaceHash(message, missing, String("#C"));
        message += String("\n\n") + *Globals::gameText->getText(124);
        this->dialog->set(message, true);
        this->dialogActive = 1;
        this->notEnoughCredits = 1;
        (*this->buttons)[8]->resetTouch();
        (*this->buttons)[9]->resetTouch();
    }

    if (!this->upgradeMode) {
        status->changeCredits(result);
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

            if (isTouched == 0 && touchDistance >= 6) {
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
    if (listItem == nullptr || !listItem->isItem() || listItem->item == nullptr ||
        listItem->item->getType() != 1 || listItem->item->getAmount() < 1) {
        return;
    }

    Item *candidate = listItem->item;
    Ship *ship = Status::gStatus->getShip();
    Array<Item *> *secondaryWeapons = ship->getEquipment(1);
    if (secondaryWeapons == nullptr) {
        return;
    }

    for (unsigned int slot = 0; slot < secondaryWeapons->size(); ++slot) {
        Item *equipped = (*secondaryWeapons)[slot];
        if (equipped == nullptr || equipped->getIndex() != candidate->getIndex()) {
            continue;
        }

        Item *merged = candidate->makeItem(equipped->getAmount() + candidate->getAmount());
        if (this->itemList != nullptr) {
            for (unsigned int i = 0; i < this->itemList->size(); ++i) {
                Item *cargoItem = (*this->itemList)[i];
                if (cargoItem != nullptr && cargoItem->getIndex() == merged->getIndex()) {
                    cargoItem->setAmount(0);
                }
            }
        }

        ship->setEquipment(merged, static_cast<int>(slot));
        ship->removeCargo(merged->getIndex(), merged->getAmount());
        this->hangarList->initShipTab(ship);

        String messageTemplate = *(String *) GameText::gGameText->getText(208);
        String itemName = *(String *) GameText::gGameText->getText(merged->getIndex() + 1274);
        String message = Status::gStatus->replaceHash(messageTemplate, itemName, String("#N"));
        this->dialog->set(message);
        this->autoEquipped = 1;
        this->dialogActive = 1;
        return;
    }
}




void HangarWindow::showFreeCreditsWindow() {
    ApplicationManager *appManager = static_cast<ApplicationManager *>(Globals::appManager);
    static_cast<uint8_t *>(appManager->GetApplicationData())[0x4c] = 0;
    static_cast<uint8_t *>(appManager->GetApplicationData())[0x3d] = 1;

    ChoiceWindow *dialog = this->dialog;
    {
        String title("");
        String message("");
        String left("");
        String right("");
        const String *text = Globals::gameText->getText(170);
        dialog->set(title, message, false, left, right, *text, -1, -1);
    }

    dialog->setHeight((*this->buttons)[18]->getHeight() * 5);

    int maxTextWidth = 0;
    for (int i = 0; i < 5; ++i) {
        int textId = i == 0 ? 3400 : 112 + i;
        const String *line = Globals::gameText->getText(textId);
        int textWidth = Globals::Canvas->GetTextWidth(Globals::font, *line);
        if (textWidth > maxTextWidth) {
            maxTextWidth = textWidth;
        }
    }
    Layout *layout = static_cast<Layout *>(Globals::layout);
    dialog->setWidth(layout->field_0x2c + (*this->buttons)[18]->getWidth() + maxTextWidth + layout->field_0x28 * 4);

    this->freeCreditsActive = 1;
    this->dialogActive = 1;
    *reinterpret_cast<uint16_t *>(&this->buyCreditsActive) = 0;
}
















void HangarWindow::initialize() {
    HangarWindow *self = this;
    Status *status = Status::gStatus;
    Layout *layout = static_cast<Layout *>(Globals::layout);

    uint8_t special = status->getStation()->getIndex() == 0x6c && status->field_114 == 3;
    self->upgradeMode = special;
    status->calcCargoPrices();

    HangarList *list = new HangarList();
    self->hangarList = list;
    self->itemList = Item::mixItems(status->getShip()->getCargo(), status->getStation()->getItems());
    list->init(status->getShip(), self->itemList, status->getStation()->getShips(), status->getBluePrints());

    self->tabButtons = new Array<TouchButton *>();
    ArraySetLength(3, *(self->tabButtons));

    int scrW = Globals::w;

    void *b0 = ::operator new(200);
    TouchButton_ctor_text(b0, GameText::gGameText->getText(272), 3,
                          scrW - layout->getHelpButtonOffset(), 0, 0x12);
    (*self->tabButtons)[(8) >> 2] = (TouchButton *) (b0);

    void *b1 = ::operator new(200);
    int w0 = ((TouchButton *) (b0))->getWidth();
    int tab1TextId = self->upgradeMode != 0 ? 186 : 185;
    TouchButton_ctor_text(b1, GameText::gGameText->getText(tab1TextId), 3,
                          (scrW - layout->getHelpButtonOffset() - w0) + layout->field_0x38, 0, 0x12);
    (*self->tabButtons)[(4) >> 2] = (TouchButton *) (b1);

    void *b2 = ::operator new(200);
    int w0b = ((TouchButton *) (b0))->getWidth();
    int w1b = ((TouchButton *) (b1))->getWidth();
    TouchButton_ctor_text(b2, GameText::gGameText->getText(183), 3,
                          (scrW - layout->getHelpButtonOffset() - w0b - w1b) + layout->field_0x38 * 2,
                          0, 0x12);
    (*self->tabButtons)[(0) >> 2] = (TouchButton *) (b2);
    self->listModeFlag = Globals::showNewCreditsMenu;

    void *icons = ::operator new[](0x18);
    self->tabIcons = icons;
    for (int i = 0; i != 6; i++)

        PaintCanvas::gCanvas->Image2DCreate((unsigned short) (i + 0x232a), *(unsigned int *) ((char *) icons + i * 4));

    Array<TouchButton *> *tabArr = self->tabButtons;
    for (unsigned int i = 0; i < tabArr->size(); i++) {
        if (i < 10) {
            float x = 0, y = 0;
            TouchButton_getPosition(tabArr->data()[i], &x, &y);
            Globals::sub_menu_buttons_x[i] = (int) x;
            TouchButton_getPosition(tabArr->data()[i], &x, &y);
            Globals::sub_menu_buttons_y[i] = (int) y;
        }
    }

    Globals::sub_menu_button_count = (int) tabArr->size();
    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x52e), self->blueprintIconImage);
    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x544), self->pendingIconImage);

    self->buttons = new Array<TouchButton *>();
    ArraySetLength(kHangarButtonCount, *(self->buttons));

    unsigned int img;
    img = 0xffffffff;
    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x470), img);
    void *e0 = ::operator new(200);
    TouchButton_ctor_img((void *) e0, (void *) (uintptr_t) img, 7, 0, 0, layout->field_0x60, 0x11, 4);
    (*self->buttons)[kHangarButtonInspect] = (TouchButton *) (e0);

    bool deepScienceCampaign = status->getCurrentCampaignMission() == 0x4d && status->getStation()->getIndex() == 100;

    void *e1 = ::operator new(200);
    TouchButton_ctor_text(e1, GameText::gGameText->getText((self->upgradeMode != 0 || deepScienceCampaign) ? 332 : 301),
                          7, 0, 0, 0x11);
    (*self->buttons)[kHangarButtonSelectShip] = (TouchButton *) (e1);
    void *e2 = ::operator new(200);
    TouchButton_ctor_text(e2, GameText::gGameText->getText(302), 7, 0, 0, 0x11);
    (*self->buttons)[kHangarButtonSelectItemFirst] = (TouchButton *) (e2);

    img = 0xffffffff;
    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x533), img);
    void *e3 = ::operator new(200);
    TouchButton_ctor_img((void *) e3, (void *) (uintptr_t) img, 7, 0, 0, layout->field_0x64, 0x11, 4);
    (*self->buttons)[kHangarButtonSelectItemSecond] = (TouchButton *) (e3);

    img = 0xffffffff;
    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x532), img);
    void *e4 = ::operator new(200);
    TouchButton_ctor_img((void *) e4, (void *) (uintptr_t) img, 7, 0, 0, layout->field_0x64, 0x11, 4);
    (*self->buttons)[kHangarButtonSelectItemThird] = (TouchButton *) (e4);

    void *e5 = ::operator new(200);
    TouchButton_ctor_text2(e5, GameText::gGameText->getText(279), 7, 0, 0, self->listEntryActionWidth, 0x11);
    (*self->buttons)[kHangarButtonSellListEntry] = (TouchButton *) (e5);
    void *e6 = ::operator new(200);
    TouchButton_ctor_text2(e6, GameText::gGameText->getText(282), 7, 0, 0, self->listEntryActionWidth, 0x11);
    (*self->buttons)[kHangarButtonMoveToCargoEntry] = (TouchButton *) (e6);
    void *e7 = ::operator new(200);
    TouchButton_ctor_text(e7, GameText::gGameText->getText(283), 7, 0, 0, 0x11);
    (*self->buttons)[kHangarButtonSelectBlueprint] = (TouchButton *) (e7);

    {
        String lbl;
        void *e8 = ::operator new(200);
        TouchButton_ctor_img((void *) e8, &lbl, 8, 0, 0, layout->field_0x50, 0x11, 4);
        (*self->buttons)[kHangarButtonCurrentAmount] = (TouchButton *) (e8);
    }
    {
        String lbl;
        void *e9 = ::operator new(200);
        TouchButton_ctor_img((void *) e9, &lbl, 9, 0, 0, layout->field_0x50, 0x11, 4);
        (*self->buttons)[kHangarButtonStationAmount] = (TouchButton *) (e9);
    }
    {
        void *e10 = ::operator new(200);
        TouchButton_ctor_img((void *) e10, GameText::gGameText->getText(330), 7, 0, 0,
                             layout->field_0x50, 0x11, 4);
        (*self->buttons)[kHangarButtonSellShip] = (TouchButton *) (e10);
    }
    {
        String credits;
        void *e11 = ::operator new(200);
        credits = Globals::layout->formatCredits(status->getCredits());
        TouchButton_ctor_img((void *) e11, &credits, 0xb, Globals::w, Globals::h,
                             layout->getFooterTransitionWidth(), 0x22, 4);
        (*self->buttons)[kHangarButtonCredits] = (TouchButton *) (e11);
    }

    uint8_t listMode = self->listModeFlag;
    int row = 0;
    for (unsigned int slot = kHangarButtonPaidCreditsFirst;
         slot <= kHangarButtonPaidCreditsLast; slot++) {
        String lbl;
        void *btn = ::operator new(200);
        int visIdx;
        if (listMode == 0) {
            TouchButton_ctor_img(btn, &lbl, 0, 0, 0, layout->field_0x264, 0x11, 1);
            visIdx = row + kHangarButtonPaidCreditsFirst;
        } else {
            TouchButton_ctor_text(btn, &lbl, 10, 0, 0, 1);
            visIdx = slot;
        }
        (*self->buttons)[slot] = (TouchButton *) (btn);
        (*self->buttons)[visIdx]->setVisible(false);
        row++;
    }
    {
        String lbl;
        void *btn = ::operator new(200);
        if (listMode == 0)
            TouchButton_ctor_img(btn, &lbl, 0, 0, 0, layout->field_0x264, 0x11, 1);
        else
            TouchButton_ctor_text(btn, &lbl, 10, 0, 0, 1);
        (*self->buttons)[kHangarButtonCreditsMore] = (TouchButton *) (btn);
        (*self->buttons)[kHangarButtonCreditsMore]->setVisible(false);
    }

    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x233e), self->scrollHintImageA);
    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x233f), self->scrollHintImageB);
    {
        String lbl;
        void *btn = ::operator new(200);
        TouchButton_ctor_text(btn, &lbl, 7, 0, 0, 0x11);
        (*self->buttons)[kHangarButtonBlueprintAutoComplete] = (TouchButton *) (btn);
    }

    unsigned int imgA, imgB;
    for (unsigned int i = kHangarButtonFreeCreditsFirst;
         i <= kHangarButtonFreeCreditsLast; i++) {
        imgB = 0xffffffff;
        if (i == kHangarButtonFreeCreditsFirst) {
            PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x233c), imgA);
            PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x233d), imgB);
        } else {
            short s = (short) (i - kHangarButtonFreeCreditsFirst);
            PaintCanvas::gCanvas->Image2DCreate((unsigned short) (s * 2 + 0x2330), imgA);
            PaintCanvas::gCanvas->Image2DCreate((unsigned short) (s * 2 + 0x2331), imgB);
        }
        void *btn = ::operator new(200);
        TouchButton_ctor_img2(btn, (void *) (uintptr_t) imgA, (void *) (uintptr_t) imgB, 0x13, 0, 0, 1);
        (*self->buttons)[i] = (TouchButton *) (btn);
        (*self->buttons)[i]->setVisible(false);
    }

    self->buttonWidth = (*self->buttons)[kHangarButtonPaidCreditsFirst]->getWidth();
    int h = (*self->buttons)[kHangarButtonPaidCreditsFirst]->getHeight();
    self->gridButtonHeight = h;
    self->gridSpacingX = (int) ((float) (-self->buttonWidth) * 0.1f);
    self->gridSpacingY = (int) ((float) (-h) * 0.1f);

    unsigned int progressBarBgImageHandle;
    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x475), progressBarBgImageHandle);
    self->progressBarBgImage = progressBarBgImageHandle;
    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x476), self->progressBarFillImage);
    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x477), self->progressBarBorderImage);
    self->progressBarWidth = PaintCanvas::gCanvas->GetImage2DWidth(self->progressBarFillImage);
    self->progressBarHeight = PaintCanvas::gCanvas->GetImage2DHeight(self->progressBarFillImage);

    if (self->itemList != 0 && Status::gStatus->inBlackMarketSystem() == 0 &&
        self->upgradeMode == 0) {
        Array<Item *> *equip = ((Ship *) (Status::gStatus->getShip()))->getEquipment();
        Array<Item *> *cargo = self->itemList;
        unsigned int n = cargo->size() + (equip ? equip->size() : 0);
        for (unsigned int i = 0; i < n; i++) {
            void *itemPtr;
            if (i < cargo->size())
                itemPtr = cargo->data()[i];
            else
                itemPtr = equip->data()[i - cargo->size()];
            if (itemPtr != 0) {
                int price = ((Item *) (itemPtr))->getSinglePrice();
                int idx = ((Item *) (itemPtr))->getIndex();
                Globals *globals = Globals::gGlobals;

                int *buyTbl = (int *) (*(int *) ((char *) (globals->field_0x40) + (4)));
                if (buyTbl[idx] < price || buyTbl[idx] == 0) {
                    buyTbl[idx] = price;
                    int sysIdx = static_cast<SolarSystem *>(status->getSystem())->getIndex();
                    *(int *) ((*(int *) ((char *) (globals->field_0x48) + (4))) + idx * 4) = sysIdx;
                }
                int *sellTbl = (int *) (*(int *) ((char *) (globals->field_0x3c) + (4)));
                if (price < sellTbl[idx] || sellTbl[idx] == 0) {
                    sellTbl[idx] = price;
                    int sysIdx = static_cast<SolarSystem *>(status->getSystem())->getIndex();
                    *(int *) ((*(int *) ((char *) ((*(void * *) ((char *) (Globals::gGlobals) + (0x44)))) + (4))) + idx * 4)
                            = sysIdx;
                }
            }
        }
    }

    ((Ship *) ((Ship *) (Status::gStatus->getShip())))->adjustPrice();

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

    int *cols = (int *) ::operator new[](0xc);
    self->columnWidths = cols;
    int third = IDIV(contentW, 3) - 2;
    cols[0] = third;
    cols[1] = third;
    cols[2] = (Globals::w - 0xe) + third * -2;

    self->hangarList->setCurrentTab(HangarWindow::lastTab, false);
    self->refreshCurrentContentHeight();

    self->currentLoad = Status::gStatus->getShip()->getCurrentLoad();
    Layout *lay2 = static_cast<Layout *>(Globals::layout);
    self->visibleHeight = ((Globals::h - lay2->field_0x10 - lay2->field_0xc) -
                           lay2->field_0x20) - lay2->field_0x24;

    int extra = 0;
    if (Globals::iPad != 0 && Globals::iPadAssetsWithLowerRes == 0) {
        unsigned int hintImageHandle;
        PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x6a4), hintImageHandle);
        self->hintImage = hintImageHandle;
        extra = (int) ((float) Globals::w * 0.2f);
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

    if (status->getCurrentCampaignMission() >= 14 && g_hangarIntroShown == 0) {
            self->dialog->set(*GameText::gGameText->getText(109));
            g_hangarIntroShown = 1;
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
