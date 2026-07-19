#include "game/ui/MenuTouchWindow.h"
#include "game/menu/ModStation.h"
#include "engine/core/AbyssEngine.h"
#include "game/ui/TouchSlider.h"
#include "game/ui/TouchButton.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/Engine.h"
#include "game/mission/GameRecord.h"
#include "game/mission/Mission.h"
#include "game/mission/Achievements.h"
#include "game/ui/Layout.h"
#include "game/mission/Status.h"
#include "game/mission/RecordHandler.h"
#include "engine/audio/FModSound.h"
#include "engine/core/ApplicationManager.h"
#include "engine/core/GameText.h"
#include "engine/render/ImageFactory.h"
#include "game/core/Globals.h"
#include "game/ui/ChoiceWindow.h"
#include "game/ui/MissionsWindow.h"
#include "game/ui/ScrollTouchWindow.h"
#include "game/ship/ShipDefTable.h"
#include "game/core/GameSettings.h"
#include "engine/core/NFC.h"
#include <math.h>
#include <cstddef>
#include <cstring>

// Minimal byte-faithful model of the untyped handle returned by
// _mtw_AppMgr_GetApplicationData(). Only the fields this translation unit
// touches are named; the rest is padding to keep offsets exact.
struct MtwAppData {
    uint8_t pad_0x0[5];
    uint8_t screenshotResultFlag;   // 0x5
    uint8_t pad_0x6[2];
    int storeResultCode;            // 0x8
    uint8_t storeInitFlag;          // 0xc
    uint8_t rateGameFlagA;          // 0xd
    uint8_t rateGameFlagB;          // 0xe
    uint8_t pad_0xf[0x2e];
    uint8_t dlcMenuRequestFlag;     // 0x3d
    uint8_t pad_0x3e[2];
    uint8_t purchaseReadyFlag;      // 0x40
    uint8_t purchaseResultFlag;     // 0x41
    uint8_t purchaseErrorFlag;      // 0x42
    uint8_t pad_0x43[5];
    unsigned int purchaseCode;      // 0x48
    uint8_t dlcMenuAckFlag;         // 0x4c
};
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(MtwAppData, screenshotResultFlag) == 0x5, "appdata 0x5");
static_assert(offsetof(MtwAppData, storeResultCode) == 0x8, "appdata 0x8");
static_assert(offsetof(MtwAppData, storeInitFlag) == 0xc, "appdata 0xc");
static_assert(offsetof(MtwAppData, rateGameFlagA) == 0xd, "appdata 0xd");
static_assert(offsetof(MtwAppData, rateGameFlagB) == 0xe, "appdata 0xe");
static_assert(offsetof(MtwAppData, dlcMenuRequestFlag) == 0x3d, "appdata 0x3d");
static_assert(offsetof(MtwAppData, purchaseReadyFlag) == 0x40, "appdata 0x40");
static_assert(offsetof(MtwAppData, purchaseResultFlag) == 0x41, "appdata 0x41");
static_assert(offsetof(MtwAppData, purchaseErrorFlag) == 0x42, "appdata 0x42");
static_assert(offsetof(MtwAppData, purchaseCode) == 0x48, "appdata 0x48");
static_assert(offsetof(MtwAppData, dlcMenuAckFlag) == 0x4c, "appdata 0x4c");
#endif

static inline void *_mtw_GameText_getText(void *gt, int id) { return ((GameText *) gt)->getText(id); }

static inline void _mtw_ChoiceWindow_set(void *cw, void *s1, void *s2, bool b) {
    if (cw && s1 && s2) ((ChoiceWindow *) cw)->set(*(const String *) s1, *(const String *) s2, b);
}

void _mtw_render3D_inner(void *obj);

static inline int _mtw_Layout_OnTouchEnd(void *layout, int y, int x) {
    return ((Layout *) layout)->OnTouchEnd(x, y);
}

static inline int _mtw_ChoiceWindow_OnTouchEnd(void *cw, int y, int x = 0) {
    return cw ? ((ChoiceWindow *) cw)->OnTouchEnd(x, y) : 0;
}

static inline void _mtw_ChoiceWindow_set(void *cw, void *s, bool b) {
    if (cw && s) ((ChoiceWindow *) cw)->set(*(const String *) s, b);
}

static inline void _mtw_ChoiceWindow_set(void *cw, void *s) {
    if (cw && s) ((ChoiceWindow *) cw)->set(*(const String *) s);
}

static inline void _mtw_FModSound_resumeAll(void *snd) { ((FModSound *) snd)->resumeAll(); }

static inline void _mtw_FModSound_stopAll() { Globals::sound->stopAll(); }

static inline void *_mtw_AppMgr_GetEngine();

void *_mtw_AppMgr_GetApplicationData();

static inline uint8_t &_mtw_option_byte(size_t offset) {
    return *(Globals::options + offset);
}

static inline float _mtw_option_float(size_t offset) {
    float value;
    std::memcpy(&value, Globals::options + offset, sizeof(value));
    return value;
}

static inline void _mtw_option_set_float(size_t offset, float value) {
    std::memcpy(Globals::options + offset, &value, sizeof(value));
}

static inline Array<TouchSlider *> *_mtw_sliders(MenuTouchWindow *window) {
    return (Array<TouchSlider *> *) window->sliders;
}

static inline TouchSlider *_mtw_slider(MenuTouchWindow *window, unsigned int idx) {
    Array<TouchSlider *> *sliders = _mtw_sliders(window);
    if (sliders == nullptr || idx >= sliders->size()) return nullptr;
    return (*sliders)[idx];
}

static inline void _mtw_options_sync_slider_values(MenuTouchWindow *window) {
    if (TouchSlider *slider = _mtw_slider(window, 1))
        _mtw_option_set_float(0x00, slider->getValue());
    if (TouchSlider *slider = _mtw_slider(window, 2))
        _mtw_option_set_float(0x04, slider->getValue());
    if (TouchSlider *slider = _mtw_slider(window, 3))
        _mtw_option_set_float(0x08, slider->getValue());
    if (TouchSlider *slider = _mtw_slider(window, 4))
        _mtw_option_set_float(0x2c, slider->getValue());
    if (TouchSlider *slider = _mtw_slider(window, 5))
        _mtw_option_set_float(0x44, slider->getValue());
}

static inline void _mtw_options_set_category(MenuTouchWindow *window,
                                             int category, size_t flagOffset, unsigned int sliderIdx, bool enabled) {
    _mtw_option_byte(flagOffset) = enabled ? 1 : 0;
    if (auto *btn = (TouchButton *) ((category == 1) ? window->optBtnD4 :
                                     (category == 2) ? window->optBtnD8 : window->optBtnDC))
        btn->setAlwaysPressed(enabled);
    if (TouchSlider *slider = _mtw_slider(window, sliderIdx))
        slider->setHalfTransparent(!enabled);
    if (Globals::sound != nullptr)
        Globals::sound->enableCategory(category, enabled);
}

static inline void _mtw_options_update_accel_from_engine() {
    auto *engine = (AbyssEngine::Engine *) _mtw_AppMgr_GetEngine();
    if (engine == nullptr) return;

    double *acc = engine->GetAccelValue();
    double x = acc[0];
    double z = acc[2];
    if (z > 0.0) {
        if (x > 1.0) {
            acc[0] = 0.0;
            acc[1] = 1.0;
        }
        x = (1.0 - engine->GetAccelValue()[0]) + 1.0;
    }
    _mtw_option_set_float(0x1c, (float) x);
    _mtw_option_set_float(0x20, (float) engine->GetAccelValue()[2]);
}

static inline void _mtw_AppMgr_SetCurrentApplicationModule(void *app, int id) {
    ((AbyssEngine::ApplicationManager *) app)->SetCurrentApplicationModule(id);
}

static inline void _mtw_AppMgr_Quit(void *app) { ((AbyssEngine::ApplicationManager *) app)->Quit(); }

void _mtw_Globals_reportLeaderboards();

extern int g_android_link_game_gp;

static constexpr int kMtwLanguageButtonIds[10] = {27, 28, 30, 29, 31, 32, 33, 35, 38, 39};
static constexpr int kMtwLanguageTextIds[10] = {2, 1, 4, 3, 5, 7, 8, 14, 12, 13};
static constexpr int kMtwLanguageByButtonOffset[13] = {0, 1, 4, 2, 3, 6, 5, 9, 12, 11, 10, 14, 15};
static constexpr unsigned short kMtwStoreImagePairs[3][2] = {
    {9500, 9501},
    {9504, 9505},
    {9502, 9503},
};

static inline String _mtw_text_copy(int textId);
static inline void _mtw_apply_ipad_control_coords(MenuTouchWindow *window);

static inline GameRecord *_mtw_selectedPreviewRecord(MenuTouchWindow *window) {
    auto *records = (Array<GameRecord *> *) window->previewRecords;
    int idx = window->selectedRow;
    if (records == nullptr || idx < 0 || (unsigned int) idx >= records->size()) return nullptr;
    return records->data_[idx];
}

static inline int _mtw_onTouchEnd_listTail(MenuTouchWindow *window, int y, int x) {
    if ((unsigned int) (window->menuState - 1) <= 1) {
        int drag = window->dragVelocity;
        int nextOffset = window->scrollOffset + drag;
        int magnitude = drag < 0 ? -drag : drag;
        window->inertiaDecay = 1063675494;
        window->dragging = 0;
        window->scrollOffset = nextOffset;
        *(int *) ((uint8_t *) window + 0x210) = nextOffset;
        window->inertiaVel = magnitude > 3 ? (float) drag : 0.0f;
    }

    if (((Layout *) Globals::layout)->OnTouchEnd(x, y) == 0) return 0;

    switch (window->menuState) {
        case 0:
            return 1;
        case 3:
            ((RecordHandler *) Globals::recordHandler)->saveOptions();
            window->menuState = 0;
            break;
        case 6:
        case 7:
        case 8:
        case 14:
            window->menuState = Globals::iPad ? 0 : 3;
            break;
        case 11:
            return 0;
        case 12:
            window->menuState = 17;
            break;
        default:
            window->menuState = 0;
            break;
    }
    return 0;
}

static inline int _mtw_onTouchEnd_listState(void *self, int y, int x, int state) {
    auto *window = (MenuTouchWindow *) self;
    auto *choice = (ChoiceWindow *) window->choiceWindow;
    auto *raw = (uint8_t *) window;

    if (state == 1) {
        if (window->messageShowing != 0) {
            if (window->listStateSuppress != 0) return 0;
            int choiceResult = _mtw_ChoiceWindow_OnTouchEnd(choice, y, x);
            if (raw[0x1da] != 0 ||
                (choiceResult == 0 && choice->hasChoice() != 0 && window->loadGame(window->selectedRow) == 0)) {
                return 0;
            }
            window->messageShowing = 0;
        }

        auto *ok = (TouchButton *) window->okButton;
        if (ok == nullptr || ok->OnTouchEnd(x, y) == 0)
            return _mtw_onTouchEnd_listTail(window, y, x);
        if (_mtw_selectedPreviewRecord(window) == nullptr)
            return 0;

        if (Globals::status->getPlayingTime() >= 1) {
            _mtw_ChoiceWindow_set(choice, _mtw_GameText_getText(Globals::gameText, 51), true);
            window->messageShowing = 1;
            return 0;
        }

        Globals::sound->play(0x7b, nullptr, nullptr, 0.0f);
        window->loadGame(window->selectedRow);
        return 0;
    }

    if (state == 2) {
        if (window->messageShowing == 0) {
            auto *ok = (TouchButton *) window->okButton;
            if (ok == nullptr || ok->OnTouchEnd(x, y) == 0)
                return _mtw_onTouchEnd_listTail(window, y, x);

            int idx = window->selectedRow;
            if (idx == 0) {
                _mtw_ChoiceWindow_set(choice, _mtw_GameText_getText(Globals::gameText, 487));
                window->messageShowing = 1;
                return 0;
            }

            if (_mtw_selectedPreviewRecord(window) != nullptr) {
                _mtw_ChoiceWindow_set(choice, _mtw_GameText_getText(Globals::gameText, 49), true);
                window->saveDialogShowing = 1;
                window->messageShowing = 1;
                return 0;
            }

            Globals::sound->play(0x7b, nullptr, nullptr, 0.0f);
            window->saveGame(idx);
            return 0;
        }

        if (window->listStateSuppress != 0 || raw[0x1da] != 0)
            return 0;

        bool confirmOverwrite = window->saveDialogShowing != 0;
        int choiceResult = _mtw_ChoiceWindow_OnTouchEnd(choice, y, x);
        if (!confirmOverwrite) {
            if (choiceResult != 0) return 0;
            window->messageShowing = 0;
            return 0;
        }

        if (choiceResult == 1) {
            window->saveDialogShowing = 0;
            window->messageShowing = 0;
            return 0;
        }
        if (choiceResult == 0) {
            window->saveGame(window->selectedRow);
            window->saveDialogShowing = 0;
        }
    }

    return 0;
}

static inline int _mtw_onTouchEnd_optionsState(void *self, int y, int x) {
    auto *window = (MenuTouchWindow *) self;
    auto *choice = (ChoiceWindow *) window->choiceWindow;
    auto *raw = (uint8_t *) window;
    int state = window->menuState;

    if (state == 7) {
        if (window->messageShowing != 0 && _mtw_ChoiceWindow_OnTouchEnd(choice, y, x) == 0) {
            window->messageShowing = 0;
            return _mtw_onTouchEnd_listTail(window, y, x);
        }

        if (window->optBtnD4 != nullptr && ((TouchButton *) window->optBtnD4)->OnTouchEnd(x, y) != 0)
            _mtw_options_set_category(window, 1, 0x0d, 1, _mtw_option_byte(0x0d) == 0);
        if (window->optBtnD8 != nullptr && ((TouchButton *) window->optBtnD8)->OnTouchEnd(x, y) != 0) {
            _mtw_options_set_category(window, 2, 0x0c, 2, _mtw_option_byte(0x0c) == 0);
            if (Globals::sound != nullptr) Globals::sound->play(0x7e, nullptr, nullptr, 0.0f);
        }
        if (window->optBtnDC != nullptr && ((TouchButton *) window->optBtnDC)->OnTouchEnd(x, y) != 0)
            _mtw_options_set_category(window, 3, 0x0e, 3, _mtw_option_byte(0x0e) == 0);

        _mtw_options_sync_slider_values(window);

        Array<TouchSlider *> *sliders = _mtw_sliders(window);
        if (sliders != nullptr) {
            for (unsigned int i = 1; i < sliders->size(); i++) {
                if (i == 5 && ((Layout *) Globals::layout)->field_0x284_sliderSlot5Enabled == 0)
                    continue;
                TouchSlider *slider = (*sliders)[i];
                if (slider == nullptr || slider->OnTouchEnd(x, y) == 0)
                    continue;

                if (i == 2) {
                    if (Globals::sound != nullptr) Globals::sound->play(0x7e, nullptr, nullptr, 0.0f);
                } else if (i == 5) {
                    float value = slider->getValue();
                    if (Globals::iPadLargePossible != 0) {
                        float scale = 1.0f;
                        if (value <= 0.66f) scale = 0.5f;
                        if (value <= 0.33f) scale = 0.0f;
                        _mtw_option_set_float(0x48, scale);
                        _mtw_ChoiceWindow_set(choice, _mtw_GameText_getText(Globals::gameText, 122));
                        if (Globals::recordHandler != nullptr)
                            ((RecordHandler *) Globals::recordHandler)->saveOptions();
                    } else {
                        int textId = 512;
                        if (value <= 0.66f) textId = 511;
                        if (value <= 0.33f) textId = 510;
                        _mtw_ChoiceWindow_set(choice, _mtw_GameText_getText(Globals::gameText, textId));
                    }
                    window->messageShowing = 1;
                }
            }
        }

        if (Globals::iPadLargePossible != 0 && window->scrollExtraButton != nullptr &&
            ((TouchButton *) window->scrollExtraButton)->OnTouchEnd(x, y) != 0) {
            _mtw_option_byte(0x4c) ^= 1u;
            if (Globals::recordHandler != nullptr)
                ((RecordHandler *) Globals::recordHandler)->saveOptions();
            ((TouchButton *) window->scrollExtraButton)->setAlwaysPressed(_mtw_option_byte(0x4c) != 0);
            _mtw_ChoiceWindow_set(choice, _mtw_GameText_getText(Globals::gameText, 122));
            window->messageShowing = 1;
        }

        return _mtw_onTouchEnd_listTail(window, y, x);
    }

    if (state == 8) {
        if (window->messageShowing != 0 && raw[0x176] != 0 &&
            _mtw_ChoiceWindow_OnTouchEnd(choice, y, x) == 0) {
            _mtw_options_update_accel_from_engine();
            raw[0x176] = 0;
            window->messageShowing = 0;
        }

        TouchSlider *mainSlider = _mtw_slider(window, 0);
        size_t activePresetOffset = _mtw_option_byte(0x30) != 0 ? 0x14 : 0x18;
        if (mainSlider != nullptr)
            _mtw_option_set_float(activePresetOffset, mainSlider->getValue());

        Layout *layout = (Layout *) Globals::layout;
        if (window->upButtonPressed != 0) {
            int rowTop = layout->buttonInsetX + window->listTopY;
            if (rowTop < y && y < rowTop + window->listEntryHeight) {
                int colLeft = layout->buttonInsetX + layout->field_0xc_leftMargin;
                if (colLeft < x && x < colLeft + layout->field_0x20_top + window->listEntryWidth) {
                    _mtw_option_byte(0x30) = 1;
                    if (window->optBtnD0 != nullptr) ((TouchButton *) window->optBtnD0)->setHalfTransparent(true);
                    if (mainSlider != nullptr) mainSlider->setValue(_mtw_option_float(0x14));
                    if (Globals::sound != nullptr) Globals::sound->play(0x7b, nullptr, nullptr, 0.0f);
                }
            }
        }
        if (window->downButtonPressed != 0) {
            int rowTop = window->listTopY + layout->buttonInsetX + window->listEntryHeight;
            if (rowTop < y && y < (window->listTopY - layout->buttonInsetX) + window->listBottomY) {
                int colLeft = layout->buttonInsetX + layout->field_0xc_leftMargin;
                if (colLeft < x && x < colLeft + layout->field_0x20_top + window->listEntryWidth) {
                    _mtw_option_byte(0x30) = 0;
                    if (window->optBtnD0 != nullptr) ((TouchButton *) window->optBtnD0)->setHalfTransparent(false);
                    if (mainSlider != nullptr) mainSlider->setValue(_mtw_option_float(0x18));
                    if (Globals::sound != nullptr) Globals::sound->play(0x7b, nullptr, nullptr, 0.0f);
                }
            }
        }

        window->upButtonPressed = 0;
        window->downButtonPressed = 0;

        if (window->optBtnCC != nullptr && ((TouchButton *) window->optBtnCC)->OnTouchEnd(x, y) != 0) {
            _mtw_option_byte(0x11) ^= 1u;
            ((TouchButton *) window->optBtnCC)->setAlwaysPressed(_mtw_option_byte(0x11) != 0);
        }
        if (window->optBtnD0 != nullptr && ((TouchButton *) window->optBtnD0)->OnTouchEnd(x, y) != 0) {
            _mtw_ChoiceWindow_set(choice, _mtw_GameText_getText(Globals::gameText, 493));
            raw[0x176] = 1;
            window->messageShowing = 1;
        }
        if (mainSlider != nullptr)
            mainSlider->OnTouchEnd(x, y);

        return _mtw_onTouchEnd_listTail(window, y, x);
    }

    return 0;
}

static inline int _mtw_onTouchEnd_scrollState(void *self, int y, int x, int which) {
    auto *window = (MenuTouchWindow *) self;
    ScrollTouchWindow *sw = (which == 0xf0) ? (ScrollTouchWindow *) window->scrollWindowA : (ScrollTouchWindow *) window->scrollWindowB;
    if (sw != nullptr) {
        sw->OnTouchEnd(x, y);
    }
    Array<TouchButton *> *entries = (Array<TouchButton *> *) window->scrollEntries;
    if (entries != nullptr) {
        for (int i = 0; i < entries->size(); i++) {
            TouchButton *btn = (*entries)[i];
            if (btn != nullptr) {
                if (which == 0xf0) {
                    if (btn->field_0x0 == 110 && btn->OnTouchEnd(x, y) != 0) {
                        NFC().openTermsOfService();
                    } else if (btn->field_0x0 == 106 && btn->OnTouchEnd(x, y) != 0) {
                        if (auto *ad = (MtwAppData *) _mtw_AppMgr_GetApplicationData())
                            ad->rateGameFlagB = 1;
                        NFC().rateGame();
                    } else if (btn->field_0x0 == 109 && btn->OnTouchEnd(x, y) != 0) {
                        NFC().openPrivacyPolicy();
                    } else if (btn->field_0x0 == 22 && btn->OnTouchEnd(x, y) != 0) {
                        if (auto *ad = (MtwAppData *) _mtw_AppMgr_GetApplicationData())
                            ad->rateGameFlagA = 1;
                        NFC().rateGame();
                    }
                } else if (which == 0xf4) {
                    if (btn->field_0x0 == 60 && btn->OnTouchEnd(x, y) != 0) {
                        window->messageShowing = 0;
                        window->dlcResultDialogShowing = 1;
                    } else if (btn->field_0x0 == 53 && btn->OnTouchEnd(x, y) != 0) {
                        NFC().iap_restore_purchases();
                        window->purchaseRestorePending = 1;
                    } else if (btn->field_0x0 == 52 && btn->OnTouchEnd(x, y) != 0) {
                        switch (window->field_0x1e0) {
                            case 0: NFC().iap_buy_dlc_valkyrie(); break;
                            case 1: NFC().iap_buy_dlc_kaamo_club(); break;
                            case 2: NFC().iap_buy_dlc_supernova(); break;
                            case 3: NFC().iap_buy_dlc_vip(); break;
                            case 4: NFC().iap_buy_dlc_full_package(); break;
                            default: break;
                        }
                        window->messageShowing = 0;
                    }
                }
            }
        }
    }

    if (which == 0xf4 && window->scrollSlots != nullptr) {
        auto *slots = (Array<TouchButton *> *) window->scrollSlots;
        for (unsigned int i = 0; i < slots->size(); i++) {
            TouchButton *btn = (*slots)[i];
            if (btn == nullptr || btn->OnTouchEnd(x, y) == 0) continue;

            if (window->field_0x1e0 == (int) i) {
                switch (window->field_0x1e0) {
                    case 0: NFC().iap_buy_dlc_valkyrie(); break;
                    case 1: NFC().iap_buy_dlc_kaamo_club(); break;
                    case 2: NFC().iap_buy_dlc_supernova(); break;
                    case 3: NFC().iap_buy_dlc_vip(); break;
                    case 4: NFC().iap_buy_dlc_full_package(); break;
                    default: break;
                }
            } else {
                window->field_0x1e0 = (int) i;
                if (window->scrollWindowB != nullptr)
                    window->scrollWindowB->setText(String(""), _mtw_text_copy(window->field_0x1e0 + 87));
            }
            window->messageShowing = 0;
            break;
        }
    }
    return _mtw_onTouchEnd_listTail(window, y, x);
}

static inline int _mtw_onTouchEnd_missionsState(void *self, int y, int x) {
    auto *window = (MenuTouchWindow *) self;
    if (window->missionsWindow != nullptr) {
        ((MissionsWindow *) window->missionsWindow)->OnTouchEnd(x, y);
        window->menuState = 0;
    }
    return 0;
}

static inline int _mtw_onTouchEnd_cinematicState(void *self, int y, int x) {
    auto *window = (MenuTouchWindow *) self;
    if (Globals::iPad != 0 && Globals::iPadAssetsWithLowerRes == 0) {
        window->cinematicTouchState = 0;
        if (window->cinematicBtnA != nullptr &&
            ((TouchButton *) window->cinematicBtnA)->OnTouchEnd(x, y) != 0) {
            window->menuState = 3;
        }
        if (window->cinematicBtnB != nullptr &&
            ((TouchButton *) window->cinematicBtnB)->OnTouchEnd(x, y) != 0) {
            float scale = _mtw_option_float(0x48);
            int steerAnchor = 583;
            int fireAnchor = 513;
            if (scale >= 1.0f) {
                steerAnchor = 830;
                fireAnchor = 730;
            }
            if (scale <= 0.0f) {
                steerAnchor = 415;
                fireAnchor = 365;
            }
            auto *settings = (GameSettings *) Globals::options;
            settings->steerAnchorX = steerAnchor;
            settings->fireAnchorX = fireAnchor;
            _mtw_apply_ipad_control_coords(window);
        }
    }
    return _mtw_onTouchEnd_listTail(window, y, x);
}

static inline int _mtw_language_from_button_id(int id) {
    int idx = id - 27;
    if (idx < 0 || idx >= (int) (sizeof(kMtwLanguageByButtonOffset) / sizeof(kMtwLanguageByButtonOffset[0])))
        return -1;
    return kMtwLanguageByButtonOffset[idx];
}

static inline void _mtw_rebuild_sound_for_language_change(int previousLang, int buttonOffset) {
    if (previousLang != 1 && buttonOffset != 1) return;

    if (Globals::sound != nullptr) {
        delete Globals::sound;
        Globals::sound = nullptr;
    }
    Globals::sound = new FModSound();
    Globals::sound->init();

    auto *app = (AbyssEngine::ApplicationManager *) Globals::appManager;
    Globals::switch_to_target_setting = (app != nullptr && app->currentModuleId == 5) ? 1 : 2;
}

static inline int _mtw_onTouchEnd_languageState(void *self, int y, int x) {
    auto *window = (MenuTouchWindow *) self;
    auto *buttons = (Array<TouchButton *> *) window->buttonsB0;
    if (buttons == nullptr) return 0;

    for (unsigned int i = 0; i < buttons->size(); i++) {
        TouchButton *button = (*buttons)[i];
        if (button == nullptr || button->OnTouchEnd(x, y) == 0) continue;

        int buttonOffset = button->field_0x0 - 27;
        int language = _mtw_language_from_button_id(button->field_0x0);
        if (language < 0) return 1;

        int previousLanguage = GameText::getLanguage();
        Globals *globals = Globals::gGlobals != nullptr ? Globals::gGlobals : (Globals *) Globals::globals;
        if (globals != nullptr)
            globals->loadFont(language);
        if (Globals::gameText != nullptr)
            ((GameText *) Globals::gameText)->setLanguage(3401, language);

        _mtw_rebuild_sound_for_language_change(previousLanguage, buttonOffset);

        if (Globals::recordHandler != nullptr)
            ((RecordHandler *) Globals::recordHandler)->saveOptions();
        if (Globals::achievements != nullptr)
            ((Achievements *) Globals::achievements)->resetNewMedals();
        if (Globals::layout != nullptr)
            ((Layout *) Globals::layout)->reload();

        auto *app = (AbyssEngine::ApplicationManager *) Globals::appManager;
        if (app != nullptr) {
            if (Globals::status != nullptr && app->currentModuleId == 5)
                Globals::status->field_0x108 = 1;
            app->SetCurrentApplicationModule(app->currentModuleId);
        }
        return 1;
    }
    return 0;
}

static inline int _mtw_onTouchEnd_storeCreditsState(void *self, int y, int x) {
    auto *window = (MenuTouchWindow *) self;
    auto *buttons = (Array<TouchButton *> *) window->buttonsB8;
    if (buttons == nullptr) return 0;

    for (unsigned int i = 0; i < buttons->size(); i++) {
        TouchButton *button = (*buttons)[i];
        if (button == nullptr || button->OnTouchEnd(x, y) == 0) continue;

        if (i == 2) {
            if (!Globals::iap_hack_dlc3Bought) {
                NFC().iap_buy_dlc_supernova();
                return 1;
            }
        } else if (i == 1) {
            if (!Globals::iap_hack_dlc1Bought) {
                NFC().iap_buy_dlc_valkyrie();
                return 1;
            }
        }
        window->menuState = 12;
        return 1;
    }
    return 0;
}

static inline int _mtw_onTouchEnd_genericButtons(void *self, int y, int x, int fieldOff) {
    auto *window = (MenuTouchWindow *) self;
    Array<TouchButton *> *btnArr = nullptr;
    if (fieldOff == 0x4) btnArr = (Array<TouchButton *> *) window->buttons;
    else if (fieldOff == 0xac) btnArr = (Array<TouchButton *> *) window->optionsButtons;
    else if (fieldOff == 0xb0) btnArr = (Array<TouchButton *> *) window->buttonsB0;
    else if (fieldOff == 0xb4) btnArr = (Array<TouchButton *> *) window->buttonsB4;
    else if (fieldOff == 0xb8) btnArr = (Array<TouchButton *> *) window->buttonsB8;
    if (btnArr != nullptr) {
        for (int i = 0; i < btnArr->size(); i++) {
            TouchButton *btn = (*btnArr)[i];
            if (btn != nullptr && btn->OnTouchEnd(x, y) != 0) {
                return 1;
            }
        }
    }
    return 0;
}

static inline void *_mtw_Array_TB_dtor(void *arr) {
    ((Array<TouchButton *> *) arr)->~Array();
    return arr;
}

static inline void *_mtw_Array_TS_dtor(void *arr) {
    ((Array<TouchSlider *> *) arr)->~Array();
    return arr;
}

static inline void *_mtw_Array_Str_dtor(void *arr) {
    ((Array<String *> *) arr)->~Array();
    return arr;
}

static inline void *_mtw_TouchButton_dtor(void *p) {
    ((TouchButton *) p)->~TouchButton();
    return p;
}

static inline void *_mtw_ChoiceWindow_dtor(void *p) {
    ((ChoiceWindow *) p)->~ChoiceWindow();
    return p;
}

static inline void *_mtw_ScrollTouchWindow_dtor(void *p) {
    ((ScrollTouchWindow *) p)->~ScrollTouchWindow();
    return p;
}

static inline void *_mtw_Array_StrArr_dtor(void *p) {
    ((Array<Array<String *> *> *) p)->~Array();
    return p;
}

static inline void _mtw_Array_StrArr_ctor(void *p) { new(p) Array<Array<String *> *>(); }

static inline void _mtw_Array_Str_ctor(void *p) { new(p) Array<String *>(); }

static inline void _mtw_ArraySetLength_StrArr(int n, void *arr) {
    ArraySetLength<Array<String *> *>((unsigned int) n, *(Array<Array<String *> *> *) arr);
}

static inline void _mtw_ArraySetLength_Str(int n, void *arr) {
    ArraySetLength<String *>((unsigned int) n, *(Array<String *> *) arr);
}


static inline void _mtw_TouchButton_ctor7(void *btn, void *label, int a, int x, int y, char type) {
    new(btn) TouchButton(*(String *) label, x, y, a, (unsigned char) type);
}

static inline void _mtw_Status_resetGame() { Globals::status->resetGame(); }

void _mtw_Status_nextCampaignMission(bool a);

static inline void _mtw_Mission_ctor(void *m) { new(m) Mission(); }

void _mtw_Status_setMission(void *status);

void *_mtw_Ship_makeShip(int shipDef);

void _mtw_Status_setShip(void *status);

void _mtw_Ship_setRace(void *ship, int race);

void *_mtw_Item_makeItem(int itemDef, int qty);

void *_mtw_makeItem2(int itemDef);

void _mtw_Status_setCredits(void *status);

void *_mtw_Galaxy_getStation(void *galaxy, int idx);

void _mtw_Status_setStation(void *status);

void _mtw_Status_setSystemVisibility(void *status, int sys, bool vis);

void _mtw_Achievements_setMedal(void *ach, int id, int n);

void _mtw_RecordHandler_saveOptions(void *rh);

void _mtw_Status_setKills(void *status, int count);

void _mtw_ChoiceWindow_OnTouchBegin(void *cw, int y);

int _mtw_Layout_OnTouchBegin(void *layout, int y);

void _mtw_TouchButton_OnTouchBegin(void *btn, int y);

void _mtw_TouchButton_OnTouchBeginXY(void *btn, int y, int x);

static inline float _mtw_TouchButton_setPosition(void *btn, int x, int y) {
    ((TouchButton *) btn)->setPosition(x, y);
    return 0.0f;
}

void _mtw_TouchSlider_OnTouchBegin(void *sl, int y);

void _mtw_ScrollTouchWindow_OnTouchBegin(void *w, int y);

void _mtw_MissionsWindow_OnTouchBegin(void *w, int y);

void _mtw_FModSound_play(void *snd, int id, void *pos, float v);

int _mtw_idiv(int a, int b);

static inline void _mtw_RecordHandler_ctor(void *rh) { new(rh) RecordHandler(); }

static inline void *_mtw_RecordHandler_readRecord(void *rh, int slot) {
    return ((RecordHandler *) rh)->readRecord(slot);
}

static inline void *_mtw_RecordHandler_dtor(void *rh) {
    ((RecordHandler *) rh)->~RecordHandler();
    return rh;
}

static inline void *_mtw_GameRecord_dtor(void *gr) {
    ((GameRecord *) gr)->~GameRecord();
    return gr;
}

static inline void _mtw_GameRecord_load(void *gr) { ((GameRecord *) gr)->load(); }

static inline void *_mtw_AppMgr_GetApplicationModule(void *app, int id) {
    return ((AbyssEngine::ApplicationManager *) app)->GetApplicationModule(id);
}

static inline void _mtw_TouchButton_getPosition(void *out, void *btn) {
    *(Vector *) out = ((TouchButton *) btn)->getPosition();
}

static inline void _mtw_TouchButton_setVisible(void *btn, bool v) {
    ((TouchButton *) btn)->setVisible(v);
}

static inline void *_mtw_Array_GameRecord_dtor(void *p) {
    ((Array<GameRecord *> *) p)->~Array();
    return p;
}

static inline void *_mtw_RecordHandler_readAllPreviewRecords(void *rh) {
    return ((RecordHandler *) rh)->readAllPreviewRecords();
}

static inline void _mtw_RecordHandler_recordStoreWrite(void *rh, int slot) {
    ((RecordHandler *) rh)->recordStoreWrite(slot);
}

static inline void _mtw_RecordHandler_recordStoreWritePreview(void *rh, int slot) {
    ((RecordHandler *) rh)->recordStoreWritePreview(slot);
}

static inline void *_mtw_RecordHandler_recordStoreReadPreview(void *rh, int slot) {
    return ((RecordHandler *) rh)->recordStoreReadPreview(slot);
}

void *_mtw_AppMgr_GetApplicationData();

static inline void *_mtw_AppMgr_GetEngine() { return GetEngine(); }

static inline void _mtw_ChoiceWindow_update(void *cw) { ((ChoiceWindow *) cw)->update(0); }

static inline void _mtw_TouchButton_setYPosition(void *btn, int y) {
    ((TouchButton *) btn)->setYPosition(y);
}

static inline int _mtw_TouchButton_isVisible(void *btn) {
    return ((TouchButton *) btn)->isVisible();
}

void _mtw_ScrollTouchWindow_update(void *w);

void _mtw_MissionsWindow_update(void *w);

void _mtw_startSupernovaChallenge_impl(void *self);

void *_mtw_GetApplicationData(void *app);

void _mtw_DlcMenu_call(void *win, void *s1, void *s2);

static inline void _mtw_Layout_drawBG() { ((Layout *) Globals::layout)->drawBG(); }

void _mtw_ChoiceWindow_OnTouchMove(void *cw, int y);

void _mtw_TouchButton_OnTouchMove(void *btn, int y);

void _mtw_TouchButton_OnTouchMoveXY(void *btn, int y, int x);

void _mtw_TouchSlider_OnTouchMove(void *sl, int y);

float _mtw_TouchSlider_getValue(void *sl);

void _mtw_ScrollTouchWindow_OnTouchMove(void *w, int y);

void _mtw_MissionsWindow_OnTouchMove(void *w, int y);

int _mtw_FModSound_tryToStopMusicForBGMusic();

void _mtw_FModSound_setVolume(void *snd, float v);

static inline void _mtw_Array_TB_ctor(void *a) { new(a) Array<TouchButton *>(); }

static inline void _mtw_TouchButton_draw(void *btn) { ((TouchButton *) btn)->draw(); }

static inline void _mtw_Layout_drawBox(void *layout, int mode, int x, int y, int w, int h, void *str) {
    ((Layout *) layout)->drawBox(mode, x, y, w, h, *(String *) str);
}

static inline void _mtw_ImageFactory_drawShip(void *imgF, unsigned int shipId, int x, int y) {
    ((ImageFactory *) imgF)->drawShip((int) shipId, x, y);
}

static inline uint32_t _mtw_u32(MenuTouchWindow *window, size_t offset) {
    return *(uint32_t *) ((uint8_t *) window + offset);
}

static inline void _mtw_set_u32(MenuTouchWindow *window, size_t offset, uint32_t value) {
    *(uint32_t *) ((uint8_t *) window + offset) = value;
}

static inline uint32_t &_mtw_u32_ref(MenuTouchWindow *window, size_t offset) {
    return *(uint32_t *) ((uint8_t *) window + offset);
}

static inline uint16_t &_mtw_u16_ref(MenuTouchWindow *window, size_t offset) {
    return *(uint16_t *) ((uint8_t *) window + offset);
}

static inline void _mtw_image2d(MenuTouchWindow *window, size_t offset, unsigned short resId) {
    if (Globals::Canvas == nullptr) return;
    ((PaintCanvas *) Globals::Canvas)->Image2DCreate(resId, _mtw_u32_ref(window, offset));
}

static inline int _mtw_default_steer_anchor() {
    float scale = _mtw_option_float(0x48);
    if (scale >= 1.0f) return 830;
    if (scale <= 0.0f) return 415;
    return 583;
}

static inline int _mtw_default_fire_anchor() {
    float scale = _mtw_option_float(0x48);
    if (scale >= 1.0f) return 730;
    if (scale <= 0.0f) return 365;
    return 513;
}

static inline void _mtw_apply_ipad_control_coords(MenuTouchWindow *window) {
    if (!Globals::iPad || Globals::Canvas == nullptr) return;

    auto *settings = (GameSettings *) Globals::options;
    if (settings->steerAnchorX == 0)
        settings->steerAnchorX = _mtw_default_steer_anchor();
    if (settings->fireAnchorX == 0)
        settings->fireAnchorX = _mtw_default_fire_anchor();

    Globals *globals = Globals::gGlobals != nullptr ? Globals::gGlobals : (Globals *) Globals::globals;
    if (globals == nullptr) return;

    auto *canvas = (PaintCanvas *) Globals::Canvas;
    globals->setCoordsSteer(settings->steerAnchorX,
                            canvas->GetImage2DWidth(_mtw_u32(window, 0x1c)),
                            canvas->GetImage2DWidth(_mtw_u32(window, 0x24)),
                            canvas->GetImage2DWidth(_mtw_u32(window, 0x20)),
                            _mtw_u16_ref(window, 0x2e),
                            _mtw_u16_ref(window, 0x30),
                            _mtw_u16_ref(window, 0x40),
                            _mtw_u16_ref(window, 0x42),
                            _mtw_u16_ref(window, 0x3c),
                            _mtw_u16_ref(window, 0x3e),
                            _mtw_u16_ref(window, 0x34),
                            _mtw_u16_ref(window, 0x32),
                            _mtw_u16_ref(window, 0x48),
                            _mtw_u16_ref(window, 0x4a));

    globals->setCoordsFire(settings->fireAnchorX,
                           canvas->GetImage2DWidth(_mtw_u32(window, 0x78)),
                           _mtw_u32(window, 0x78),
                           _mtw_u32(window, 0x7c),
                           _mtw_u32_ref(window, 0x74),
                           _mtw_u16_ref(window, 0x60),
                           _mtw_u16_ref(window, 0x62),
                           _mtw_u16_ref(window, 0x64),
                           _mtw_u16_ref(window, 0x66),
                           _mtw_u16_ref(window, 0x68),
                           _mtw_u16_ref(window, 0x6a),
                           _mtw_u16_ref(window, 0x6c),
                           _mtw_u16_ref(window, 0x6e),
                           _mtw_u16_ref(window, 0x70),
                           _mtw_u16_ref(window, 0x72),
                           _mtw_u16_ref(window, 0x4c),
                           _mtw_u16_ref(window, 0x4e));
}

static inline String _mtw_text_copy(int textId) {
    auto *text = (String *) _mtw_GameText_getText(Globals::gameText, textId);
    return text != nullptr ? *text : String("");
}

static inline void _mtw_add_text_button(MenuTouchWindow *window, int id, int textId,
                                        int row, Array<TouchButton *> *buttons, int yOff) {
    String label = _mtw_text_copy(textId);
    window->addButton(id, label, row, buttons, yOff);
}

static inline TouchButton *_mtw_new_text_button(int id, int textId, int x, int y, unsigned char type) {
    auto *button = new TouchButton(_mtw_text_copy(textId), x, y, 0, type);
    button->field_0x0 = id;
    button->field_0x4 = 0;
    return button;
}

static inline TouchButton *_mtw_new_width_button(int id, int textId, int x, int y,
                                                 int width, unsigned char flags0,
                                                 unsigned char flags1 = 4) {
    auto *button = new TouchButton(_mtw_text_copy(textId), 0, x, y, width, flags0, flags1);
    button->field_0x0 = id;
    button->field_0x4 = 0;
    return button;
}

static inline TouchButton *_mtw_new_empty_icon_button(int id, int x, int y, unsigned char type) {
    auto *button = new TouchButton(String(""), 13, x, y, type);
    button->field_0x0 = id;
    button->field_0x4 = 0;
    return button;
}

static inline void _mtw_add_button_ptr(Array<TouchButton *> *arr, TouchButton *button) {
    if (arr != nullptr && button != nullptr)
        ArrayAdd<TouchButton *>(button, *arr);
}

static inline Array<TouchButton *> *_mtw_new_button_array() {
    return new Array<TouchButton *>();
}

static inline void _mtw_build_language_buttons(MenuTouchWindow *window, bool includeList) {
    auto *buttons = (Array<TouchButton *> *) window->buttonsB0;
    if (!includeList || buttons == nullptr) return;

    auto *layout = (Layout *) Globals::layout;
    if (layout == nullptr) return;

    float rowHeight = (float) layout->field_0x34;
    if (!Globals::iPad)
        rowHeight *= 0.9f;
    int step = (int) rowHeight + 2 + layout->field_0x30;
    int yBase = Globals::h / 2 - window->buttonYBias;
    auto *canvas = (PaintCanvas *) Globals::Canvas;

    for (int i = 0; i < 10; i++) {
        int x = Globals::iPad
                    ? Globals::w / 2 - window->buttonWidth / 2
                    : ((i & 1) ? Globals::w - Globals::w / 4 : Globals::w / 4) - window->buttonWidth / 2;
        int y = Globals::iPad
                    ? yBase - layout->field_0x29c_buttonRowGap - layout->field_0x30 + step * i
                    : yBase - 12 - layout->field_0x29c_buttonRowGap + step * (i >> 1);

        unsigned int font = (unsigned int) (uintptr_t) Globals::font;
        if ((i | 1) == 9)
            font = (unsigned int) Globals::fontLangSelect;
        int spacing = canvas != nullptr ? canvas->FontGetSpacing(font) : 0;

        auto *button = new TouchButton(_mtw_text_copy(kMtwLanguageTextIds[i]), 0, x, y,
                                       window->buttonWidth, 0x11, 4, font, spacing);
        button->field_0x0 = kMtwLanguageButtonIds[i];
        button->field_0x4 = 0;
        if ((i | 1) == 9)
            button->fontId = (unsigned int) Globals::fontLangSelect;
        _mtw_add_button_ptr(buttons, button);
    }
}

static inline void _mtw_build_language_options_buttons(MenuTouchWindow *window) {
    auto *buttons = (Array<TouchButton *> *) window->buttonsB4;
    if (buttons == nullptr) return;
    _mtw_add_text_button(window, 16, 519, 0, buttons, 0);
    _mtw_add_text_button(window, 50, 25, 1, buttons, 0);
}

static inline void _mtw_build_store_credit_buttons(MenuTouchWindow *window) {
    auto *buttons = (Array<TouchButton *> *) window->buttonsB8;
    auto *canvas = (PaintCanvas *) Globals::Canvas;
    if (buttons == nullptr || canvas == nullptr) return;

    for (int i = 0; i < 3; i++) {
        uint32_t image = 0;
        uint32_t overlay = 0;
        canvas->Image2DCreate(kMtwStoreImagePairs[i][0], image);
        canvas->Image2DCreate(kMtwStoreImagePairs[i][1], overlay);

        int width = canvas->GetImage2DWidth(image);
        int height = canvas->GetImage2DHeight(image);
        int x = Globals::w / 2 - width / 2;
        if (i == 0)
            x -= width;
        else if (i == 2)
            x += width;
        int y = Globals::h / 2 - height / 2;

        auto *button = new TouchButton(image, overlay, 19, x, y, 1u);
        button->field_0x0 = i;
        button->field_0x4 = 0;
        _mtw_add_button_ptr(buttons, button);
    }
}

static inline void _mtw_build_footer_button(MenuTouchWindow *window, int id, int textId) {
    Layout *layout = (Layout *) Globals::layout;
    auto *buttons = (Array<TouchButton *> *) window->buttons;
    int x = Globals::w - layout->field_0x2c;
    int y = Globals::h - layout->field_0x2c;
    _mtw_add_button_ptr(buttons, _mtw_new_text_button(id, textId, x, y, 0x22));
}

static inline void _mtw_build_social_buttons(MenuTouchWindow *window) {
    auto *buttons = (Array<TouchButton *> *) window->buttons;
    if (buttons == nullptr) return;

    Vector base{};
    if (buttons->size() > 0 && buttons->data_[0] != nullptr)
        base = buttons->data_[0]->getPosition();
    int y = (int) base.y;
    int w = window->buttonWidth;
    int leadX = (int) (base.x + (float) w + (float) (w / 3));
    int achX = (int) (base.x - (float) (w / 3));

    auto *leaderboards = new TouchButton(String("Leaderboards"), 17, leadX, y, 0x44);
    leaderboards->field_0x0 = 23;
    leaderboards->field_0x4 = 0;
    _mtw_add_button_ptr(buttons, leaderboards);

    auto *achievements = new TouchButton(String("Achievements"), 17, achX, y, 0x44);
    achievements->field_0x0 = 24;
    achievements->field_0x4 = 0;
    _mtw_add_button_ptr(buttons, achievements);
}

static inline void _mtw_build_policy_buttons(MenuTouchWindow *window) {
    Layout *layout = (Layout *) Globals::layout;
    auto *entries = (Array<TouchButton *> *) window->scrollEntries;
    if (entries == nullptr) return;

    int y = Globals::h - layout->field_0x2c;
    int xRight = Globals::w - layout->field_0x2c;
    TouchButton *back = _mtw_new_text_button(109, 5000, xRight, y, 0x22);
    _mtw_add_button_ptr(entries, back);

    int xTerms = xRight - layout->field_0x2c * 3;
    if (back != nullptr)
        xTerms -= back->getWidth();
    _mtw_add_button_ptr(entries, _mtw_new_text_button(110, 5001, xTerms, y, 0x22));
}

static inline bool _mtw_pause_menu_should_show_supernova() {
    if (Globals::status == nullptr) return false;
    if (Globals::status->getCurrentCampaignMission() < 16) return false;
    if (Globals::status->inAlienOrbit()) return false;
    Mission *mission = Globals::status->getMission();
    return mission == nullptr || mission->getType() != 183;
}

static inline bool _mtw_pause_menu_should_show_status() {
    if (Globals::status == nullptr) return false;
    if (Globals::status->getCurrentCampaignMission() < 2) return false;
    Mission *mission = Globals::status->getMission();
    return mission == nullptr || mission->getType() != 183;
}

static inline void _mtw_build_main_buttons(MenuTouchWindow *window, int menuType) {
    auto *buttons = (Array<TouchButton *> *) window->buttons;
    int yOff = menuType == 0 ? window->listRowGap : 0;

    if (menuType == 0) {
        _mtw_image2d(window, 0x120, 0x1b5a);
        _mtw_add_text_button(window, 0, 28, 0, buttons, yOff);

        int row = 1;
        auto *records = (Array<GameRecord *> *) window->previewRecords;
        int lastRecord = Globals::lastRecordWritten;
        int recordSlots = (int) (uintptr_t) Globals::recordSlots;
        if (lastRecord >= 0 && records != nullptr && lastRecord < recordSlots &&
            records->data_ != nullptr && records->data_[lastRecord] != nullptr) {
            _mtw_add_text_button(window, 11, 41, row++, buttons, yOff);
        }

        _mtw_add_text_button(window, 1, 29, row++, buttons, yOff);
        _mtw_add_text_button(window, 3, 31, row++, buttons, yOff);
        if (Globals::iPad)
            _mtw_add_text_button(window, 25, 0, row++, buttons, yOff);
        _mtw_add_text_button(window, 4, 43, row, buttons, yOff);
        return;
    }

    if (menuType == 1) {
        _mtw_image2d(window, 0x120, 0x534);
        int row = 0;
        _mtw_add_text_button(window, 3, 31, row++, buttons, 0);
        if (_mtw_pause_menu_should_show_supernova())
            _mtw_add_text_button(window, 10, 129, row++, buttons, 0);
        if (_mtw_pause_menu_should_show_status())
            _mtw_add_text_button(window, 12, 166, row++, buttons, 0);
        _mtw_add_text_button(window, 6, 522, row++, buttons, 0);
        _mtw_add_text_button(window, 19, 59, row++, buttons, 0);

        unsigned int mission = Globals::status != nullptr
                                   ? (unsigned int) (Globals::status->getCurrentCampaignMission() - 154)
                                   : 0xffffffffu;
        if (mission <= 4 && ((1u << mission) & 0x19u) != 0)
            _mtw_add_text_button(window, 18, 395, row, buttons, 0);

        _mtw_build_footer_button(window, 20, 61);
        TouchButton *right = _mtw_new_text_button(21, 60,
                                                  Globals::w - ((Layout *) Globals::layout)->field_0x2c,
                                                  Globals::h - ((Layout *) Globals::layout)->field_0x2c,
                                                  0x22);
        _mtw_add_button_ptr(buttons, right);
        if (right != nullptr)
            right->setPosition(Globals::w - ((Layout *) Globals::layout)->field_0x2c,
                               Globals::h - ((Layout *) Globals::layout)->field_0x2c);
        return;
    }

    if (menuType == 2) {
        int row = 0;
        _mtw_add_text_button(window, 0, 28, row++, buttons, 0);
        _mtw_add_text_button(window, 1, 29, row++, buttons, 0);
        _mtw_add_text_button(window, 2, 30, row++, buttons, 0);
        _mtw_add_text_button(window, 3, 31, row++, buttons, 0);
        if (Globals::iPad)
            _mtw_add_text_button(window, 25, 0, row++, buttons, 0);
        _mtw_add_text_button(window, 4, 43, row, buttons, 0);
        return;
    }

    Layout *layout = (Layout *) Globals::layout;
    if (layout != nullptr && layout->field_0x285 != 0)
        _mtw_build_footer_button(window, 17, 33);
}

static inline void _mtw_build_options_buttons(MenuTouchWindow *window, bool includeReset) {
    window->optionsButtons = (Array<void *> *) _mtw_new_button_array();
    auto *options = (Array<TouchButton *> *) window->optionsButtons;
    _mtw_add_text_button(window, 8, 489, 0, options, 0);
    _mtw_add_text_button(window, 9, 498, 1, options, 0);
    if (includeReset)
        _mtw_add_text_button(window, 25, 0, 2, options, 0);

    window->buttonsB0 = (Array<void *> *) _mtw_new_button_array();
    window->buttonsB4 = (Array<void *> *) _mtw_new_button_array();
    window->buttonsB8 = (Array<void *> *) _mtw_new_button_array();
    _mtw_build_language_buttons(window, includeReset);
    _mtw_build_language_options_buttons(window);
    _mtw_build_store_credit_buttons(window);
}

static inline void _mtw_build_list_resources(MenuTouchWindow *window) {
    _mtw_image2d(window, 0x10c, 0x515);
    _mtw_image2d(window, 0x110, 0x516);
    _mtw_image2d(window, 0x114, 0x517);
    _mtw_image2d(window, 0x118, 0x518);

    window->heapBufA = ::operator new[](5 * sizeof(uint32_t));
    window->heapBufB = ::operator new[](5 * sizeof(uint32_t));
    auto *a = (uint32_t *) window->heapBufA;
    auto *b = (uint32_t *) window->heapBufB;
    for (int i = 0; i < 5; ++i) {
        a[i] = 0;
        b[i] = 0;
    }

    PaintCanvas *canvas = (PaintCanvas *) Globals::Canvas;
    if (canvas != nullptr) {
        canvas->Image2DCreate(0x0bba, a[0]);
        canvas->Image2DCreate(0x0bbc, a[1]);
        canvas->Image2DCreate(0x0bc3, a[2]);
        canvas->Image2DCreate(0x0bc5, a[3]);
        canvas->Image2DCreate(0x0bc7, a[4]);
        canvas->Image2DCreate(0x0bbb, b[0]);
        canvas->Image2DCreate(Globals::iPad ? 3014 : 3005, b[1]);
        canvas->Image2DCreate(0x0bc4, b[2]);
        canvas->Image2DCreate(0x0bc6, b[3]);
        canvas->Image2DCreate(0x0bc8, b[4]);
    }

    if (Globals::iPad && !Globals::iPadAssetsWithLowerRes)
        _mtw_image2d(window, 0x14c, 0x0c1d);
    _mtw_image2d(window, 0x13c, 0x0bb8);
    _mtw_image2d(window, 0x140, 0x0bb9);
    _mtw_image2d(window, 0x144, 0x0bbe);
    _mtw_image2d(window, 0x148, 0x0bbf);

    Layout *layout = (Layout *) Globals::layout;
    window->listStateSuppress = 0;
    if (canvas != nullptr && a != nullptr) {
        window->listEntryHeight = canvas->GetImage2DHeight(a[0]);
        window->listEntryWidth = canvas->GetImage2DWidth(a[0]);
    }
    window->listX = layout->field_0x28 + layout->field_0x2c;
    window->listTopY = layout->field_0x20 + layout->field_0xc;
    int scrollbarExtra = (!Globals::iPad || Globals::iPadAssetsWithLowerRes) && canvas != nullptr
                             ? canvas->GetImage2DWidth(_mtw_u32(window, 0x13c))
                             : 0;
    window->listBottomY = scrollbarExtra;
    window->contentHeightCache = b != nullptr && canvas != nullptr ? canvas->GetImage2DWidth(b[0]) : 0;
    window->pageHeight = b != nullptr && canvas != nullptr ? canvas->GetImage2DHeight(b[0]) : 0;
    window->field_0x230 = window->listEntryWidth + 2 * layout->field_0x2c;
    window->field_0x234 = Globals::w - window->field_0x230 - 2 * layout->field_0x28 - scrollbarExtra;

    window->scrollSlots = (Array<void *> *) _mtw_new_button_array();
    auto *slots = (Array<TouchButton *> *) window->scrollSlots;
    ArraySetLength<TouchButton *>(5, *slots);
    for (unsigned int i = 0; i < slots->size(); ++i) {
        uint32_t image = a != nullptr ? a[i] : 0;
        auto *button = new TouchButton(image, 16, window->listX,
                                       window->listTopY + (window->listRowGap + window->listEntryHeight) * (int) i,
                                       0x11);
        button->field_0x0 = 54 + (int) i;
        button->field_0x4 = 0;
        slots->data_[i] = button;
    }
}

static inline void _mtw_build_scroll_windows(MenuTouchWindow *window) {
    Layout *layout = (Layout *) Globals::layout;
    int y = layout->field_0x2c + layout->field_0xc + layout->field_0x20 + window->pageHeight;
    int h = Globals::h - layout->field_0xc - layout->field_0x20 - window->pageHeight -
            layout->field_0x2c - layout->field_0x10 - layout->field_0x24;
    if (Globals::iPad && !Globals::iPadAssetsWithLowerRes) {
        uint32_t sideImage = _mtw_u32(window, 0x14c);
        if (sideImage != 0 && Globals::Canvas != nullptr)
            h -= 2 * ((PaintCanvas *) Globals::Canvas)->GetImage2DHeight(sideImage);
    }
    window->scrollWindowB = new ScrollTouchWindow(Globals::w - layout->field_0x28 - window->field_0x234,
                                                  y, window->field_0x234, h, false);
    if (window->scrollWindowB != nullptr)
        window->scrollWindowB->setText(String(""), _mtw_text_copy(window->field_0x1e0 + 87));

    auto *entries = (Array<TouchButton *> *) window->scrollEntries;
    auto *mainButtons = (Array<TouchButton *> *) window->buttons;
    if (entries != nullptr && mainButtons != nullptr && mainButtons->size() > 0 && mainButtons->data_[0] != nullptr) {
        Vector base = mainButtons->data_[0]->getPosition();
        int x = (int) (base.x + (float) window->buttonWidth + (float) (window->buttonWidth / 3));
        int restoreY = (int) (base.y - (float) (3 * layout->field_0x2c));
        auto *restore = new TouchButton(_mtw_text_copy(97), 17, x, restoreY, 0x44);
        restore->field_0x0 = 53;
        restore->field_0x4 = 0;
        if (!Globals::inAppPurchaseSupported)
            restore->setVisible(false);
        _mtw_add_button_ptr(entries, restore);
    }

    _mtw_image2d(window, 0x124, 0x0bc2);
    _mtw_image2d(window, 0x128, 0x233f);

    int xA = Globals::iPad ? layout->field_0x28 + (Globals::w >> 2) : layout->field_0x28;
    int wA = Globals::iPad ? (Globals::w >> 1) - 2 * layout->field_0x28
                           : Globals::w - 2 * layout->field_0x28;
    window->scrollWindowA = new ScrollTouchWindow(xA, layout->field_0xc + layout->field_0x20,
                                                  wA,
                                                  Globals::h - (layout->field_0xc + layout->field_0x20) -
                                                  layout->field_0x10 - layout->field_0x24,
                                                  false);
    if (window->scrollWindowA != nullptr) {
        String title("Galaxy on Fire 2 HD\n\n");
        title += _mtw_text_copy(45);
        title += String("\n\n\n");
        title += _mtw_text_copy(48);
        window->scrollWindowA->setText(String(""), title);
        window->scrollWindowA->setTextCentered(true);
    }
}

static inline float _mtw_opt_float_or(size_t offset, float fallback) {
    float value = _mtw_option_float(offset);
    return value == 0.0f ? fallback : value;
}

static inline void _mtw_add_slider(Array<TouchSlider *> *sliders, int type, int x, int y, float value) {
    if (sliders != nullptr)
        ArrayAdd<TouchSlider *>(new TouchSlider(type, x, y, value), *sliders);
}

static inline void _mtw_build_option_controls(MenuTouchWindow *window) {
    Layout *layout = (Layout *) Globals::layout;
    int baseX = layout->field_0x28 + window->metricC;
    int baseY = layout->field_0xc + layout->field_0x20 + layout->field_0x34 + layout->field_0x2c;
    int row = layout->field_0x2c + window->metricB;

    window->sliders = new Array<TouchSlider *>();
    auto *sliders = (Array<TouchSlider *> *) window->sliders;
    float steeringValue = _mtw_option_byte(0x30) != 0 ? _mtw_option_float(0x14) : _mtw_option_float(0x18);
    _mtw_add_slider(sliders, 0, Globals::w - layout->field_0x28 - window->listEntryWidth, baseY + row, steeringValue);
    _mtw_add_slider(sliders, 1, baseX, baseY, _mtw_opt_float_or(0x00, 1.0f));
    _mtw_add_slider(sliders, 1, baseX, baseY + row * 2, _mtw_opt_float_or(0x04, 1.0f));
    _mtw_add_slider(sliders, 1, baseX, baseY + row * 4, _mtw_opt_float_or(0x08, 1.0f));
    _mtw_add_slider(sliders, 2, baseX, baseY + row * 6, _mtw_option_float(0x2c));
    _mtw_add_slider(sliders, 2, baseX, baseY + row * 8, _mtw_option_float(0x44));

    window->optBtnCC = _mtw_new_empty_icon_button(0, Globals::w - layout->field_0x28,
                                                  baseY + row, 0x42);
    window->optBtnCC->setAlwaysPressed(_mtw_option_byte(0x11) != 0);
    window->optBtnD0 = _mtw_new_width_button(0, 492,
                                             Globals::w - layout->field_0x28 - window->listEntryWidth / 2,
                                             baseY + row * 2, window->listEntryWidth - 20, 0x44);

    window->optBtnD4 = _mtw_new_empty_icon_button(0, Globals::w - layout->field_0x28,
                                                  baseY, 0x12);
    window->optBtnD4->setAlwaysPressed(_mtw_option_byte(0x0d) != 0);
    if (TouchSlider *slider = _mtw_slider(window, 1))
        slider->setHalfTransparent(_mtw_option_byte(0x0d) == 0);

    window->optBtnD8 = _mtw_new_empty_icon_button(0, Globals::w - layout->field_0x28,
                                                  baseY + row * 2, 0x12);
    window->optBtnD8->setAlwaysPressed(_mtw_option_byte(0x0c) != 0);
    if (TouchSlider *slider = _mtw_slider(window, 2))
        slider->setHalfTransparent(_mtw_option_byte(0x0c) == 0);

    window->optBtnDC = _mtw_new_empty_icon_button(0, Globals::w - layout->field_0x28,
                                                  baseY + row * 4, 0x12);
    window->optBtnDC->setAlwaysPressed(_mtw_option_byte(0x0e) != 0);
    if (TouchSlider *slider = _mtw_slider(window, 3))
        slider->setHalfTransparent(_mtw_option_byte(0x0e) == 0);

    window->scrollExtraButton = _mtw_new_empty_icon_button(0, Globals::w - layout->field_0x28,
                                                           baseY + row * 7, 0x42);
    window->scrollExtraButton->setAlwaysPressed(_mtw_option_byte(0x4c) != 0);
}

static inline void _mtw_build_cinematic_controls(MenuTouchWindow *window) {
    if (!Globals::iPad) return;

    _mtw_image2d(window, 0x1c, 0x4c1);
    _mtw_image2d(window, 0x20, 0x4b2);
    _mtw_image2d(window, 0x24, 0x4b0);
    _mtw_image2d(window, 0x28, 0x4b6);
    _mtw_image2d(window, 0x78, 0x4c6);
    _mtw_image2d(window, 0x7c, 0x6aa);
    _mtw_image2d(window, 0x84, 0x4ba);
    _mtw_image2d(window, 0x80, 0x4b4);
    _mtw_image2d(window, 0x88, 0x4be);
    _mtw_image2d(window, 0x8c, 0x4bc);
    _mtw_image2d(window, 0x50, 0x6a5);
    _mtw_image2d(window, 0x54, 0x6a6);
    _mtw_image2d(window, 0x58, 0x6a7);
    _mtw_image2d(window, 0x5c, 0x6a8);
    _mtw_image2d(window, 0x11c, 0x6a4);
    _mtw_apply_ipad_control_coords(window);

    Layout *layout = (Layout *) Globals::layout;
    int buttonWidth = Globals::iPadHD ? 281 : (Globals::iPadLarge ? 400 : 200);
    int yA = layout->field_0x300 + layout->field_0x308 -
             2 * layout->field_0x34 - 2 * layout->field_0x30;
    int yB = layout->field_0x300 + layout->field_0x308 -
             layout->field_0x34 - layout->field_0x30;
    window->cinematicBtnB = _mtw_new_width_button(0, 497, Globals::w >> 1, yA, buttonWidth, 0x44);
    window->cinematicBtnA = _mtw_new_width_button(0, 496, Globals::w >> 1, yB, buttonWidth, 0x44);
}

static void _mtw_buildMenu(void *self, int menuType) {
    auto *window = (MenuTouchWindow *) self;
    if (window == nullptr) return;

    _mtw_build_main_buttons(window, menuType);

    bool includeReset = menuType != 1;
    _mtw_build_footer_button(window, 22, 62);
    _mtw_build_social_buttons(window);
    _mtw_build_policy_buttons(window);
    _mtw_build_options_buttons(window, includeReset);
    _mtw_build_list_resources(window);
    _mtw_build_scroll_windows(window);
    _mtw_build_option_controls(window);
    _mtw_build_cinematic_controls(window);

    Layout *layout = (Layout *) Globals::layout;
    if (!Globals::iPad || Globals::iPadAssetsWithLowerRes) {
        window->scrollUpButton = _mtw_new_text_button(52, 98,
                                                      Globals::w - layout->field_0x2c,
                                                      Globals::h - layout->field_0x2c,
                                                      0x22);
    } else {
        window->scrollUpButton = _mtw_new_width_button(52, 98,
                                                       window->field_0x230 + window->pageHeight / 2,
                                                       Globals::h - layout->field_0x10 - layout->field_0x24,
                                                       0x24, 0x24);
    }
    _mtw_add_button_ptr((Array<TouchButton *> *) window->buttons, window->scrollUpButton);

    window->choiceWindow = new ChoiceWindow();
    if (auto *ad = (MtwAppData *) _mtw_AppMgr_GetApplicationData()) {
        if (ad->purchaseResultFlag != 0) {
            window->choiceWindow->set(_mtw_text_copy(74));
            window->messageShowing = 1;
            window->storeInitDialogShowing = 1;
            ad->purchaseResultFlag = 0;
        }
    }

    window->contentHeight = 0;
    window->pageHeight = 0;
    window->screenshotState = -1;
    window->scrollOffset = 0;
    window->dragLastX = 0;
    window->dragVelocity = 0;
    window->inertiaDecay = 0x3f000000;
    window->inertiaVel = 0.0f;
    window->dragStartX = -1;
    window->dragging = 0;
    window->scrollbarHit = 0;
    window->purchaseRestorePending = 0;
    window->genericConfirmB = 0;
    window->upButtonPressed = 0;
    window->downButtonPressed = 0;
    window->messageShowing = window->choiceWindow != nullptr && window->messageShowing != 0;
}

static inline void _mtw_draw_header_text(int textId) {
    Layout *layout = (Layout *) Globals::layout;
    auto *title = (String *) _mtw_GameText_getText(Globals::gameText, textId);
    if (title != nullptr) {
        layout->drawHeader(*title);
    } else {
        layout->drawHeader();
    }
}

static inline void _mtw_draw_button_array(Array<void *> *buttons) {
    if (buttons == nullptr) return;
    for (unsigned int i = 0; i < buttons->size(); i++) {
        if (buttons->data_[i] != nullptr)
            ((TouchButton *) buttons->data_[i])->draw();
    }
}

static inline void _mtw_draw_ipad_logo(MenuTouchWindow *window) {
    if (!Globals::iPad || Globals::iPadAssetsWithLowerRes) return;
    uint32_t logoImage = _mtw_u32(window, 0x120);
    if (logoImage == UINT32_MAX) return;
    ((PaintCanvas *) Globals::Canvas)->DrawImage2D(logoImage, Globals::w >> 1, 80, 0x11u, 0x14u);
}

static inline void _mtw_draw_main_logo(MenuTouchWindow *window) {
    PaintCanvas *canvas = (PaintCanvas *) Globals::Canvas;
    uint32_t logoImage = _mtw_u32(window, 0x120);
    if (logoImage == UINT32_MAX) return;

    canvas->SetColor(0xffffffff);
    if (Globals::iPad && !Globals::iPadAssetsWithLowerRes) {
        canvas->DrawImage2D(logoImage, Globals::w >> 1, 80, 0x11u, 0x14u);
        return;
    }

    int imageW = canvas->GetImage2DWidth(logoImage);
    int imageH = canvas->GetImage2DHeight(logoImage);
    canvas->DrawImage2D(logoImage, Globals::w >> 1, ((Layout *) Globals::layout)->field_0xc / 4,
                        imageW / 2, imageH / 2, 0x11u, 0x14u, 0);
}

static inline void _mtw_draw_footer_tail(MenuTouchWindow *window) {
    Layout *layout = (Layout *) Globals::layout;
    unsigned int state = (unsigned int) window->menuState;

    bool skipFooter = state == 11;
    if (state != 11)
        skipFooter = (window->backgroundEnabled == 0 && window->menuState == 0);
    if (!skipFooter && (state > 0x0f || ((1u << state) & 0xa010u) == 0))
        layout->drawEmptyFooter(true);

    if (layout->field_0x285 != 0 &&
        (window->backgroundEnabled | 2) == 2 &&
        window->menuState == 0 &&
        window->scrollEntries != nullptr) {
        for (unsigned int i = 0; i < window->scrollEntries->size(); i++) {
            auto *button = (TouchButton *) window->scrollEntries->data_[i];
            if (button != nullptr && button->field_0x0 == 17 && button->field_0x4 == 0)
                button->draw();
        }
    }

    if (window->messageShowing != 0 && window->choiceWindow != nullptr)
        ((ChoiceWindow *) window->choiceWindow)->draw();
    if (layout->choiceWindowOpen != 0)
        layout->drawHelpWindow();
}

void _mtw_Ship_addCargo(void *ship, void *item);

float _mtw_FModSound_stop(void *snd);

void MenuTouchWindow::showSupernovaMessage() {
    void *cw = this->choiceWindow;
    void *gt = Globals::gameText;
    void *s1 = _mtw_GameText_getText(gt, 0x266);
    void *s2 = _mtw_GameText_getText(Globals::gameText, 0x267);
    _mtw_ChoiceWindow_set(cw, s1, s2, false);
    this->messageShowing = 1;
    this->supernovaMessageShowing = 1;
}

bool MenuTouchWindow::isInMissionScreen() {
    return this->menuState == 9;
}

uint8_t MenuTouchWindow::isShowingMessage() {
    return this->messageShowing;
}

bool MenuTouchWindow::isMakingScreenshot() {
    return this->screenshotState < 0x80000000u;
}

void MenuTouchWindow::hideMessage() {
    this->messageShowing = 0;
}

void MenuTouchWindow::render3D() {
    if (this->menuState != 9)
        return;
    void *obj = this->missionsWindow;
    if (obj == 0)
        return;
    return _mtw_render3D_inner(obj);
}

bool MenuTouchWindow::inCinematicMode() {
    return this->menuState == 0xd;
}

float MenuTouchWindow::getRelativeScrollStartPos() {
    int offset = this->scrollOffset;
    if (offset > 0)
        return 0.0f;
    return -(float) offset / (float) this->pageHeight;
}

int MenuTouchWindow::OnTouchEnd(int y, int x, void *touchId) {
    if (this->menuState == 0xb && touchId != 0) {
        if (this->cinematicTouchIdA == touchId)
            this->cinematicTouchIdA = 0;
        if (this->cinematicTouchIdB == touchId)
            this->cinematicTouchIdB = 0;
        this->cinematicTouchState = 0;
    }

    this->pendingActivate = 0;
    this->dragging = 0;

    Layout *layoutGuard = (Layout *) Globals::layout;
    if (layoutGuard->choiceWindowOpen != 0) {
        if (layoutGuard->OnTouchEnd(x, y) != 0)
            layoutGuard->choiceWindowOpen = 0;
        return 0;
    }

    int state = this->menuState;

    if (state == 0) {
        if (this->messageShowing != 0) {
            void *cw = this->choiceWindow;
            if (this->returnToMenuShowing != 0) {
                int r = _mtw_ChoiceWindow_OnTouchEnd(cw, y);
                if (r == 1) {
                    this->returnToMenuShowing = 0;
                    this->messageShowing = 0;
                } else if (r == 0) {
                    ApplicationManager::gAppManager->Quit();
                }
                return 0;
            }
            if (this->genericConfirmB != 0) {
                int r = _mtw_ChoiceWindow_OnTouchEnd(cw, y);
                if (r != 1) {
                    if (r != 0) return 0;
                    g_android_link_game_gp = 1;
                }
                this->messageShowing = 0;
                this->genericConfirmB = 0;
                return 0;
            }
            if (this->quitConfirmShowing != 0) {
                int r = _mtw_ChoiceWindow_OnTouchEnd(cw, y);
                if (r == 1) {
                    this->quitConfirmShowing = 0;
                    this->messageShowing = 0;
                    this->returnToMenuSubFlag = 0;
                    return 0;
                }
                if (r != 0) return 0;
                Globals::sound->resumeAll();
                Globals::sound->stopAll();
                Globals::switch_to_target_setting = 2;
                ApplicationManager::gAppManager->SetCurrentApplicationModule(1);
                this->quitConfirmShowing = 0;
                this->messageShowing = 0;
                return 0;
            }
            if (this->leaderboardDialogShowing != 0) {
                unsigned int r = (unsigned int) _mtw_ChoiceWindow_OnTouchEnd(cw, y);
                if (r < 2) _mtw_Globals_reportLeaderboards();
                this->leaderboardDialogShowing = 0;
                this->messageShowing = 0;
            }
            if (this->dlcMessageShowing != 0) {
                _mtw_ChoiceWindow_OnTouchEnd(cw, y);
                this->messageShowing = 0;
                return 0;
            }
            if (this->dlcErrorDialogShowing != 0 || this->dlcResultDialogShowing != 0) {
                _mtw_ChoiceWindow_OnTouchEnd(cw, y);
                this->messageShowing = 0;
                return 0;
            }
            if (this->loadFailedDialogShowing != 0) {
                this->loadFailedDialogShowing = 0;
            } else if (this->genericConfirmA != 0 && _mtw_ChoiceWindow_OnTouchEnd(cw, y) == 0) {
                this->messageShowing = 0;
                this->genericConfirmA = 0;
                return 0;
            } else if (this->supernovaMessageShowing != 0 && _mtw_ChoiceWindow_OnTouchEnd(cw, y) == 0) {
                this->supernovaMessageShowing = 0;
            } else if (this->supernovaPurchaseDialogShowing != 0 && _mtw_ChoiceWindow_OnTouchEnd(cw, y) == 0) {
                this->supernovaPurchaseDialogShowing = 0;
            } else {
                _mtw_ChoiceWindow_OnTouchEnd(cw, y);
            }
            this->messageShowing = 0;
            return 0;
        }
        return _mtw_onTouchEnd_genericButtons(this, y, x, 0x4), 0;
    }

    switch (state) {
        case 1:
        case 2:
            _mtw_onTouchEnd_listState(this, y, x, state);
            break;
        case 3:
            _mtw_onTouchEnd_genericButtons(this, y, x, 0xac);
            break;
        case 4:
            _mtw_onTouchEnd_scrollState(this, y, x, 0xf0);
            break;
        case 7:
        case 8:
            _mtw_onTouchEnd_optionsState(this, y, x);
            break;
        case 9:
            _mtw_onTouchEnd_missionsState(this, y, x);
            break;
        case 0xb:
            _mtw_onTouchEnd_cinematicState(this, y, x);
            break;
        case 0xc:
            _mtw_onTouchEnd_genericButtons(this, y, x, 0xb4);
            break;
        case 0xd:
            _mtw_onTouchEnd_genericButtons(this, y, x, 0x4);
            break;
        case 0xe:
            _mtw_onTouchEnd_languageState(this, y, x);
            break;
        case 0xf:
        case 0x10:
            _mtw_onTouchEnd_scrollState(this, y, x, 0xf4);
            break;
        case 0x11:
            _mtw_onTouchEnd_storeCreditsState(this, y, x);
            break;
        default:
            _mtw_onTouchEnd_genericButtons(this, y, x, 0x4);
            break;
    }
    return 0;
}

static inline void _mtw_ArrayReleaseClasses_TB(void *arr) {
    ArrayReleaseClasses<TouchButton *>(*(Array<TouchButton *> *) arr);
}

static inline void _mtw_ArrayReleaseClasses_Str(void *arr) {
    ArrayReleaseClasses<String *>(*(Array<String *> *) arr);
}

template <typename T>
static inline void freeObj(T **slot, void *(*dtor)(void *)) {
    void *o = (void *) *slot;
    if (o != 0) ::operator delete(dtor(o));
    *slot = 0;
}

MenuTouchWindow::~MenuTouchWindow() {
    {
        void *a = this->buttons;
        if (a != 0) {
            _mtw_ArrayReleaseClasses_TB(a);
            void *b = this->buttons;
            if (b != 0) ::operator delete(_mtw_Array_TB_dtor(b));
        }
        this->buttons = 0;
    }
    {
        void *a = this->optionsButtons;
        if (a != 0) {
            _mtw_ArrayReleaseClasses_TB(a);
            void *b = this->optionsButtons;
            if (b != 0) ::operator delete(_mtw_Array_TB_dtor(b));
        }
        this->optionsButtons = 0;
    }
    {
        void *a = this->scrollEntries;
        if (a != 0) {
            _mtw_ArrayReleaseClasses_TB(a);
            void *b = this->scrollEntries;
            if (b != 0) ::operator delete(_mtw_Array_TB_dtor(b));
        }
        this->scrollEntries = 0;
    }
    {
        void *a = this->buttonsB4;
        if (a != 0) {
            _mtw_ArrayReleaseClasses_TB(a);
            void *b = this->buttonsB4;
            if (b != 0) ::operator delete(_mtw_Array_TB_dtor(b));
        }
        this->buttonsB4 = 0;
    }
    {
        void *a = this->buttonsB8;
        if (a != 0) {
            _mtw_ArrayReleaseClasses_TB(a);
            void *b = this->buttonsB8;
            if (b != 0) ::operator delete(_mtw_Array_TB_dtor(b));
        }
        this->buttonsB8 = 0;
    }
    {
        void *a = this->buttonsB0;
        if (a != 0) {
            _mtw_ArrayReleaseClasses_TB(a);
            void *b = this->buttonsB0;
            if (b != 0) ::operator delete(_mtw_Array_TB_dtor(b));
        }
        this->buttonsB0 = 0;
    }

    {
        Array<TouchSlider *> *a = (Array<TouchSlider *> *) this->sliders;
        if (a != 0) {
            ArrayReleaseClasses<TouchSlider *>(*a);
            void *b = this->sliders;
            if (b != 0) ::operator delete(_mtw_Array_TS_dtor(b));
        }
        this->sliders = 0;
    }

    {
        void *a = this->scrollSlots;
        if (a != 0) {
            _mtw_ArrayReleaseClasses_TB(a);
            void *b = this->scrollSlots;
            if (b != 0) ::operator delete(_mtw_Array_TB_dtor(b));
        }
        this->scrollSlots = 0;
    }

    {
        void **slots[2] = {(void **) &this->previewStrings0, (void **) &this->previewStrings1};
        for (int k = 0; k < 2; k++) {
            void *a = *slots[k];
            if (a != 0) {
                _mtw_ArrayReleaseClasses_Str(a);
                void *b = *slots[k];
                if (b != 0) ::operator delete(_mtw_Array_Str_dtor(b));
            }
            *slots[k] = 0;
        }
    }

    freeObj(&this->optBtnCC, _mtw_TouchButton_dtor);
    freeObj(&this->okButton, _mtw_TouchButton_dtor);
    freeObj(&this->optBtnD0, _mtw_TouchButton_dtor);
    freeObj(&this->optBtnD4, _mtw_TouchButton_dtor);
    freeObj(&this->optBtnD8, _mtw_TouchButton_dtor);
    freeObj(&this->optBtnDC, _mtw_TouchButton_dtor);

    freeObj(&this->choiceWindow, _mtw_ChoiceWindow_dtor);
    freeObj(&this->backButton, _mtw_TouchButton_dtor);
    freeObj(&this->scrollExtraButton, _mtw_TouchButton_dtor);

    freeObj(&this->scrollWindowA, _mtw_ScrollTouchWindow_dtor);
    freeObj(&this->scrollWindowB, _mtw_ScrollTouchWindow_dtor);

    if (this->heapBufA != 0) ::operator delete[](this->heapBufA);
    this->heapBufA = 0;
    if (this->heapBufB != 0) ::operator delete[](this->heapBufB);
    this->heapBufB = 0;
}

static inline void _mtw_ArrayReleaseClasses_StrArr(void *arr) {
    ArrayReleaseClasses<Array<String *> *>(*(Array<Array<String *> *> *) arr);
}

void *_mtw_String_op_plus(void *out, void *rhs);

static inline void _mtw_TouchButton_ctorFull(void *btn, void *label, int a, int x, int y, int w,
                                             char type, char id) {
    new(btn) TouchButton(*(String *) label, a, x, y, w, (unsigned char) type, (unsigned char) id);
}

void MenuTouchWindow::createRecordButtons(bool inSaveMode) {
    Array<void *> *rows = this->recordRows;
    if (rows != 0) {
        for (uint32_t i = 0; i < rows->size_; i++) {
            void *row = rows->data_[i];
            if (row != 0) {
                _mtw_ArrayReleaseClasses_Str(row);
                void *r2 = this->recordRows->data_[i];
                if (r2 != 0) ::operator delete(_mtw_Array_Str_dtor(r2));
                this->recordRows->data_[i] = 0;
                rows = this->recordRows;
            }
        }
        _mtw_ArrayReleaseClasses_StrArr(rows);
        if (this->recordRows != 0) ::operator delete(_mtw_Array_StrArr_dtor(this->recordRows));
        this->recordRows = 0;
    }

    void *outer = ::operator new(sizeof(Array<Array<String *> *>));
    _mtw_Array_StrArr_ctor(outer);
    this->recordRows = (Array<void *> *) outer;
    int rowCount = (int) (uintptr_t) Globals::recordSlots;
    _mtw_ArraySetLength_StrArr(rowCount, outer);

    void *gtHolder = Globals::gameText;

    for (int i = 0; i < rowCount; i++) {
        void *row = ::operator new(sizeof(Array<String *>));
        _mtw_Array_Str_ctor(row);
        this->recordRows->data_[i] = row;
        _mtw_ArraySetLength_Str(6, this->recordRows->data_[i]);

        String s48;
        { if (s48.data) delete[] s48.data; s48.data = nullptr; s48.length = 0; }
        String s54;

        Array<GameRecord *> *rec = (Array<GameRecord *> *) this->previewRecords;
        bool empty = (rec == 0) || (rec->data_[i] == 0);

        if (empty) {
            { if (s54.data) delete[] s54.data; s54.data = nullptr; s54.length = 0; }
            Globals::gGlobals->longToTimeStringNoSeconds(0, s54);
            String *e;
            void **rowData = ((Array<void *> *) this->recordRows->data_[i])->data_;

            e = new String();
            e->Set((s54).data);
            rowData[0] = e;
            e = new String();
            e->Set(((String *) _mtw_GameText_getText(gtHolder, 0xae))->data);
            rowData = ((Array<void *> *) this->recordRows->data_[i])->data_;
            rowData[1] = e;

            e = new String();
            if (i == 0) e->Set(((String *) _mtw_GameText_getText(gtHolder, 0x1e6))->data);
            else e->ctor_char("", false);
            rowData = ((Array<void *> *) this->recordRows->data_[i])->data_;
            rowData[2] = e;

            e = new String();
            e->ctor_char("", false);
            rowData = ((Array<void *> *) this->recordRows->data_[i])->data_;
            rowData[3] = e;

            e = new String();
            e->ctor_char("", false);
            rowData = ((Array<void *> *) this->recordRows->data_[i])->data_;
            rowData[4] = e;

            e = new String();
            e->ctor_char("", false);
            rowData = ((Array<void *> *) this->recordRows->data_[i])->data_;
            rowData[5] = e;
        } else {
            { if (s54.data) delete[] s54.data; s54.data = nullptr; s54.length = 0; }
            GameRecord *slot = rec->data_[i];
            Globals::gGlobals->longToTimeStringNoSeconds(slot->playTime64, s54);
            String *e;
            void **rowData;

            e = new String();
            e->Set((s54).data);
            rowData = ((Array<void *> *) this->recordRows->data_[i])->data_;
            rowData[0] = e;

            e = new String();
            e->Set(((String *) &slot->pilotName)->data);
            rowData = ((Array<void *> *) this->recordRows->data_[i])->data_;
            rowData[1] = e;

            e = new String();
            if (i == 0) e->Set(((String *) _mtw_GameText_getText(gtHolder, 0x1e6))->data);
            else e->ctor_char("", false);
            rowData = ((Array<void *> *) this->recordRows->data_[i])->data_;
            rowData[2] = e;

            auto *credits = new String();
            *credits = Layout::formatCredits(slot->field_0x08);
            rowData = ((Array<void *> *) this->recordRows->data_[i])->data_;
            rowData[3] = credits;

            auto *combined = new String();
            void *label = _mtw_GameText_getText(gtHolder, 0x141);
            String s6c;
            s6c.ctor_char("", false);
            String s60;
            _mtw_String_op_plus(&s60, label);
            String s78;
            s78.Set(((String *) &slot->killsText)->data);
            _mtw_String_op_plus(combined, &s60);
            rowData = ((Array<void *> *) this->recordRows->data_[i])->data_;
            rowData[4] = combined;

            e = new String();
            float rank = slot->rank;
            void *rankTxt;
            if (rank <= 0.0f) rankTxt = _mtw_GameText_getText(gtHolder, 0x207);
            else if (rank <= 0.5f) rankTxt = _mtw_GameText_getText(gtHolder, 0x207);
            else if (rank <= 1.0f) rankTxt = _mtw_GameText_getText(gtHolder, 0x207);
            else rankTxt = _mtw_GameText_getText(gtHolder, 0x19);
            e->Set(((String *) rankTxt)->data);
            rowData = ((Array<void *> *) this->recordRows->data_[i])->data_;
            rowData[5] = e;
        }
    }

    if (this->okButton != 0) {
        ::operator delete(_mtw_TouchButton_dtor(this->okButton));
        this->okButton = 0;
    }
    Layout *layout = (Layout *) Globals::layout;
    bool backForm = Globals::iPad != 0;
    int extra = layout->field_0x108;
    void *okBtn = ::operator new(sizeof(TouchButton));
    int screenW = Globals::w;
    int x = (screenW - this->listX) - layout->buttonInsetX;
    int y = layout->field_0x20_top + layout->field_0xc + extra;
    void *okLabel = _mtw_GameText_getText(gtHolder, inSaveMode ? 0x1fa : 0x1f9);
    if (backForm)
        _mtw_TouchButton_ctor7(okBtn, okLabel, 7, x, y, 0x12);
    else
        _mtw_TouchButton_ctorFull(okBtn, okLabel, 0, x, y, layout->field_0x2a4_metricA, 0x12, 0x04);
    this->okButton = (TouchButton *) okBtn;

    _mtw_TouchButton_setPosition(okBtn,
                                 (screenW - this->listX) - layout->buttonInsetX,
                                 (layout->field_0x70_rowHeight + this->listRowGap) * this->selectedRow
                                 + layout->field_0xc + extra + layout->field_0x20_top +
                                 this->scrollOffset);

    if (this->backButton != 0) {
        ::operator delete(_mtw_TouchButton_dtor(this->backButton));
        this->backButton = 0;
    }
    void *backBtn = ::operator new(sizeof(TouchButton));
    void *backLabel = _mtw_GameText_getText(gtHolder, 0x41);
    _mtw_TouchButton_ctor7(backBtn, backLabel, 7,
                           (screenW - this->listX) - layout->buttonInsetX,
                           layout->field_0xc + extra + layout->field_0x20_top, 0x12);
    this->backButton = (TouchButton *) backBtn;
}

void *_mtw_Status_getShip(void *status);

void *_mtw_Item_make(int itemDef);

void *_mtw_Item_makeQty(int itemDef, int qty);

void _mtw_Ship_setItem(void *ship, void *item, int slot);

typedef void (*TransitionFn)(void *app, int mode);

void MenuTouchWindow::startValkyrie() {
    Status **statusHolder = &Globals::status;
    _mtw_Status_resetGame();
    for (int i = 0x2d; i != 0; i--)
        _mtw_Status_nextCampaignMission((bool) (unsigned char) (uintptr_t) *statusHolder);

    void *status = *statusHolder;
    void *mission = ::operator new(sizeof(Mission));
    _mtw_Mission_ctor(mission);
    _mtw_Status_setMission(status);

    ShipDefTable *row = *(ShipDefTable **) (*(int *) ((int *) Globals::ships) + 4);
    status = *statusHolder;
    _mtw_Ship_makeShip(row->shipDefId);
    _mtw_Status_setShip(status);

    void *ship = *statusHolder;
    int race = (int) (uintptr_t) _mtw_Status_getShip(ship);
    _mtw_Ship_setRace(ship, race);

    void *it;
    void *sh;
    it = _mtw_Item_make(row->itemDef_primary);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 0);
    it = _mtw_Item_make(row->itemDef_secondary);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 1);
    it = _mtw_Item_makeQty(row->itemDef_missile, 0xa);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 0);
    it = _mtw_Item_make(row->itemDef_equip0x144);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 0);
    it = _mtw_Item_make(row->itemDef_equip0xcc);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 1);
    it = _mtw_Item_makeQty(row->itemDef_equip0x158, 0xa);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 2);
    it = _mtw_Item_make(row->itemDef_equip0x154);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 3);

    _mtw_Status_setCredits(*statusHolder);
    void *station = *statusHolder;
    _mtw_Galaxy_getStation(Globals::galaxy, 0x5b);
    _mtw_Status_setStation(station);
    _mtw_Status_setSystemVisibility(*statusHolder, 6, true);
    _mtw_Status_setSystemVisibility(*statusHolder, 0x19, true);
    _mtw_Status_setCredits(*statusHolder);

    OptionsRecord *opt = (OptionsRecord *) Globals::hints;
    void *ach = Globals::achievements;
    OptionsRecord *optB = (OptionsRecord *) Globals::options;
    ((Status *) *statusHolder)->preSetField0x84 = 0x1a0a;
    opt->flag_word_0x8 = 0x101;
    opt->flag_word_0xd = 0x101;
    opt->flag_word_0x1c = 0x101;
    opt->flag_dword_0x20 = 0x1010101;
    opt->flag_0x17 = 1;
    opt->flag_0xa = 1;
    opt->flag_0x15 = 1;
    opt->flag_0x13 = 1;
    opt->flag_0xf = 1;
    opt->flag_0x1e = 1;
    opt->flag_0x24 = 1;
    opt->flag_0x36 = 0;
    opt->flag_dword_0x32 = 0;
    opt->flag_0x38 = 1;
    optB->flag_0x34 = 1;

    _mtw_Achievements_setMedal(ach, 0x17, 3);
    _mtw_Achievements_setMedal(Globals::achievements, 0x1e, 1);
    _mtw_RecordHandler_saveOptions(Globals::recordHandler);
    _mtw_Status_setKills(*statusHolder, 0xc5);
    _mtw_FModSound_stop(Globals::sound);

    void *app = Globals::appManager;
    optB->fadeValue = this->fadeValue;
    ModuleTransitionThunk *thunk = (ModuleTransitionThunk *) app;
    thunk->transitionFn(app, 5);
}

int MenuTouchWindow::OnTouchBegin(int y, int x, void *touchId) {
    if (this->messageShowing != 0) {
        _mtw_ChoiceWindow_OnTouchBegin(this->choiceWindow, y);
        return 0;
    }

    Layout *layout = (Layout *) Globals::layout;
    if (layout->choiceWindowOpen != 0) {
        _mtw_Layout_OnTouchBegin(layout, y);
        return 0;
    }

    int state = this->menuState;
    switch (state) {
        case 1:
        case 2: {
            this->dragStartX = x;
            this->dragLastX = x;
            this->dragVelocity = 0;
            this->dragging = 1;
            int oldRow = this->selectedRow;
            int leftMargin = layout->field_0xc_leftMargin;
            if (leftMargin < x && x < Globals::h - layout->field_0x10_rightMargin) {
                int rowH = layout->field_0x70_rowHeight;
                int gap = this->listRowGap;
                int top = layout->field_0x20_top;
                int off = this->scrollOffset;
                int rowStep = rowH + gap;
                int row = _mtw_idiv(x + ((-top - leftMargin) - off), rowStep);
                if (row < (int) (uintptr_t) Globals::recordSlots) {
                    this->selectedRow = row;
                    float v = _mtw_TouchButton_setPosition(this->okButton,
                                                           (Globals::w - this->listX) - layout->buttonInsetX,
                                                           row * rowStep + top + leftMargin + off +
                                                           layout->field_0x108);
                    _mtw_FModSound_play(Globals::sound, 0x7c, 0, v);
                }
            }
            if (oldRow == this->selectedRow)
                _mtw_TouchButton_OnTouchBegin(this->okButton, y);
        }
        break;
        case 3: {
            if (Globals::iPad == 0) {
                void **arr = (void **) this->optionsButtons;
                for (unsigned int i = 0; i < *(unsigned int *) arr; i++)
                    _mtw_TouchButton_OnTouchBegin(((void **) arr[1])[i], y);
                break;
            }
            _mtw_TouchButton_OnTouchBegin(this->scrollUpButton, y);
            this->upButtonPressed = 0;
            int b28 = layout->buttonInsetX;
            int top = b28 + this->listTopY;
            if (top < y && y < this->listEntryHeight + top &&
                layout->field_0xc_leftMargin + b28 < x &&
                x < layout->field_0xc_leftMargin + layout->field_0x20_top + this->listEntryWidth) {
                this->upButtonPressed = 1;
                _mtw_FModSound_play(Globals::sound, 0x7c, 0, 0);
                b28 = layout->buttonInsetX;
                top = b28 + this->listTopY;
            }
            int bottom = top + this->listEntryHeight;
            if (bottom < y && y < (this->listTopY - b28) + this->listBottomY &&
                b28 + layout->field_0xc_leftMargin < x &&
                x < layout->field_0x20_top + layout->field_0xc_leftMargin + this->listEntryWidth) {
                this->downButtonPressed = 1;
                _mtw_FModSound_play(Globals::sound, 0x7c, 0, 0);
            }
            _mtw_TouchButton_OnTouchBeginXY(this->optBtnCC, y, x);
            _mtw_TouchButton_OnTouchBeginXY(this->optBtnD0, y, x);
            _mtw_TouchSlider_OnTouchBegin(*(void **) ((Array<TouchSlider *> *) this->sliders)->data_, y);
            _mtw_TouchButton_OnTouchBeginXY(this->optBtnD4, y, x);
            _mtw_TouchButton_OnTouchBeginXY(this->optBtnD8, y, x);
            _mtw_TouchButton_OnTouchBeginXY(this->optBtnDC, y, x);
            void **arr = (void **) this->sliders;
            for (unsigned int i = 1; i < *(unsigned int *) arr; i++)
                _mtw_TouchSlider_OnTouchBegin(((void **) arr[1])[i], y);
            if (Globals::iPadLargePossible != 0 && this->scrollExtraButton != 0)
                _mtw_TouchButton_OnTouchBegin(this->scrollExtraButton, y);
        }
        break;
        case 4: {
            _mtw_ScrollTouchWindow_OnTouchBegin(this->scrollWindowA, y);
            void **arr = (void **) this->scrollEntries;
            for (unsigned int i = 0; i < *(unsigned int *) arr; i++) {
                unsigned int *e = (unsigned int *) ((void **) arr[1])[i];
                unsigned int t = e[0];
                if (((unsigned int) (t - 0x6a) < 5 && ((1u << ((t - 0x6a) & 0xff)) & 0x19u) != 0)
                    || (e[1] == 0 && t == 0x16))
                    _mtw_TouchButton_OnTouchBegin(e, y);
            }
        }
        break;
        case 6:
        case 0xd:
            break;
        case 7: {
            _mtw_TouchButton_OnTouchBeginXY(this->optBtnD4, y, x);
            _mtw_TouchButton_OnTouchBeginXY(this->optBtnD8, y, x);
            _mtw_TouchButton_OnTouchBeginXY(this->optBtnDC, y, x);
            void **arr = (void **) this->sliders;
            unsigned int n = *(unsigned int *) arr;
            for (unsigned int i = 1; i < n; i++) {
                if (i == 5 && ((Layout *) Globals::layout)->field_0x284_sliderSlot5Enabled == 0) continue;
                _mtw_TouchSlider_OnTouchBegin(((void **) arr[1])[i], y);
            }
        }
        break;
        case 8: {
            this->upButtonPressed = 0;
            int b28 = layout->buttonInsetX;
            int top = b28 + this->listTopY;
            if (top < y && y < this->listEntryHeight + top &&
                layout->field_0xc_leftMargin + b28 < x &&
                x < layout->field_0xc_leftMargin + layout->field_0x20_top + this->listEntryWidth) {
                this->upButtonPressed = 1;
                _mtw_FModSound_play(Globals::sound, 0x7c, 0, 0);
                layout = (Layout *) Globals::layout;
                b28 = layout->buttonInsetX;
                top = b28 + this->listTopY;
            }
            int bottom = top + this->listEntryHeight;
            if (bottom < y && y < (this->listTopY - b28) + this->listBottomY &&
                b28 + layout->field_0xc_leftMargin < x &&
                x < layout->field_0x20_top + layout->field_0xc_leftMargin + this->listEntryWidth) {
                this->downButtonPressed = 1;
                _mtw_FModSound_play(Globals::sound, 0x7c, 0, 0);
            }
            _mtw_TouchButton_OnTouchBegin(this->optBtnCC, y);
            _mtw_TouchButton_OnTouchBegin(this->optBtnD0, y);
            _mtw_TouchSlider_OnTouchBegin(*(void **) ((Array<TouchSlider *> *) this->sliders)->data_, y);
        }
        break;
        case 9:
            _mtw_MissionsWindow_OnTouchBegin(this->missionsWindow, y);
            break;
        case 0xb: {
            if (Globals::iPad != 0 && Globals::iPadAssetsWithLowerRes == 0) {
                _mtw_TouchButton_OnTouchBegin(this->cinematicBtnA, y);
                _mtw_TouchButton_OnTouchBegin(this->cinematicBtnB, y);
                GameSettings *steerCtl = (GameSettings *) Globals::options;
                GameSettings *fireCtl = (GameSettings *) Globals::options;
                if (touchId != 0 &&
                    (this->cinematicTouchIdA != 0 || y > 0xd1 || this->cinematicTouchIdB == touchId ||
                     x <= steerCtl->steerAnchorX - 0x14 ||
                     steerCtl->steerAnchorX + 300 <= x)) {
                    if (this->cinematicTouchIdB == touchId || touchId == 0 ||
                        this->cinematicTouchIdB != 0 || y <= Globals::h - 0xdc ||
                        x <= fireCtl->fireAnchorX - 0x14 ||
                        fireCtl->fireAnchorX + 0xe6 <= x) {
                        this->cinematicTouchState = 0;
                    } else {
                        this->cinematicTouchIdB = touchId;
                        this->cinematicTouchState = 0x100;
                        this->cinematicAnchorB = x;
                    }
                } else {
                    this->cinematicTouchIdA = touchId;
                    this->cinematicTouchState = 1;
                    this->cinematicAnchorA = x;
                }
            }
        }
        break;
        case 0xc: {
            void **arr = (void **) this->buttonsB4;
            for (unsigned int i = 0; i < *(unsigned int *) arr; i++)
                _mtw_TouchButton_OnTouchBegin(((void **) arr[1])[i], y);
        }
        break;
        case 0xe: {
            void **arr = (void **) this->buttonsB0;
            for (unsigned int i = 0; i < *(unsigned int *) arr; i++)
                _mtw_TouchButton_OnTouchBegin(((void **) arr[1])[i], y);
        }
        break;
        case 0x10: {
            void **arr = (void **) this->scrollEntries;
            for (unsigned int i = 0; i < *(unsigned int *) arr; i++) {
                unsigned int *e = (unsigned int *) ((void **) arr[1])[i];
                if ((unsigned int) (e[0] - 0x65) < 5 && e[1] == 0)
                    _mtw_TouchButton_OnTouchBegin(e, y);
            }
        }
            [[fallthrough]];
        case 0xf: {
            _mtw_ScrollTouchWindow_OnTouchBegin(this->scrollWindowB, y);
            void **arr = (void **) this->scrollEntries;
            for (unsigned int i = 0; i < *(unsigned int *) arr; i++) {
                unsigned int *e = (unsigned int *) ((void **) arr[1])[i];
                if ((e[0] | 8) == 0x3c && e[1] == 0)
                    _mtw_TouchButton_OnTouchBegin(e, y);
            }
            void **slots = (void **) this->scrollSlots;
            for (unsigned int i = 0; i < *(unsigned int *) slots; i++)
                _mtw_TouchButton_OnTouchBegin(((void **) slots[1])[i], y);

            int bound = Globals::w;
            int b28 = ((Layout *) Globals::layout)->buttonInsetX;
            PaintCanvas *canvas = (PaintCanvas *) Globals::Canvas;
            int iw = canvas->GetImage2DWidth(this->scrollbarImageId);
            unsigned char hit;
            if ((bound - b28) - iw < y) {
                int lc = ((Layout *) Globals::layout)->field_0xc_leftMargin;
                int tp = ((Layout *) Globals::layout)->field_0x20_top;
                int ih = canvas->GetImage2DHeight(this->scrollbarImageId);
                hit = (x < ih + tp + lc) ? 1 : 0;
            } else hit = 0;
            this->scrollbarHit = hit;

            b28 = ((Layout *) Globals::layout)->buttonInsetX;
            iw = canvas->GetImage2DWidth(this->scrollbarImageId);
            if (y < iw + b28) {
                this->dragStartX = x;
                this->dragLastX = x;
                this->dragVelocity = 0;
                this->dragging = 1;
            }
        }
        break;
        case 0x11: {
            void **arr = (void **) this->buttonsB8;
            for (unsigned int i = 0; i < *(unsigned int *) arr; i++)
                _mtw_TouchButton_OnTouchBegin(((void **) arr[1])[i], y);
        }
        break;
        default: {
            void **arr = (void **) this->buttons;
            for (unsigned int i = 0; i < *(unsigned int *) arr; i++)
                _mtw_TouchButton_OnTouchBegin(((void **) arr[1])[i], y);
            void **arr2 = (void **) this->scrollEntries;
            unsigned int n = *(unsigned int *) arr2;
            for (unsigned int i = 0; i < n; i++) {
                int *e = (int *) ((void **) arr2[1])[i];
                if ((unsigned int) (e[0] - 0x17) < 2) _mtw_TouchButton_OnTouchBegin(e, y);
            }
            for (unsigned int i = 0; i < n; i++) {
                unsigned int *e = (unsigned int *) ((void **) arr2[1])[i];
                unsigned int t = e[0], id = e[1];
                bool go;
                if (t == 5 && id == 0) go = true;
                else {
                    unsigned int x2 = (t != 0x11 || id != 0) ? (t ^ 100) : 0;
                    go = (id == 0 && (t == 0x11 || x2 == 0)) || (id == 0 && t == 0x35);
                }
                if (go) {
                    _mtw_TouchButton_OnTouchBegin(e, y);
                    arr2 = (void **) this->scrollEntries;
                    n = *(unsigned int *) arr2;
                }
            }
        }
        break;
    }

    int r = _mtw_Layout_OnTouchBegin(layout, y);
    if (r != 0 && this->menuState == 0xd)
        this->pendingActivate = 1;
    return 0;
}

int MenuTouchWindow::loadGame(int slot) {
    void *rh = ::operator new(sizeof(RecordHandler));
    _mtw_RecordHandler_ctor(rh);
    void *rec = _mtw_RecordHandler_readRecord(rh, slot);

    if (rec == 0) {
        _mtw_Status_resetGame();
        void *cw = this->choiceWindow;
        void *s = _mtw_GameText_getText(Globals::gameText, 0x64);
        _mtw_ChoiceWindow_set(cw, s, false);
        this->loadFailedDialogShowing = 1;
        this->messageShowing = 1;
        ::operator delete(_mtw_RecordHandler_dtor(rh));
        return 0;
    }

    Status *flags = Globals::status;
    GameRecord *record = (GameRecord *) rec;
    bool versionOk = (record->versionMismatchFlag == 0) || (flags->flag_0x37 != 0);
    if (versionOk) {
        bool dlcOk = (record->dlcRequiredFlag == 0) || (flags->flag_0x35 != 0);
        if (dlcOk) {
            _mtw_Status_resetGame();
            _mtw_GameRecord_load(rec);
            ::operator delete(_mtw_RecordHandler_dtor(rh));
            ::operator delete(_mtw_GameRecord_dtor(rec));
            void *app = Globals::appManager;
            void *ms = _mtw_AppMgr_GetApplicationModule(app, 5);
            ((ModStation *) ms)->setGameLoaded();
            Globals::switch_to_target_setting = 0;
            _mtw_AppMgr_SetCurrentApplicationModule(app, 5);
            return 1;
        }

        void *cw = this->choiceWindow;
        void *s = _mtw_GameText_getText(Globals::gameText, 0x65);
        _mtw_ChoiceWindow_set(cw, s, false);
        this->messageShowing = 1;
        ::operator delete(_mtw_GameRecord_dtor(rec));
        ::operator delete(_mtw_RecordHandler_dtor(rh));
        return 0;
    }

    void *cw = this->choiceWindow;
    void *s = _mtw_GameText_getText(Globals::gameText, 0x66);
    _mtw_ChoiceWindow_set(cw, s, false);
    this->messageShowing = 1;
    ::operator delete(_mtw_GameRecord_dtor(rec));
    ::operator delete(_mtw_RecordHandler_dtor(rh));
    return 0;
}

static inline void _mtw_TouchButton_ctor(void *btn, void *label, int a, int x, int y, int w,
                                         char type, char id) {
    new(btn) TouchButton(*(String *) label, a, x, y, w, (unsigned char) type, (unsigned char) id);
}

void MenuTouchWindow::addButton(int id, AbyssEngine::String label, int row, Array<TouchButton *> *arr, int yOff) {
    void *btn = ::operator new(sizeof(TouchButton));

    int btnW = this->buttonWidth;
    int screenW = Globals::w;
    int screenH = Globals::h;
    int rowH = ((Layout *) Globals::layout)->field_0x30_rowHeight;

    int x = screenW / 2 - btnW / 2;
    int y = (rowH + this->buttonRowGap) * row + (yOff - this->buttonYBias) + screenH / 2;

    _mtw_TouchButton_ctor(btn, &label, 0, x, y, btnW, 0x11, 0x04);
    ((TouchButton *) btn)->field_0x0 = id;
    ((TouchButton *) btn)->field_0x4 = id >> 31;
    ArrayAdd<TouchButton *>((TouchButton *) btn, *arr);

    int *posX = Globals::sub_menu_buttons_x;
    int *posY = Globals::sub_menu_buttons_y;
    for (uint32_t i = 0; i < arr->size_; i++) {
        if (i < 10) {
            char buf[12];
            TouchButton *b = arr->data_[i];
            _mtw_TouchButton_getPosition(buf, b);
            posX[i] = (int) *(float *) (buf + 0);
            b = arr->data_[i];
            _mtw_TouchButton_getPosition(buf, b);
            posY[i] = (int) *(float *) (buf + 4);
        }
    }
    Globals::sub_menu_button_count = arr->size_;
}

void MenuTouchWindow::setCutsceneMode(bool mode) {
    this->cutsceneMode = (uint8_t) mode;
    void **arr = (void **) this->buttons;
    for (uint32_t i = 0; i < *(uint32_t *) arr; i++) {
        TouchButton *btn = (TouchButton *) ((void **) arr[1])[i];
        if (btn->field_0x0 == 0x13 && btn->field_0x4 == 0) {
            _mtw_TouchButton_setVisible(btn, (bool) ((uint8_t) mode ^ 1));
        }
        arr = (void **) this->buttons;
    }
}

void MenuTouchWindow::loadPreviewRecords() {
    this->scrollOffset = 0;

    this->dragLastX = 0;
    this->dragVelocity = 0;
    this->inertiaDecay = 0;
    this->inertiaVel = 0.0f;
    this->dragStartX = 0;
    this->dragging = 0;

    Layout *layout = (Layout *) Globals::layout;
    int rowCount = (int) (uintptr_t) Globals::recordSlots;

    this->contentHeight = (((Globals::h - layout->field_0xc) - layout->windowTopInset)
                           - layout->field_0x20) - layout->field_0x24;

    this->pageHeight = rowCount * (layout->field_0x70_rowHeight + this->listRowGap);

    void *rec = this->previewRecords;
    if (rec != 0) {
        ::operator delete(_mtw_Array_GameRecord_dtor(rec));
        this->previewRecords = 0;
    }
    void *rh = ::operator new(sizeof(RecordHandler));
    _mtw_RecordHandler_ctor(rh);
    this->previewRecords = _mtw_RecordHandler_readAllPreviewRecords(rh);
    _mtw_RecordHandler_dtor(rh);
    ::operator delete(rh);
}

void MenuTouchWindow::saveGame(int slot) {
    void *rh = ::operator new(sizeof(RecordHandler));
    _mtw_RecordHandler_ctor(rh);
    _mtw_RecordHandler_recordStoreWrite(rh, slot);
    _mtw_RecordHandler_recordStoreWritePreview(rh, slot);

    GameRecord **arr = ((Array<GameRecord *> *) this->previewRecords)->data_;
    void *rec = arr[slot];
    GameRecord **cell;
    if (rec == 0) {
        cell = arr + slot;
    } else {
        ::operator delete(_mtw_GameRecord_dtor(rec));
        cell = &((Array<GameRecord *> *) this->previewRecords)->data_[slot];
    }
    *cell = 0;

    void *preview = _mtw_RecordHandler_recordStoreReadPreview(rh, slot);
    ((Array<GameRecord *> *) this->previewRecords)->data_[slot] = (GameRecord *) preview;
    ::operator delete(_mtw_RecordHandler_dtor(rh));

    this->createRecordButtons(true);
    void *cw = this->choiceWindow;
    void *s = _mtw_GameText_getText(Globals::gameText, 0x32);
    _mtw_ChoiceWindow_set(cw, s, false);
    this->saveDialogShowing = 0;
    this->messageShowing = 1;
}

static const int g_mtw_upTextIds[16] = {};

void MenuTouchWindow::update(int dt) {
    unsigned int st = this->menuState;
    if (st < 0x10 && ((1u << (st & 0xff)) & 0x8006u) != 0) {
        if (this->dragging == 0) {
            float v = *(float *) &this->inertiaDecay * this->inertiaVel;
            this->inertiaVel = v;
            if (fabsf(v) > 1.0f) {
                this->scrollOffset = (int) (v + (float) this->scrollOffset);
            }
        }
        int off = this->scrollOffset;
        if (off > 0) {
            this->inertiaDecay = 0x3f800000;
            this->inertiaVel = (float) (-off) * 0.5f;
        }
        int overshoot = this->contentHeight - this->pageHeight;
        if (overshoot < 0) {
            if (off < overshoot) {
                this->inertiaDecay = 0x3f800000;
                this->inertiaVel = (float) (overshoot - off) * 0.5f;
            }
        } else {
            this->inertiaVel = 0;
            this->scrollOffset = 0;
        }
    }

    MtwAppData *appData = (MtwAppData *) _mtw_AppMgr_GetApplicationData();
    unsigned char busy = this->dlcMessageShowing;

    if (busy != 0 || this->purchaseRestorePending != 0) {
        if (appData->purchaseReadyFlag != 0) {
            Layout *layout = (Layout *) Globals::layout;
            void *canvas = Globals::Canvas;
            this->contentHeight = (((Globals::h - layout->field_0x10_rightMargin)
                                    - layout->field_0xc) - layout->field_0x20) - layout->field_0x24;
            int ih = ((PaintCanvas *) canvas)->GetImage2DHeight(this->scrollbarImageId);
            this->contentHeightCache = ih;
            OptionsRecord *optObj = (OptionsRecord *) Globals::options;
            int rowH = layout->field_0x2c_rowHeight;
            this->menuState = 0xf;
            this->messageShowing = 0;
            this->dlcMessageShowing = 0;
            this->pageHeight = (ih + rowH) * 5;
            appData->purchaseReadyFlag = 0;
            optObj->flag_0x3b = 1;
            _mtw_RecordHandler_saveOptions(Globals::recordHandler);
            busy = this->dlcMessageShowing;
        }
        if (busy != 0) {
            if (appData->purchaseErrorFlag != 0) {
                void *cw = this->choiceWindow;
                void *s = _mtw_GameText_getText(Globals::gameText, g_mtw_upTextIds[0]);
                _mtw_ChoiceWindow_set(cw, s);
                this->dlcErrorDialogShowing = 1;
                this->messageShowing = 1;
                this->dlcMessageShowing = 0;
                appData->purchaseErrorFlag = 0;
                this->purchaseRestorePending = 0;
            }
        }
    }

    unsigned int code = appData->purchaseCode;
    bool handled = false;
    if (code <= 4 && appData->purchaseResultFlag != 0) {
        void *cw = this->choiceWindow;
        switch (code) {
            case 0: {
                Status *status = Globals::status;
                status->flag_0x35 = 1;
                _mtw_Status_setSystemVisibility(status, 0x19, true);
                _mtw_ChoiceWindow_set(cw, _mtw_GameText_getText(Globals::gameText, g_mtw_upTextIds[1]));
            }
            break;
            case 1: {
                Globals::status->mode_0x114 = 3;
                ((OptionsRecord *) Globals::options)->flag_0x36 = 1;
                _mtw_ChoiceWindow_set(cw, _mtw_GameText_getText(Globals::gameText, g_mtw_upTextIds[2]));
            }
            break;
            case 2: {
                Status *status = Globals::status;
                status->flag_0x37 = 1;
                _mtw_Status_setSystemVisibility(status, 0x19, true);
                _mtw_ChoiceWindow_set(cw, _mtw_GameText_getText(Globals::gameText, g_mtw_upTextIds[3]));
            }
            break;
            case 3: {
                ((OptionsRecord *) Globals::options)->flag_0x38 = 1;
                _mtw_ChoiceWindow_set(cw, _mtw_GameText_getText(Globals::gameText, g_mtw_upTextIds[4]));
            }
            break;
            case 4: {
                OptionsRecord *flags = (OptionsRecord *) Globals::options;
                Status *status = Globals::status;
                flags->flag_0x35 = 1;
                flags->flag_0x39 = 1;
                flags->flag_0x37 = 1;
                _mtw_Status_setSystemVisibility(status, 0x19, true);
                _mtw_ChoiceWindow_set(cw, _mtw_GameText_getText(Globals::gameText, g_mtw_upTextIds[5]));
            }
            break;
        }
        _mtw_RecordHandler_saveOptions(Globals::recordHandler);
        this->dlcResultDialogShowing = 1;
        this->dlcMessageShowing = 0;
        this->messageShowing = 1;
        appData->purchaseResultFlag = 0;
        handled = true;
    } else if (this->purchaseRestorePending != 0 && appData->purchaseResultFlag != 0) {
        _mtw_RecordHandler_saveOptions(Globals::recordHandler);
        if (this->purchaseRestorePending != 0) {
            void *cw = this->choiceWindow;
            _mtw_ChoiceWindow_set(cw, _mtw_GameText_getText(Globals::gameText, g_mtw_upTextIds[6]));
            this->purchaseRestorePending = 0;
        }
        this->dlcResultDialogShowing = 1;
        this->dlcMessageShowing = 0;
        this->messageShowing = 1;
        appData->purchaseResultFlag = 0;
        handled = true;
    }
    (void) handled;

    if (Globals::isTelekomCustomer != 0) {
        OptionsRecord *flags = (OptionsRecord *) Globals::options;
        if (this->messageShowing == 0 && flags->flag_0x35 == 0) {
            void *cw = this->choiceWindow;
            _mtw_ChoiceWindow_set(cw, _mtw_GameText_getText(Globals::gameText, g_mtw_upTextIds[7]));
            this->supernovaPurchaseDialogShowing = 1;
            this->messageShowing = 1;
            flags->flag_0x35 = 1;
            _mtw_Status_setSystemVisibility(Globals::status, 0x19, true);
            _mtw_RecordHandler_saveOptions(Globals::recordHandler);
        }
    }

    int state = this->menuState;
    if (state == 0) {
        if (this->backgroundEnabled == 1) {
            int rowGap = this->buttonRowGap;
            Layout *layout = (Layout *) Globals::layout;
            void **arr = (void **) this->buttons;
            unsigned int n = *(unsigned int *) arr;
            int total = n * layout->field_0x30_rowHeight;
            int screenH = Globals::h;
            for (unsigned int i = 0; i < n; i++) {
                _mtw_TouchButton_setYPosition(((void **) arr[1])[i],
                                              (layout->field_0x30_rowHeight + this->buttonRowGap) * i +
                                              (screenH / 2 - (int) ((unsigned int) (rowGap * (n - 1) + total) >> 1)));
                TouchButton *b = (TouchButton *) this->buttons->data_[i];
                if (b->field_0x0 == 0x12 && b->field_0x4 == 0 && _mtw_TouchButton_isVisible(b) != 0 &&
                    this->cutsceneMode != 0) {
                    b = (TouchButton *) this->buttons->data_[i];
                    char pos[12];
                    _mtw_TouchButton_getPosition(pos, b);
                    _mtw_TouchButton_setYPosition(b, (int) *(float *) (pos + 4));
                }
                arr = (void **) this->buttons;
                n = *(unsigned int *) arr;
            }
        }
    } else if (state == 3) {
        int rowGap = this->buttonRowGap;
        Layout *layout = (Layout *) Globals::layout;
        void **arr = (void **) this->optionsButtons;
        unsigned int n = *(unsigned int *) arr;
        int firstCount = *(int *) *(void **) this->buttons;
        int total = n * layout->field_0x30_rowHeight;
        int screenH = Globals::h;
        for (unsigned int i = 0; i < n; i++) {
            _mtw_TouchButton_setYPosition(((void **) arr[1])[i],
                                          (layout->field_0x30_rowHeight + this->buttonRowGap) * i +
                                          (screenH / 2 - (int) (
                                               (unsigned int) ((firstCount - 1) * rowGap + total) >> 1)));
            arr = (void **) this->optionsButtons;
            n = *(unsigned int *) arr;
        }
    } else if (state == 4) {
        _mtw_ScrollTouchWindow_update(this->scrollWindowA);
    } else if (state == 9) {
        if (this->missionsWindow != 0) _mtw_MissionsWindow_update(this->missionsWindow);
    } else if (state == 0xf) {
        _mtw_ScrollTouchWindow_update(this->scrollWindowB);
    }

    if (this->messageShowing != 0)
        _mtw_ChoiceWindow_update(this->choiceWindow);

    if (state == 0xd) {
        appData = (MtwAppData *) _mtw_AppMgr_GetApplicationData();
        if (appData->storeInitFlag != 0) {
            Engine *eng = (Engine *) _mtw_AppMgr_GetEngine();
            int r = eng->field_0x100;
            if (r == 2 || r == 1) {
                void *cw = this->choiceWindow;
                int id = (r == 2) ? g_mtw_upTextIds[8] : g_mtw_upTextIds[9];
                _mtw_ChoiceWindow_set(cw, _mtw_GameText_getText(Globals::gameText, id));
                this->storeInitDialogShowing = 0;
                this->messageShowing = 1;
                appData = (MtwAppData *) _mtw_AppMgr_GetApplicationData();
                appData->storeInitFlag = 0;
            }
        }
    }

    appData = (MtwAppData *) _mtw_AppMgr_GetApplicationData();
    if (appData->screenshotResultFlag != 0) {
        appData = (MtwAppData *) _mtw_AppMgr_GetApplicationData();
        int r = appData->storeResultCode;
        if (r == 2 || r == 1) {
            void *cw = this->choiceWindow;
            int id = (r == 2) ? g_mtw_upTextIds[10] : g_mtw_upTextIds[11];
            _mtw_ChoiceWindow_set(cw, _mtw_GameText_getText(Globals::gameText, id));
            this->screenshotState = -1;
            this->messageShowing = 1;
            appData = (MtwAppData *) _mtw_AppMgr_GetApplicationData();
            appData->screenshotResultFlag = 0;
        }
    }
}

void MenuTouchWindow::startSupernovaChallenge() {
    _mtw_startSupernovaChallenge_impl(this);
}

void MenuTouchWindow::callDlcMenu() {
    MtwAppData *ad = (MtwAppData *) _mtw_GetApplicationData(Globals::appManager);
    ad->dlcMenuAckFlag = 0;
    ad = (MtwAppData *) _mtw_GetApplicationData(Globals::appManager);
    void *gt = Globals::gameText;
    ad->dlcMenuRequestFlag = 1;
    void *win = this->choiceWindow;
    this->messageShowing = 1;
    this->dlcMessageShowing = 1;
    void *s1 = _mtw_GameText_getText(gt, 0x47);
    void *s2 = _mtw_GameText_getText(gt, 0x1a9);
    return _mtw_DlcMenu_call(win, s1, s2);
}

static inline void _mtw_draw_mainMenu(void *self) {
    auto *window = (MenuTouchWindow *) self;
    if (window->backgroundEnabled != 0) {
        _mtw_draw_header_text(window->backgroundEnabled == 1 ? 40 : 171);
    } else {
        _mtw_draw_main_logo(window);
    }

    _mtw_draw_button_array(window->buttons);

    if (window->scrollEntries == nullptr) return;
    for (unsigned int i = 0; i < window->scrollEntries->size(); i++) {
        auto *button = (TouchButton *) window->scrollEntries->data_[i];
        if (button == nullptr || button->field_0x4 != 0) continue;
        if (button->field_0x0 == 53 || button->field_0x0 == 23 || button->field_0x0 == 24)
            button->draw();
    }
}

static inline void _mtw_draw_loadSaveList(void *self) {
    auto *window = (MenuTouchWindow *) self;
    _mtw_Layout_drawBG();
    int textId = (window->menuState == 2) ? 30 : 29;
    auto *titleString = (String *) _mtw_GameText_getText(Globals::gameText, textId);
    if (titleString != nullptr) {
        ((Layout *) Globals::layout)->drawHeader(*titleString);
    } else {
        ((Layout *) Globals::layout)->drawHeader();
    }
    window->drawLoadSaveMenu(false);
}

static inline void _mtw_draw_opt_btn(void *btn) {
    if (btn != nullptr)
        ((TouchButton *) btn)->draw();
}

static inline void _mtw_draw_optionsTabs(void *self) {
    auto *window = (MenuTouchWindow *) self;
    _mtw_draw_header_text(500);
    _mtw_draw_button_array(window->optionsButtons);
    _mtw_draw_opt_btn(window->optBtnCC);
    _mtw_draw_opt_btn(window->optBtnD0);
    _mtw_draw_opt_btn(window->optBtnD4);
    _mtw_draw_opt_btn(window->optBtnD8);
    _mtw_draw_opt_btn(window->optBtnDC);
    if (window->scrollExtraButton != nullptr && Globals::iPadLargePossible != 0) {
        ((TouchButton *) window->scrollExtraButton)->draw();
    }
    if (window->sliders != nullptr) {
        auto *sliders = (Array<TouchSlider *> *) window->sliders;
        for (unsigned int i = 0; i < sliders->size(); i++) {
            if ((*sliders)[i] != nullptr)
                (*sliders)[i]->draw();
        }
    }
}

static inline void _mtw_draw_scrollA(void *self) {
    auto *window = (MenuTouchWindow *) self;
    _mtw_Layout_drawBG();
    _mtw_draw_header_text(43);
    if (window->scrollWindowA != nullptr)
        ((ScrollTouchWindow *) window->scrollWindowA)->draw();
    ((Layout *) Globals::layout)->drawEmptyFooter(true);

    if (window->scrollEntries == nullptr) return;
    for (unsigned int i = 0; i < window->scrollEntries->size(); i++) {
        auto *button = (TouchButton *) window->scrollEntries->data_[i];
        if (button == nullptr || button->field_0x4 != 0) continue;
        unsigned int id = (unsigned int) button->field_0x0;
        if (((id - 106u) <= 4u && ((1u << ((id - 106u) & 0xffu)) & 0x19u) != 0) || id == 22u)
            button->draw();
    }
}

static inline void _mtw_draw_audioOptions(void *self) {
    auto *window = (MenuTouchWindow *) self;
    _mtw_draw_header_text(489);
    _mtw_draw_opt_btn(window->optBtnD4);
    _mtw_draw_opt_btn(window->optBtnD8);
    _mtw_draw_opt_btn(window->optBtnDC);
    if (window->sliders != nullptr) {
        auto *sliders = (Array<TouchSlider *> *) window->sliders;
        for (unsigned int i = 1; i < sliders->size(); i++) {
            if (i == 5 && ((Layout *) Globals::layout)->field_0x284_sliderSlot5Enabled == 0) continue;
            if ((*sliders)[i] != nullptr)
                (*sliders)[i]->draw();
        }
    }
}

static inline void _mtw_draw_keyConfig(void *self) {
    auto *window = (MenuTouchWindow *) self;
    _mtw_draw_header_text(498);
    _mtw_draw_opt_btn(window->optBtnCC);
    _mtw_draw_opt_btn(window->optBtnD0);
    if (window->sliders != nullptr) {
        auto *sliders = (Array<TouchSlider *> *) window->sliders;
        if (sliders->size() > 0) {
            auto *s0 = (TouchSlider *) sliders->data_[0];
            if (s0 != nullptr)
                s0->draw();
        }
    }
}

static inline void _mtw_draw_missions(void *self) {
    auto *window = (MenuTouchWindow *) self;
    if (window->missionsWindow != nullptr) {
        ((MissionsWindow *) window->missionsWindow)->draw();
    }
}

static inline void _mtw_draw_cinematic(void *self) {
    auto *window = (MenuTouchWindow *) self;
    _mtw_draw_header_text(494);
    if (window->scrollWindowA != nullptr)
        ((ScrollTouchWindow *) window->scrollWindowA)->draw();
    if (Globals::iPad != 0 && Globals::iPadAssetsWithLowerRes == 0) {
        _mtw_draw_opt_btn(window->cinematicBtnA);
        _mtw_draw_opt_btn(window->cinematicBtnB);
    }
}

static inline void _mtw_draw_buttonsB4(void *self) {
    auto *window = (MenuTouchWindow *) self;
    _mtw_draw_ipad_logo(window);
    _mtw_draw_header_text(517);
    _mtw_draw_button_array(window->buttonsB4);
}

static inline void _mtw_draw_store(void *self) {
    auto *window = (MenuTouchWindow *) self;
    if (window->scrollWindowB != nullptr)
        ((ScrollTouchWindow *) window->scrollWindowB)->draw();
}

static inline void _mtw_draw_buttonsB0(void *self) {
    auto *window = (MenuTouchWindow *) self;
    _mtw_draw_ipad_logo(window);
    _mtw_draw_header_text(0);

    PaintCanvas *canvas = (PaintCanvas *) Globals::Canvas;
    unsigned char originalRtl = canvas->field_0x1c;
    int language = GameText::getLanguage();

    if (window->buttonsB0 == nullptr) return;
    for (unsigned int i = 0; i < window->buttonsB0->size(); i++) {
        auto *button = (TouchButton *) window->buttonsB0->data_[i];
        if (button == nullptr) continue;

        bool flipForDraw = false;
        bool restoreValue = originalRtl != 0;
        if (button->field_0x4 == 0) {
            unsigned int id = (unsigned int) button->field_0x0;
            if (id == 34u) {
                flipForDraw = true;
                restoreValue = language != 9;
                canvas->field_0x1c = (unsigned char) !restoreValue;
            } else if ((id - 33u) <= 4u && ((1u << ((id - 33u) & 0xffu)) & 0x19u) != 0 &&
                       language == 9) {
                flipForDraw = true;
                restoreValue = false;
                canvas->field_0x1c = 1;
            }
        }

        button->draw();
        if (flipForDraw)
            canvas->field_0x1c = (unsigned char) restoreValue;
    }
    canvas->field_0x1c = originalRtl;
}

static inline void _mtw_draw_scrollB(void *self) {
    auto *window = (MenuTouchWindow *) self;
    if (window->scrollEntries == nullptr) return;
    for (unsigned int i = 0; i < window->scrollEntries->size(); i++) {
        auto *button = (TouchButton *) window->scrollEntries->data_[i];
        if (button == nullptr || button->field_0x4 != 0) continue;
        unsigned int id = (unsigned int) button->field_0x0;
        if ((id - 101u) <= 4u || id == 60u || id == 52u)
            button->draw();
    }
}

static inline void _mtw_draw_buttonsB8(void *self) {
    auto *window = (MenuTouchWindow *) self;
    _mtw_draw_ipad_logo(window);
    _mtw_draw_header_text(103);
    _mtw_draw_button_array(window->buttonsB8);

    if (window->buttonsB8 == nullptr || window->buttonsB8->size() == 0) return;
    uint32_t badgeImage = _mtw_u32(window, 0x128);
    if (badgeImage == UINT32_MAX) return;

    auto *firstButton = (TouchButton *) window->buttonsB8->data_[0];
    if (firstButton == nullptr) return;

    Vector pos = firstButton->getPosition();
    int x = (int) ((pos.x + (float) firstButton->getWidth()) - (float) (20 * ((Layout *) Globals::layout)->field_0x44));
    int y = (int) (pos.y + (float) (5 * ((Layout *) Globals::layout)->field_0x44));
    ((PaintCanvas *) Globals::Canvas)->DrawImage2D(badgeImage, x, y, 0x44u);
}

void MenuTouchWindow::draw() {
    Globals::is_menu_visible = 1;
    int state = this->menuState;
    Globals::topMenuIndex = state;

    Array<void *> *btnArr = this->buttons;
    if (state == 0xc) btnArr = this->buttonsB4;
    if (state == 0xe) btnArr = this->buttonsB0;
    void **arr = (void **) btnArr;

    int *posX = Globals::sub_menu_buttons_x;
    int *posY = Globals::sub_menu_buttons_y;
    for (unsigned int i = 0; i < *(unsigned int *) arr; i++) {
        if (i < 10) {
            char buf[12];
            _mtw_TouchButton_getPosition(buf, ((void **) arr[1])[i]);
            posX[i] = (int) *(float *) (buf + 0);
            _mtw_TouchButton_getPosition(buf, ((void **) arr[1])[i]);
            posY[i] = (int) *(float *) (buf + 4);
        }
    }
    Globals::sub_menu_button_count = *(unsigned int *) arr;

    if (this->backgroundEnabled != 0) {
        unsigned int s = this->menuState;
        if ((s | 4) != 0xd) {
            _mtw_Layout_drawBG();
        }
    }

    switch (this->menuState) {
        case 0: _mtw_draw_mainMenu(this);
            break;
        case 1:
        case 2: _mtw_draw_loadSaveList(this);
            break;
        case 3: _mtw_draw_optionsTabs(this);
            break;
        case 4: _mtw_draw_scrollA(this);
            break;
        case 7: _mtw_draw_audioOptions(this);
            break;
        case 8: _mtw_draw_keyConfig(this);
            break;
        case 9: _mtw_draw_missions(this);
            break;
        case 0xb: _mtw_draw_cinematic(this);
            break;
        case 0xc: _mtw_draw_buttonsB4(this);
            break;
        case 0xd: _mtw_draw_store(this);
            break;
        case 0xe: _mtw_draw_buttonsB0(this);
            break;
        case 0xf:
        case 0x10: _mtw_draw_scrollB(this);
            break;
        case 0x11: _mtw_draw_buttonsB8(this);
            break;
        default: break;
    }

    _mtw_draw_footer_tail(this);
}

float MenuTouchWindow::getRelativeScrollHeight() {
    int content = this->contentHeight;
    int page = this->pageHeight;
    if (page < content)
        return 0.0f;
    int off = this->scrollOffset;
    int numer;
    if (off >= 1) {
        numer = content - off;
    } else {
        if (content - page <= off) {
            return (float) content / (float) page;
        }
        numer = off + page;
    }
    return (float) numer / (float) page;
}

void _mtw_steerFromTouchId(void *self, int y, int x, void *touchId);

void _mtw_Layout_OnTouchMove(void *layout, int y);

int MenuTouchWindow::OnTouchMove(int y, int x, void *touchId) {
    if (this->messageShowing != 0) {
        _mtw_ChoiceWindow_OnTouchMove(this->choiceWindow, y);
        return 0;
    }
    Layout *layout = (Layout *) Globals::layout;
    if (layout->choiceWindowOpen != 0) {
        _mtw_Layout_OnTouchMove(layout, y);
        return 0;
    }

    int state = this->menuState;
    switch (state - 1) {
        case 0:
        case 1:
            if (layout->field_0xc < x &&
                x < Globals::h - layout->field_0x10_rightMargin) {
                int dx = x - this->dragLastX;
                this->dragLastX = x;
                this->dragVelocity = dx;
                this->inertiaDecay = 0x3f800000;
                this->scrollOffset = this->scrollOffset + dx;
            }
            _mtw_TouchButton_OnTouchMove(this->okButton, y);
            break;
        case 2: {
            if (Globals::iPad == 0) {
                void **arr = (void **) this->optionsButtons;
                for (unsigned int i = 0; i < *(unsigned int *) arr; i++)
                    _mtw_TouchButton_OnTouchMove(((void **) arr[1])[i], y);
            } else {
                _mtw_TouchButton_OnTouchMove(this->okButton, y);
                this->upButtonPressed = 0;
                int base = layout->buttonInsetX;
                int top = base + this->listTopY;
                int bottom = this->listEntryHeight + top;
                if (top < y && y < bottom &&
                    layout->field_0xc + base < x &&
                    x < layout->field_0xc + layout->field_0x20_top + this->listEntryWidth)
                    this->upButtonPressed = 1;
                if (bottom < y && y < (this->listTopY - base) + this->listBottomY &&
                    base + layout->field_0xc < x &&
                    x < layout->field_0x20_top + layout->field_0xc + this->listEntryWidth)
                    this->downButtonPressed = 1;
                _mtw_TouchButton_OnTouchMoveXY(this->optBtnCC, y, x);
                _mtw_TouchButton_OnTouchMoveXY(this->optBtnD0, y, x);
                _mtw_TouchSlider_OnTouchMove(*(void **) ((Array<TouchSlider *> *) this->sliders)->data_, y);
                _mtw_TouchButton_OnTouchMoveXY(this->optBtnD4, y, x);
                _mtw_TouchButton_OnTouchMoveXY(this->optBtnD8, y, x);
                _mtw_TouchButton_OnTouchMoveXY(this->optBtnDC, y, x);
                {
                    void *fmod = Globals::sound;
                    int *sl = (int *) ((Array<TouchSlider *> *) this->sliders)->data_;
                    if (_mtw_FModSound_tryToStopMusicForBGMusic() == 0)
                        _mtw_FModSound_setVolume(fmod, _mtw_TouchSlider_getValue(((void **) sl)[1]));
                    _mtw_FModSound_setVolume(fmod, _mtw_TouchSlider_getValue(((void **) sl)[2]));
                    _mtw_FModSound_setVolume(fmod, _mtw_TouchSlider_getValue(((void **) sl)[3]));
                }
                void **arr = (void **) this->sliders;
                for (unsigned int i = 1; i < *(unsigned int *) arr; i++)
                    _mtw_TouchSlider_OnTouchMove(((void **) arr[1])[i], y);
                if (Globals::iPadLargePossible != 0 && this->okButton != 0)
                    _mtw_TouchButton_OnTouchMove(this->okButton, y);
            }
        }
        break;
        case 3: {
            _mtw_ScrollTouchWindow_OnTouchMove(this->scrollWindowA, y);
            void **arr = (void **) this->scrollEntries;
            for (unsigned int i = 0; i < *(unsigned int *) arr; i++) {
                unsigned int *e = (unsigned int *) ((void **) arr[1])[i];
                unsigned int t = e[0];
                if (((unsigned int) (t - 0x6a) < 5 && ((1u << ((t - 0x6a) & 0xff)) & 0x19u) != 0)
                    || (e[1] == 0 && t == 0x16))
                    _mtw_TouchButton_OnTouchMove(e, y);
            }
        }
        break;
        case 5:
        case 0xc:
            break;
        case 6:
            _mtw_TouchButton_OnTouchMoveXY(this->optBtnD4, y, x);
            _mtw_TouchButton_OnTouchMoveXY(this->optBtnD8, y, x);
            _mtw_TouchButton_OnTouchMoveXY(this->optBtnDC, y, x);
            {
                void *fmod = Globals::sound;
                int *sl = (int *) ((Array<TouchSlider *> *) this->sliders)->data_;
                if (_mtw_FModSound_tryToStopMusicForBGMusic() == 0)
                    _mtw_FModSound_setVolume(fmod, _mtw_TouchSlider_getValue(((void **) sl)[1]));
                _mtw_FModSound_setVolume(fmod, _mtw_TouchSlider_getValue(((void **) sl)[2]));
                _mtw_FModSound_setVolume(fmod, _mtw_TouchSlider_getValue(((void **) sl)[3]));
            }
            {
                void **arr = (void **) this->sliders;
                for (unsigned int i = 1; i < *(unsigned int *) arr; i++) {
                    if (i == 5 && ((Layout *) Globals::layout)->field_0x284_sliderSlot5Enabled == 0) continue;
                    _mtw_TouchSlider_OnTouchMove(((void **) arr[1])[i], y);
                }
            }
            break;
        case 7: {
            this->upButtonPressed = 0;
            int base = layout->buttonInsetX;
            int top = base + this->listTopY;
            int bottom = this->listEntryHeight + top;
            if (top < y && y < bottom &&
                layout->field_0xc + base < x &&
                x < layout->field_0xc + layout->field_0x20_top + this->listEntryWidth)
                this->upButtonPressed = 1;
            if (bottom < y && y < (this->listTopY - base) + this->listBottomY &&
                base + layout->field_0xc < x &&
                x < layout->field_0x20_top + layout->field_0xc + this->listEntryWidth)
                this->downButtonPressed = 1;
            _mtw_TouchButton_OnTouchMove(this->optBtnCC, y);
            _mtw_TouchButton_OnTouchMove(this->optBtnD0, y);
            _mtw_TouchSlider_OnTouchMove(*(void **) ((Array<TouchSlider *> *) this->sliders)->data_, y);
        }
        break;
        case 8:
            _mtw_MissionsWindow_OnTouchMove(this->missionsWindow, y);
            break;
        case 0xa:
            _mtw_steerFromTouchId(this, y, x, touchId);
            break;
        case 0xb: {
            void **arr = (void **) this->buttonsB4;
            for (unsigned int i = 0; i < *(unsigned int *) arr; i++)
                _mtw_TouchButton_OnTouchMove(((void **) arr[1])[i], y);
        }
        break;
        case 0xd: {
            void **arr = (void **) this->buttonsB0;
            for (unsigned int i = 0; i < *(unsigned int *) arr; i++)
                _mtw_TouchButton_OnTouchMove(((void **) arr[1])[i], y);
        }
        break;
        case 0xe: {
            Layout *slayout = (Layout *) Globals::layout;

            _mtw_ScrollTouchWindow_OnTouchMove(this->scrollWindowB, y);

            void **arr = (void **) this->scrollEntries;
            for (unsigned int i = 0; i < *(unsigned int *) arr; i++) {
                unsigned int *e = (unsigned int *) ((void **) arr[1])[i];
                if ((e[0] | 8) == 0x3c && e[1] == 0)
                    _mtw_TouchButton_OnTouchMove(e, y);
            }

            void **slots = (void **) this->scrollSlots;
            for (unsigned int i = 0; i < *(unsigned int *) slots; i++)
                _mtw_TouchButton_OnTouchMove(((void **) slots[1])[i], y);

            if (slayout->field_0xc < x &&
                x < Globals::h - slayout->field_0x10_rightMargin) {
                int b28 = slayout->buttonInsetX;
                int iw = ((PaintCanvas *) Globals::Canvas)->GetImage2DWidth(*(unsigned int *) this->heapBufA);
                if (y < iw + b28) {
                    int dx = x - this->dragLastX;
                    this->dragLastX = x;
                    this->dragVelocity = dx;
                    this->inertiaDecay = 0x3f800000;
                    this->scrollOffset = this->scrollOffset + dx;
                }
            }
            break;
        }
        case 0xf: {
            void **arr = (void **) this->scrollEntries;
            for (unsigned int i = 0; i < *(unsigned int *) arr; i++) {
                unsigned int *e = (unsigned int *) ((void **) arr[1])[i];
                if ((unsigned int) (e[0] - 0x65) < 5)
                    _mtw_TouchButton_OnTouchMove(e, y);
            }
            {
                Layout *slayout = (Layout *) Globals::layout;

                _mtw_ScrollTouchWindow_OnTouchMove(this->scrollWindowB, y);

                void **arr2 = (void **) this->scrollEntries;
                for (unsigned int i = 0; i < *(unsigned int *) arr2; i++) {
                    unsigned int *e = (unsigned int *) ((void **) arr2[1])[i];
                    if ((e[0] | 8) == 0x3c && e[1] == 0)
                        _mtw_TouchButton_OnTouchMove(e, y);
                }

                void **slots = (void **) this->scrollSlots;
                for (unsigned int i = 0; i < *(unsigned int *) slots; i++)
                    _mtw_TouchButton_OnTouchMove(((void **) slots[1])[i], y);

                if (slayout->field_0xc < x &&
                    x < Globals::h - slayout->field_0x10_rightMargin) {
                    int b28 = slayout->buttonInsetX;
                    int iw = ((PaintCanvas *) Globals::Canvas)->GetImage2DWidth(*(unsigned int *) this->heapBufA);
                    if (y < iw + b28) {
                        int dx = x - this->dragLastX;
                        this->dragLastX = x;
                        this->dragVelocity = dx;
                        this->inertiaDecay = 0x3f800000;
                        this->scrollOffset = this->scrollOffset + dx;
                    }
                }
            }
        }
        break;
        case 0x10: {
            void **arr = (void **) this->buttonsB8;
            for (unsigned int i = 0; i < *(unsigned int *) arr; i++)
                _mtw_TouchButton_OnTouchMove(((void **) arr[1])[i], y);
        }
        break;
        default: {
            void **arr = (void **) this->buttons;
            for (unsigned int i = 0; i < *(unsigned int *) arr; i++)
                _mtw_TouchButton_OnTouchMove(((void **) arr[1])[i], y);
            void **arr2 = (void **) this->scrollEntries;
            unsigned int n = *(unsigned int *) arr2;
            for (unsigned int i = 0; i < n; i++) {
                int *e = (int *) ((void **) arr2[1])[i];
                if ((unsigned int) (e[0] - 0x17) < 2)
                    _mtw_TouchButton_OnTouchBegin(e, y);
            }
            for (unsigned int i = 0; i < n; i++) {
                unsigned int *e = (unsigned int *) ((void **) arr2[1])[i];
                unsigned int t = e[0], id = e[1];
                bool hit;
                if (t == 5 && id == 0) hit = true;
                else {
                    unsigned int x2 = (t != 0x11 || id != 0) ? (t ^ 100) : 0;
                    hit = (id == 0 && (t == 0x11 || x2 == 0)) || (id == 0 && t == 0x35);
                }
                if (hit) {
                    _mtw_TouchButton_OnTouchMove(e, y);
                    arr2 = (void **) this->scrollEntries;
                    n = *(unsigned int *) arr2;
                }
            }
        }
        break;
    }

    _mtw_Layout_OnTouchMove(Globals::layout, y);
    return 0;
}

MenuTouchWindow::MenuTouchWindow(int menuType) {
    Layout *layout = (Layout *) Globals::layout;
    this->buttonWidth = layout->field_0x294_buttonWidth;
    this->buttonYBias = layout->field_0x298_buttonYBias;
    this->buttonRowGap = layout->field_0x29c_buttonRowGap;
    this->listRowGap = layout->field_0x2a0_listRowGap;
    this->metricA = layout->field_0x2a4_metricA;
    this->metricB = layout->field_0x2a8_metricB;
    this->metricC = layout->field_0x2ac_metricC;

    this->backgroundEnabled = menuType;

    void *arr1 = ::operator new(sizeof(Array<TouchButton *>));
    _mtw_Array_TB_ctor(arr1);
    this->buttons = (Array<void *> *) arr1;
    void *arr2 = ::operator new(sizeof(Array<TouchButton *>));
    _mtw_Array_TB_ctor(arr2);
    this->scrollEntries = (Array<void *> *) arr2;

    this->scrollUpButton = 0;
    this->scrollExtraButton = 0;
    this->buttonsB8 = 0;
    this->buttonsB0 = 0;
    this->buttonsB4 = 0;
    this->cinematicSteerActive = 0;
    this->field_0x120 = -1;
    this->selectedRow = 0;
    this->field_0x1c4 = 0;
    this->heapBufA = 0;
    this->heapBufB = 0;
    this->field_0x1e0 = 0;
    this->scrollWindowA = 0;
    this->scrollWindowB = 0;
    this->scrollSlots = 0;
    this->field_0x230 = 0;
    this->field_0x234 = 0;
    this->cutsceneMode = 0;
    this->previewStrings0 = 0;
    this->previewStrings1 = 0;
    this->previewRecords = 0;

    this->loadPreviewRecords();

    _mtw_buildMenu(this, menuType);
}

void MenuTouchWindow::setSkipButtonVisible(bool visible) {
    void **arr = (void **) this->buttons;
    if (arr != 0) {
        for (uint32_t i = 0; i < *(uint32_t *) arr; i++) {
            TouchButton *btn = (TouchButton *) ((void **) arr[1])[i];
            if (btn != 0 && btn->field_0x0 == 0x12 && btn->field_0x4 == 0) {
                _mtw_TouchButton_setVisible(btn, visible);
                arr = (void **) this->buttons;
            }
        }
    }
}

void MenuTouchWindow::drawLoadSaveMenu(bool param1) {
    (void) param1;
    Layout *layout = (Layout *) Globals::layout;
    PaintCanvas *canvas = (PaintCanvas *) Globals::Canvas;

    int rowBaseY = layout->field_0x10c_rowBaseY;
    canvas->SetColor(0xffffffff);

    int scrollOff = this->listX;
    int margin = layout->buttonInsetX;
    int strip58 = layout->field_0x110_strip58;
    int strip5c = layout->field_0x114_strip5c;
    int screenBound = Globals::w;
    int inner = screenBound + margin * -2 + scrollOff * -2;

    if (Globals::iPad && !Globals::iPadAssetsWithLowerRes) {
        strip5c = 8;
        if (!Globals::iPadHD) {
            strip58 = 0xc;
            if (!Globals::iPadLarge) {
                strip5c = 4;
                strip58 = 6;
            }
        } else {
            strip58 = 8;
            strip5c = 5;
        }
        int iw = canvas->GetImage2DWidth(this->scrollbarImageId);
        int ih = canvas->GetImage2DHeight(this->scrollbarImageId);
        int count = (ih != 0) ? (Globals::h / ih) : 0;
        int yy = 0;
        for (int k = 0; k <= count; k++) {
            canvas->DrawImage2D(this->scrollbarImageId,
                                (layout->buttonInsetX - iw) + this->listX, yy, (unsigned char) 1);
            canvas->DrawImage2D(this->scrollbarImageId,
                                layout->buttonInsetX + inner + this->listX, yy, (unsigned char) 0);
            yy += ih;
        }
        scrollOff = this->listX;
        margin = layout->buttonInsetX;
    }

    _mtw_TouchButton_setPosition(this->okButton,
                                 (screenBound - scrollOff) - margin,
                                 (layout->field_0x70_rowHeight + this->listRowGap) * this->selectedRow
                                 + this->scrollOffset
                                 + layout->field_0x20_top + layout->field_0xc + layout->field_0x108);

    int rowCount = (int) (uintptr_t) Globals::recordSlots;
    int rowMax = Globals::h;

    for (int i = 0; i < rowCount; i++) {
        int rowY = (layout->field_0x70_rowHeight + this->listRowGap) * i + this->scrollOffset
                   + layout->field_0x20_top + layout->field_0xc;
        if (rowY < 0 || rowY > rowMax) continue;

        canvas->SetColor(0xffffffff);
        int boxX = layout->buttonInsetX + this->listX;
        String box;
        box.ctor_char("", false);
        int mode = (i == this->selectedRow) ? 4 : 3;
        _mtw_Layout_drawBox(layout, mode, boxX, rowY, inner - 3, layout->field_0x70_rowHeight, &box);

        void *font = Globals::font;
        int yName = strip58 + rowY;
        String **cols = (String **) (uintptr_t) ((Array<void *> *) this->recordRows->data_[i])->data_;

        canvas->DrawString((unsigned int) (uintptr_t) font, *cols[0],
                           layout->buttonInsetX + this->listX + layout->field_0x2c_rowHeight,
                           (char) yName, false);

        GameRecord *slot = ((Array<GameRecord *> *) this->previewRecords)->data_[i];
        if (slot != 0) {
            unsigned int shipId = slot->shipId;
            if (shipId < 0x40) {
                _mtw_ImageFactory_drawShip(Globals::imageFactory, shipId,
                                           layout->field_0x2c_rowHeight + layout->buttonInsetX + this->listX +
                                           this->metricB,
                                           rowBaseY + rowY);
            }
        }

        canvas->DrawString((unsigned int) (uintptr_t) font, *cols[1],
                           layout->buttonInsetX + this->listX + layout->field_0x2c_rowHeight * 2 +
                           this->metricB + layout->field_0x2c4,
                           yName, (bool) 0);

        canvas->SetColor(0x777777ff);
        int rowY2 = rowY + strip5c;
        canvas->DrawString((unsigned int) (uintptr_t) font, *cols[2],
                           layout->buttonInsetX + this->listX + layout->field_0x2c_rowHeight,
                           rowY2 + layout->field_0x70_rowHeight / 2, (bool) 0);

        canvas->DrawString((unsigned int) (uintptr_t) font, *cols[3],
                           layout->buttonInsetX + this->listX + layout->field_0x2c_rowHeight * 2 +
                           this->metricB + layout->field_0x2c4,
                           rowY2 + layout->field_0x70_rowHeight / 2, (bool) 0);

        canvas->DrawString((unsigned int) (uintptr_t) font, *cols[4],
                           layout->buttonInsetX + this->listX + layout->field_0x2c4 +
                           (layout->field_0x2c_rowHeight + this->metricA) * 2,
                           yName, (bool) 0);

        canvas->DrawString((unsigned int) (uintptr_t) font, *cols[5],
                           layout->buttonInsetX + this->listX + layout->field_0x2c4 +
                           (layout->field_0x2c_rowHeight + this->metricA) * 2,
                           rowY2 + layout->field_0x70_rowHeight / 2, (bool) 0);

        if (i == this->selectedRow)
            _mtw_TouchButton_draw(this->okButton);
    }
}

void *_mtw_Status_getShip(void *status);

void *_mtw_Item_make(int itemDef);

void *_mtw_Item_makeQty(int itemDef, int qty);

void _mtw_Ship_setItem(void *ship, void *item, int slot);

typedef void (*TransitionFn)(void *app, int mode);

void MenuTouchWindow::startSupernova() {
    Status **statusHolder = &Globals::status;
    _mtw_Status_resetGame();
    for (int i = 0x54; i != 0; i--)
        _mtw_Status_nextCampaignMission((bool) (unsigned char) (uintptr_t) *statusHolder);

    _mtw_Status_setMission(*statusHolder);
    void *status = *statusHolder;
    ShipDefTable *row = *(ShipDefTable **) (*(int *) ((int *) Globals::ships) + 4);
    _mtw_Ship_makeShip(row->shipDefId_0x78);
    _mtw_Status_setShip(status);

    void *ship = *statusHolder;
    int race = (int) (uintptr_t) _mtw_Status_getShip(ship);
    _mtw_Ship_setRace(ship, race);

    void *it;
    void *sh;
    it = _mtw_Item_make(row->itemDef_0x2c0);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 0);
    it = _mtw_Item_make(row->itemDef_0x50);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 1);
    it = _mtw_Item_makeQty(row->itemDef_missile, 0x14);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 0);
    it = _mtw_Item_makeQty(row->itemDef_0xb0, 0x14);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 1);
    it = _mtw_Item_make(row->itemDef_equip0x144);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 0);
    it = _mtw_Item_make(row->itemDef_equip0xcc);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 1);
    it = _mtw_Item_make(row->itemDef_0x110);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 2);
    it = _mtw_Item_make(row->itemDef_equip0x158);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 3);
    it = _mtw_Item_make(row->itemDef_equip0x154);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 4);
    it = _mtw_Item_make(row->itemDef_0xe0);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_setItem(sh, it, 5);
    it = _mtw_Item_makeQty(row->itemDef_cargo0x1e8, 8);
    sh = _mtw_Status_getShip(*statusHolder);
    _mtw_Ship_addCargo(sh, it);

    _mtw_Status_setCredits(*statusHolder);
    void *station = *statusHolder;
    _mtw_Galaxy_getStation(Globals::galaxy, 0x46);
    _mtw_Status_setStation(station);
    _mtw_Status_setSystemVisibility(*statusHolder, 6, true);
    _mtw_Status_setSystemVisibility(*statusHolder, 0x19, true);
    _mtw_Status_setCredits(*statusHolder);

    OptionsRecord *opt = (OptionsRecord *) Globals::hints;
    OptionsRecord *optB = (OptionsRecord *) Globals::options;
    ((Status *) *statusHolder)->preSetField0x84 = 0x1a0a;
    opt->flag_dword_0x20 = 0x1010101;
    opt->flag_0x17 = 1;
    opt->flag_0x36 = 0;
    opt->flag_dword_0x32 = 0;
    opt->flag_word_0x8 = 0x101;
    opt->flag_0xa = 1;
    opt->flag_0x15 = 1;
    opt->flag_0x13 = 1;
    opt->flag_word_0xd = 0x101;
    opt->flag_0xf = 1;
    opt->flag_0x26 = 1;
    opt->flag_0x31 = 1;
    opt->flag_0x24 = 1;
    opt->flag_word_0x1c = 0x101;
    opt->flag_0x1e = 1;
    opt->flag_word_0x38 = 0x101;
    void *ach = Globals::achievements;
    optB->flag_0x34 = 1;

    _mtw_Achievements_setMedal(ach, 0x17, 3);
    _mtw_Achievements_setMedal(Globals::achievements, 0x1e, 1);
    _mtw_Status_setKills(*statusHolder, 0x182);
    _mtw_RecordHandler_saveOptions(Globals::recordHandler);
    _mtw_FModSound_stop(Globals::sound);

    void *app = Globals::appManager;
    optB->fadeValue = this->fadeValue;
    ModuleTransitionThunk *thunk = (ModuleTransitionThunk *) app;
    thunk->transitionFn(app, 5);
}

void MenuTouchWindow::startGOF2() {
    _mtw_Status_resetGame();
    void *snd = Globals::sound;

    ((Status *) Globals::options)->fadeValue = this->fadeValue;
    float v = _mtw_FModSound_stop(snd);
    _mtw_FModSound_play(snd, 0x8f, 0, v);
    void *appHolder = Globals::appManager;
    ModuleTransitionThunk *thunk = (ModuleTransitionThunk *) appHolder;
    thunk->transitionFn3(appHolder, 2, 0);
}
