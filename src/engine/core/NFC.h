#ifndef GOF2_NFC_H
#define GOF2_NFC_H

typedef void *(*NFC_FindClassFn)(void *, void *);

typedef void *(*NFC_GetStaticMethodFn)(void *, void *, const char *, const char *);

static inline void *nfc_jni_slot(void *env, unsigned offset) {
    return *(void **) ((char *) *(void **) env + offset);
}

static inline void *nfc_find_class(void *env, void *name) {
    return ((NFC_FindClassFn) nfc_jni_slot(env, 0x18))(env, name);
}

static inline void *nfc_get_static_method(void *env, void *cls, const char *name, const char *sig) {
    return ((NFC_GetStaticMethodFn) nfc_jni_slot(env, 0x1c4))(env, cls, name, sig);
}

void NFC_CallStaticVoidMethod(void *env, void *cls, void *method);

int NFC_CallStaticBooleanMethod(void *env, void *cls, void *method);

int NFC_CallStaticIntMethod(void *env, void *cls, void *method);

void NFC_DeleteLocalRef(void *env);

class NFC {
public:
    NFC();

    void iap_buy_dlc_full_package();

    void iap_buy_dlc_vip();

    void iap_buy_dlc_supernova();

    void iap_buy_dlc_kaamo_club();

    void iap_buy_dlc_valkyrie();

    void iap_buy_credits_100_000();

    void iap_buy_credits_300_000();

    void iap_buy_credits_1_000_000();

    void iap_buy_credits_3_000_000();

    void iap_buy_credits_10_000_000();

    void iap_restore_purchases();

    void free_credits_rateGame();

    void free_credits_subscribeToYoutubeChannel();

    void free_credits_likeGOF2OnFacebook();

    void free_credits_likeFishlabsOnFacebook();

    void free_credits_followOnTwitter();

    void showMoreGames();

    void rateGame();

    void openTermsOfService();

    void openPrivacyPolicy();

    bool isPad();

    int getWidth();

    int getHeight();

    // Static data members present in the original binary (defined for symbol parity).
    static void *interface_path;
};

// The original exports this as a plain global (symbol `g_android_back_button_pressed`),
// set by the Android back-button JNI hook.
extern int g_android_back_button_pressed;

bool IsDialogVisible(int);

bool IsDialogNotVisible(int);

bool IsDialogNotVisible2(int);

bool IsStarMapNotVisible(int);

bool IsInGameSubMenuNotActive(int);

bool IsInGameSubMenuActive(int);

bool IsInPrimaryMenu(int);

int HideMouse();

void CaptureMouse(int capture);

int SwitchToOtherMouseConifguration(int config);

void UseJoystick(int use);

int GetUseJoystick();

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_BackButtonPressed();

#endif
