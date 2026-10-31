#include "engine/core/NFC.h"
#include "engine/input/VirtualInput.h"

#if defined(__ANDROID__)
#include <jni.h>
extern JNIEnv *g_pEnv;
#endif

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
#if defined(__ANDROID__)
    JNIEnv *env = g_pEnv;
    if (env == nullptr)
        return;
    jclass cls = env->FindClass(interface_path);
    jmethodID method = env->GetStaticMethodID(cls, "showMoreGames", "()V");
    env->CallStaticVoidMethod(cls, method);
#endif
}

void NFC::rateGame() {
#if defined(__ANDROID__)
    JNIEnv *env = g_pEnv;
    if (env == nullptr)
        return;
    jclass cls = env->FindClass(interface_path);
    jmethodID method = env->GetStaticMethodID(cls, "rateGame", "()V");
    env->CallStaticVoidMethod(cls, method);
#endif
}

void NFC::openTermsOfService() {
#if defined(__ANDROID__)
    JNIEnv *env = g_pEnv;
    if (env == nullptr)
        return;
    jclass cls = env->FindClass(interface_path);
    jmethodID method = env->GetStaticMethodID(cls, "openTermsOfService", "()V");
    env->CallStaticVoidMethod(cls, method);
#endif
}

void NFC::openPrivacyPolicy() {
#if defined(__ANDROID__)
    JNIEnv *env = g_pEnv;
    if (env == nullptr)
        return;
    jclass cls = env->FindClass(interface_path);
    jmethodID method = env->GetStaticMethodID(cls, "openPrivacyPolicy", "()V");
    env->CallStaticVoidMethod(cls, method);
#endif
}

bool NFC::isPad() {
#if defined(__ANDROID__)
    JNIEnv *env = g_pEnv;
    jclass cls = env->FindClass("net/fishlabs/ow_utils/DeviceInfo");
    jmethodID method = env->GetStaticMethodID(cls, "isPad", "()Z");
    jboolean value = env->CallStaticBooleanMethod(cls, method);
    if (env->ExceptionCheck())
        env->ExceptionClear();
    return value != JNI_FALSE;
#else
    return false;
#endif
}

int NFC::getWidth() {
#if defined(__ANDROID__)
    JNIEnv *env = g_pEnv;
    jclass cls = env->FindClass("net/fishlabs/ow_utils/DeviceInfo");
    jmethodID method = env->GetStaticMethodID(cls, "getDisplayWidth", "()I");
    jint value = env->CallStaticIntMethod(cls, method);
    if (env->ExceptionCheck())
        env->ExceptionClear();
    return value;
#else
    return 0;
#endif
}

int NFC::getHeight() {
#if defined(__ANDROID__)
    JNIEnv *env = g_pEnv;
    jclass cls = env->FindClass("net/fishlabs/ow_utils/DeviceInfo");
    jmethodID method = env->GetStaticMethodID(cls, "getDisplayHeight", "()I");
    jint value = env->CallStaticIntMethod(cls, method);
    if (env->ExceptionCheck())
        env->ExceptionClear();
    return value;
#else
    return 0;
#endif
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

int g_mouseCaptured __attribute__((visibility("hidden")));
int g_useJoystick __attribute__((visibility("hidden")));

int HideMouse() {
    return g_mouseCaptured;
}

void CaptureMouse(int capture) {
    g_mouseCaptured = capture;
}

int SwitchToOtherMouseConifguration(int config) {
    g_stick.useJoystick = config;
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
const char *NFC::interface_path = "net/fishlabs/gof2hdallandroid2012/NativeFunctionCalls";
