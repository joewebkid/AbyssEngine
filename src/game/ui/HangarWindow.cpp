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
// The ARM renderer uses separate persisted bytes for these offers. Their
// RecordHandler slots are not typed yet, so keep the state local until that
// persistence mapping is recovered instead of dereferencing placeholder data.
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
        this->currentContentHeight = this->field_0x100.d * (n - 1) + n * rowH;
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
        ArrayReleaseClasses(*this->tabButtons); ArrayRemoveAll(*(this->tabButtons));
        delete this->tabButtons;
    }
    this->tabButtons = nullptr;
    if (this->buttons != nullptr) {
        ArrayReleaseClasses(*this->buttons); ArrayRemoveAll(*(this->buttons));
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
    const unsigned int font = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(Globals::font));

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

    if (stationModule == nullptr || stationModule->pendingHangarClose == 0) {
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
                for (int i = 0; i != 0x18; i++) {
                    if (this->dragging == 0) {
                        TouchButton *btn = buttonAt(i);
                        if (btn != nullptr)
                            btn->setVisible(false);
                    }
                }

                int boxW = rowGap - 2;

                for (unsigned int i = 0; i < items->size(); i++) {
                    int y = (layout->field_0x70 + this->field_0x100.d) * (int) i +
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
                            String price = Layout::formatCredits(shipPrice);
                            canvas->DrawString(font, price, titleX, priceY, false);
                            if (imageFactory != nullptr) {
                                imageFactory->drawShip(shipIndex, iconX, iconY);
                            }
                            canvas->SetColor(0xffffffffu);
                        } else if (li->isSlot()) {
                            label = textFor(174);
                            if (tab == 4 && i + 1 == items->size()) {
                                TouchButton *createButton = buttonAt(23);
                                if (createButton != nullptr) {
                                    createButton->setPosition(this->hintOffsetX + layout->field_0x28 + topY / 2,
                                                              this->field_0x114 + y, 0x14);
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
                                                   y + layout->field_0x70 / 2 + this->field_0x114, false);
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
                            TouchButton *moveButton = buttonAt(6);
                            if (moveButton != nullptr) {
                                moveButton->setPosition(this->hintOffsetX + layout->field_0x28, y, 0x11);
                                moveButton->setVisible(true);
                                moveButton->draw();
                            }
                        } else if (li->isSellButton()) {
                            TouchButton *sellButton = buttonAt(5);
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
                            String price = Layout::formatCredits(li->item->getSinglePrice());
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
                        button->setPosition(rightEdge - xOffset, this->field_0x114 + y, 0x12);
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
                        TouchButton *cargoButton = buttonAt(8);
                        if (currentAmount >= 1 && cargoButton != nullptr) {
                            cargoButton->setPosition(this->hintOffsetX + layout->field_0x28,
                                                     this->field_0x114 + y, 0x11);
                            cargoButton->setVisible(true);
                            cargoButton->draw();
                        }
                        TouchButton *stationButton = buttonAt(9);
                        if (stationAmount >= 1 && stationButton != nullptr) {
                            stationButton->setPosition(rightEdge, this->field_0x114 + y, 0x12);
                            stationButton->setVisible(true);
                            stationButton->draw();
                            occupiedRightWidth = stationButton->getWidth() + layout->field_0x2c;
                        }
                    }

                    if (li->isShip() && li->ship != nullptr) {
                        const bool isCurrentShip = li->ship == status->getShip();
                        if (tab != 0 && !(tab == 3 && isCurrentShip)) {
                            occupiedRightWidth = drawRightAction(1, 0);
                        }
                    } else if (tab == 2 && li->isBluePrint()) {
                        occupiedRightWidth = drawRightAction(7, 0);
                    } else if (tab == 0 && li->isItem() && li->item != nullptr) {
                        Ship *ship = status->getShip();
                        Item *installed = ship == nullptr ? nullptr :
                            ship->getFirstEquipmentOfSort(li->item->getSort());
                        const bool blockedBySlots = installed != nullptr &&
                            !li->item->canBeInstalledMultipleTimes() &&
                            ship->getFreeSlots(li->item->getType()) == 0;
                        if (!blockedBySlots) {
                            const unsigned int actionIndex = li->inTabIndex < 0 ? 2 : 3;
                            occupiedRightWidth = drawRightAction(actionIndex, layout->field_0x2c * 2);
                        }
                    }

                    if (li->isItem() || li->isShip() || li->isBluePrint() || li->isPendingProduct()) {
                        drawRightAction(0, occupiedRightWidth);
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
    TouchButton *creditsButton = buttonAt(11);
    if (creditsButton != nullptr) {
        creditsButton->setVisible(true);
        creditsButton->setAlwaysPressed(g_hangarCreditOfferShown == 0);
        creditsButton->setText(Layout::formatCredits(status->getCredits()));
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
            for (unsigned int i = 0; i < 5; i++) {
                TouchButton *button = buttonAt(18 + i);
                if (button == nullptr) {
                    continue;
                }
                const bool visible = i != 0 && g_hangarSocialCreditClaimed[i] == 0;
                button->setVisible(visible);
                if (!visible) {
                    continue;
                }
                const int y = this->dialog->y + layout->field_0x8 + layout->field_0x2c * 2 +
                              static_cast<int>(i - 1) * (layout->field_0x30 + layout->field_0x34);
                button->setPosition(this->dialog->x + layout->field_0x28, y);
                button->draw();

                const int textId = i == 0 ? 3400 : 112 + static_cast<int>(i);
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
            for (unsigned int i = 0; i < 5; ++i) {
                TouchButton *button = buttonAt(12 + i);
                if (button == nullptr) {
                    continue;
                }
                const int row = static_cast<int>(i / 3);
                const int column = static_cast<int>(i % 3);
                const int x = Globals::w / 2 - this->buttonWidth - this->gridSpacingX +
                              column * (this->buttonWidth + this->gridSpacingX);
                const int y = static_cast<int>(-3 * layout->field_0x20 + Globals::h / 2 -
                                               this->gridButtonHeight / 2 - this->gridSpacingY * 0.5f) +
                              row * (this->gridButtonHeight + this->gridSpacingY);
                String label(descriptions[i] == nullptr ? "" : descriptions[i]);
                String split(prices[i] == nullptr ? "" : prices[i]);
                button->setVisible(true);
                button->setPosition(x, y, 0x44);
                button->replaceTextKeepSize(label);
                button->setSplitText(split);
                button->draw();
                if (productIcons != nullptr) {
                    canvas->DrawImage2D(productIcons[i], x, y - layout->field_0x2c, 0x11, 0x44);
                }
            }
        } else {
            const int startY = this->dialog->y + layout->field_0x8 + 5 * layout->field_0x2c;
            const int stepY = layout->field_0x30 + layout->field_0x34;
            for (unsigned int i = 0; i < 5; ++i) {
                TouchButton *button = buttonAt(12 + i);
                if (button == nullptr) {
                    continue;
                }
                String label(descriptions[i] == nullptr ? "" : descriptions[i]);
                String split(prices[i] == nullptr ? "" : prices[i]);
                button->setVisible(true);
                button->setText(label);
                button->setSplitText(split);
                button->setPosition(Globals::w / 2, startY + static_cast<int>(i) * stepY, 0x14);
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
                    (((coord - layout->field_0xc) - layout->field_0x20) - self->field_0x100.d) -
                    self->scrollOffset,
                    layout->field_0x70 + self->field_0x100.d);
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
                priceStr = Layout::formatCredits(0);
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
                priceStr = Layout::formatCredits(Status::gStatus->getCredits());
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
                    priceStr = Layout::formatCredits(Status::gStatus->getCredits());
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
                    priceStr = Layout::formatCredits(Status::gStatus->getCredits());
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

void HangarWindow::OnTouchEnd(int touch, int coord) {
    Layout *layout = static_cast<Layout *>(Globals::layout);
    Status *status = Globals::status != nullptr ? Globals::status : Status::gStatus;
    GameText *gameText = static_cast<GameText *>(Globals::gameText);
    if (gameText == nullptr) {
        gameText = GameText::gGameText;
    }
    if (layout == nullptr || status == nullptr || gameText == nullptr || this->hangarList == nullptr) {
        return;
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
        return Layout::formatCredits(credits);
    };
    auto showNotEnoughCredits = [&]() {
        String message = *gameText->getText(203);
        message = status->replaceHash(message, String("#C"), formatCredits(status->getCredits()));
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
        return;
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
                row = IDIV(coord - layout->field_0xc - layout->field_0x20 - this->field_0x100.d -
                               this->scrollOffset,
                           layout->field_0x70 + this->field_0x100.d);
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
                return;
            }

            TouchButton *autoComplete = buttonAt(23);
            if (autoComplete != nullptr && this->bluePrint != nullptr && this->dialog != nullptr &&
                autoComplete->OnTouchEnd(touch, coord) != 0) {
                String message = *gameText->getText(195);
                message = status->replaceHash(message, String("#C"),
                                              formatCredits(this->bluePrint->getAutoCompletionPrice()));
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
                    case 0:
                        if (this->listItemWindow != nullptr) {
                            this->listItemWindow->set(this->hangarList->getCurrentItem(), 0, 0, 0, 0, 1);
                            this->viewMode = 1;
                            if (Globals::sound != nullptr) {
                                Globals::sound->play(0x61, nullptr, nullptr, 0.0f);
                            }
                        }
                        return;
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        this->selectItem(this->selectedItem);
                        return;
                    case 8:
                        this->transaction(false);
                        if (Globals::sound != nullptr) {
                            Globals::sound->play(0x64, nullptr, nullptr, 0.0f);
                        }
                        return;
                    case 9: {
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
                        return;
                    }
                    case 10:
                        if (this->dialog != nullptr) {
                            this->dialog->set(*gameText->getText(334), true);
                            this->sellShipPending = 1;
                            this->dialogActive = 1;
                        }
                        return;
                    case 11: {
                        g_hangarCreditOfferShown = 1;
                        RecordHandler *recordHandler = static_cast<RecordHandler *>(Globals::recordHandler);
                        if (recordHandler != nullptr) {
                            recordHandler->saveOptions();
                        }
                        this->showCreditsBuyWindow();
                        return;
                    }
                    default:
                        break;
                    }
                }
            }

            if (layout->helpPressed() != 0) {
                const int helpTextIds[] = {623, 622, 625, 624, 626};
                const unsigned int tab = this->hangarList->getCurrentTab();
                if (this->viewMode == 1) {
                    layout->initHelpWindow(*gameText->getText(643));
                } else if (tab < 5) {
                    layout->initHelpWindow(*gameText->getText(helpTextIds[tab]));
                }
            }

            TouchButton *credits = buttonAt(11);
            if (credits != nullptr && credits->OnTouchEnd(touch, coord) != 0) {
                g_hangarCreditOfferShown = 1;
                RecordHandler *recordHandler = static_cast<RecordHandler *>(Globals::recordHandler);
                if (recordHandler != nullptr) {
                    recordHandler->saveOptions();
                }
                this->showCreditsBuyWindow();
            }
            return;
        }

        if (this->viewMode == 1) {
            layout->resetWindowDimensions();
            this->viewMode = 0;
            return;
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
            return;
        }
        if (tab == 3) {
            this->hangarList->setCurrentTab(0, true);
            this->refreshCargoAvailabilityForBlueprints();
            this->refreshCurrentContentHeight();
        } else if (this->readyToClose()) {
            this->setSellMode(false);
            resetSelection();
        }
        return;
    }

    if (this->dialog == nullptr) {
        this->dialogActive = 0;
        return;
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
        return;
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
        return;
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
        return;
    }

    if (this->buyCreditsActive != 0) {
        const int result = this->dialog->OnTouchEnd(touch, coord);
        if (result == 0) {
            this->buyCreditsActive = 0;
            this->dialogActive = 0;
            setCreditButtonsVisible(12, 18, false);
            uint8_t *appData = ApplicationManager::gAppManager != nullptr
                                   ? static_cast<uint8_t *>(ApplicationManager::gAppManager->GetApplicationData())
                                   : nullptr;
            if (appData != nullptr) {
                appData[64] = 0;
            }
            ModStation *module = stationModule();
            if (module != nullptr) {
                module->pendingHangarClose = 0;
            }
            return;
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
        for (unsigned int i = 0; i < 5; ++i) {
            TouchButton *button = buttonAt(12 + i);
            if (button != nullptr && button->OnTouchEnd(touch, coord) != 0) {
                buyCredits(i);
                return;
            }
        }
        TouchButton *more = buttonAt(17);
        if (more != nullptr && more->OnTouchEnd(touch, coord) != 0) {
            const bool allSocialOffersClaimed = std::all_of(
                g_hangarSocialCreditClaimed, g_hangarSocialCreditClaimed + 4,
                [](uint8_t claimed) { return claimed != 0; });
            uint8_t *appData = ApplicationManager::gAppManager != nullptr
                                   ? static_cast<uint8_t *>(ApplicationManager::gAppManager->GetApplicationData())
                                   : nullptr;
            if (!allSocialOffersClaimed || (appData != nullptr && appData[21] == 0)) {
                setCreditButtonsVisible(12, 17, false);
                this->showFreeCreditsWindow();
            }
        }
        return;
    }

    if (this->freeCreditsActive != 0) {
        const int result = this->dialog->OnTouchEnd(touch, coord);
        if (result == 0) {
            setCreditButtonsVisible(18, 23, false);
            this->freeCreditsActive = 0;
            this->showCreditsBuyWindow();
            return;
        }

        uint8_t *appData = ApplicationManager::gAppManager != nullptr
                               ? static_cast<uint8_t *>(ApplicationManager::gAppManager->GetApplicationData())
                               : nullptr;
        for (unsigned int i = 0; i < 5; ++i) {
            TouchButton *button = buttonAt(18 + i);
            if (button == nullptr || button->OnTouchEnd(touch, coord) == 0) {
                continue;
            }
            RecordHandler *recordHandler = static_cast<RecordHandler *>(Globals::recordHandler);
            switch (i) {
            case 0:
                if (recordHandler != nullptr) {
                    recordHandler->recordStoreWrite(0);
                    recordHandler->recordStoreWritePreview(0);
                }
                break;
            case 1:
                if (appData != nullptr) appData[160] = 1;
                NFC().free_credits_likeGOF2OnFacebook();
                g_hangarSocialCreditClaimed[0] = 1;
                break;
            case 2:
                if (appData != nullptr) appData[161] = 1;
                NFC().free_credits_likeFishlabsOnFacebook();
                g_hangarSocialCreditClaimed[1] = 1;
                break;
            case 3:
                if (appData != nullptr) appData[162] = 1;
                NFC().free_credits_subscribeToYoutubeChannel();
                g_hangarSocialCreditClaimed[2] = 1;
                break;
            case 4:
                if (appData != nullptr) appData[163] = 1;
                NFC().free_credits_followOnTwitter();
                g_hangarSocialCreditClaimed[3] = 1;
                break;
            default:
                break;
            }
            // The ARM reward table (dword_202844) has no recovered data object yet.
            // Do not invent a credit amount; NFC and claim-state routing are confirmed.
            return;
        }
        return;
    }

    if (this->bluePrintPurchasePending != 0) {
        const int result = this->dialog->OnTouchEnd(touch, coord);
        Item *item = this->bluePrintItem != nullptr ? this->bluePrintItem->item : nullptr;
        const int cost = item != nullptr ? item->getBlueprintAmount() * 200 : 0;
        if (result == 0 && this->localBluePrint == 0 && cost <= status->getCredits()) {
            status->changeCredits(-cost);
            this->setSellMode(false);
            resetSelection();
            this->localBluePrint = 0;
            this->bluePrintPurchasePending = 0;
        } else if (result == 1 && this->localBluePrint == 0 && cost <= status->getCredits()) {
            this->bluePrintPurchasePending = 0;
            this->dialogActive = 0;
            this->localBluePrint = 0;
        } else {
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
                message = status->replaceHash(message, String("#C"), formatCredits(status->getCredits()));
                this->dialog->set(message, true);
                this->dialogActive = 1;
            }
            this->localBluePrint = 0;
        }
        this->refreshCurrentContentHeight();
        return;
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
        return;
    }

    if (this->shipSwapPending != 0) {
        const int result = this->dialog->OnTouchEnd(touch, coord);
        if (result == 1) {
            this->shipSwapPending = 0;
            this->dialogActive = 0;
            return;
        }
        if (result != 0 || this->selectedItem == nullptr || this->selectedItem->ship == nullptr ||
            status->getShip() == nullptr || status->getStation() == nullptr) {
            return;
        }

        Ship *oldShip = status->getShip();
        Ship *shopShip = this->selectedItem->ship;
        Ship *newShip = shopShip->clone();
        Ship *returnedShip = oldShip->clone();
        if (newShip == nullptr || returnedShip == nullptr) {
            delete newShip;
            delete returnedShip;
            return;
        }
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
                newShip->addMod(mods->data()[i]);
            }
        }
        if (this->upgradeMode == 0) {
            status->changeCredits(oldShip->getPrice() - shopShip->getPrice());
        }
        Station *station = status->getStation();
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
        this->dialogActive = 0;
        resetSelection();
        return;
    }

    const int result = this->dialog->OnTouchEnd(touch, coord);
    if (this->dlcMenuPending != 0) {
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
        return;
    }

    if (this->buyMode != 0) {
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
                return;
            } else {
                resetTabs();
                return;
            }
        } else {
            return;
        }
        resetSelection();
        return;
    }

    if (result == 0) {
        this->dialogActive = 0;
        ModStation *module = stationModule();
        if (module != nullptr) {
            module->pendingHangarClose = 0;
        }
    }
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
    Array<TouchButton *> *buttons = this->tabButtons;
    for (unsigned int i = 0; i < buttons->size(); i++) {
        bool pressed = true;
        if (i != tab && !(tab == 3 && i == 0))
            pressed = (i == 2 && tab == 4);
        ((TouchButton *) (buttons->data()[i]))->setAlwaysPressed(pressed);
    }

    if (this->dragging == 0) {
        float v = this->damping * this->velocity;
        this->velocity = v;
        float mag = v > 0.0f ? v : -v;
        if (mag >= 1.0f) {
            float pos = VectorSignedToFloat(this->scrollOffset, 0);
            this->scrollOffset = (int) (v + pos);
        }
    }

    if (this->scrollOffset > 0) {
        float f = VectorSignedToFloat(-this->scrollOffset, 0);
        this->damping = 1.0f;
        this->velocity = f * 0.5f;
    }

    if (((HangarList *) this->hangarList)->getCurrentTabItems() != 0) {
        int diff = this->visibleHeight - this->currentContentHeight;
        if (diff < 0) {
            if (this->scrollOffset < diff) {
                float f = VectorSignedToFloat(diff - this->scrollOffset, 0);
                this->damping = 1.0f;
                this->velocity = f * 0.5f;
            }
        } else {
            this->velocity = 0;
            this->scrollOffset = 0;
        }
    }

    if (this->buyMode != 0) {
        void *btnUp = (*this->buttons)[(0x20) >> 2];
        void *btnDown = (*this->buttons)[(0x24) >> 2];
        if (((TouchButton *) (btnUp))->isTouched() != 0 || ((TouchButton *) (btnDown))->isTouched() != 0) {
            int t6c = this->holdTime + delta;
            int t70 = this->repeatTimer + delta;
            this->holdTime = t6c;
            this->repeatTimer = t70;
            int threshold = (t6c > 0x5dc) ? 0x1e : 200;
            if (t70 > threshold && (this->repeatTimer = 0,
                                    this->buyMode != 0 || this->specialMode != 0)) {
                if (((TouchButton *) (btnDown))->isTouched() != 0 && ((TouchButton *) (btnDown))->isVisible() != 0) {
                    int n = (this->holdTime > 4000) ? 5 : 1;
                    for (; n != 0; n--)
                        this->transaction(true);
                } else if (((TouchButton *) (btnUp))->isTouched() != 0 &&
                           ((TouchButton *) (btnUp))->isVisible() != 0) {
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
    int type = item->getType();
    if (type == 1)
        item->getAmount();

    Item *made = item->makeItem();
    Status::gStatus->getShip()->addCargo(made);

    Ship *ship = Status::gStatus->getShip();
    if (slot < 0)
        ship->freeSlot(item);
    else
        ship->freeSlot(item, slot);

    bool merged = false;
    Array<Item *> *cargo = this->itemList;
    for (unsigned int i = 0; i < cargo->size(); i++) {
        Item *cur = cargo->data()[i];
        if (cur->getIndex() == made->getIndex()) {
            cur->changeAmount(item->getAmount());
            merged = true;
            break;
        }
    }
    if (!merged)
        ArrayAdd(made, *(this->itemList));

    Status::gStatus->getShip()->setCargo(Item::extractItems(this->itemList, true));

    if (this->itemList != 0) {
        ArrayReleaseClasses(*this->itemList); ArrayRemoveAll(*(this->itemList));
        delete this->itemList;
    }
    this->itemList = 0;

    this->itemList = Item::mixItems(Status::gStatus->getShip()->getCargo(), Status::gStatus->getStation()->getItems());
    this->hangarList->initShipTab(Status::gStatus->getShip());

    ItemArray *items = Item::mixItems(Status::gStatus->getShip()->getCargo(), Status::gStatus->getStation()->getItems());
    this->hangarList->initShopTab(items, Status::gStatus->getStation()->getShips());
    this->hangarList->setCurrentTab(0, false);

    refreshCurrentContentHeight();
    this->scrollOffset = this->savedScrollOffset;
    Globals::sound->play(0x60, nullptr, nullptr, 0.0f);
}






void HangarWindow::OnTouchBegin(int touch, int coord) {
    Layout *layout = static_cast<Layout *>(Globals::layout);
    Status *status = Globals::status != nullptr ? Globals::status : Status::gStatus;
    GameText *gameText = static_cast<GameText *>(Globals::gameText);
    if (gameText == nullptr) {
        gameText = GameText::gGameText;
    }
    if (layout == nullptr || status == nullptr || gameText == nullptr || this->hangarList == nullptr) {
        return;
    }

    auto buttonAt = [this](unsigned int index) -> TouchButton * {
        if (this->buttons == nullptr || index >= this->buttons->size()) {
            return nullptr;
        }
        return this->buttons->data()[index];
    };

    this->holdTime = 0;
    this->repeatTimer = 0;
    bool handled = layout->OnTouchBegin(touch, coord) != 0;

    if (this->dialogActive != 0) {
        const unsigned int first = this->buyCreditsActive != 0 ? 12 : 18;
        const unsigned int last = this->buyCreditsActive != 0 ? 17 : 23;
        if (this->buyCreditsActive != 0 || this->freeCreditsActive != 0) {
            for (unsigned int i = first; i < last; ++i) {
                TouchButton *button = buttonAt(i);
                if (button != nullptr) {
                    button->OnTouchBegin(touch, coord);
                }
            }
            if (this->buyCreditsActive != 0) {
                TouchButton *moreButton = buttonAt(17);
                if (moreButton != nullptr) {
                    moreButton->OnTouchBegin(touch, coord);
                }
            }
        }
        if (this->dialog != nullptr) {
            this->dialog->OnTouchBegin(touch, coord);
        }
        return;
    }

    this->touchStartY = coord;
    this->lastTouchY = coord;
    this->scrollDelta = 0;
    this->dragging = 1;

    if (this->viewMode == 1) {
        if (this->listItemWindow != nullptr) {
            this->listItemWindow->OnTouchBegin(touch, coord);
        }
        return;
    }

    bool outsideList = true;
    if (layout->field_0xc < coord && coord < Globals::h - layout->field_0x10) {
        const int row = IDIV(coord - layout->field_0xc - layout->field_0x20 - this->field_0x100.d -
                                 this->scrollOffset,
                             layout->field_0x70 + this->field_0x100.d);
        if (row >= 0 && row < this->hangarList->getCurrentLength()) {
            this->hangarList->setCurrentItemIndex(row);
            this->highlightItem(this->hangarList->getCurrentItem());
            outsideList = false;

            ListItem *current = this->hangarList->getCurrentItem();
            if (this->upgradeMode != 0 && current != nullptr && current->isShip() &&
                this->hangarList->getCurrentTab() == 1) {
                Ship *ship = status->getShip();
                Station *station = status->getStation();
                if (ship != nullptr && station != nullptr && this->itemList != nullptr) {
                    ship->setCargo(Item::extractItems(this->itemList, true));
                    station->setItems(Item::extractItems(this->itemList, false), false);
                }
            }
        }
    }

    const bool mustConfirmBlueprint = this->hangarList->getCurrentTab() == 4 &&
                                      this->buyMode != 0 &&
                                      (outsideList || this->selectedItem != this->bluePrintItem) &&
                                      this->bluePrintBuyCount > 0 &&
                                      this->dialogActive == 0 && this->bluePrint != nullptr &&
                                      !this->bluePrint->isEmpty() && status->getStation() != nullptr &&
                                      this->bluePrint->getStationIndex() != status->getStation()->getIndex();
    if (mustConfirmBlueprint && this->bluePrintItem != nullptr && this->bluePrintItem->item != nullptr &&
        this->dialog != nullptr) {
        const int itemIndex = this->bluePrintItem->item->getIndex();
        this->localBluePrint = itemIndex == 209 || itemIndex == 204;
        String message = *gameText->getText(this->localBluePrint != 0 ? 289 : 288);
        if (this->localBluePrint == 0) {
            message = status->replaceHash(message, String("#S"), this->bluePrint->getStationName());
            message = status->replaceHash(message, String("#C"),
                                          Layout::formatCredits(this->bluePrintItem->item->getBlueprintAmount()));
        }
        this->dialog->set(message, this->localBluePrint == 0);
        this->dialogActive = 1;
        this->bluePrintPurchasePending = 1;
        this->suppressTouchEnd = 1;
        return;
    }

    if (this->tabButtons != nullptr) {
        for (unsigned int i = 0; i < this->tabButtons->size(); ++i) {
            TouchButton *tab = this->tabButtons->data()[i];
            if (tab != nullptr) {
                handled = tab->OnTouchBegin(touch, coord) != 0 || handled;
            }
        }
    }
    if (this->buttons != nullptr) {
        for (unsigned int i = 0; i < this->buttons->size(); ++i) {
            TouchButton *button = this->buttons->data()[i];
            if (button != nullptr) {
                button->OnTouchBegin(touch, coord);
            }
        }
    }

    if (this->autoEquipPending != 0 && this->hangarList->getCurrentTab() == 1) {
        const int index = static_cast<int>(this->autoEquipIndex);
        if (index >= 0 && (handled || index != this->hangarList->getCurrentItemIndex())) {
            this->autoEquipPending = 0;
            this->autoEquipSecondaryWeapons(index);
        }
    }
}








void HangarWindow::showCreditsBuyWindow() {
    uint8_t *appData = static_cast<uint8_t *>(ApplicationManager::gAppManager->GetApplicationData());
    appData[0x4c] = 0;
    appData[0x3d] = 1;

    String empty("");
    String text = *(String *) GameText::gGameText->getText(170);
    if (this->listModeFlag != 0) {
        this->dialog->set(empty, empty, false, empty, empty, text, -1, -1);
        if (Globals::iPad != 0) {
            this->dialog->setWidth(this->buttonWidth * 3);
            this->dialog->setHeight(static_cast<int>(static_cast<float>(this->gridButtonHeight) * 2.3f));
        } else {
            this->dialog->setWidth(Globals::w);
            this->dialog->setHeight(Globals::h);
        }
    } else {
        String spacing("\n\n\n\n\n\n\n\n");
        this->dialog->set(empty, spacing, false, empty, empty, text, -1, -1);
    }

    this->dialogActive = 1;
    this->buyCreditsActive = 1;
    this->notEnoughCredits = 0;
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
    HangarWindow *self = this;
    ListItem *item = (ListItem *) self->selectedItem;

    if (item == 0 ||
        ((ListItem *) (item))->isShip() != 0 || ((ListItem *) (item))->isSlot() != 0 ||
        ((ListItem *) (item))->isTextButton() != 0 || ((ListItem *) (item))->isSelectable() == 0 ||
        ((ListItem *) (item))->isBluePrint() != 0) {
        self->buyMode = 0;
        return;
    }

    self->buyMode = buy;

    int tab = self->hangarList->getCurrentTab();
    if (tab == 1) {
        if (self->buyMode == 0) {
            if (((ListItem *) (item))->isItem() != 0 && ((Item *) (item->field_0x10))->getType() != 4) {
                void *flags = *g_hw_itemFlags;
                if ((*(uint8_t *) ((char *) (flags) + (0x1e))) == 0) {
                    GameText::gGameText->getText(*g_hw_sellTextId1);
                    self->dialog->set(g_HangarWindow_emptyDialogText);
                    (*(uint8_t *) ((char *) (flags) + (0x1e))) = 1;
                    self->dialogActive = 1;
                }
            }
            self->autoEquipPending = 1;
            self->autoEquipIndex = self->hangarList->getCurrentItemIndex();
        } else {
            void *flags = *g_hw_itemFlags;
            if ((*(uint8_t *) ((char *) (flags) + (0x1d))) == 0) {
                GameText::gGameText->getText(*g_hw_sellTextId2);
                self->dialog->set(g_HangarWindow_emptyDialogText);
                (*(uint8_t *) ((char *) (flags) + (0x1d))) = 1;
                self->dialogActive = 1;
            }
            self->savedStationAmount = ((Item *) (item->field_0x10))->getStationAmount();
            self->savedAmount = ((Item *) (item->field_0x10))->getAmount();
            self->savedCredits = Status::gStatus->getCredits();
            self->savedLoad = self->currentLoad;
        }

        Status::gStatus->getShip()->setCargo(Item::extractItems((ItemArray *) (self->itemList), true));
        Status::gStatus->getStation()->setItems(Item::extractItems(self->itemList, false), false);
        if (self->itemList != 0) {
            ArrayReleaseClasses(*self->itemList); ArrayRemoveAll(*(self->itemList));
            delete self->itemList;
        }
        self->itemList = 0;

        ItemArray *mixed = Item::mixItems((ItemArray *) (Status::gStatus->getShip()->getCargo()),
                                          (ItemArray *) (Status::gStatus->getStation()->getItems()));
        self->itemList = mixed;
        self->hangarList->initShopTab((Array<Item *> *) (mixed), Status::gStatus->getStation()->getShips());
        self->hangarList->initShipTab(Status::gStatus->getShip());

        ListItem *ci = self->hangarList->getCurrentItem();
        self->selectedItem = ci;
        if (ci != nullptr && ci->isShip() != 0)
            self->selectedItem->ship->adjustPrice();
        refreshCurrentContentHeight();
        return;
    }

    if (self->hangarList->getCurrentTab() != 4)
        return;

    if (self->buyMode == 0) {
        if (self->bluePrintItem != 0 && self->bluePrint != 0) {
            void *bpItem = ((ListItem *) self->bluePrintItem)->item;
            self->bluePrint->addItem((Item *) bpItem, ((Item *) (bpItem))->getBlueprintAmount(),
                                     Status::gStatus->getStation()->getIndex());
        }

        uint8_t completedFlag = 0;
        if (self->bluePrint->isCompleted() != 0) {
            Globals *globals = (Globals *) *g_hw_globals;
            if (self->bluePrint->getStationIndex() == Status::gStatus->getStation()->getIndex()) {
                String line, copy, name, fmt, result;
                ((String *) &copy)->Set((line).data);
                Status_replaceHash(&result, globals, &copy, &name, &fmt);
                self->dialog->set(g_HangarWindow_emptyDialogText);

                int *stations = g_hw_bpStations;
                int idx = stations[self->bluePrint->getIndex()];
                self->bluePrint->getQuantity();
                void *made = ((Item *) ((void *) (uintptr_t) idx))->makeItem();
                Status::gStatus->getShip()->addCargo((Item *) made);
                ArrayAdd((Item *) made, *(self->itemList));
                self->hangarList->setCurrentTab(0, true);
                self->refreshCurrentContentHeight();
            } else {
                String line, copy, name, fmt, result, line2, sname, fmt2;
                ((String *) &copy)->Set((line).data);
                Status_replaceHash(&result, globals, &copy, &name, &fmt);

                ((String *) &line2)->Set((line).data);
                self->bluePrint->getStationName();
                String result2;
                Status_replaceHash(&result2, globals, &line2, &sname, &fmt2);

                self->dialog->set(g_HangarWindow_emptyDialogText);
                ((Status *) (globals))->addPendingProduct((BluePrint *) self->bluePrint);
                self->hangarList->setCurrentTab(0, true);
                self->refreshCargoAvailabilityForBlueprints();
            }
            self->bluePrint->reset();
            completedFlag = 1;
        }

        Globals *globals = Globals::gGlobals;
        ((Ship *) (Status::gStatus->getShip()))->setCargo(Item::extractItems((ItemArray *) (((Ship *) (0))->getCargo()), true));
        Status::gStatus->getShip();
        ItemArray *items = Item::mixItems((ItemArray *) (((Ship *) (0))->getCargo()),
                                          (ItemArray *) (Status::gStatus->getStation()->getItems()));
        self->hangarList->initShopTab((Array<Item *> *) (items), Status::gStatus->getStation()->getShips());
        self->hangarList->initBlueprintTab(Status::gStatus->getBluePrints());
        ItemArray *mix = Item::mixItems((ItemArray *) (((Ship *) (0))->getCargo()),
                                        (ItemArray *) (Status::gStatus->getStation()->getItems()));
        self->itemList = mix;
        self->dialogActive = completedFlag;

        if (completedFlag) {
            Array<ListItem *> *items2 = ((HangarList *) self->hangarList)->getCurrentTabItems();
            for (unsigned int i = 0; i < items2->size(); i++) {
                void *li = items2->data()[i];
                if (li != 0 && ((ListItem *) (li))->isItem() != 0 &&
                    ((Item *) ((ListItem *) li)->item)->getIndex() == self->bluePrint->getIndex()) {
                    if (Status::gStatus->getShip()->hasEquipment(((Item *) ((ListItem *) li)->item)->getIndex(), 1) != 0) {
                        self->autoEquipIndex = i;
                        self->autoEquipPending = 1;
                        self->autoEquipSecondaryWeapons(i);
                        self->autoEquipPending = 0;
                        break;
                    }
                }
            }
        }
        return;
    }

    self->bluePrintBuyCount = 0;
    if (self->bluePrint->isEmpty() != 0 && ((Item *) (item->field_0x10))->getAmount() > 0) {
        int idx = self->bluePrint->getIndex();
        bool flag;
        void *text;
        if (idx == 0xd2 || self->bluePrint->getIndex() == 0xdf) {
            if (static_cast<SolarSystem *>(Status::gStatus->getSystem())->getRoutes() != 0) {
                text = GameText::gGameText->getText(*g_hw_sellTextId2);
                flag = true;
            } else {
                self->routeWarningPending = 1;
                text = GameText::gGameText->getText(*g_hw_routesTextId);
                flag = false;
            }
        } else {
            text = GameText::gGameText->getText(*g_hw_sellTextId2);
            flag = true;
        }
        self->dialog->set(*(String *) text, flag);
        self->dialogActive = 1;
    }

    self->savedBlueprintAmount = ((Item *) (item->field_0x10))->getBlueprintAmount();
    self->savedAmount = ((Item *) (item->field_0x10))->getAmount();
    self->savedCredits = Status::gStatus->getCredits();
    self->savedLoad = self->currentLoad;
    self->bluePrintItem = self->selectedItem;
}










void HangarWindow::selectItem(ListItem *item) {
    HangarWindow *self = this;
    ListItem *li = item;
    self->selectedItem = item;
    if (item != nullptr && item->isShip() != 0)
        self->selectedItem->ship->adjustPrice();

    int tab = self->hangarList->getCurrentTab();

    if (tab == 2) {
        if (item->isSelectable() != 0 && item->isPendingProduct() == 0) {
            BluePrint *bp = li->bluePrint;
            self->bluePrint = bp;
            self->hangarList->fillIngredientsList(bp, bp != nullptr);
            self->hangarList->setCurrentTab(0, true);
            self->refreshCurrentContentHeight();
            if (self->specialMode != 0)
                self->specialMode = 0;
        }
        return;
    }

    if (tab == 1) {
        if (((ListItem *) (item))->isSelectable() == 0)
            return;
        if (((ListItem *) (item))->isShip() == 0) {
            if (((Item *) (li->field_0x10))->isUnsaleable() != 0)
                return;

            uint8_t was = self->buyMode;
            self->buyMode = (uint8_t)(was ^ 1);
            if (was == 0) {
                void *flags = *g_hw_itemFlags;
                if ((*(uint8_t *) ((char *) (flags) + (0x1d))) == 0) {
                    GameText::gGameText->getText(*g_hw_sellMsgTextId1);
                    self->dialog->set(g_HangarWindow_emptyDialogText);
                    (*(uint8_t *) ((char *) (flags) + (0x1d))) = 1;
                    self->dialogActive = 1;
                }
                self->savedStationAmount = ((Item *) (li->field_0x10))->getStationAmount();
                self->savedAmount = ((Item *) (li->field_0x10))->getAmount();
                self->savedCredits = Status::gStatus->getCredits();
                self->savedLoad = self->currentLoad;
            } else {
                if (((ListItem *) (item))->isItem() != 0 && ((Item *) (li->field_0x10))->getType() != 4) {
                    void *flags = *g_hw_itemFlags;
                    if ((*(uint8_t *) ((char *) (flags) + (0x1e))) == 0) {
                        GameText::gGameText->getText(*g_hw_sellMsgTextId2);
                        self->dialog->set(g_HangarWindow_emptyDialogText);
                        (*(uint8_t *) ((char *) (flags) + (0x1e))) = 1;
                        self->dialogActive = 1;
                    }
                }
                int li = ((ListItem *) (item))->getIndex();
                if (li > 0x83 && ((ListItem *) (item))->getIndex() < 0x9a) {
                    int base = ((Globals *) *g_hw_globals)->field_0xac;
                    *((uint8_t *) (*(int *) ((char *) ((void *) (uintptr_t) base) + (4))) + ((ListItem *) (item))->
                      getIndex() - 0x84) = 1;
                }
                self->autoEquipPending = 1;
                self->autoEquipIndex = self->hangarList->getCurrentItemIndex();

                Status::gStatus->getShip()->setCargo(Item::extractItems((ItemArray *) (self->itemList), true));
                Status::gStatus->getStation()->setItems(Item::extractItems(self->itemList, false), false);
                if (self->itemList != 0) {
                    ArrayReleaseClasses(*self->itemList); ArrayRemoveAll(*(self->itemList));
                    delete self->itemList;
                }
                self->itemList = 0;

                ItemArray *mixed = Item::mixItems((ItemArray *) (Status::gStatus->getShip()->getCargo()),
                                                  (ItemArray *) (Status::gStatus->getStation()->getItems()));
                self->itemList = mixed;
                self->hangarList->initShopTab((Array<Item *> *) (mixed), Status::gStatus->getStation()->getShips());
                self->hangarList->initShipTab(Status::gStatus->getShip());
            }
            return;
        }

        int price = ((Ship *) (li->ship))->getPrice();
        Globals *globals = (Globals *) *g_hw_globals;
        int credits = Status::gStatus->getCredits();
        int oldPrice = Status::gStatus->getShip()->getPrice();
        if (oldPrice + credits < price && self->upgradeMode == 0) {
            String line, priceStr, fmt, msg, suffix, combined;
            priceStr = Layout::formatCredits(
                ((ListItem *) (item))->getPrice() - Status::gStatus->getCredits() - Status::gStatus->getShip()->getPrice());
            ((String *) &line)->Set((priceStr).data);
            Status_replaceHash(&msg, globals, &line, &priceStr, &fmt);
            GameText::gGameText->getText(*g_hw_notEnoughTextId);
            combined = suffix + suffix;
            *((String *) &msg) += combined;
            self->dialog->set(*(String *) &msg, true);
            self->dialogActive = 1;
            self->notEnoughCredits = 1;
            return;
        }

        if (globals->field_0x34 < 1) {
            if (Status::gStatus->getCurrentCampaignMission() == 0x4d &&
                Status::gStatus->getShip()->getIndex() == 0x25) {
                GameText::gGameText->getText(*g_hw_unsaleableTextId);
                self->dialog->set(g_HangarWindow_emptyDialogText);
                self->dialogActive = 1;
                return;
            }
            int a = Status::gStatus->getShip()->getIndex();
            int b = ((Ship *) (li->ship))->getIndex();
            if (a != b) {
                self->shipSwapPending = 1;
                GameText::gGameText->getText(*g_hw_sellMsgTextId2);
                self->dialog->set(*(String *) GameText::gGameText->getText(*g_hw_sellMsgTextId2), true);
                self->dialogActive = 1;
                return;
            }
            GameText::gGameText->getText(*g_hw_buyBaseTextId);
            self->dialog->set(g_HangarWindow_emptyDialogText);
            self->dialogActive = 1;
        } else {
            GameText::gGameText->getText(*g_hw_buyBaseTextId);
            self->dialog->set(g_HangarWindow_emptyDialogText);
            self->dialogActive = 1;
        }
        return;
    }

    if (tab != 0) {
        return;
    }

    if (((ListItem *) (item))->isSelectable() == 0)
        return;

    self->scrollOffsetBackup = 0;
    self->savedScrollOffset = self->scrollOffset;
    self->scrollOffset = 0;

    void *flags0 = *g_hw_itemFlags;
    if ((*(uint8_t *) ((char *) (flags0) + (0x1f))) == 0 && ((ListItem *) (item))->isSlot() != 0) {
        GameText::gGameText->getText(*g_hw_slotMsgTextId);
        self->dialog->set(g_HangarWindow_emptyDialogText);
        (*(uint8_t *) ((char *) (flags0) + (0x1f))) = 1;
        self->dialogActive = 1;
    }

    if (((ListItem *) (item))->isSelectable() == 0)
        return;

    ListItem *cur = self->hangarList->getCurrentItem();
    Item *curItem = cur->item;
    if (curItem != nullptr) {
        if (curItem->isUnsaleable() != 0) {
            GameText::gGameText->getText(*g_hw_unsaleableTextId);
            self->dialog->set(g_HangarWindow_emptyDialogText);
            self->dialogActive = 1;
            return;
        }
        if (((Item *) (curItem))->getSort() == 0x14) {
            Globals *globals = (Globals *) *g_hw_globals;
            int passengerCount = globals->field_0x34;
            int chosen = passengerCount;
            if (passengerCount > 0)
                chosen = li->field_0x3c;
            int adj = passengerCount - 1;
            if (passengerCount >= 1)
                adj = chosen;
            if (adj >= 0 == (passengerCount >= 1)) {
                int maxPax = Status::gStatus->getShip()->getMaxPassengers();
                int used = ((Item *) (curItem))->getAttribute(0);
                if (maxPax - used < globals->field_0x34) {
                    GameText::gGameText->getText(*g_hw_unsaleableTextId);
                    self->dialog->set(g_HangarWindow_emptyDialogText);
                    self->dialogActive = 1;
                    return;
                }
            }
        }
    }

    if (li->field_0x3c >= 0) {
        ListItem *ci = self->hangarList->getCurrentItem();
        self->demountItem(curItem, ci->field_0x40);
        return;
    }

    Globals *globals = (Globals *) *g_hw_globals;
    int sort = li->field_0x10->getSort();
    Item *existing = Status::gStatus->getShip()->getFirstEquipmentOfSort(sort);
    bool conflict = false;
    if (((Item *) (li->field_0x10))->getSort() == 0x15) {
        if (Status::gStatus->getShip()->getIndex() != 0x2c &&
            existing != 0 && Status::gStatus->getShip()->getIndex() != 0x31)
            conflict = true;
    } else if (existing != 0) {
        conflict = true;
    }

    if (conflict && li->field_0x10->canBeInstalledMultipleTimes() == 0) {
        String name, copy, etext, fmt, result, etext2, fmt2, result2;
        ((String *) &copy)->Set((name).data);
        ((Item *) (existing))->getIndex();
        Status_replaceHash(&result, globals, &copy, &etext, &fmt);
        ((Item *) (li->field_0x10))->getIndex();
        Status_replaceHash(&result2, globals, &result, &etext2, &fmt2);
        self->dialog->set(*(String *) &name, true);
        self->replaceEquipPending = 1;
        self->dialogActive = 1;
        self->pendingMountItem = li->field_0x10;
        self->pendingDemountItem = existing;
        return;
    }

    self->mountItem(li->field_0x10);
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
    unsigned int tab = this->hangarList->getCurrentTab();
    Item *cur = this->selectedItem == nullptr ? nullptr : this->selectedItem->item;
    if (cur == nullptr) {
        return;
    }
    Status *status = Status::gStatus;

    if (tab < 2) {
        if (cur->isUnsaleable() != 0) {
            this->dialog->set(*(String *) GameText::gGameText->getText(323));
            this->buyMode = 0;
            this->dialogActive = 1;
            return;
        }

        int result = cur->transaction(buy, this->currentLoad, this->upgradeMode);
        unsigned int idx = cur->getIndex();

        // Android Status+0x54 is the item-discovery/availability array.
        if (status->field_54 != nullptr && idx < status->field_54->size()) {
            (*status->field_54)[idx] = true;
        }

        if (result < 0 && buy) {
            this->currentLoad = this->currentLoad + 1;
            status->getShip()->changeLoad(1);
            int li = ((ListItem *) (this->selectedItem))->getIndex();
            if (li >= 0x84 && ((ListItem *) (this->selectedItem))->getIndex() < 0x9a) {
                unsigned int specialIndex = static_cast<unsigned int>(li - 0x84);
                if (status->field_ac != nullptr && specialIndex < status->field_ac->size()) {
                    (*status->field_ac)[specialIndex] = true;
                }
            }
        } else if (result == 0 && buy) {
            if (status->getCredits() < cur->getSinglePrice()) {
                if (this->upgradeMode != 0)
                    return;
                String line, priceStr, fmt, msg, suffix, combined;
                priceStr = Layout::formatCredits(cur->getSinglePrice());
                ((String *) &line)->Set((priceStr).data);
                Status_replaceHash(&msg, Globals::gGlobals, &line, &priceStr, &fmt);
                GameText::gGameText->getText(203);
                combined = suffix + suffix;
                *((String *) &msg) += combined;
                this->dialog->set(*(String *) &msg, true);
                this->dialogActive = 1;
                this->notEnoughCredits = 1;
                (*this->buttons)[(0x20) >> 2]->resetTouch();
                (*this->buttons)[(0x24) >> 2]->resetTouch();
            }
        } else if (result > 0 && !buy) {
            this->currentLoad = this->currentLoad - 1;
            status->getShip()->changeLoad(-1);
        }

        if (this->upgradeMode == 0)
            status->changeCredits(result);
    } else if (tab == 4) {
        if (buy) {
            int bpAmt = cur->getBlueprintAmount();
            void *bp = this->bluePrint;
            int remaining = ((BluePrint *) (bp))->getRemainingAmount(cur->getIndex());
            if (bpAmt < remaining) {
                int r = cur->transactionBlueprint(false, this->currentLoad);
                if (r < 0) {
                    this->currentLoad = this->currentLoad + 1;
                } else if (r != 0) {
                    this->bluePrintBuyCount = this->bluePrintBuyCount + 1;
                    status->getShip()->changeLoad(-1);
                }
            }
        }
        void *cargo = status->getShip()->getCargo();
        if (cargo != 0) {
            Array<void *> *arr = (Array<void *> *) cargo;
            for (unsigned int i = 0; i < arr->size(); i++) {
                if (((Item *) (arr->data()[i]))->getIndex() == cur->getIndex()) {
                    ((Item *) (arr->data()[i]))->setAmount(cur->getAmount());
                    ((Item *) (arr->data()[i]))->setBlueprintAmount(cur->getBlueprintAmount());
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

    Item *made = item->makeItem();
    Ship *ship = Status::gStatus->getShip();
    ship->addEquipment(made);
    Status::gStatus->getShip()->removeCargo(made->getIndex(), (type == 1) ? amount : 1);

    Array<Item *> *cargo = this->itemList;
    if (cargo != nullptr) {
        for (unsigned int i = 0; i < cargo->size(); i++) {
            Item *cur = cargo->data()[i];
            if (cur->getIndex() == item->getIndex()) {
                int change;
                if (cur->getStationAmount() == 0) {
                    if (type == 1 || item->getAmount() == 1) {
                        this->itemList->erase(std::find(this->itemList->begin(), this->itemList->end(), cur));
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

    Status::gStatus->getShip()->setCargo(Item::extractItems(this->itemList, true));
    this->hangarList->initShipTab(Status::gStatus->getShip());

    ItemArray *items = Item::mixItems(Status::gStatus->getShip()->getCargo(), Status::gStatus->getStation()->getItems());
    this->hangarList->initShopTab(items, Status::gStatus->getStation()->getShips());
    this->hangarList->setCurrentTab(0, false);

    refreshCurrentContentHeight();
    this->scrollOffset = this->savedScrollOffset;
    Globals::sound->play(0x62, nullptr, nullptr, 0.0f);
}




unsigned int HangarWindow::OnTouchMove(int touch, int coord) {
    Layout *layout = static_cast<Layout *>(Globals::layout);
    ((Layout *) (layout))->OnTouchMove(touch, coord);

    if (this->dialogActive != 0) {
        if (this->buyCreditsActive != 0) {
            for (int i = 0xc; i != 0x11; i++)
                (*this->buttons)[(i * 4) >> 2]->OnTouchMove(touch, coord);
            (*this->buttons)[(0x44) >> 2]->OnTouchMove(touch, coord);
        } else if (this->freeCreditsActive != 0) {
            for (int i = 0x12; i != 0x17; i++)
                (*this->buttons)[(i * 4) >> 2]->OnTouchMove(touch, coord);
        }
        this->dialog->OnTouchMove(touch, coord);
        return 0;
    }

    if (this->viewMode == 1) {
        this->listItemWindow->OnTouchMove(touch, coord);
        return 0;
    }

    if (layout->field_0xc < coord && coord < Globals::h - layout->field_0x10) {
        int dy = coord - this->lastTouchY;
        this->scrollDelta = dy;
        this->damping = 1.0f;
        this->scrollOffset = this->scrollOffset + dy;
        this->lastTouchY = coord;

        void *btnUp = (*this->buttons)[(0x20) >> 2];
        void *btnDown = (*this->buttons)[(0x24) >> 2];
        int touched = ((TouchButton *) (btnUp))->isTouched();
        if (touched != 0)
            touched = 1;
        else
            touched = ((TouchButton *) (btnDown))->isTouched();

        int adist = coord - this->touchStartY;
        if (adist < 0)
            adist = -adist;

        if (touched == 0 && adist > 5) {
            this->holdTime = 0;
            this->repeatTimer = 0;
            Array<TouchButton *> *buttons = this->buttons;
            for (unsigned int i = 0; i < buttons->size(); i++)
                buttons->data()[i]->OnTouchMove(touch, coord);
            this->setSellMode(false);
            this->sellConfirmPending = 0;
            this->selectedItem = 0;
            ((TouchButton *) (btnUp))->resetTouch();
            ((TouchButton *) (btnDown))->resetTouch();
        }
    }

    Array<TouchButton *> *tabs = this->tabButtons;
    for (unsigned int i = 0; i < tabs->size(); i++)
        ((TouchButton *) (tabs->data()[i]))->OnTouchMove(touch, coord);
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
        String message = Status::gStatus->replaceHash(messageTemplate, String("#N"), itemName);
        this->dialog->set(message);
        this->autoEquipped = 1;
        this->dialogActive = 1;
        return;
    }
}




void HangarWindow::showFreeCreditsWindow() {
    uint8_t *appData = static_cast<uint8_t *>(ApplicationManager::gAppManager->GetApplicationData());
    appData[0x4c] = 0;
    appData[0x3d] = 1;

    String empty("");
    String text = *(String *) GameText::gGameText->getText(170);
    this->dialog->set(empty, empty, false, empty, empty, text, -1, -1);

    TouchButton *creditButton = (*this->buttons)[18];
    this->dialog->setHeight(creditButton->getHeight() * 5);

    int maxTextWidth = 0;
    for (int i = 0; i < 5; ++i) {
        int textId = i == 0 ? 3400 : 112 + i;
        String line = *(String *) GameText::gGameText->getText(textId);
        int textWidth = PaintCanvas::gCanvas->GetTextWidth(
            static_cast<unsigned int>(reinterpret_cast<uintptr_t>(Globals::font)), line);
        if (textWidth > maxTextWidth) {
            maxTextWidth = textWidth;
        }
    }
    Layout *layout = static_cast<Layout *>(Globals::layout);
    this->dialog->setWidth(layout->field_0x2c + creditButton->getWidth() + maxTextWidth + layout->field_0x28 * 4);

    this->freeCreditsActive = 1;
    this->dialogActive = 1;
    this->buyCreditsActive = 0;
    this->notEnoughCredits = 0;
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
    ArraySetLength(0x18, *(self->buttons));

    unsigned int img;
    img = 0xffffffff;
    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x470), img);
    void *e0 = ::operator new(200);
    TouchButton_ctor_img((void *) e0, (void *) (uintptr_t) img, 7, 0, 0, layout->field_0x60, 0x11, 4);
    (*self->buttons)[(0) >> 2] = (TouchButton *) (e0);

    bool deepScienceCampaign = status->getCurrentCampaignMission() == 0x4d && status->getStation()->getIndex() == 100;

    void *e1 = ::operator new(200);
    TouchButton_ctor_text(e1, GameText::gGameText->getText((self->upgradeMode != 0 || deepScienceCampaign) ? 332 : 301),
                          7, 0, 0, 0x11);
    (*self->buttons)[(4) >> 2] = (TouchButton *) (e1);
    void *e2 = ::operator new(200);
    TouchButton_ctor_text(e2, GameText::gGameText->getText(302), 7, 0, 0, 0x11);
    (*self->buttons)[(8) >> 2] = (TouchButton *) (e2);

    img = 0xffffffff;
    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x533), img);
    void *e3 = ::operator new(200);
    TouchButton_ctor_img((void *) e3, (void *) (uintptr_t) img, 7, 0, 0, layout->field_0x64, 0x11, 4);
    (*self->buttons)[(0xc) >> 2] = (TouchButton *) (e3);

    img = 0xffffffff;
    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x532), img);
    void *e4 = ::operator new(200);
    TouchButton_ctor_img((void *) e4, (void *) (uintptr_t) img, 7, 0, 0, layout->field_0x64, 0x11, 4);
    (*self->buttons)[(0x10) >> 2] = (TouchButton *) (e4);

    void *e5 = ::operator new(200);
    TouchButton_ctor_text2(e5, GameText::gGameText->getText(279), 7, 0, 0, self->buttonHeight, 0x11);
    (*self->buttons)[(0x14) >> 2] = (TouchButton *) (e5);
    void *e6 = ::operator new(200);
    TouchButton_ctor_text2(e6, GameText::gGameText->getText(282), 7, 0, 0, self->buttonHeight, 0x11);
    (*self->buttons)[(0x18) >> 2] = (TouchButton *) (e6);
    void *e7 = ::operator new(200);
    TouchButton_ctor_text(e7, GameText::gGameText->getText(283), 7, 0, 0, 0x11);
    (*self->buttons)[(0x1c) >> 2] = (TouchButton *) (e7);

    {
        String lbl;
        void *e8 = ::operator new(200);
        TouchButton_ctor_img((void *) e8, &lbl, 8, 0, 0, layout->field_0x50, 0x11, 4);
        (*self->buttons)[(0x20) >> 2] = (TouchButton *) (e8);
    }
    {
        String lbl;
        void *e9 = ::operator new(200);
        TouchButton_ctor_img((void *) e9, &lbl, 9, 0, 0, layout->field_0x50, 0x11, 4);
        (*self->buttons)[(0x24) >> 2] = (TouchButton *) (e9);
    }
    {
        void *e10 = ::operator new(200);
        TouchButton_ctor_img((void *) e10, GameText::gGameText->getText(330), 7, 0, 0,
                             layout->field_0x50, 0x11, 4);
        (*self->buttons)[(0x28) >> 2] = (TouchButton *) (e10);
    }
    {
        String credits;
        void *e11 = ::operator new(200);
        credits = Layout::formatCredits(status->getCredits());
        TouchButton_ctor_img((void *) e11, &credits, 0xb, Globals::w, Globals::h,
                             layout->getFooterTransitionWidth(), 0x22, 4);
        (*self->buttons)[(0x2c) >> 2] = (TouchButton *) (e11);
    }

    uint8_t listMode = self->listModeFlag;
    int row = 0;
    for (int slot = 0xc; (unsigned int) (slot - 0xc) < 5; slot++) {
        String lbl;
        void *btn = ::operator new(200);
        int visIdx;
        if (listMode == 0) {
            TouchButton_ctor_img(btn, &lbl, 0, 0, 0, layout->field_0x264, 0x11, 1);
            visIdx = row + 0xc;
        } else {
            TouchButton_ctor_text(btn, &lbl, 10, 0, 0, 1);
            visIdx = slot;
        }
        (*self->buttons)[(slot * 4) >> 2] = (TouchButton *) (btn);
        (*self->buttons)[(visIdx * 4) >> 2]->setVisible(false);
        row++;
    }
    {
        String lbl;
        void *btn = ::operator new(200);
        if (listMode == 0)
            TouchButton_ctor_img(btn, &lbl, 0, 0, 0, layout->field_0x264, 0x11, 1);
        else
            TouchButton_ctor_text(btn, &lbl, 10, 0, 0, 1);
        (*self->buttons)[(0x44) >> 2] = (TouchButton *) (btn);
        (*self->buttons)[(0x44) >> 2]->setVisible(false);
    }

    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x233e), self->scrollHintImageA);
    PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x233f), self->scrollHintImageB);
    {
        String lbl;
        void *btn = ::operator new(200);
        TouchButton_ctor_text(btn, &lbl, 7, 0, 0, 0x11);
        (*self->buttons)[(0x5c) >> 2] = (TouchButton *) (btn);
    }

    unsigned int imgA, imgB;
    for (int i = 0x12; (unsigned int) (i - 0x12) < 5; i++) {
        imgB = 0xffffffff;
        if (i == 0x12) {
            PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x233c), imgA);
            PaintCanvas::gCanvas->Image2DCreate((unsigned short) (0x233d), imgB);
        } else {
            short s = (short) (i - 0x12);
            PaintCanvas::gCanvas->Image2DCreate((unsigned short) (s * 2 + 0x2330), imgA);
            PaintCanvas::gCanvas->Image2DCreate((unsigned short) (s * 2 + 0x2331), imgB);
        }
        void *btn = ::operator new(200);
        TouchButton_ctor_img2(btn, (void *) (uintptr_t) imgA, (void *) (uintptr_t) imgB, 0x13, 0, 0, 1);
        (*self->buttons)[(i * 4) >> 2] = (TouchButton *) (btn);
        (*self->buttons)[(i * 4) >> 2]->setVisible(false);
    }

    self->buttonWidth = (*self->buttons)[12]->getWidth();
    int h = (*self->buttons)[12]->getHeight();
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

    self->field_0xc1 = 0;
    self->field_0xc5 = 0;
    self->field_0xc9 = 0;
    self->field_0xcd = 0;
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
    // Android constructor at 0x00147d20: all interaction state starts empty,
    // then the four row-layout values are copied from Layout.
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
    this->scrollOffset = 0;
    this->lastTouchY = 0;
    this->scrollOffsetBackup = 0;
    this->scrollDelta = 0;
    this->field_0xc1 = 0;
    this->damping = 0.0f;
    this->field_0xc5 = 0;
    this->velocity = 0.0f;
    this->field_0xc9 = 0;
    this->touchStartY = 0;
    this->field_0xcd = 0;
    this->dragging = 0;
    this->suppressTouchEnd = 0;
    this->sellConfirmPending = 0;
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
        this->field_0x100 = layout->field_0x238_blk16;
        this->buttonHeight = layout->field_0x248;
        this->field_0x114 = layout->field_0x24c;
        this->iconOffsetY = layout->field_0x250;
    }
}

bool HangarWindow::isInitialized() {
    return this->active != 0;
}

ListItem *HangarWindow::getCurrentItem() {
    return this->hangarList == nullptr ? nullptr : this->hangarList->getCurrentItem();
}

// Static data members present in the original binary (defined for symbol parity).
int HangarWindow::lastTab;
