

#include "platform/android/NdkHooks.h"

#include <cstdlib>
#include <cstring>

#include "game/mission/Status.h"
#include "game/mission/RecordHandler.h"
#include "engine/core/ApplicationManager.h"
#include "engine/core/IApplicationModule.h"
#include "engine/render/Engine.h"
#include "engine/audio/FModSound.h"
#include "game/core/Globals.h"

using AbyssEngine::Engine;

int gi_iap_buy_dlc1_pressed;
int gi_iap_buy_dlc2_pressed;
int gi_iap_buy_dlc3_pressed;
int gi_iap_buy_dlc4_pressed;
int gi_iap_buy_dlc5_pressed;

int gi_iap_buy_credit_pack1_pressed;
int gi_iap_buy_credit_pack2_pressed;
int gi_iap_buy_credit_pack3_pressed;
int gi_iap_buy_credit_pack4_pressed;
int gi_iap_buy_credit_pack5_pressed;

extern "C" {

int setBaughtCredits(int amount) {
    int *packFlag;
    if (amount == 10000000) {
        Globals::status->changeCredits(amount);
        packFlag = &gi_iap_buy_credit_pack1_pressed;
    } else if (amount == 300000) {
        Globals::status->changeCredits(amount);
        packFlag = &gi_iap_buy_credit_pack2_pressed;
    } else if (amount == 1000000) {
        Globals::status->changeCredits(amount);
        packFlag = &gi_iap_buy_credit_pack3_pressed;
    } else if (amount == 3000000) {
        Globals::status->changeCredits(amount);
        packFlag = &gi_iap_buy_credit_pack4_pressed;
    } else if (amount == 100000) {
        Globals::status->changeCredits(amount);
        packFlag = &gi_iap_buy_credit_pack5_pressed;
    } else {
        return 0;
    }

    if (*packFlag != 0) {
        *packFlag = 0;
        checkFirstCreditPackBoughtWriteAction();
    }
    ndk_autosave();
    return 1;
}

void checkFirstCreditPackBoughtWriteAction() {
    Globals::status->getCurrentCampaignMission();
    if (Globals::options[0x62] == 0) {
        Globals::options[0x62] = 1;
        RecordHandler handler;
        handler.saveOptions();
    }
}

const char *getStringUTFChars(JNIEnv *env, jstring str) {
    if (env != nullptr && str != nullptr) {
        return reinterpret_cast<const char *(*)(JNIEnv *, jstring)>(
            env->functions->GetStringUTFChars)(env, str);
    }
    return nullptr;
}

void releaseStringUTFChars(JNIEnv *env, jstring str, const char *chars) {
    if (env != nullptr && str != nullptr && chars != nullptr) {
        env->ReleaseStringUTFChars(str, chars);
        return;
    }
    if (chars != nullptr) {
        operator delete(const_cast<char *>(chars));
    }
    if (str != nullptr) {
        operator delete(reinterpret_cast<void *>(str));
    }
}

char *pConstToNonConst(const char *s) {
    if (s != nullptr) {
        char *copy = static_cast<char *>(std::malloc(std::strlen(s) + 1));
        if (copy != nullptr) {
            std::strcpy(copy, s);
            return copy;
        }
    }
    return nullptr;
}

void ndk23_sendingPauseSignal() {
    if (*Engine::g_pEngine == nullptr)
        return;
    Globals::sound->pauseAll();

    static_cast<IApplicationModule *>(ApplicationManager::gAppManager->currentModule)->OnSuspend();
}

void ndk23_sendingResumeSignal() {
    if (*Engine::g_pEngine == nullptr)
        return;
    Globals::sound->resumeAll();
}

void ndk_autosave() {
    RecordHandler *handler = new RecordHandler;
    handler->recordStoreWrite(0);
    handler->recordStoreWritePreview(0);
    delete handler;
}

int ndk_getCurrentApplicationModule() {
    if (*Engine::g_pEngine == nullptr)
        return -1;
    return static_cast<int>(ApplicationManager::gAppManager->currentModuleId);
}

int ndk_getLogoShown() {
    return Globals::logoIsShown;
}

int ndk_isInMainMenu() {
    return Globals::isInMainMenu;
}

bool ndk_getDLC_1_BOUGHT() {
    return *Engine::g_pEngine != nullptr && Globals::options[0x35] != 0;
}

bool ndk_getDLC_2_BOUGHT() {
    return *Engine::g_pEngine != nullptr && Globals::options[0x36] != 0;
}

bool ndk_getDLC_3_BOUGHT() {
    return *Engine::g_pEngine != nullptr && Globals::options[0x37] != 0;
}

bool ndk_getDLC_4_BOUGHT() {
    return *Engine::g_pEngine != nullptr && Globals::options[0x38] != 0;
}

bool ndk_getDLC_5_BOUGHT() {
    return *Engine::g_pEngine != nullptr && Globals::options[0x39] != 0;
}

void ndk_iapBoughtConsumable(unsigned int consumable) {
    if (consumable > 4 || *Engine::g_pEngine == nullptr)
        return;

    static const int kCreditPackValue[5] = {
        100000, 300000, 1000000, 3000000, 10000000
    };
    setBaughtCredits(kCreditPackValue[consumable]);
}

void ndk_iapBoughtPremium(unsigned int pack, unsigned int bought) {
    if (*Engine::g_pEngine == nullptr || pack > 4)
        return;

    int *pendingFlags[5] = {
        &gi_iap_buy_dlc1_pressed, &gi_iap_buy_dlc2_pressed,
        &gi_iap_buy_dlc3_pressed, &gi_iap_buy_dlc4_pressed,
        &gi_iap_buy_dlc5_pressed
    };
    if (*pendingFlags[pack] != 0)
        *pendingFlags[pack] = 0;

    Globals::options[0x35 + pack] = (bought == 1) ? 1 : 0;
}

void ndk_setNativeItemInformationList(JNIEnv *env, jclass /*clazz*/,
                                      jobjectArray ids, jobjectArray names,
                                      jobjectArray descriptions,
                                      jobjectArray currencies,
                                      jobjectArray prices) {
    if (env == nullptr)
        return;

    jstring elems[5][5];
    jobjectArray arrays[5] = {ids, names, descriptions, currencies, prices};
    for (int col = 0; col < 5; ++col)
        for (int row = 0; row < 5; ++row)
            elems[col][row] =
                    static_cast<jstring>(env->GetObjectArrayElement(arrays[col], row));

    const char *utf[5][5];
    for (int col = 0; col < 5; ++col)
        for (int row = 0; row < 5; ++row)
            utf[col][row] = env->GetStringUTFChars(elems[col][row], nullptr);

    char **slots[5][5] = {
        {
            &Globals::cItemListID_00, &Globals::cItemListID_01, &Globals::cItemListID_02,
            &Globals::cItemListID_03, &Globals::cItemListID_04
        },
        {
            &Globals::cItemListName_00, &Globals::cItemListName_01, &Globals::cItemListName_02,
            &Globals::cItemListName_03, &Globals::cItemListName_04
        },
        {
            &Globals::cItemListDescription_00, &Globals::cItemListDescription_01,
            &Globals::cItemListDescription_02, &Globals::cItemListDescription_03,
            &Globals::cItemListDescription_04
        },
        {
            &Globals::cItemListCurrency_00, &Globals::cItemListCurrency_01,
            &Globals::cItemListCurrency_02, &Globals::cItemListCurrency_03,
            &Globals::cItemListCurrency_04
        },
        {
            &Globals::cItemListPrice_00, &Globals::cItemListPrice_01, &Globals::cItemListPrice_02,
            &Globals::cItemListPrice_03, &Globals::cItemListPrice_04
        }
    };
    for (int col = 0; col < 5; ++col)
        for (int row = 0; row < 5; ++row)
            *slots[col][row] = pConstToNonConst(utf[col][row]);

    for (int col = 0; col < 5; ++col)
        for (int row = 0; row < 5; ++row)
            env->ReleaseStringUTFChars(elems[col][row], utf[col][row]);
}

} // extern "C"
