

#include <jni.h>
#include <android/log.h>

#include <cstdlib>
#include <cstring>

#include "engine/render/Engine.h"
#include "engine/render/PaintCanvas.h"
#include "engine/core/ApplicationManager.h"
#include "engine/core/GameData.h"
#include "engine/core/GameText.h"
#include "engine/core/AERandom.h"
#include "engine/file/AEFile.h"
#include "engine/file/FileInterfaceAndroid.h"
#include <GLES2/gl2.h>
#include "game/mission/Status.h"
#include "game/mission/RecordHandler.h"
#include "game/core/Globals.h"
#include "game/core/CheatHandler.h"
#include "game/ui/Layout.h"
#include "game/menu/MGame.h"
#include "game/menu/ModMainMenu.h"
#include "game/menu/ModStation.h"
#include "game/menu/MTitle.h"
#include "platform/android/NdkHooks.h"
#include "engine/input/VirtualInput.h"
#include "engine/core/NFC.h"
#include "engine/render/LODManager.h"

JavaVM *g_pVM;

JNIEnv *g_pEnv;
jobject g_pClass;
jobject g_pActivity;

static char *g_apkPath;
static char *g_zipPath;

static bool g_slowMotion;
static bool g_speedUp;

static float g_gamepadAxisX;
static float g_gamepadAxisY;

extern "C" void ndk_checkPlaytimeAndSpendOfferwallCredits();

extern "C" void ndk23_InitWithZip(const char *apkPath, const char *zipPath,
                       int width, int height);

extern "C" void ndk23_setRootDirectory(const char *path);

extern "C" void ndk23_setZipDirectory(const char *path);

void ndk23_renderstep(int width, int height);

extern "C" int ndk23_getExitFlag();

extern "C" int ndk23_getScreenshotFlag();

extern "C" void ndk23_resetScreenshotFlag();

extern "C" void ndk23_setCountryCode(unsigned int code);

extern "C" void ndk23_handleAcceleration(float x, float y, float z);

int gRealWidth;
int gRealHeight;

char *rootDirectory;
char *ZIPDirectory;

int forceExit;

static unsigned int countryCode;

int gb_android_offerwallCreditAmount;

AbyssEngine::Engine **AbyssEngine::Engine::g_pEngine;
static AbyssEngine::Engine **&g_pEngine = AbyssEngine::Engine::g_pEngine;

extern "C" int loadAPKAndZip(const char *apkPath, const char *patchPath);

void OnCreateApplication(AbyssEngine::Engine * engine);

void OnCreateApplication(AbyssEngine::Engine *engine) {
    using AbyssEngine::Engine;

    GameData *data = new GameData();
    data->field_0x04 = 0;
    data->field_0x0c = 0;
    data->field_0x0e = 0;
    data->field_0x14 = 0;
    data->field_0x10 = -1;
    data->field_0x44 = 0;
    data->field_0x3c = 0;
    data->field_0x40 = 0;
    data->field_0x48 = -1;
    data->field_0x4c = 1;
    data->field_0x08 = 0;
    data->field_0x77 = 0;
    data->field_0x75 = 0;
    data->field_0x71 = 0;
    data->field_0x6d = 0;
    data->field_0x50 = 0;
    data->field_0x54 = 0;
    data->field_0x58 = 0;
    data->field_0x5c = 0;
    data->field_0x60 = 0;
    data->field_0x64 = 0;
    data->field_0x68 = 0;
    data->field_0x6c = 0;

    data->globals = new Globals();

    DeviceInfo dev = engine->GetDeviceInfo();

    if (dev.height < 640 || dev.isPad != 0) {
        DeviceInfo padDev = engine->GetDeviceInfo();
        Globals::retinaDisplay = (padDev.isPad != 0) && (static_cast<int>(dev.height) > 800);
    } else {
        Globals::retinaDisplay = true;
    }

    data->field_0xa8 = AbyssEngine::String("", false);
    data->field_0xa5 = 0;
    data->field_0xb8 = AbyssEngine::String("", false);
    data->field_0xc4 = 0;
    data->field_0xb4 = 0;
    data->field_0x78 = 0;
    data->field_0x7a = 0;
    data->field_0x7c = AbyssEngine::String("", false);
    data->field_0x88 = AbyssEngine::String("", false);
    data->field_0x94 = AbyssEngine::String("", false);
    data->field_0xa4 = 0;
    data->field_0xa0 = 0;

    Globals::iPad = (dev.height == 480) && (dev.width == 864);

    engine->GetDeviceInfo();
    Globals::iPadHD = 0;
    Globals::iPad = dev.isPad;
    Globals::iPadLargePossible = 0;
    Globals::iPadLarge = (dev.isPad != 0) ? Globals::retinaDisplay : 0;
    if (dev.isPad != 0) {
        Globals::iPadAssetsWithLowerRes =
                (static_cast<int>(dev.height) < 768) && (static_cast<int>(dev.width) < 1024);
    } else {
        Globals::iPadAssetsWithLowerRes = 0;
    }
    Globals::switch_to_target_setting = -1;
    Globals::enterSpaceLounge = 0;

    BuildResourceList(engine);

    ApplicationManager::gAppManager = engine->appManager;
    PaintCanvas::gCanvas = engine->appManager->paintCanvas;
    Globals::gScreenWidth = PaintCanvas::gCanvas->GetWidth();
    Globals::gScreenHeight = PaintCanvas::gCanvas->GetHeight();

    GameText *text = new GameText();
    GameText::gGameText = text;
    text->setLanguage(static_cast<short>(Engine::countryCode), 3401);
    data->globals->loadFont(GameText::getLanguage());
    data->globals->init(engine->appManager, engine);

    engine->appManager->SetApplicationData(data);
    engine->appManager->SetLoadingCallback(&loadingScreen, Globals::gFont);

    engine->appManager->RegisterApplicationModule(
        2, reinterpret_cast<AbyssEngine::IApplicationModule *>(new MGame()));
    engine->appManager->RegisterApplicationModule(
        1, reinterpret_cast<AbyssEngine::IApplicationModule *>(new ModMainMenu()));
    engine->appManager->RegisterApplicationModule(
        5, reinterpret_cast<AbyssEngine::IApplicationModule *>(new ModStation()));
    engine->appManager->RegisterApplicationModule(
        0, reinterpret_cast<AbyssEngine::IApplicationModule *>(new MTitle()));

    engine->appManager->CheatSetCallback(&OnCheatActivated, nullptr);
    engine->appManager->CheatAddCode(AbyssEngine::String("754753835", false), 0);
    engine->appManager->CheatAddCode(AbyssEngine::String("448366639", false), 1);
    engine->appManager->CheatAddCode(AbyssEngine::String("373352623", false), 2);

    Globals::gLayout->initTip();
    Status::gStatus->resetGame();
    AbyssEngine::Engine::vfc = true;
    AERandom::gRandom->reset();
    engine->appManager->SetCurrentApplicationModule(0);
    AbyssEngine::Engine::clampTextures = false;

    engine->appManager->paintCanvas->TextureCreateGlobal(
        AbyssEngine::String("data/textures/pow_texture.aei", false), 2);

    AbyssEngine::Engine::vboSupported = true;
    AbyssEngine::Engine::clampTextures = false;
    engine->field_0x74 = false;
    AbyssEngine::Engine::lodBiasNormal = -0.5f;
    AbyssEngine::Engine::lodBiasDiffuse = -1.3f;
}

extern "C" void ndk23_setRootDirectory(const char *path) {
    rootDirectory = static_cast<char *>(std::malloc(std::strlen(path) + 1));
    std::strcpy(rootDirectory, path);
}

extern "C" void ndk23_setZipDirectory(const char *path) {
    ZIPDirectory = static_cast<char *>(std::malloc(std::strlen(path) + 1));
    std::strcpy(ZIPDirectory, path);
}

extern "C" void ndk23_setCountryCode(unsigned int code) {
    countryCode = (code > 15) ? 0u : code;
}

extern "C" int ndk23_getExitFlag() {
    return forceExit;
}

extern "C" int ndk23_getScreenshotFlag() {
    return 0;
}

extern "C" void ndk23_resetScreenshotFlag() {
}

extern "C" void ndk_checkPlaytimeAndSpendOfferwallCredits() {
    if (Globals::status->getPlayingTime() >= 1) {
        int amount = gb_android_offerwallCreditAmount;
        if (amount > 0) {
            Globals::status->changeCredits(amount);
            ndk_autosave();
            gb_android_offerwallCreditAmount = 0;
        }
    }
}

double gAccelFilterState[6];

extern "C" void ndk23_InitWithZip(const char *apkPath, const char *zipPath,
                       int width, int height) {
    glViewport(0, 0, width, height);

    for (int i = 0; i < 6; ++i)
        gAccelFilterState[i] = 0.0;

    gRealWidth = width;
    gRealHeight = height;

    loadAPKAndZip(apkPath, zipPath);

    AbyssEngine::Engine *engine = new AbyssEngine::Engine();
    *g_pEngine = engine;
    engine->str_0x3c = String("2.0.16", false);

    if (rootDirectory != nullptr)
        AEFile::SetAppRootDir(rootDirectory);

    engine->Initialize(&OnCreateApplication);
    engine->SetOnDestroyApp(&OnDestroyApplication);
    engine->appManager->paintCanvas->SetGameOrientation(AbyssEngine::LandscapeMode_2);
}

int rotateAccelValues;

extern "C" void ndk23_handleAcceleration(float x, float y, float z) {
    double tiltA, tiltB;
    if (rotateAccelValues != 0) {
        tiltA = -x;
        tiltB = y;
    } else {
        tiltA = y;
        tiltB = x;
    }

    AbyssEngine::Engine *engine = *g_pEngine;
    if (engine == nullptr)
        return;

    double *gravity = gAccelFilterState;
    double gz;
    if (engine->appManager->paintCanvas->initialized) {
        gravity[0] = gravity[0] * 0.95 + tiltB * 0.05;
        gravity[1] = gravity[1] * 0.95 + tiltA * 0.05;
        gz = gravity[2] * 0.95 + z * 0.05;
    } else {
        gravity[0] = tiltB * 0.95;
        gravity[1] = tiltB * 0.95;
        gz = tiltB * 0.95;
    }
    gravity[2] = gz;
    gravity[3] = -gravity[0];
    gravity[4] = -gravity[1];
    gravity[5] = -gz;

    engine->SetAccelValue(-tiltB, -tiltA, -z);
    engine->SetGravValue(gravity[3], gravity[4], gravity[5]);
}

extern "C" jint JNI_OnLoad(JavaVM *vm, void * /*reserved*/) {
    g_pVM = vm;
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4) != JNI_OK)
        return -1;
    if (env->FindClass("net/fishlabs/gof2hdallandroid2012/GOF2HD2012") == nullptr)
        return -1;
    return JNI_VERSION_1_4;
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_setEnvironmentVariables(
    JNIEnv *env, jclass clazz, jobject activity) {
    g_pEnv = env;
    g_pClass = clazz;
    g_pActivity = activity;
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_setAPKPath(
    JNIEnv *env, jclass /*clazz*/, jstring path) {
    jboolean isCopy;
    const char *utf = env->GetStringUTFChars(path, &isCopy);
    g_apkPath = static_cast<char *>(std::malloc(std::strlen(utf) + 1));
    if (isCopy) {
        std::strcpy(g_apkPath, utf);
        env->ReleaseStringUTFChars(path, utf);
    }
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_setZIPPath(
    JNIEnv *env, jclass /*clazz*/, jstring path) {
    jboolean isCopy;
    const char *utf = env->GetStringUTFChars(path, &isCopy);
    g_zipPath = static_cast<char *>(std::malloc(std::strlen(utf) + 1));
    if (isCopy) {
        std::strcpy(g_zipPath, utf);
        env->ReleaseStringUTFChars(path, utf);
    }
}

static char *g_rootDir;
static char *g_zipDir;

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_SetDirectories(
    JNIEnv *env, jclass /*clazz*/, jstring rootDir, jstring zipDir) {
    jboolean isCopy;
    const char *utf = env->GetStringUTFChars(rootDir, &isCopy);
    g_rootDir = static_cast<char *>(std::malloc(std::strlen(utf) + 1));
    g_zipDir = static_cast<char *>(std::malloc(std::strlen(utf) + 1));
    if (isCopy) {
        std::strcpy(g_rootDir, utf);
        std::strcpy(g_zipDir, utf);
        ndk23_setRootDirectory(g_rootDir);
        __android_log_print(ANDROID_LOG_ERROR, "gof2", "rootDir: %s", g_rootDir);
        __android_log_print(ANDROID_LOG_ERROR, "gof2", "zipDir: %s", g_zipDir);
        env->ReleaseStringUTFChars(rootDir, utf);
    }

    utf = env->GetStringUTFChars(zipDir, &isCopy);
    g_zipDir = static_cast<char *>(std::malloc(std::strlen(utf) + 1));
    if (isCopy) {
        std::strcpy(g_zipDir, utf);
        ndk23_setZipDirectory(g_zipDir);
        __android_log_print(ANDROID_LOG_ERROR, "gof2", "zipDir: %s", g_zipDir);
        env->ReleaseStringUTFChars(zipDir, utf);
    }
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_STARTUP(
    JNIEnv * /*env*/, jclass /*clazz*/) {
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_initialize(
    JNIEnv * /*env*/, jclass /*clazz*/, jint width, jint height) {
    ndk23_InitWithZip(g_apkPath, g_zipPath, width, height);
    ndk23_renderstep(width, height);
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_renderstep(
    JNIEnv * /*env*/, jclass /*clazz*/, jint width, jint height) {
    ndk23_renderstep(width, height);
}

extern "C" jboolean Java_net_fishlabs_gof2hdallandroid2012_ToJNI_getExitFlag(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    return ndk23_getExitFlag() != 0;
}

extern "C" jboolean Java_net_fishlabs_gof2hdallandroid2012_ToJNI_getScreenshotFlag(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    return ndk23_getScreenshotFlag() != 0;
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_resetScreenshotFlag(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    ndk23_resetScreenshotFlag();
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_sendPauseSignalToGame(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    ndk23_sendingPauseSignal();
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_sendResumeSignalToGame(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    ndk23_sendingResumeSignal();
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_setCountryCodeOfDevice(
    JNIEnv * /*env*/, jclass /*clazz*/, jint code) {
    ndk23_setCountryCode(static_cast<unsigned int>(code));
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_handleAccelerometer(
    JNIEnv * /*env*/, jclass /*clazz*/, jfloat x, jfloat y, jfloat z) {
    ndk23_handleAcceleration(-x, y, z);
}

// Exit callback registered with the engine; signals the platform exit flag.
// C linkage: the original exports it as the unmangled symbol `ExitFunction`.
extern "C" void ExitFunction() {
    forceExit = -1;
}

extern "C" void ndk23_newrender(long long now) {
    ApplicationManager *manager = (*g_pEngine)->appManager;
    manager->SetExitCallback(&ExitFunction);

    int touchCount = GetTouchCount();
    for (int i = 0; i < touchCount; ++i) {
        Touch rec = GetTouch(i);
        void *touch = reinterpret_cast<void *>(rec.x);
        int phase = rec.y;
        int x = rec.id;
        int y = rec.action;
        if (phase == 2) {
            manager->OnTouchMove(x, y, touch);
        } else if (phase == 1) {
            manager->OnTouchEnd(x, y, touch);
            manager->OnTouchEnd();
        } else if (phase == 0) {
            manager->OnTouchBegin(x, y, touch);
        }
    }

    if (g_android_back_button_pressed != 0) {
        keyPressed(*g_pEngine, 0x35);
        keyReleased(*g_pEngine, 0x35);
        g_android_back_button_pressed = 0;
    }

    RemoveTouches();
    manager->OnUpdate(now);

    char *appData = static_cast<char *>(manager->GetApplicationData());
    if (appData[0x3d] != 0) {
        appData[0x40] = 1;
        appData[0x3d] = 0;
    } else if (appData[0x3c] != 0) {
        appData[0x3c] = 0;
    } else if (appData[0x3e] != 0) {
        appData[0x3e] = 0;
    } else if (appData[0x3f] != 0) {
        appData[0x3f] = 0;
    } else if (appData[0xa0] != 0) {
        appData[0xa0] = 0;
    } else if (appData[0xa1] != 0) {
        appData[0xa1] = 0;
    } else if (appData[0xa2] != 0) {
        appData[0xa2] = 0;
    } else if (appData[0xa3] != 0) {
        appData[0xa3] = 0;
    }

    ndk_checkPlaytimeAndSpendOfferwallCredits();
}

extern "C" void ndk23_handleTouchPadEvent(jclass /*clazz*/, void *touch, int phase,
                               float x, float y) {
    ApplicationManager *manager = (*g_pEngine)->appManager;
    int px = static_cast<int>(x);
    int py = static_cast<int>(y);
    if (phase == 2) {
        manager->OnTouchMove(px, py, touch);
    } else if (phase == 1) {
        manager->OnTouchEnd(px, py, touch);
        manager->OnTouchEnd();
    } else if (phase == 0) {
        manager->OnTouchBegin(px, py, touch);
    }
}

extern "C" void ndk23_handleTouchScreenEvent(jclass clazz, void *touch, int phase,
                                  float x, float y) {
    ndk23_handleTouchPadEvent(clazz, touch, phase, x, y);
}

extern "C" void ndk23_ndkDone() {
    AbyssEngine::Engine *engine = *g_pEngine;
    if (engine != nullptr) {
        engine->Release();
        delete engine;
        *g_pEngine = nullptr;
    }
}

bool SlowMotion() {
    return g_slowMotion;
}

bool SpeedUp() {
    return g_speedUp;
}

void setValuesForGamepad(float x, float y) {
    g_gamepadAxisX = x;
    g_gamepadAxisY = y;
}

extern "C" jboolean Java_net_fishlabs_gof2hdallandroid2012_ToJNI_getDLC1BOUGHT(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    return ndk_getDLC_1_BOUGHT();
}

extern "C" jboolean Java_net_fishlabs_gof2hdallandroid2012_ToJNI_getDLC2BOUGHT(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    return ndk_getDLC_2_BOUGHT();
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_correctBoughtDLC1(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    ndk_iapBoughtPremium(0, 1);
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_correctBoughtDLC2(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    ndk_iapBoughtPremium(1, 1);
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_correctBoughtDLC3(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    ndk_iapBoughtPremium(2, 1);
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_correctBoughtDLC4(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    ndk_iapBoughtPremium(3, 1);
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_correctBoughtDLC5(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    ndk_iapBoughtPremium(4, 1);
}

// Google Play game-services state, exported as plain globals (the original's
// exact storage: achievements is int[3], leaderboard scores int[8], the rest
// scalar ints). link_game_gp is read by GetLinkGameGP; gp_is_linked is the
// separate flag set by SetGPIsLinked.
int g_android_current_achievements[3];
int g_android_leaderboard_scores[8];
int g_android_link_game_gp;
int g_android_gp_is_linked;
int g_android_show_achievements;
int g_android_show_leaderboards;

extern "C" jint Java_net_fishlabs_gof2hdallandroid2012_GOF2HD2012_GetAchievementId(
    JNIEnv * /*env*/, jclass /*clazz*/, jint index) {
    return g_android_current_achievements[index];
}

extern "C" jint Java_net_fishlabs_gof2hdallandroid2012_GOF2HD2012_GetLeaderboardScore(
    JNIEnv * /*env*/, jclass /*clazz*/, jint index) {
    return g_android_leaderboard_scores[index];
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_GOF2HD2012_ResetLeaderboardScore(
    JNIEnv * /*env*/, jclass /*clazz*/, jint index) {
    g_android_leaderboard_scores[index] = 0;
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_GOF2HD2012_ResetAchievements(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    g_android_current_achievements[0] = 0;
    g_android_current_achievements[1] = 0;
    g_android_current_achievements[2] = 0;
}

extern "C" jint Java_net_fishlabs_gof2hdallandroid2012_GOF2HD2012_GetShowAchievements(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    return g_android_show_achievements;
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_GOF2HD2012_ResetShowAchievements(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    g_android_show_achievements = 0;
}

extern "C" jint Java_net_fishlabs_gof2hdallandroid2012_GOF2HD2012_GetLinkGameGP(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    return g_android_link_game_gp;
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_GOF2HD2012_ResetLinkGameGP(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    g_android_link_game_gp = 0;
}

extern "C" jint Java_net_fishlabs_gof2hdallandroid2012_GOF2HD2012_GetShowLeaderboards(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    return g_android_show_leaderboards;
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_GOF2HD2012_ResetShowLeaderboards(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    g_android_show_leaderboards = 0;
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_GOF2HD2012_SetGPIsLinked(
    JNIEnv * /*env*/, jclass /*clazz*/, jint linked) {
    g_android_gp_is_linked = linked;
}

extern "C" jboolean Java_net_fishlabs_gof2hdallandroid2012_ToJNI_getDLC3BOUGHT(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    return ndk_getDLC_3_BOUGHT();
}

extern "C" jboolean Java_net_fishlabs_gof2hdallandroid2012_ToJNI_getDLC4BOUGHT(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    return ndk_getDLC_4_BOUGHT();
}

extern "C" jboolean Java_net_fishlabs_gof2hdallandroid2012_ToJNI_getDLC5BOUGHT(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    return ndk_getDLC_5_BOUGHT();
}

extern "C" jboolean Java_net_fishlabs_gof2hdallandroid2012_ToJNI_getLogoShown(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    return ndk_getLogoShown() != 0;
}

extern "C" jboolean Java_net_fishlabs_gof2hdallandroid2012_ToJNI_isInMainMenu(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    return ndk_isInMainMenu() != 0;
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_resize(
    JNIEnv * /*env*/, jclass /*clazz*/, jint width, jint height) {
    ndk23_renderstep(width, height);
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_handleTouchEvent(
    JNIEnv * /*env*/, jclass clazz, jint touch, jint phase, jfloat x, jfloat y) {
    ndk23_handleTouchScreenEvent(clazz, reinterpret_cast<void *>(touch), phase, x, y);
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_testPurchase(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    ndk_iapBoughtConsumable(0);
}

extern "C" void Java_net_fishlabs_googleplay_ToJNI_iapBoughtConsumable(
    JNIEnv * /*env*/, jclass /*clazz*/, jint consumable) {
    ndk_iapBoughtConsumable(static_cast<unsigned int>(consumable));
}

extern "C" void Java_net_fishlabs_googleplay_ToJNI_iapBoughtPremium(
    JNIEnv * /*env*/, jclass /*clazz*/, jint pack, jint bought) {
    ndk_iapBoughtPremium(static_cast<unsigned int>(pack),
                         static_cast<unsigned int>(bought));
}

extern "C" void Java_net_fishlabs_googleplay_ToJNI_iapSetNativeItemInformationList(
    JNIEnv *env, jclass clazz, jobjectArray ids, jobjectArray names,
    jobjectArray descriptions, jobjectArray currencies, jobjectArray prices) {
    ndk_setNativeItemInformationList(env, clazz, ids, names, descriptions,
                                     currencies, prices);
}

extern "C" void Java_net_fishlabs_googleplay_ToJNI_iapResetNativeItemInformationList(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    ndk_resetNativeItemInformationList();
}

static const char *const kConsumableSKU[5] = {
    "net.fishlabs.gof2hd2012.creditpack1",
    "net.fishlabs.gof2hd2012.creditpack2",
    "net.fishlabs.gof2hd2012.creditpack3",
    "net.fishlabs.gof2hd2012.creditpack4",
    "net.fishlabs.gof2hd2012.creditpack5",
};

static const char *const kPremiumSKU[5] = {
    "net.fishlabs.gof2hd2012.dlc1",
    "net.fishlabs.gof2hd2012.dlc2",
    "net.fishlabs.gof2hd2012.dlc3",
    "net.fishlabs.gof2hd2012.dlc4",
    "net.fishlabs.gof2hd2012.dlc5",
};

extern "C" jstring Java_net_fishlabs_googleplay_ToJNI_gof2hd2012getConsumableSKU(
    JNIEnv *env, jclass /*clazz*/, jint index) {
    if (index < 0 || index > 4)
        return nullptr;
    return env->NewStringUTF(kConsumableSKU[index]);
}

extern "C" jstring Java_net_fishlabs_googleplay_ToJNI_gof2hd2012getPremiumSKU(
    JNIEnv *env, jclass /*clazz*/, jint index) {
    if (index < 0 || index > 4)
        return nullptr;
    return env->NewStringUTF(kPremiumSKU[index]);
}

extern "C" jstring Java_net_fishlabs_googleplay_ToJNI_gof2hd2012apk(
    JNIEnv *env, jclass /*clazz*/) {
    return env->NewStringUTF("net.fishlabs.gof2hdallandroid2012");
}

extern "C" jint Java_net_fishlabs_playhaven_ToJNI_getCurrentApplicationModule(
    JNIEnv * /*env*/, jclass /*clazz*/) {
    return ndk_getCurrentApplicationModule();
}

extern "C" void Java_net_fishlabs_tapjoy_ToJNI_spentAmountOfCredits(
    JNIEnv * /*env*/, jclass /*clazz*/, jint amount) {
    gb_android_offerwallCreditAmount = amount;
    ndk_checkPlaytimeAndSpendOfferwallCredits();
}

extern "C" int loadAPK(const char *path);

extern "C" void ndk23_Init(const char *apkPath, int width, int height) {
    glViewport(0, 0, width, height);

    for (int i = 0; i < 6; ++i)
        gAccelFilterState[i] = 0.0;

    gRealWidth = width;
    gRealHeight = height;

    loadAPK(apkPath);

    AbyssEngine::Engine *engine = new AbyssEngine::Engine();
    *g_pEngine = engine;
    engine->str_0x3c = String("2.0.16", false);

    if (rootDirectory != nullptr)
        AEFile::SetAppRootDir(rootDirectory);

    engine->Initialize(&OnCreateApplication);
    engine->SetOnDestroyApp(&OnDestroyApplication);
    engine->appManager->paintCanvas->SetGameOrientation(AbyssEngine::LandscapeMode_2);
}

extern "C" void ndk23_resize(int width, int height) {
    glViewport(0, 0, width, height);

    gRealHeight = height;
    gRealWidth = width;

    simulateTouch(gEngine);
}

extern "C" void ndk23_setDisplayHeightAndWidth(int height, int width) {
    (void) height;
    (void) width;
}

extern "C" int ndk23_getCurrentFiredStatus() {
    return 0;
}
