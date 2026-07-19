#include "engine/core/NFC.h"

static void **nfc_env;
static int *nfc_purchase_flag;
static void **nfc_class_slot;
static char nfc_class_name[1];
static const char nfc_method_name[1] = {0};
static const char nfc_method_sig[1] = {0};

NFC::NFC() {
}

void NFC::iap_buy_dlc_full_package() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    *nfc_purchase_flag = 1;
    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::iap_buy_dlc_vip() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    *nfc_purchase_flag = 1;
    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::iap_buy_dlc_supernova() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    *nfc_purchase_flag = 1;
    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::iap_buy_dlc_kaamo_club() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    *nfc_purchase_flag = 1;
    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::iap_buy_dlc_valkyrie() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    *nfc_purchase_flag = 1;
    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::iap_buy_credits_100_000() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    *nfc_purchase_flag = 1;
    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::iap_buy_credits_300_000() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    *nfc_purchase_flag = 1;
    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::iap_buy_credits_1_000_000() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    *nfc_purchase_flag = 1;
    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::iap_buy_credits_3_000_000() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    *nfc_purchase_flag = 1;
    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::iap_buy_credits_10_000_000() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    *nfc_purchase_flag = 1;
    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::iap_restore_purchases() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::free_credits_rateGame() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::free_credits_subscribeToYoutubeChannel() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::free_credits_likeGOF2OnFacebook() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::free_credits_likeFishlabsOnFacebook() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::free_credits_followOnTwitter() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::showMoreGames() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::rateGame() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::openTermsOfService() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

void NFC::openPrivacyPolicy() {
    void *env = *nfc_env;
    if (env == nullptr)
        return;

    void *cls = nfc_find_class(env, *nfc_class_slot);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    NFC_CallStaticVoidMethod(env, cls, method);
}

bool NFC::isPad() {
    void *env = *nfc_env;
    void *cls = nfc_find_class(env, nfc_class_name);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    int value = NFC_CallStaticBooleanMethod(env, cls, method);
    NFC_DeleteLocalRef(env);
    return value != 0;
}

int NFC::getWidth() {
    void *env = *nfc_env;
    void *cls = nfc_find_class(env, nfc_class_name);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    int value = NFC_CallStaticIntMethod(env, cls, method);
    NFC_DeleteLocalRef(env);
    return value;
}

int NFC::getHeight() {
    void *env = *nfc_env;
    void *cls = nfc_find_class(env, nfc_class_name);
    void *method = nfc_get_static_method(env, cls, nfc_method_name, nfc_method_sig);
    int value = NFC_CallStaticIntMethod(env, cls, method);
    NFC_DeleteLocalRef(env);
    return value;
}

static int is_dialogue_window_visible;
static int is_choice_window_visible;
static int is_menu_visible;
static bool isStarMapVisible;
static int subMenuIndex;
static int topMenuIndex;
static int menu_touch_window_type;
int g_android_back_button_pressed = 0;

bool IsDialogVisible(int) {
    return is_dialogue_window_visible != 0 || is_choice_window_visible != 0;
}

bool IsDialogNotVisible(int) {
    return is_dialogue_window_visible == 0 && is_choice_window_visible == 0;
}

bool IsDialogNotVisible2(int) {
    return is_dialogue_window_visible == 0 && is_choice_window_visible == 0 &&
           is_menu_visible == 0;
}

bool IsStarMapNotVisible(int) {
    return !isStarMapVisible;
}

bool IsInGameSubMenuNotActive(int) {
    if (subMenuIndex == -1 && topMenuIndex == 0)
        return is_menu_visible == 0;
    return false;
}

bool IsInGameSubMenuActive(int) {
    return subMenuIndex != -1;
}

bool IsInPrimaryMenu(int) {
    if (is_menu_visible == 0)
        return false;
    if (topMenuIndex != 0 || menu_touch_window_type != 0 ||
        is_dialogue_window_visible != 0 || is_choice_window_visible != 0)
        return false;
    return !isStarMapVisible;
}

static int g_mouseHidden;
static int g_mouseCaptured;
static int g_mouseConfig;
static int g_useJoystick;

int HideMouse() {
    return g_mouseHidden;
}

void CaptureMouse(int capture) {
    g_mouseCaptured = capture;
}

int SwitchToOtherMouseConifguration(int config) {
    g_mouseConfig = config;
    return 0;
}

void UseJoystick(int use) {
    g_useJoystick = use;
}

int GetUseJoystick() {
    return g_useJoystick;
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_BackButtonPressed() {
    g_android_back_button_pressed = 1;
}

// Static data members present in the original binary (defined for symbol parity).
void *NFC::interface_path;
