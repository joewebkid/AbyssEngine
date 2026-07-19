#include "game/core/Globals.h"
#include <arm_neon.h>
#include <cstdint>

// Named models for the untyped handles that Globals.cpp touches by raw byte
// offset. These structs reconstruct the field layout of opaque engine/game
// blobs (the video/options settings record, the per-frame mission counter
// object, the campaign zero-init object, the line-height metrics record, ...)
// so the call sites can use named members instead of pointer arithmetic.
//
// Layout is byte-exact for the 32-bit MATCH build; the static_asserts below
// pin every accessed offset. Multi-byte fields that genuinely overlap in the
// original packed blob are expressed through unions so each named view keeps
// its exact offset.
namespace {

#pragma pack(push, 1)

// The global video / audio / control settings record.  Accessed via the
// `settings` handle in Globals::Globals() and Globals::init().
struct GameSettingsRecord {
    union { float colorR; int32_t colorRBits; }; // 0x00
    union { float colorG; int32_t colorGBits; }; // 0x04
    union { float colorB; int32_t colorBBits; }; // 0x08
    union {                   // 0x0c .. 0x13 packed volume / flag region (8 bytes)
        uint8_t  _region[8];
        struct {
            uint8_t  musicVolume;   // 0x0c
            uint8_t  sfxVolume;     // 0x0d
            uint8_t  ambientVolume; // 0x0e
            int16_t  flagAt0f;      // 0x0f (overlaps flagAt10's low byte)
        };
        struct {
            int16_t  volumePair;    // 0x0c (overlaps music/sfx volume)
            uint8_t  _pad0e;        // 0x0e
            uint8_t  _pad0f;        // 0x0f
            int16_t  flagAt10;      // 0x10 (overlaps flagAt0f high byte)
        };
        struct {
            uint8_t  _pad0c[5];     // 0x0c .. 0x10
            uint8_t  flagAt11;      // 0x11
        };
    };
    float    glowR;           // 0x14
    float    glowG;           // 0x18
    float    tintR;           // 0x1c
    float    tintG;           // 0x20
    float    tintB;           // 0x24
    float    brightness;      // 0x28
    float    contrast;        // 0x2c
    uint8_t  enableFlag30;    // 0x30
    int32_t  intAt31;         // 0x31
    int32_t  intAt35;         // 0x35
    uint8_t  flagAt39;        // 0x39
    uint8_t  _pad3a;          // 0x3a
    int32_t  intAt3b;         // 0x3b
    uint8_t  flagAt3f;        // 0x3f
    int16_t  shortAt40;       // 0x40
    uint8_t  _pad42[2];       // 0x42
    float    qualityLevel;    // 0x44
    int32_t  intAt48;         // 0x48
    int16_t  shortAt4c;       // 0x4c
    uint8_t  flagAt4e;        // 0x4e
    uint8_t  _pad4f;          // 0x4f
    int32_t  intAt50;         // 0x50
    int32_t  resWidth;        // 0x54
    int32_t  resHeight;       // 0x58
    uint8_t  _pad5c[4];       // 0x5c
    int16_t  shortAt60;       // 0x60
};

// Object zero-initialised in Globals::init() through the `obj` handle: a run
// of twelve int slots followed by four unaligned int writes that overlap the
// tail of the run (the original blob is packed).
struct InitZeroObject {
    union {
        int32_t  slots[12];   // 0x00 .. 0x2f
        struct {
            uint8_t  _lead[0x2b];
            int32_t  tailAt2b; // 0x2b
            int32_t  tailAt2f; // 0x2f
            int32_t  tailAt33; // 0x33
            int32_t  tailAt37; // 0x37
        };
    };
};

// Small per-frame mission/work object reached through the `busy` handle in
// Globals::getAgentMissionText(); only the recursion-guard counter at 0xd0 is
// touched here.
struct AgentBusyObject {
    uint8_t  _pad[0xd0];      // 0x00
    int32_t  guardCounter;    // 0xd0
};

// Line metrics record reached through the `lineHeight` handle in the
// Globals::drawLines() family: the advance between rows lives at +4.
struct LineMetrics {
    int32_t  field0;          // 0x00
    int32_t  lineHeight;      // 0x04
};

// Coordinate helper object used by setCoordsSteer/setCoordsFire; the steer
// path writes its computed value at +0x54, the fire path at +0x58.
struct CoordsObject {
    uint8_t  _pad[0x54];      // 0x00
    int32_t  steerValue;      // 0x54
    int32_t  fireValue;       // 0x58
};

// Object reached through the `secondary` handle in Globals::Globals(): two
// leading int slots plus a flag byte at 0x13.
struct CtorSecondaryObject {
    int32_t  slot0;           // 0x00
    int32_t  slot1;           // 0x04
    uint8_t  _pad08[0x0b];    // 0x08
    uint8_t  flagAt13;        // 0x13
};

// Object reached through gI_g3822 in Globals::init(): only a leading flag byte
// is cleared.
struct InitFlagByte {
    uint8_t  flag;            // 0x00
};

#pragma pack(pop)

#if __SIZEOF_POINTER__ == 4
#include <cstddef>
static_assert(offsetof(GameSettingsRecord, colorR) == 0x00, "colorR");
static_assert(offsetof(GameSettingsRecord, colorG) == 0x04, "colorG");
static_assert(offsetof(GameSettingsRecord, colorB) == 0x08, "colorB");
static_assert(offsetof(GameSettingsRecord, musicVolume) == 0x0c, "musicVolume");
static_assert(offsetof(GameSettingsRecord, sfxVolume) == 0x0d, "sfxVolume");
static_assert(offsetof(GameSettingsRecord, ambientVolume) == 0x0e, "ambientVolume");
static_assert(offsetof(GameSettingsRecord, volumePair) == 0x0c, "volumePair");
static_assert(offsetof(GameSettingsRecord, flagAt0f) == 0x0f, "flagAt0f");
static_assert(offsetof(GameSettingsRecord, flagAt10) == 0x10, "flagAt10");
static_assert(offsetof(GameSettingsRecord, flagAt11) == 0x11, "flagAt11");
static_assert(offsetof(GameSettingsRecord, glowR) == 0x14, "glowR");
static_assert(offsetof(GameSettingsRecord, glowG) == 0x18, "glowG");
static_assert(offsetof(GameSettingsRecord, tintR) == 0x1c, "tintR");
static_assert(offsetof(GameSettingsRecord, tintG) == 0x20, "tintG");
static_assert(offsetof(GameSettingsRecord, tintB) == 0x24, "tintB");
static_assert(offsetof(GameSettingsRecord, brightness) == 0x28, "brightness");
static_assert(offsetof(GameSettingsRecord, contrast) == 0x2c, "contrast");
static_assert(offsetof(GameSettingsRecord, enableFlag30) == 0x30, "enableFlag30");
static_assert(offsetof(GameSettingsRecord, intAt31) == 0x31, "intAt31");
static_assert(offsetof(GameSettingsRecord, intAt35) == 0x35, "intAt35");
static_assert(offsetof(GameSettingsRecord, flagAt39) == 0x39, "flagAt39");
static_assert(offsetof(GameSettingsRecord, intAt3b) == 0x3b, "intAt3b");
static_assert(offsetof(GameSettingsRecord, flagAt3f) == 0x3f, "flagAt3f");
static_assert(offsetof(GameSettingsRecord, shortAt40) == 0x40, "shortAt40");
static_assert(offsetof(GameSettingsRecord, qualityLevel) == 0x44, "qualityLevel");
static_assert(offsetof(GameSettingsRecord, intAt48) == 0x48, "intAt48");
static_assert(offsetof(GameSettingsRecord, shortAt4c) == 0x4c, "shortAt4c");
static_assert(offsetof(GameSettingsRecord, flagAt4e) == 0x4e, "flagAt4e");
static_assert(offsetof(GameSettingsRecord, intAt50) == 0x50, "intAt50");
static_assert(offsetof(GameSettingsRecord, resWidth) == 0x54, "resWidth");
static_assert(offsetof(GameSettingsRecord, resHeight) == 0x58, "resHeight");
static_assert(offsetof(GameSettingsRecord, shortAt60) == 0x60, "shortAt60");

static_assert(offsetof(InitZeroObject, tailAt2b) == 0x2b, "tailAt2b");
static_assert(offsetof(InitZeroObject, tailAt2f) == 0x2f, "tailAt2f");
static_assert(offsetof(InitZeroObject, tailAt33) == 0x33, "tailAt33");
static_assert(offsetof(InitZeroObject, tailAt37) == 0x37, "tailAt37");

static_assert(offsetof(AgentBusyObject, guardCounter) == 0xd0, "guardCounter");
static_assert(offsetof(LineMetrics, lineHeight) == 0x04, "lineHeight");
static_assert(offsetof(CoordsObject, steerValue) == 0x54, "steerValue");
static_assert(offsetof(CoordsObject, fireValue) == 0x58, "fireValue");
static_assert(offsetof(CtorSecondaryObject, slot1) == 0x04, "slot1");
static_assert(offsetof(CtorSecondaryObject, flagAt13) == 0x13, "flagAt13");
#endif

} // anonymous namespace

Globals *Globals::gGlobals = nullptr;

int Globals::is_dialogue_window_visible = 0;
int Globals::is_choice_window_visible = 0;
int Globals::is_menu_visible = 0;
int Globals::is_hacking_visible = 0;
unsigned char Globals::isStarMapVisible = 0;
unsigned char Globals::isCinematicModeActive = 0;
int Globals::mouseCursorActivated = 0;
unsigned char Globals::showMouseDuringGameOver = 0;
unsigned char Globals::keyBindings[8] = {};

int Globals::left_edge = 0;
int Globals::right_edge = 0;
int Globals::top_edge = 0;
int Globals::bottom_edge = 0;
int Globals::resetKeyboard = 0;
int Globals::rotateShipInStation = 0;
int Globals::translateStarMapInXDirection = 0;
int Globals::translateStarMapInYDirection = 0;
int Globals::smallButton_dim = 0;
int Globals::touch_stick_x = 0;
int Globals::touch_stick_y = 0;

unsigned char Globals::iPad = 0;
unsigned char Globals::iPadHD = 0;
unsigned char Globals::retinaDisplay = 0;
unsigned char Globals::n9 = 0;
unsigned char Globals::iPadLarge = 0;
unsigned char Globals::iPadLargePossible = 0;
unsigned char Globals::iPadAssetsWithLowerRes = 0;
unsigned char Globals::enterSpaceLounge = 0;
int Globals::switch_to_target_setting = 0;

Status *Globals::status = nullptr;
unsigned char Globals::options[100] = {};
FModSound *Globals::sound = nullptr;
int Globals::logoIsShown = 0;
int Globals::isInMainMenu = 0;

char *Globals::cItemListID_00 = nullptr;
char *Globals::cItemListID_01 = nullptr;
char *Globals::cItemListID_02 = nullptr;
char *Globals::cItemListID_03 = nullptr;
char *Globals::cItemListID_04 = nullptr;
char *Globals::cItemListID_05 = nullptr;
char *Globals::cItemListID_06 = nullptr;
char *Globals::cItemListID_07 = nullptr;
char *Globals::cItemListID_08 = nullptr;
char *Globals::cItemListID_09 = nullptr;
char *Globals::cItemListID_10 = nullptr;
char *Globals::cItemListID_11 = nullptr;
char *Globals::cItemListID_12 = nullptr;
char *Globals::cItemListID_13 = nullptr;
char *Globals::cItemListID_14 = nullptr;
char *Globals::cItemListID_15 = nullptr;
char *Globals::cItemListID_16 = nullptr;
char *Globals::cItemListID_17 = nullptr;
char *Globals::cItemListID_18 = nullptr;
char *Globals::cItemListID_19 = nullptr;
char *Globals::cItemListID_20 = nullptr;
char *Globals::cItemListID_21 = nullptr;
char *Globals::cItemListID_22 = nullptr;
char *Globals::cItemListID_23 = nullptr;
char *Globals::cItemListID_24 = nullptr;
char *Globals::cItemListName_00 = nullptr;
char *Globals::cItemListName_01 = nullptr;
char *Globals::cItemListName_02 = nullptr;
char *Globals::cItemListName_03 = nullptr;
char *Globals::cItemListName_04 = nullptr;
char *Globals::cItemListDescription_00 = nullptr;
char *Globals::cItemListDescription_01 = nullptr;
char *Globals::cItemListDescription_02 = nullptr;
char *Globals::cItemListDescription_03 = nullptr;
char *Globals::cItemListDescription_04 = nullptr;
char *Globals::cItemListCurrency_00 = nullptr;
char *Globals::cItemListCurrency_01 = nullptr;
char *Globals::cItemListCurrency_02 = nullptr;
char *Globals::cItemListCurrency_03 = nullptr;
char *Globals::cItemListCurrency_04 = nullptr;
char *Globals::cItemListPrice_00 = nullptr;
char *Globals::cItemListPrice_01 = nullptr;
char *Globals::cItemListPrice_02 = nullptr;
char *Globals::cItemListPrice_03 = nullptr;
char *Globals::cItemListPrice_04 = nullptr;

Layout *Globals::gLayout = nullptr;
void *Globals::gFont = nullptr;
int Globals::gScreenWidth = 0;
int Globals::gScreenHeight = 0;

// Static data members present in the original binary (defined for symbol parity).
void *Globals::appManager;
unsigned char Globals::gameLoaded;
unsigned char Globals::gameSaving;
float Globals::sec_fire_x;
float Globals::sec_fire_y;
float Globals::sec_fire_z;
float Globals::autopilot_x;
float Globals::autopilot_y;
float Globals::autopilot_z;
int Globals::mouseDeltaX;
int Globals::mouseDeltaY;
int Globals::mouse_wheel;
void *Globals::recordSlots;
void *Globals::achievements;
void *Globals::imageFactory;
int Globals::mouse_wheelX;
int Globals::mouse_wheelY;
int Globals::qualityLevel;
void *Globals::shipTemplate;
unsigned char Globals::showBestDeal;
int Globals::simulateFire;
int Globals::subMenuIndex;
int Globals::topMenuIndex;
float Globals::action_menu_x;
float Globals::action_menu_y;
float Globals::action_menu_z;
void *Globals::recordHandler;
float Globals::touch_stick_z;
float Globals::turret_view_x;
float Globals::turret_view_y;
float Globals::turret_view_z;
float Globals::fast_forward_x;
float Globals::fast_forward_y;
float Globals::fast_forward_z;
int Globals::fontLangSelect;
int Globals::keyboard_swipe;
unsigned char Globals::iCloudSupported;
unsigned char Globals::isIOS6Installed;
int Globals::other_buttons_x[10];
int Globals::other_buttons_y[10];
unsigned char Globals::showWingmanMenu;
unsigned char Globals::first_start_ever;
int Globals::globalPriceRaise;
unsigned char Globals::initMemoryWarning;
int Globals::instantActionType;
int Globals::instantActionWave;
unsigned char Globals::isTelekomCustomer;
int Globals::lastRecordWritten;
int Globals::instantActionScore;
unsigned char Globals::showNewCreditsMenu;
int Globals::sub_menu_buttons_x[10];
int Globals::sub_menu_buttons_y[10];
unsigned char Globals::iap_hack_dlc1Bought;
unsigned char Globals::iap_hack_dlc2Bought;
unsigned char Globals::iap_hack_dlc3Bought;
int Globals::instantActionPoints;
unsigned char Globals::useRefractionShader;
int Globals::h;
int Globals::w;
int Globals::lastSpaceMusicPlayed;
int Globals::energyCellsProbChange;
int Globals::sub_menu_button_count;
unsigned char Globals::inAppPurchaseSupported;
int Globals::lastStationMusicPlayed;
int Globals::menu_touch_window_type;
unsigned char Globals::useLowResTexturesForHD;
void *Globals::instantActionPlayerName;
unsigned char Globals::isRunningHDonWeakDevice;
float Globals::quickmenu_button_start_x;
float Globals::quickmenu_button_start_y;
int Globals::lastCampaignMissionFailed;
int Globals::secondaryWeaponsProbChange;
int Globals::lastCampaignMissionFailCount;
unsigned char Globals::startLiteVersionWithMoreCredits;
void *Globals::rnd;
void *Globals::font;
unsigned char Globals::keys[1020];
void *Globals::bankZ;
unsigned char Globals::hints[59];
void *Globals::items;
void *Globals::ships;
void *Globals::Canvas;
float Globals::fire_x;
float Globals::fire_y;
float Globals::fire_z;
void *Globals::galaxy;
void *Globals::layout;
float Globals::boost_x;
float Globals::boost_y;
float Globals::boost_z;
void *Globals::globals;
float Globals::pause_x;
float Globals::pause_y;
float Globals::pause_z;
void *Globals::gameText;
void *Globals::fontAlien;
void *Globals::generator;
AbyssEngine::ApplicationManager *ApplicationManager::gAppManager = nullptr;
#include "engine/render/Mesh.h"
#include "game/ship/Ship.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/AEGeometry.h"
#include "engine/audio/FModSound.h"

#include "game/mission/Status.h"
#include "game/world/Galaxy.h"
#include "game/mission/Achievements.h"
#include "engine/core/AERandom.h"
#include "game/ship/Agent.h"
#include "engine/core/ApplicationManager.h"
#include "engine/core/GameText.h"
#include "engine/render/ImageFactory.h"
#include "game/ui/Layout.h"

#include "game/mission/Mission.h"
#include "game/world/Station.h"
#include "game/mission/RecordHandler.h"
#include "game/world/SolarSystem.h"
#include "game/core/String.h"
#include "engine/math/BoundingSphere.h"

#include <new>

float VectorSignedToFloat(int v, int mode);

float VectorUnsignedToFloat(unsigned v, int mode);

static inline uint32_t nextInt_71aa4(AbyssEngine::AERandom *self) { return self->nextInt(); }

static inline int nextInt_71ad0(AbyssEngine::AERandom *self, int bound) { return self->nextInt(bound); }

int idiv(int a, int b);

void MatrixSetTranslation(void *m, float x, float y, float z);

int AERandom_nextIntB(int rng, int bound);

void FileRead_ctor(void *self);

void *FileRead_dtor(void *self);

float VectorScale(void *vec, float scalar);

void *Galaxy_dtor(void *g);

void *Status_dtor(void *s);

void *AERandom_dtor(void *r);

void *Layout_dtor(void *l);

void *Generator_dtor(void *g);

void *FModSound_dtor(void *s);

void *Achievements_dtor(void *a);

void *ImageFactory_dtor(void *f);

void Mission_ctor(void *m);

void Galaxy_ctor(void *g);

void Achievements_ctor(void *a);

void Status_ctor(void *s);

void FileRead_ctor(void *f);

void *FileRead_dtor(void *f);

void AERandom_ctor(void *r);

void Generator_ctor(void *g);

void FModSound_ctor(void *s);

int FModSound_tryToStopMusicForBGMusic();

void ParticleSettingsRef_initialize();

int Station_getIndex(int station);

static Status **g_status;

static void *const gLB_dest = nullptr;

void Globals::reportLeaderboards() {
    int kills = Status::gStatus->getKills();
    *(int *) gLB_dest = kills;
}

Array<int> *Globals::getSoundResourceList() {
    return soundResources;
}

#pragma pack(push, 1)
struct Q16 { int32x4_t v; };

struct HintsBuffer {
    union {
        Q16 quad[4]; // 16-byte SIMD-aligned views at 0x00/0x10/0x20/0x30
        struct {
            uint8_t _pad2b[0x2b];
            Q16     quadAt2b; // unaligned 16-byte store overlapping quad[2]/quad[3]
        };
    };
};
#pragma pack(pop)

static void *const gHints = nullptr;

void Globals::resetHints() {
    HintsBuffer *hints = (HintsBuffer *) gHints;
    const int32x4_t z = vdupq_n_s32(0);
    hints->quad[0].v = z;    // 0x00
    hints->quadAt2b.v = z;   // 0x2b
    hints->quad[2].v = z;    // 0x20
    hints->quad[1].v = z;    // 0x10
}

void Globals::startNewSoundResourceList() {
    if (this->soundResources != nullptr) {
        ArrayRemoveAll(*(this->soundResources));
        delete this->soundResources;
    }
    this->soundResources = nullptr;
    Array<int> *list = new Array<int>();
    this->soundResources = list;
    ArrayAdd<int>(0x7c, *list);
    ArrayAdd<int>(0x7b, *this->soundResources);
}

static void *const gItemNameGameText = nullptr;

String Globals::getItemName(int item) {
    String *src = (String *) ((GameText *) (*(void **) gItemNameGameText))->getText(item + 0x4fa);
    return *(String *) src;
}

String Globals::getKeyActionName(int action) {
    (void) action;
    String r;
    return r;
}

float Globals::sqrt(float x) {
    (void) this;
    return __builtin_sqrtf(x);
}

static void *const gDrinks_a = nullptr;
static void *const gDrinks_rng = nullptr;

void Globals::getRandomSystemForDrinks() {
    int slot = *(int *) gDrinks_a;
    int picked = nextInt_71ad0((AbyssEngine::AERandom *) *(int *) gDrinks_rng, 0x16);
    *(int *) (long) slot = picked;
}

void Globals::addSoundResourceToList(int snd) {
    Array<int> *a = this->soundResources;
    if (a == nullptr) {
        return;
    }
    for (unsigned i = 0; i < a->size(); ++i) {
        if ((*a)[i] == snd) {
            return;
        }
    }
    ArrayAdd<int>(snd, *a);
}

String Globals::replaceKeyBindingTokens(const String &src) {
    return src;
}

struct FileRead {
    Station *loadStation(int32_t id);

    Array<int32_t> *loadWreckCollision(int32_t id);

    Array<Item *> *loadItemsBinary();

    Array<Ship *> *loadShipsBinary();

    Array<String *> *loadNamesBinary(int32_t type, bool first, bool second);
};

static void *const gStationRng = nullptr;

Station *Globals::getRandomStation() {
    FileRead *f = (FileRead *) ::operator new(1);
    FileRead_ctor(f);
    int which = nextInt_71ad0((AbyssEngine::AERandom *) *(int *) gStationRng, 0x87);
    Station *r = f->loadStation(which);
    ::operator delete(FileRead_dtor(f));
    return r;
}

void Globals::drawLines(unsigned int font, Array<String *> *lines, int baseX, int startY) {
    drawLines(font, lines, baseX, startY, false);
}

void Globals::reportSupernovaChallengeScore() {
}

static const char gGLA_newline[] = "";

void Globals::getLineArray(unsigned int font, const String &text, int maxWidth,
                           Array<String *> *out) {
    String *line = static_cast<String *>(::operator new(sizeof(String)));
    { String *_s = line; if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }

    String work;
    work.Set((const_cast<String *>(&text))->data);
    String nl;
    nl.ctor_char(gGLA_newline, false);
    work += nl;

    const int total = static_cast<int>(work.size());

    unsigned count = 0;
    for (int consumed = 0; consumed < total;) {
        String rest;
        rest = work.SubString(consumed, total);
        getLine(font, rest, maxWidth, line);
        consumed += static_cast<int>(line->size());
        count++;
    }
    { String *_s = line; if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    ::operator delete(line);

    ArraySetLength(count, *out);
    for (unsigned i = 0; i < count; i++) {
        String *s = static_cast<String *>(::operator new(sizeof(String)));
        { String *_s = s; if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        (*out)[i] = s;
    }

    int consumed = 0;
    for (unsigned i = 0; i < count; i++) {
        String *slot = (*out)[i];
        String rest;
        rest = work.SubString(consumed, total);
        getLine(font, rest, maxWidth, slot);
        consumed += static_cast<int>(slot->size());

        int lo = 0;
        int hi = static_cast<int>(slot->size());
        while (*slot->index(lo) == 0x20) {
            lo++;
        }
        hi++;
        do {
            hi--;
        } while (*slot->index(hi - 2) == 0x20);

        String trimmed;
        trimmed = slot->SubString(lo, hi);
        *slot = trimmed;
    }
}

static void *const gLTS2_guardHolder = nullptr;
static const char gLTS2_secTens[] = "";
static const char gLTS2_secEmpty[] = "";
static const char gLTS2_minTens[] = "";
static const char gLTS2_minEmpty[] = "";
static const char gLTS2_zeroPrefix[] = "";
static const char gLTS2_hrTens[] = "";
static const char gLTS2_hrEmpty[] = "";
static const char gLTS2_sep1[] = "";
static const char gLTS2_sep2[] = "";

void Globals::longToTimeString(long long ms, String &out) {
    long long secQ = ms / 1000;
    int seconds = (int) (secQ % 0x3c);

    long long minQ = ms / 0xea60;
    int minute = (int) (minQ % 0x3c);

    long long hrQ = ms / 0xea60;
    int hours = (int) (hrQ % 0x18);

    String secPart, secNum, secStr;
    secPart.ctor_char(seconds < 10 ? gLTS2_secTens : gLTS2_secEmpty, false);
    secNum.Set((long long) (seconds));
    secStr = secPart + secNum;

    String minPart, minNum, minStr;
    minPart.ctor_char(minute < 10 ? gLTS2_minTens : gLTS2_minEmpty, false);
    minNum.Set((long long) (minute));
    minStr = minPart + minNum;

    if (hours == 0) {
        String prefix, left;
        prefix.ctor_char(gLTS2_zeroPrefix, false);
        left = prefix + minStr;
        out = left + secStr;
    } else {
        long long h = ms / 0xea60;
        int hv = (int) h * 0x18 + hours;

        String hrPart, hrNum, hrStr;
        hrPart.ctor_char(hv < 10 ? gLTS2_hrTens : gLTS2_hrEmpty, false);
        hrNum.Set((long long) (hv));
        hrStr = hrPart + hrNum;

        String s1, a, b, s2, c;
        s1.ctor_char(gLTS2_sep1, false);
        a = s1 + hrStr;
        b = a + minStr;
        s2.ctor_char(gLTS2_sep2, false);
        c = b + s2;
        out = c + secStr;
    }

    return;
}

static void *const gGBS_guardHolder = nullptr;
static void *const gGBS_strPtr = nullptr;
static void *const gGBS_canvas = nullptr;
static const char gGBS_prefix[] = "";

String Globals::getBoundedString(const String &text, int width) {
    int *guardP = *(int **) gGBS_guardHolder;
    volatile int saved = *guardP;

    String result;
    result.Set((const_cast<String *>(&text))->data);

    String **strPtr = *(String ***) gGBS_strPtr;
    int **canvas = *(int ***) gGBS_canvas;
    int w = ((PaintCanvas *) (long) **canvas)->GetTextWidth(0, **strPtr);
    if (width < w) {
        String *line = (String *) ::operator new(0xc);
        { String *_s = line; if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }

        int font = (int) (long) *strPtr;
        String tmpText;
        tmpText.Set((const_cast<String *>(&text))->data);
        Globals::gGlobals->getLine((unsigned) font, tmpText, width - 3, line);

        String prefix;
        prefix.ctor_char(gGBS_prefix, false);
        result = prefix + *line;

        { String *_s = line; if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    }

    return result;
}

static inline unsigned short globals_u16(float value) {
    return (unsigned short) ((0.0f < value) ? (short) (int) value : 0);
}

void Globals::setCoordsSteer(int p1, int p2, int p3, int p4,
                             unsigned short &o5, unsigned short &o6, unsigned short &o7,
                             unsigned short &o8, unsigned short &o9, unsigned short &o10,
                             unsigned short &o11, unsigned short &o12, unsigned short &o13,
                             unsigned short &o14) {
    int bottom = ((-25 - p2) - p3) + Globals::h;
    float threshold = Globals::iPadHD ? 210.9375f : (Globals::iPadLarge ? 300.0f : 150.0f);
    if (bottom < p1) {
        p1 = bottom;
    }
    float fp1 = (float) p1;

    float center;
    if (threshold <= fp1) {
        center = fp1;
    } else if (Globals::iPadHD) {
        center = 210.0f;
    } else {
        center = Globals::iPadLarge ? 300.0f : 150.0f;
    }

    if (Globals::iPadHD) {
        o5 = 28;
        o6 = (unsigned short) (int) center;
        o13 = 28;
        o14 = globals_u16((float) o6 - 126.5625f);
        o7 = 20;
    } else {
        bool large = Globals::iPadLarge != 0;
        o5 = large ? 40 : 20;
        o6 = (unsigned short) (int) center;
        o13 = o5;
        o14 = globals_u16((float) o6 - (large ? 180.0f : 90.0f));
        o7 = 20;
    }

    o8 = globals_u16((float) o6 + (Globals::iPadHD ? 92.8125f : (Globals::iPadLarge ? 132.0f : 66.0f)));
    o9 = (unsigned short) (o7 + (short) ((unsigned) p2 >> 1));
    o10 = (unsigned short) (o8 + (short) ((unsigned) p2 >> 1));

    float tail;
    float minGap;
    if (Globals::iPadHD) {
        tail = (float) o6 + 313.59375f;
        minGap = 2.8125f;
    } else if (Globals::iPadLarge) {
        tail = (float) o6 + 446.0f;
        minGap = 4.0f;
    } else {
        tail = (float) o6 + 223.0f;
        minGap = 2.0f;
    }
    o12 = globals_u16(tail);

    float top = (float) (Globals::h - p4);
    if ((float) o12 <= top - minGap) {
        o11 = Globals::iPadHD ? 28 : (Globals::iPadLarge ? 40 : 20);
    } else {
        unsigned int overflow = globals_u16((float) o12 - (top - minGap));
        o12 = (unsigned short) overflow;

        int hi;
        int lo;
        if (Globals::iPadHD) {
            hi = 243;
            lo = 187;
        } else if (Globals::iPadLarge) {
            hi = 346;
            lo = 266;
        } else {
            hi = 173;
            lo = 133;
        }

        float scale = (float) overflow / 54.0f;
        if (scale > 1.0f) scale = 1.0f;
        o11 = globals_u16((float) lo + scale * (float) (hi - lo));
        o12 = globals_u16(top - minGap);
    }
}

static void *const gGAMT_guard = nullptr;
static const char gGAMT_noAgent[] = "";
static void *const gGAMT_busyObj = nullptr;
static void *const gGAMT_modText = nullptr;

String Globals::getAgentMissionText(Agent *agent) {
    int *guardP = *(int **) gGAMT_guard;
    volatile int saved = *guardP;

    String result;

    if (agent == nullptr) {
        result.ctor_char(gGAMT_noAgent, false);
        return result;
    }

    String acc;

    if (agent->isGenericAgent() == 0) {
        int event = agent->getEvent();
        if (event < 1 && agent->hasAcceptedOffer() == 0) {
            AgentBusyObject **busySlot = *(AgentBusyObject ***) gGAMT_busyObj;
            AgentBusyObject *busy = *busySlot;

            busy->guardCounter += 1;
            int offer = agent->getOffer();

            if (offer == 8) {
                int ship = (int) (long) Status::gStatus->getShip();
                int price = ((Ship *) (ship))->getPrice();
                int pct = agent->getModPricePercentage();
                agent->setSellItemPrice(idiv(price * pct, 100));
                ship = (int) (long) Status::gStatus->getShip();
                int modIdx = agent->getSellModIndex();
                if (((Ship *) (ship))->hasModInstalled(modIdx) != 0) {
                    void *t = ((GameText *) ((void *) (long) **(int **) gGAMT_modText))->getText(modIdx);
                    (void) t;
                    busy->guardCounter -= 1;
                    result.Set((acc).data);
                    return result;
                }
            }

            {
                Agent *agentArg = agent;
                int bamtOffer = offer;
                String mt;
                { if (mt.data) delete[] mt.data; mt.data = nullptr; mt.length = 0; }
                if (bamtOffer < 0 || agentArg == 0) {
                    void *line = ((GameText *) (void *) (long) **(int **) &g_status)->getText(0x300);
                    mt = *(String *) line;
                    acc.Set((mt).data);
                } else {
                    int baseKey;
                    switch (bamtOffer) {
                        case 8: baseKey = 0x36a;
                            break;
                        case 9: baseKey = 0x36a;
                            break;
                        case 10: baseKey = 0x36a;
                            break;
                        case 4: baseKey = 0x36a;
                            break;
                        default: baseKey = 0x36a;
                            break;
                    }
                    void *base = ((GameText *) (void *) (long) **(int **) &g_status)->getText(baseKey);
                    mt = *(String *) base;
                    {
                        String tok;
                        { if (tok.data) delete[] tok.data; tok.data = nullptr; tok.length = 0; }
                        ((Agent *) agentArg)->getName();
                    }
                    acc.Set((mt).data);
                }
            }
            busy->guardCounter -= 1;
        } else {
            {
                Agent *agentArg = agent;
                int bamtOffer = -1;
                String mt;
                { if (mt.data) delete[] mt.data; mt.data = nullptr; mt.length = 0; }
                if (bamtOffer < 0 || agentArg == 0) {
                    void *line = ((GameText *) (void *) (long) **(int **) &g_status)->getText(0x300);
                    mt = *(String *) line;
                    acc.Set((mt).data);
                } else {
                    int baseKey;
                    switch (bamtOffer) {
                        case 8: baseKey = 0x36a;
                            break;
                        case 9: baseKey = 0x36a;
                            break;
                        case 10: baseKey = 0x36a;
                            break;
                        case 4: baseKey = 0x36a;
                            break;
                        default: baseKey = 0x36a;
                            break;
                    }
                    void *base = ((GameText *) (void *) (long) **(int **) &g_status)->getText(baseKey);
                    mt = *(String *) base;
                    {
                        String tok;
                        { if (tok.data) delete[] tok.data; tok.data = nullptr; tok.length = 0; }
                        ((Agent *) agentArg)->getName();
                    }
                    acc.Set((mt).data);
                }
            }
        }
    } else {
        {
            Agent *agentArg = agent;
            int bamtOffer = -1;
            String mt;
            { if (mt.data) delete[] mt.data; mt.data = nullptr; mt.length = 0; }
            if (bamtOffer < 0 || agentArg == 0) {
                void *line = ((GameText *) (void *) (long) **(int **) &g_status)->getText(0x300);
                mt = *(String *) line;
                acc.Set((mt).data);
            } else {
                int baseKey;
                switch (bamtOffer) {
                    case 8: baseKey = 0x36a;
                        break;
                    case 9: baseKey = 0x36a;
                        break;
                    case 10: baseKey = 0x36a;
                        break;
                    case 4: baseKey = 0x36a;
                        break;
                    default: baseKey = 0x36a;
                        break;
                }
                void *base = ((GameText *) (void *) (long) **(int **) &g_status)->getText(baseKey);
                mt = *(String *) base;
                {
                    String tok;
                    { if (tok.data) delete[] tok.data; tok.data = nullptr; tok.length = 0; }
                    ((Agent *) agentArg)->getName();
                }
                acc.Set((mt).data);
            }
        }
    }

    result.Set((acc).data);
    return result;
}

static void *const gIAP_guardHolder = nullptr;
static const char gIAP_prefixA[] = "";
static const char gIAP_prefixB[] = "";
static const char gIAP_id50[] = "";
static const char gIAP_id0[] = "";
static const char gIAP_id1[] = "";
static const char gIAP_id2[] = "";
static const char gIAP_id3[] = "";
static const char gIAP_id4[] = "";
static const char gIAP_id51[] = "";
static const char gIAP_id52[] = "";
static const char gIAP_id53[] = "";
static const char gIAP_id54[] = "";

int Globals::getInAppPurchaseArrayIndex(int productCode, Array<String *> *list) {
    int *guardP = *(int **) gIAP_guardHolder;
    volatile int saved = *guardP;
    int result = -1;

    if (list != 0) {
        unsigned len = list->size();
        unsigned i = 0;
        bool keepGoing = true;
        while (i < len && keepGoing) {
            String *entry = (*list)[i];
            String base, pb, prefix;
            base.ctor_char(gIAP_prefixA, false);
            pb.ctor_char(gIAP_prefixB, false);
            prefix = base + pb;

            if (entry == 0) {
                result = -1;
                keepGoing = false;
            } else {
                const char *suffix = 0;
                bool known = true;
                switch (productCode) {
                    case 0: suffix = gIAP_id0;
                        break;
                    case 1: suffix = gIAP_id1;
                        break;
                    case 2: suffix = gIAP_id2;
                        break;
                    case 3: suffix = gIAP_id3;
                        break;
                    case 4: suffix = gIAP_id4;
                        break;
                    case 0x32: suffix = gIAP_id50;
                        break;
                    case 0x33: suffix = gIAP_id51;
                        break;
                    case 0x34: suffix = gIAP_id52;
                        break;
                    case 0x35: suffix = gIAP_id53;
                        break;
                    case 0x36: suffix = gIAP_id54;
                        break;
                    default: known = false;
                        break;
                }
                if (!known) {
                    keepGoing = true;
                } else {
                    String suf, full;
                    suf.ctor_char(suffix, false);
                    full = prefix + suf;
                    int cmp = entry->Compare_str(&full);
                    if (cmp == 0) {
                        result = (int) i;
                        keepGoing = false;
                    } else {
                        keepGoing = true;
                    }
                }
            }
            i++;
        }
    }

    return result;
}

String Globals::getKeyBindingReplaceString(int key) {
    (void) key;

    String tmp;
    { if (tmp.data) delete[] tmp.data; tmp.data = nullptr; tmp.length = 0; }
    tmp.ToUpperCase();
    String result;
    return result;
}

static void *const gLTS_guardHolder = nullptr;
static const char gLTS_minTens[] = "";
static const char gLTS_minEmpty[] = "";
static const char gLTS_hrTens[] = "";
static const char gLTS_hrEmpty[] = "";
static const char gLTS_sep[] = "";

void Globals::longToTimeStringNoSeconds(long long ms, String &out) {
    long long q = ms / 0xea60;
    int minute = (int) (q % 0x3c);

    long long q2 = ms / 0xea60;
    int hours = (int) (q2 % 0x18);

    String mPart, mNum, minStr;
    mPart.ctor_char(minute < 10 ? gLTS_minTens : gLTS_minEmpty, false);
    mNum.Set((long long) (minute));
    minStr = mPart + mNum;

    long long h = ms / 0xea60;
    int hv = (int) h * 0x18 + hours;

    String hPart, hNum, hrStr;
    hPart.ctor_char(hv < 10 ? gLTS_hrTens : gLTS_hrEmpty, false);
    hNum.Set((long long) (hv));
    hrStr = hPart + hNum;

    String sep, left;
    sep.ctor_char(gLTS_sep, false);
    left = sep + hrStr;
    out = left + minStr;

    return;
}

static const unsigned short gGSG_resTable[1] = {};
static const unsigned short gGSG_meshTable[1] = {};
static const short gGSG_extraTable[1] = {};
static const unsigned gGSG_lodTable[1] = {};
static const unsigned gGSG_childTable[1] = {};

AEGeometry *Globals::getShipGroup(int kind, int variant, bool wireframe) {
    PaintCanvas *canvas = PaintCanvas::gCanvas;

    if (kind == 0xf) {
        AEGeometry *geom;

        if (variant == 0) {
            geom = new AEGeometry((uint16_t) 0x42a9, canvas, false);

            unsigned mesh0 = 0xffffffff, mesh1 = 0xffffffff, mesh2 = 0xffffffff;
            canvas->TransformCreate(mesh0);
            canvas->TransformAddMesh(mesh0, 0x42ae, false);
            geom->addChild(mesh0);
            canvas->TransformCreate(mesh1);
            canvas->TransformAddMesh(mesh1, 0x42b2, false);
            geom->addChild(mesh1);
            canvas->TransformCreate(mesh2);
            canvas->TransformAddMesh(mesh2, 0x42ad, false);
            geom->addChild(mesh2);

            uint16_t lodMeshes[2] = {0x42aa, 0x42ab};
            int lodDists[2] = {25000, 45000};
            geom->setLodMeshes(lodMeshes, lodDists, 2);
            uint16_t lodChild = 0x42ae;
            geom->setLodChildMeshes(&lodChild);
        } else if (variant == 3) {
            geom = new AEGeometry((uint16_t) 0x4299, canvas, false);

            unsigned head = 0xffffffff;
            canvas->TransformCreate(head);
            canvas->TransformAddMesh(head, 0x4299, true);
            geom->addChild(head);

            int segments = AERandom_nextIntB(0, 4);
            unsigned prevA = 0xffffffff, prevB = 0xffffffff;
            for (int i = 0; i < segments; i++) {
                unsigned a = 0xffffffff, b = 0xffffffff;
                canvas->TransformCreate(a);
                canvas->TransformCreate(b);
                canvas->TransformAddMesh(a, 0x429a, true);
                canvas->TransformAddMesh(b, 0x429a, true);
                unsigned keepA = a, keepB = b;
                if (prevA != 0xffffffff) {
                    canvas->TransformAddChild(prevA, a);
                    canvas->TransformAddChild(prevB, b);
                    keepA = prevA;
                    keepB = prevB;
                }
                prevA = keepA;
                prevB = keepB;
            }
            if (prevA != 0xffffffff) {
                geom->addChild(prevA);
            }

            unsigned tail = 0xffffffff;
            canvas->TransformCreate(tail);
            canvas->TransformAddMesh(tail, 0x429a, true);
            geom->addChild(tail);

            uint16_t lodMeshes[1] = {0x429a};
            int lodDists[1] = {35000};
            geom->setLodMeshes(lodMeshes, lodDists, 1);
            geom->setLodChildTransform(prevB);
        } else {
            geom = new AEGeometry((uint16_t) 0x42a4, canvas, false);

            unsigned mesh0 = 0xffffffff, mesh1 = 0xffffffff;
            canvas->TransformCreate(mesh0);
            canvas->TransformAddMesh(mesh0, 0x42a4, true);
            geom->addChild(mesh0);
            canvas->TransformCreate(mesh1);
            canvas->TransformAddMesh(mesh1, 0x42a4, true);
            geom->addChild(mesh1);

            uint16_t lodMeshes[2] = {0x42a6, 0x42a7};
            int lodDists[2] = {35000, 60000};
            geom->setLodMeshes(lodMeshes, lodDists, 2);
        }

        return geom;
    }
    if (kind == 0xe || kind == 0xd) {
        int resId = (kind == 0xe) ? 0x37e7 : 0x4275;
        AEGeometry *geom = new AEGeometry((uint16_t) resId, canvas, false);
        unsigned t0 = 0xffffffff;
        canvas->TransformCreate(t0);
        canvas->TransformAddMesh((unsigned) t0, 0, (bool) (1));
        geom->addChild(t0);
        unsigned t1 = 0xffffffff;
        canvas->TransformCreate(t1);
        canvas->TransformAddMesh((unsigned) t1, 0, (bool) (1));
        geom->addChild(t1);
        unsigned short lodMeshes[2] = {0, 0};
        int dist[2];
        dist[0] = (kind == 0xe) ? 35000 : 35000;
        dist[1] = (kind == 0xe) ? 60000 : 45000;
        geom->setLodMeshes(lodMeshes, dist, 2);
        unsigned short childMeshes[1] = {0};
        geom->setLodChildMeshes(childMeshes);
        if (kind == 0xe) {
            geom->setScaling(1.0f);
        }
        return geom;
    }

    {
        AEGeometry *geom = new AEGeometry((uint16_t) gGSG_resTable[kind], canvas, true);
        unsigned short mesh = gGSG_meshTable[kind];
        unsigned mainT = 0xffffffff;
        unsigned mainMesh = 0xffffffff;
        if (mesh != 0xffff) {
            canvas->MeshCreate(mesh, mainMesh, true);
            canvas->TransformCreate(mainT);
            canvas->TransformAddMeshId(mainT, mainMesh);
            geom->addChild(mainT);
            geom->meshHandle = mainMesh;
        }
        if (!wireframe) {
            unsigned short mat = (unsigned short) ((short) kind + 0x7dc8);
            unsigned matH = 0xffffffff;
            canvas->MaterialCreate(mat, matH);
            canvas->MeshChangeResourceMaterial(geom->meshId, mat);
        }
        short extra = gGSG_extraTable[kind];
        if (extra != -1) {
            unsigned t = 0xffffffff;
            canvas->TransformCreate(t);
            canvas->TransformAddMesh((unsigned) t, 0, (bool) ((int) (unsigned char) (char) extra));
            geom->addChild(t);
        }
        if (wireframe) {
            if (kind != 0x27 && kind != 0x29) {
                unsigned t = 0xffffffff;
                canvas->TransformCreate(t);
                canvas->TransformAddMesh((unsigned) t, 0, (bool) ((int) (char) (-0x14 + (char) kind)));
                geom->addChild(t);
            }
        } else {
            unsigned t = 0xffffffff;
            canvas->TransformCreate(t);
            canvas->TransformAddMesh((unsigned) t, 0, (bool) ((int) (char) (0x50 + (char) kind)));
            geom->addChild(t);
        }

        unsigned lastVisibleDist = 2;
        const unsigned *lod = &gGSG_lodTable[kind * 3];
        unsigned count = 0;
        for (int i = 0; i != 2; i++) {
            if (lod[i] != 0xffff) count++;
        }
        if (count != 0) {
            unsigned short *meshes = new unsigned short[count];
            unsigned *ids = new unsigned[count];
            int *dist = new int[count];
            int d = 5000;
            unsigned *idp = ids;
            const unsigned *src = lod;
            for (unsigned i = 0; i < count; i++) {
                unsigned m = *src;
                dist[i] = d;
                meshes[i] = (unsigned short) m;
                if (!wireframe) {
                    canvas->MeshCreate((unsigned short) m, *idp, true);
                    canvas->MeshChangeResourceMaterial(*idp,
                                                       (unsigned short) ((short) kind + 0x7dc8));
                }
                d += 8000;
                idp++;
                src++;
            }
            lastVisibleDist = (unsigned) d;
            if (wireframe) {
                geom->setLodMeshes(meshes, dist, count);
            } else {
                geom->setLodMeshesWithMeshIds(meshes, ids, dist, count);
            }

            const unsigned *childSrc = &gGSG_childTable[kind * 3];
            int childCount = 0;
            for (int i = 0; i != 2; i++) {
                if (childSrc[i] != 0xffff) childCount++;
            }
            if (childCount != 0) {
                unsigned short *childMeshes = new unsigned short[childCount];
                for (int i = 0; i != childCount; i++) {
                    childMeshes[i] = (unsigned short) childSrc[i];
                }
                geom->setLodChildMeshes(childMeshes);
                delete[] childMeshes;
            }
            delete[] meshes;
            delete[] dist;
        }
        geom->setLodLastVisibleDistance(lastVisibleDist);
        return geom;
    }
}

static void *const gREF_rng1 = nullptr;
static void *const gREF_rng2 = nullptr;
static const int gREF_table = 0;

unsigned Globals::getRandomEnemyFighter(int kind) {
    (void) this;
    int t = kind;
    if ((unsigned) (kind - 9) > 1) {
        t = 8;
    }
    if (kind <= 3) {
        t = kind;
    }
    unsigned r;
    if (t == 1) {
        if (Status::gStatus->dlc1Won() == 0) {
            r = 9;
        } else {
            int n = AERandom_nextIntB(*(int *) gREF_rng1, 0x64);
            r = 0x27;
            if (n < 0x55) {
                r = 0x29;
            }
            if (n < 0x3c) {
                r = 9;
            }
        }
    } else if (t == 9) {
        r = 8;
    } else if (t == 0xa) {
        r = 0x2c;
    } else {
        const int *table = &gREF_table;
        int *rng = (int *) gREF_rng2;
        do {
            do {
                r = AERandom_nextIntB(*rng, 0x25);
            } while ((unsigned) ((r & ~4u) - 9) < 2);
        } while (((r < 0x10) && ((1u << r & 0x8101u) != 0)) ||
                 (table[r] != t));
    }
    return r;
}

static void *const gDL2_canvas = nullptr;
static void *const gDL2_lineHeight = nullptr;

void Globals::drawLines(unsigned int font, Array<String *> *lines, int baseX, int startY,
                        unsigned int rightX, bool centered) {
    int *cv = (int *) gDL2_canvas;
    LineMetrics **lh = (LineMetrics **) gDL2_lineHeight;
    int yacc = startY;
    int dx = 0;
    for (unsigned i = 0; i < lines->size(); i++) {
        if (!centered) {
            int w = ((PaintCanvas *) (long) *cv)->GetTextWidth(font, *(*lines)[i]);
            dx = (int) rightX - w;
        }
        ((PaintCanvas *) (long) *cv)->DrawString(font, *(*lines)[i], dx + baseX, yacc, false);
        yacc += (*lh)->lineHeight;
    }
}

static void *const gCBB_counter = nullptr;
static void *const gCBB_canvas = nullptr;

unsigned int Globals::createBillBoard(int p1, int height, float u0, float v0, float u1, float v1,
                                      int width) {
    (void) p1;
    unsigned *counter = *(unsigned **) gCBB_counter;
    int *canvasP = *(int **) gCBB_canvas;
    int snapshot = *counter;

    unsigned int meshOut = 0;
    ((PaintCanvas *) (long) *canvasP)->MeshCreate((unsigned short) 0xc, (unsigned short) 6, (signed char) 0x13,
                                                  (unsigned short) 0, meshOut);
    int mesh = (int) meshOut;
    int cv = *canvasP;

    PaintCanvas *meshCanvas = (PaintCanvas *) (long) cv;
    meshCanvas->MeshSetTriangle((unsigned int) mesh, 0, 0, 1, 2);
    meshCanvas->MeshSetTriangle((unsigned int) mesh, 1, 2, 1, 3);
    meshCanvas->MeshSetTriangle((unsigned int) mesh, 2, 4, 5, 6);
    meshCanvas->MeshSetTriangle((unsigned int) mesh, 3, 6, 5, 7);
    meshCanvas->MeshSetTriangle((unsigned int) mesh, 4, 8, 9, 10);
    meshCanvas->MeshSetTriangle((unsigned int) mesh, 5, 10, 9, 0xb);

    meshCanvas->MeshSetUv((unsigned int) mesh, 0, u0, 0);
    meshCanvas->MeshSetUv((unsigned int) mesh, 1, u1, 0);
    meshCanvas->MeshSetUv((unsigned int) mesh, 2, v1, u0);
    meshCanvas->MeshSetUv((unsigned int) mesh, 3, v1, v0);
    meshCanvas->MeshSetUv((unsigned int) mesh, 4, u0, 0);
    meshCanvas->MeshSetUv((unsigned int) mesh, 5, u1, 0);
    meshCanvas->MeshSetUv((unsigned int) mesh, 6, v1, u0);
    meshCanvas->MeshSetUv((unsigned int) mesh, 7, v1, v0);
    meshCanvas->MeshSetUv((unsigned int) mesh, 8, u0, 0);
    meshCanvas->MeshSetUv((unsigned int) mesh, 9, u1, 0);
    meshCanvas->MeshSetUv((unsigned int) mesh, 10, v1, u0);
    meshCanvas->MeshSetUv((unsigned int) mesh, 0xb, v1, v0);

    float pw = VectorSignedToFloat(width, 0);
    float nh = VectorSignedToFloat(-height, 0);
    meshCanvas->MeshSetPoint((unsigned int) mesh, 0, nh, 0, pw);
    float nw = VectorSignedToFloat(-width, 0);
    meshCanvas->MeshSetPoint((unsigned int) mesh, 1, nh, 0, nw);
    float ph = VectorSignedToFloat(height, 0);
    meshCanvas->MeshSetPoint((unsigned int) mesh, 2, ph, 0, pw);
    meshCanvas->MeshSetPoint((unsigned int) mesh, 3, ph, 0, nw);
    meshCanvas->MeshSetPoint((unsigned int) mesh, 4, 0, nh, pw);
    meshCanvas->MeshSetPoint((unsigned int) mesh, 5, 0, nh, nw);
    meshCanvas->MeshSetPoint((unsigned int) mesh, 6, 0, ph, pw);
    meshCanvas->MeshSetPoint((unsigned int) mesh, 7, 0, ph, nw);
    meshCanvas->MeshSetPoint((unsigned int) mesh, 8, nh, ph, 0);
    meshCanvas->MeshSetPoint((unsigned int) mesh, 9, nh, nh, 0);
    meshCanvas->MeshSetPoint((unsigned int) mesh, 10, ph, ph, 0);
    meshCanvas->MeshSetPoint((unsigned int) mesh, 0xb, ph, nh, 0);

    return meshOut;
}

void BoundingAAB_ctor(void *self, float x0, float y0, float z0, float x1, float y1,
                                 float z1);

static void *const gGWC_guardHolder = nullptr;

Array<BoundingVolume *> *Globals::getWreckCollision(int kind, AEGeometry *geom) {
    int *guardP = *(int **) gGWC_guardHolder;
    volatile int saved = *guardP;

    void *fr = ::operator new(1);
    FileRead_ctor(fr);
    Array<int> *data = ((FileRead *) fr)->loadWreckCollision(kind);
    ::operator delete(FileRead_dtor(fr));

    Array<BoundingVolume *> *outArr = nullptr;
    if (data != 0) {
        int count = (*data)[0];

        float v[3] = {0, 0, 0};
        float c[3] = {0, 0, 0};

        outArr = new Array<BoundingVolume *>();
        ArraySetLength((unsigned) count, *outArr);

        int pos = 1;
        for (int i = 0; i < count; i++) {
            int *base = data->data();
            int kindWord = base[pos];
            void *bound = 0;
            if (kindWord == 1) {
                int *rec = base + pos;
                c[2] = (float) (-base[pos + 1]);
                c[0] = (float) rec[2];
                c[1] = (float) rec[3];
                v[0] = (float) rec[4];
                v[1] = (float) rec[5];
                v[2] = (float) rec[6];
                float mag = VectorScale(v, c[1]);
                if (geom == 0) {
                    mag = VectorScale(c, mag);
                    VectorScale(v, mag);
                }
                bound = ::operator new(0x2c);
                BoundingAAB_ctor(bound, v[2] + v[2], c[1] + c[1], v[1] + v[1], c[2], c[0], v[0]);
                pos += 7;
            } else if (kindWord == 0) {
                int *rec = base + pos;
                c[2] = (float) (-base[pos + 1]);
                c[0] = (float) rec[2];
                c[1] = (float) rec[3];
                v[0] = (float) rec[4];
                VectorScale(v, v[0]);
                float mag = v[0];
                if (v[0] < 0.0f) {
                    mag = VectorScale(v, v[0]);
                }
                if (geom == 0) {
                    mag = VectorScale(c, mag);
                    VectorScale(v, mag);
                }
                bound = ::operator new(0x48);
                new(bound) BoundingSphere(c[2], c[1], c[0], 0.0f, 0.0f, 0.0f, v[0]);
                pos += 5;
            } else {
                pos += 1;
                continue;
            }
            (*outArr)[i] = reinterpret_cast<BoundingVolume *>(bound);
        }

        ArrayRemoveAll(*data);
        delete data;
    }

    return outArr;
}

// Globals::Globals() below zero-initializes the Globals static members directly.
// The original accesses these through GOT-indirect loads; mapping the former
// gGC_p_* placeholders to real members keeps the constructor from folding away.

Globals::Globals() {
    Globals *self = this;
    GameSettingsRecord *settings = (GameSettingsRecord *) Globals::options;
    CtorSecondaryObject *secondary = (CtorSecondaryObject *) Globals::hints;

    Globals::Canvas = 0;
    Globals::shipTemplate = 0;
    Globals::gameText = 0;
    Globals::font = 0;
    Globals::fontAlien = 0;
    Globals::fontLangSelect = 0;
    Globals::rnd = 0;
    Globals::globals = 0;
    Globals::layout = 0;
    Globals::appManager = 0;
    Globals::recordHandler = 0;
    Globals::generator = 0;

    *(int *) self = 0;

    settings->colorR = 0.5f;
    settings->colorG = 0.5f;
    settings->colorB = 0.5f;
    settings->volumePair = 0x101;
    settings->flagAt10 = 0;

    settings->glowR = 0.5f;
    settings->glowG = 0.5f;

    settings->tintR = 0.6f;
    settings->tintG = 0.6f;
    settings->tintB = 0.5f;
    settings->brightness = 1.0f;
    settings->contrast = 0.5f;
    settings->ambientVolume = 1;
    settings->enableFlag30 = 1;
    settings->shortAt40 = 0;
    settings->intAt50 = 0;
    settings->flagAt39 = 0;
    settings->intAt35 = 0;
    settings->intAt31 = 0;
    secondary->slot1 = 0;
    secondary->flagAt13 = 0;

    settings->flagAt3f = 0;
    settings->intAt3b = 0;
    settings->flagAt4e = 0;
    settings->shortAt4c = 0;
    settings->intAt48 = 0;
    secondary->slot0 = 0;

    Globals::gameLoaded = 0;
    Globals::gameSaving = 0;
    Globals::lastSpaceMusicPlayed = -1;
    Globals::initMemoryWarning = 0;
    Globals::lastStationMusicPlayed = -1;
    Globals::lastCampaignMissionFailCount = 0;
    Globals::lastCampaignMissionFailed = -1;

    float fv = settings->qualityLevel;
    int v54 = 0x247;
    if (1.0f <= fv) v54 = 0x33e;
    if (fv <= 0.0f) v54 = 0x19f;
    settings->resWidth = v54;

    int v58 = 0x201;
    if (1.0f <= fv) v58 = 0x2da;
    if (fv <= 0.0f) v58 = 0x16d;
    settings->resHeight = v58;

    Globals::instantActionScore = 0;
    Globals::instantActionWave = 0;
    Globals::instantActionType = -1;
    Globals::instantActionPlayerName = 0;
    Globals::items = 0;
    Globals::status = 0;
    settings->shortAt60 = 0x100;
    Globals::galaxy = 0;
    Globals::achievements = 0;
    Globals::imageFactory = 0;
    Globals::ships = 0;
    self->soundResources = 0;
}

static void **const gG_recordHandler = nullptr;
static void **const gG_galaxy = nullptr;
static void **const gG_status = nullptr;
static void **const gG_gameText = nullptr;
static void **const gG_random = nullptr;
static void **const gG_layout = nullptr;
static void **const gG_generator = nullptr;
static void **const gG_polyObj = nullptr;
static void **const gG_fmod = nullptr;
static void **const gG_items = nullptr;
static void **const gG_ships = nullptr;
static void **const gG_achievements = nullptr;
static void **const gG_imageFactory = nullptr;
static int **const gG_tail = nullptr;

struct PolymorphicSingleton {
    virtual ~PolymorphicSingleton() {
    }
};

Globals::~Globals() {
    void **rhSlot = gG_recordHandler;
    if (*rhSlot != 0) {
        ((RecordHandler *) (*rhSlot))->saveOptions();
    }
    void **galSlot = gG_galaxy;
    if (*galSlot != 0) {
        ::operator delete(Galaxy_dtor(*galSlot));
    }
    *galSlot = 0;
    void **statSlot = gG_status;
    if (*statSlot != 0) {
        ::operator delete(Status_dtor(*statSlot));
    }
    *statSlot = 0;
    void **gtSlot = gG_gameText;
    if (*gtSlot != 0) {
        delete (GameText *) (*gtSlot);
    }
    *gtSlot = 0;
    void **rngSlot = gG_random;
    if (*rngSlot != 0) {
        ::operator delete(AERandom_dtor(*rngSlot));
    }
    *rngSlot = 0;
    void **laySlot = gG_layout;
    if (*laySlot != 0) {
        ::operator delete(Layout_dtor(*laySlot));
    }
    *laySlot = 0;
    void **genSlot = gG_generator;
    if (*genSlot != 0) {
        ::operator delete(Generator_dtor(*genSlot));
    }
    *genSlot = 0;
    if (*rhSlot != 0) {
        delete (RecordHandler *) (*rhSlot);
    }
    *rhSlot = 0;
    void **polySlot = gG_polyObj;
    void *poly = *polySlot;
    if (poly != 0) {
        delete static_cast<PolymorphicSingleton *>(poly);
    }
    *polySlot = 0;
    void **fmodSlot = gG_fmod;
    if (*fmodSlot != 0) {
        ::operator delete(FModSound_dtor(*fmodSlot));
    }
    *fmodSlot = 0;
    void **itemSlot = gG_items;
    if (*itemSlot != 0) {
        Array<Item *> *items = (Array<Item *> *) *itemSlot;
        ArrayReleaseClasses(*items);
        delete items;
    }
    *itemSlot = 0;
    void **shipSlot = gG_ships;
    if (*shipSlot != 0) {
        Array<Ship *> *ships = (Array<Ship *> *) *shipSlot;
        ArrayReleaseClasses(*ships);
        delete ships;
    }
    *shipSlot = 0;
    if (*galSlot != 0) {
        ::operator delete(Galaxy_dtor(*galSlot));
    }
    *galSlot = 0;
    void **achSlot = gG_achievements;
    if (*achSlot != 0) {
        ::operator delete(Achievements_dtor(*achSlot));
    }
    *achSlot = 0;
    if (*statSlot != 0) {
        ::operator delete(Status_dtor(*statSlot));
    }
    *statSlot = 0;
    void **ifSlot = gG_imageFactory;
    if (*ifSlot != 0) {
        ::operator delete(ImageFactory_dtor(*ifSlot));
    }
    *ifSlot = 0;

    if (this->soundResources != 0) {
        ArrayRemoveAll(*(this->soundResources));
        delete this->soundResources;
    }
    this->soundResources = 0;
    **gG_tail = 0;
}

static void *const gDL_canvas = nullptr;
static void *const gDL_lineHeight = nullptr;

void Globals::drawLines(unsigned int font, Array<String *> *lines, int baseX, int startY,
                        bool centered) {
    int *cv = (int *) gDL_canvas;
    LineMetrics **lh = (LineMetrics **) gDL_lineHeight;
    int yacc = startY;
    int dx = 0;
    for (unsigned i = 0; i < lines->size(); i++) {
        if (centered) {
            int w = ((PaintCanvas *) (long) *cv)->GetTextWidth(font, *(*lines)[i]);
            dx = -(w >> 1);
        }
        ((PaintCanvas *) (long) *cv)->DrawString(font, *(*lines)[i], dx + baseX, yacc, false);
        yacc += (*lh)->lineHeight;
    }
}

void Globals::setCoordsFire(int p1, int p2, unsigned p3, unsigned p4,
                            unsigned &o5, unsigned short &o6, unsigned short &o7,
                            unsigned short &o8, unsigned short &o9, unsigned short &o10,
                            unsigned short &o11, unsigned short &o12, unsigned short &o13,
                            unsigned short &o14, unsigned short &o15, unsigned short &o16,
                            unsigned short &o17) {
    float widthCap = Globals::iPadHD ? 63.28125f : (Globals::iPadLarge ? 90.0f : 45.0f);
    float base = Globals::iPadHD ? 210.9375f : (Globals::iPadLarge ? 300.0f : 150.0f);
    float colDelta = (float) ((int) Globals::w - (int) p2);
    float candidate = (float) p1;
    if (widthCap + colDelta < candidate)
        candidate = widthCap + colDelta;

    float fireCenter;
    float xAdjust;
    if (base <= candidate) {
        fireCenter = (float) p1;
        if (widthCap + colDelta < fireCenter)
            fireCenter = widthCap + colDelta;
        xAdjust = Globals::iPadHD ? 56.25f : (Globals::iPadLarge ? 80.0f : 40.0f);
    } else {
        fireCenter = Globals::iPadHD ? 210.0f : (Globals::iPadLarge ? 300.0f : 150.0f);
        xAdjust = Globals::iPadHD ? 56.25f : (Globals::iPadLarge ? 80.0f : 40.0f);
    }

    float xBase = (float) ((int) Globals::w - (int) p2);
    o6 = globals_u16(xBase + xAdjust);
    o7 = (unsigned short) (int) fireCenter;
    o8 = (unsigned short) (o6 + (short) (p2 >> 1));
    o9 = (unsigned short) (o7 + (short) (p2 >> 1));

    float tail;
    unsigned short u16;
    if (Globals::iPadHD) {
        o10 = globals_u16((float) o6 + 107.28125f);
        o11 = globals_u16((float) o7 + 5.625f);
        o12 = globals_u16((float) o6 + 18.09375f);
        o13 = globals_u16((float) o7 + 19.28125f);
        u16 = globals_u16((float) o6 + 112.5f);
        tail = 94.21875f;
    } else if (Globals::iPadLarge) {
        o10 = globals_u16((float) o6 + 152.0f);
        o11 = globals_u16((float) o7 + 8.0f);
        o12 = globals_u16((float) o6 + 28.0f);
        o13 = globals_u16((float) o7 + 27.0f);
        u16 = globals_u16((float) o6 + 160.0f);
        tail = 134.0f;
    } else {
        o10 = globals_u16((float) o6 + 77.0f);
        o11 = globals_u16((float) o7 + 4.0f);
        o12 = globals_u16((float) o6 + 15.0f);
        o13 = globals_u16((float) o7 + 13.0f);
        u16 = globals_u16((float) o6 + 80.0f);
        tail = 67.0f;
    }

    o16 = u16;
    o17 = globals_u16((float) o7 - tail);

    if ((int) fireCenter <= Globals::h - p2) {
        o5 = p4;
        o14 = globals_u16((float) o6 + (Globals::iPadHD ? 100.0625f : (Globals::iPadLarge ? 144.0f : 74.0f)));
        o15 = globals_u16((float) o7 + (Globals::iPadHD ? 209.53125f : (Globals::iPadLarge ? 298.0f : 149.0f)));
    } else {
        o5 = p3;
        o14 = globals_u16((float) o6 + (Globals::iPadHD ? -2.0f : (Globals::iPadLarge ? -2.0f : 0.0f)));
        o15 = globals_u16((float) o7 + (Globals::iPadHD ? 132.1875f : (Globals::iPadLarge ? 189.0f : 94.0f)));
    }
}

static void *const gRR_arg = nullptr;

void Globals::releaseResources() {
    PaintCanvas::gCanvas->ReleaseAllResources();

    PaintCanvas *secondaryCanvas = *(PaintCanvas **) gRR_arg;
    if (secondaryCanvas != nullptr) {
        secondaryCanvas->ReleaseAllResources();
    }
}

static inline unsigned int &globals_font_slot(void *&slot) {
    return *reinterpret_cast<unsigned int *>(&slot);
}

static inline unsigned int &globals_font_slot(int &slot) {
    return *reinterpret_cast<unsigned int *>(&slot);
}

void Globals::loadFont(int kind) {
    auto *canvas = (PaintCanvas *) Globals::Canvas;
    if (canvas == nullptr) return;

    unsigned short glyph = 0x0457;
    short spacing = -2;
    unsigned char isMainFontPersian = 1;
    switch (kind) {
        case 9:
            glyph = 0x2d74;
            spacing = Globals::iPadHD ? -6 : (Globals::retinaDisplay ? -8 : -4);
            isMainFontPersian = 0;
            break;
        case 10:
            glyph = 0x2d78;
            spacing = Globals::iPadHD ? -6 : (Globals::retinaDisplay ? -8 : -4);
            break;
        case 0xb:
            glyph = 0x2d76;
            spacing = Globals::iPadHD ? -6 : (Globals::retinaDisplay ? -8 : -4);
            break;
        case 0xe:
            glyph = 0x2d7c;
            spacing = Globals::iPadHD ? -6 : (Globals::retinaDisplay ? -8 : -4);
            break;
        default:
            if (kind == 0xf) {
                glyph = 0x2d7e;
                spacing = Globals::iPadHD ? -7 : (Globals::retinaDisplay ? -10 : -5);
            } else {
                glyph = 0x0457;
                if (Globals::retinaDisplay)
                    spacing = -5;
                else if (Globals::n9 || Globals::iPadHD)
                    spacing = -4;
                else
                    spacing = -2;
            }
            break;
    }

    canvas->FontCreate(glyph, globals_font_slot(Globals::font), true);
    canvas->FontSetSpacing(globals_font_slot(Globals::font), spacing);
    canvas->field_0x1c = isMainFontPersian;
    canvas->FontCreate(0x051e, globals_font_slot(Globals::fontAlien), true);
    canvas->FontSetSpacing(globals_font_slot(Globals::fontAlien), 0);
    canvas->FontCreate(0x2d7a, globals_font_slot(Globals::fontLangSelect), true);
    canvas->FontSetSpacing(globals_font_slot(Globals::fontLangSelect), 0);
}

// Globals::init accesses these globals directly. The original code reaches them
// through GOT-indirect loads; the former gI_* placeholders are now mapped to the
// recovered members instead of remaining null stubs.

int Globals::init(AbyssEngine::ApplicationManager *app, AbyssEngine::Engine *engine) {
    (void) engine;
    void **missionSlot = &Mission::empty;
    if (*missionSlot == 0) {
        void *m = ::operator new(0x78);
        Mission_ctor(m);
        *missionSlot = m;
    }

    GameSettingsRecord *settings = (GameSettingsRecord *) Globals::options;
    int *flagFFFF = &Globals::lastRecordWritten;
    int *langSettingSlot = (int *) &Globals::recordSlots;
    char *langFlag = (char *) &Globals::iPad;
    char *zeroByte = (char *) &Globals::startLiteVersionWithMoreCredits;

    settings->flagAt11 = 1;
    settings->enableFlag30 = 1;
    settings->colorR = 0.5f;
    settings->colorG = 0.5f;
    settings->colorB = 0.5f;
    settings->tintB = 0.5f;
    settings->brightness = 1.0f;
    *zeroByte = 0;
    char lang = *langFlag;
    settings->flagAt0f = 0x101;
    *flagFFFF = -1;
    *langSettingSlot = (lang == 0) ? 6 : 0xc;

    void *galaxy = ::operator new(8);
    Galaxy_ctor(galaxy);
    Globals::galaxy = galaxy;
    Galaxy::gGalaxy = (Galaxy *) galaxy;
    void *ach = ::operator new(0x28);
    Achievements_ctor(ach);
    Globals::achievements = ach;
    Achievements::gAchievements = (Achievements *) ach;
    void *status = ::operator new(0x1f0);
    Status_ctor(status);
    Globals::status = (Status *) status;
    Status::gStatus = (Status *) status;
    ImageFactory *imgFac = new ImageFactory();
    Globals::imageFactory = imgFac;

    void *fr = ::operator new(1);
    FileRead_ctor(fr);
    Globals::items = ((FileRead *) fr)->loadItemsBinary();
    Globals::ships = ((FileRead *) fr)->loadShipsBinary();
    ::operator delete(FileRead_dtor(fr));

    int *engineSlot = (int *) &Globals::Canvas;
    if (*engineSlot == 0) {
        *engineSlot = *reinterpret_cast<int *>(app);
    }
    Globals::appManager = app;
    ApplicationManager::gAppManager = app;
    app->VibrateEnable(0);

    void *rng = ::operator new(8);
    AERandom_ctor(rng);
    Globals::globals = this;
    Globals::rnd = rng;
    AERandom::gRandom = (AbyssEngine::AERandom *) rng;

    void *gen = ::operator new(1);
    Generator_ctor(gen);
    Globals::generator = gen;

    RecordHandler *rh = new RecordHandler();
    void **rhSlotP = &Globals::recordHandler;
    *rhSlotP = rh;
    Status::gStatus->resetGame();
    ((RecordHandler *) (*rhSlotP))->loadOptions();

    void *fmod = ::operator new(0x243c);
    FModSound_ctor(fmod);
    FModSound **fmodSlotP = &Globals::sound;
    *fmodSlotP = (FModSound *) fmod;
    (*fmodSlotP)->init();

    (*fmodSlotP)->enableCategory(1, settings->sfxVolume);
    (*fmodSlotP)->enableCategory(2, settings->musicVolume);
    (*fmodSlotP)->enableCategory(3, settings->ambientVolume);
    (*fmodSlotP)->setVolume(1, settings->colorR);
    (*fmodSlotP)->setVolume(2, settings->colorG);
    (*fmodSlotP)->setVolume(3, settings->colorB);

    if (FModSound_tryToStopMusicForBGMusic() != 0) {
        settings->sfxVolume = 0;
    }

    Globals::instantActionPoints = 0;
    Globals::first_start_ever = 1;
    InitZeroObject *obj = (InitZeroObject *) Globals::hints;
    obj->slots[0] = 0;
    obj->slots[1] = 0;
    obj->slots[2] = 0;
    obj->slots[3] = 0;
    obj->slots[4] = 0;
    obj->slots[5] = 0;
    obj->slots[6] = 0;
    obj->slots[7] = 0;
    obj->slots[8] = 0;
    obj->slots[9] = 0;
    obj->slots[10] = 0;
    obj->slots[11] = 0;
    obj->tailAt2b = 0;
    obj->tailAt2f = 0;
    obj->tailAt33 = 0;
    obj->tailAt37 = 0;
    InitFlagByte *flagByteObj = (InitFlagByte *) &Globals::gameLoaded;
    flagByteObj->flag = 0;
    Globals::gameSaving = 0;
    Globals::initMemoryWarning = 0;

    Layout *layout = new Layout();
    Globals::layout = layout;
    layout->reload();
    ParticleSettingsRef_initialize();

    Array<int> *arr = new Array<int>();
    this->soundResources = arr;
    return (int) (long) arr;
}

static void *const gPM_snd0 = nullptr;
static void *const gPM_snd1 = nullptr;
static void *const gPM_snd2 = nullptr;
static void *const gPM_sndStatus = nullptr;
static const int gPM_table0 = 0;
static const int gPM_table1 = 0;

void Globals::playMusicAndFadeOutCurrent(int mode) {
    int snd;
    int track;
    int vol = 0;

    if (mode == 2) {
        int *sndP = *(int **) gPM_snd2;
        ((FModSound *) (*sndP))->stop(0);
        snd = *sndP;
        track = 0x91;
        ((FModSound *) (snd))->play(track, 0, 0, (float) vol);
        return;
    }
    if (mode == 1) {
        int *statSnd = *(int **) gPM_sndStatus;
        if (Status::gStatus->inAlienOrbit() != 0) {
            int *sndP = *(int **) gPM_snd1;
            ((FModSound *) (*sndP))->stop(0);
            snd = *sndP;
            track = 0x88;
            int m = Status::gStatus->getCurrentCampaignMission();
            if (m > 0x92 && Status::gStatus->getCurrentCampaignMission() < 0x9a) {
                track = 0x91;
            }
            ((FModSound *) (snd))->play(track, 0, 0, (float) vol);
            return;
        }
        ((SolarSystem *) (long) Status::gStatus->getSystem())->getRace();
        int *sndP = *(int **) gPM_snd1;
        ((FModSound *) (*sndP))->stop(0);
        if (Station_getIndex((int) (long) Status::gStatus->getStation()) == 0x6c) {
            ((FModSound *) (*sndP))->play(0x92, 0, 0, (float) vol);
            return;
        }
        if (Station_getIndex((int) (long) Status::gStatus->getStation()) == 0x65) {
            ((FModSound *) (*sndP))->play(0x93, 0, 0, (float) vol);
            return;
        }
        if (Status::gStatus->inSupernovaSystem() != 0) {
            if (Status::gStatus->getCurrentCampaignMission() == 0x59) {
                ((FModSound *) (*sndP))->play(0x8be, 0, 0, (float) vol);
                return;
            }
            if (Status::gStatus->getMission() != 0 && ((Mission *) (long) Status::gStatus->getMission())->isEmpty() == 0) {
                int tgt = ((Mission *) (long) Status::gStatus->getMission())->getTargetStation();
                if (tgt == Station_getIndex((int) (long) Status::gStatus->getStation())) {
                    int cm = Status::gStatus->getCurrentCampaignMission();
                    track = cm < 0x6a ? 0x8c1 : 0x8c2;
                    ((FModSound *) (*sndP))->play(track, 0, 0, (float) vol);
                    return;
                }
            }
            ((FModSound *) (*sndP))->play(0x94, 0, 0, (float) vol);
            return;
        }
        if (Status::gStatus->inDeepScienceOrbit() != 0) {
            ((FModSound *) (*sndP))->play(0x98, 0, 0, (float) vol);
            return;
        }
        if (Station_getIndex((int) (long) Status::gStatus->getStation()) == 0x78 &&
            (Status::gStatus->getCurrentCampaignMission() == 0x7e ||
             Status::gStatus->getCurrentCampaignMission() == 0x85)) {
            ((FModSound *) (*sndP))->play(0x8bf, 0, 0, (float) vol);
            return;
        }
        const int *table = &gPM_table1;
        track = table[((SolarSystem *) (long) Status::gStatus->getSystem())->getRace()];
        ((FModSound *) (*sndP))->play(track, 0, 0, (float) vol);
        return;
    }
    if (mode != 0) {
        return;
    }

    int race = ((SolarSystem *) (long) Status::gStatus->getSystem())->getRace();
    int *sndP = *(int **) gPM_snd0;
    ((FModSound *) (*sndP))->stop(0);
    if (Station_getIndex((int) (long) Status::gStatus->getStation()) == 0x6c) {
        ((FModSound *) (*sndP))->play(0x84, 0, 0, (float) vol);
        return;
    }
    if (Station_getIndex((int) (long) Status::gStatus->getStation()) == 0x65) {
        ((FModSound *) (*sndP))->play(0x83, 0, 0, (float) vol);
        return;
    }
    int idx = Station_getIndex((int) (long) Status::gStatus->getStation());
    if (idx == 10 || Station_getIndex((int) (long) Status::gStatus->getStation()) == 100) {
        if (Station_getIndex((int) (long) Status::gStatus->getStation()) == 10 &&
            Status::gStatus->getCurrentCampaignMission() == 0x9f) {
            ((FModSound *) (*sndP))->play(0x90, 0, 0, (float) vol);
            return;
        }
        ((FModSound *) (*sndP))->play(0x85, 0, 0, (float) vol);
        return;
    }
    const int *table = &gPM_table0;
    track = table[race];
    ((FModSound *) (*sndP))->play(track, 0, 0, (float) vol);
}

static const int gGDS_pairTable[1] = {};

int Globals::getDialogueSoundId(int code, Agent *agent) {
    const int *t = gGDS_pairTable;
    for (unsigned i = 0; (i >> 6) < 0x2f; i += 2) {
        if (t[i] == code) {
            return t[i + 1];
        }
    }

    if (agent == 0) {
        return -1;
    }

    int race = ((Agent *) (agent))->getRace();
    int male = ((Agent *) (agent))->isMale();

    int category;
    if (race == 3) {
        int *parts = ((Agent *) (agent))->getImageParts();
        if (parts != 0) {
            int *p = ((Agent *) (agent))->getImageParts();
            category = (*p == 2) ? 3 : 0;
        } else {
            category = 2;
        }
    } else {
        category = race;
    }

    int isMale = male;
    switch (category) {
        case 0:
        case 5: {
            if (isMale == 0) {
                if ((unsigned) (code - 0x172) > 0x13) {
                    return -1;
                }
                static const int femaleTable[20] = {
                    0x2a7, 0x29b, 0x2a1, 0x2a2, 0x2a3, 0x2a4, 0x2a5, 0x2a6, 0x295, 0x29c,
                    0x29d, 0x29e, 0x29f, 0x2a0, 0x296, 0x297, 0x298, 0x299, 0x29a, 0x294,
                };
                return femaleTable[code - 0x172];
            }

            switch (code) {
                case 0x172: return 0x2bb;
                case 0x173: return 0x2af;
                case 0x174: return 0x2b5;
                case 0x175: return 0x2b6;
                case 0x176: return 0x2b7;
                case 0x177: return 0x2b8;
                case 0x178: return 0x2b9;
                case 0x179: return 0x2ba;
                case 0x17a: return 0x2a9;
                case 0x17b: return 0x2b0;
                case 0x17c: return 0x2b1;
                case 0x17d: return 0x2b2;
                case 0x17e: return 0x2b3;
                case 0x17f: return 0x2b4;
                case 0x180: return 0x2aa;
                case 0x181: return 0x2ab;
                case 0x182: return 0x2ac;
                case 0x183: return 0x2ad;
                case 0x184: return 0x2ae;
                case 0x185: return 0x2a8;
                case 0x1aa: return 0x2c1;
                case 0x1ab: return 0x2c2;
                case 0x1ac: return 0x2c3;
                case 0x1ad: return 0x2c6;
                case 0x1ae: return 0x2c7;
                case 0x1af: return 0x2c8;
                case 0x1bd: return 0x2bc;
                case 0x1be: return 0x2bd;
                case 0x1bf: return 0x2be;
                case 0x139: return 0x2cb;
                default: return -1;
            }
        }
        case 1: {
            switch (code) {
                case 0x172: return 0x2df;
                case 0x173: return 0x2d3;
                case 0x174: return 0x2d9;
                case 0x175: return 0x2da;
                case 0x176: return 0x2db;
                case 0x177: return 0x2dc;
                case 0x178: return 0x2dd;
                case 0x179: return 0x2de;
                case 0x17a: return 0x2cd;
                case 0x17b: return 0x2d4;
                case 0x17c: return 0x2d5;
                case 0x17d: return 0x2d6;
                case 0x17e: return 0x2d7;
                case 0x17f: return 0x2d8;
                case 0x180: return 0x2ce;
                case 0x181: return 0x2cf;
                case 0x182: return 0x2d0;
                case 0x183: return 0x2d1;
                case 0x184: return 0x2d2;
                case 0x185: return 0x2cc;
                case 0x1aa: return 0x2e5;
                case 0x1ab: return 0x2e6;
                case 0x1ac: return 0x2e7;
                case 0x1ad: return 0x2ea;
                case 0x1ae: return 0x2eb;
                case 0x1af: return 0x2ec;
                case 0x1bd: return 0x2e0;
                case 0x1be: return 0x2e1;
                case 0x1bf: return 0x2e2;
                default: return -1;
            }
        }
        case 2:
        case 3:
            switch (code) {
                case 0x172: return 0x27e;
                case 0x173: return 0x272;
                case 0x174: return 0x278;
                case 0x175: return 0x279;
                case 0x176: return 0x27a;
                case 0x177: return 0x27b;
                case 0x178: return 0x27c;
                case 0x179: return 0x27d;
                case 0x17a: return 0x26c;
                case 0x17b: return 0x273;
                case 0x17c: return 0x274;
                case 0x17d: return 0x275;
                case 0x17e: return 0x276;
                case 0x17f: return 0x277;
                case 0x180: return 0x26d;
                case 0x181: return 0x26e;
                case 0x182: return 0x26f;
                case 0x183: return 0x270;
                case 0x184: return 0x271;
                case 0x185: return 0x26b;
                case 0x1aa: return 0x285;
                case 0x1ab: return 0x286;
                case 0x1ac: return 0x287;
                case 0x1ad: return 0x28a;
                case 0x1ae: return 0x28b;
                case 0x1af: return 0x28c;
                case 0x1b0: return 0x24c;
                case 0x1b2: return 0x24a;
                case 0x1ba: return 0x24d;
                case 0x1bd: return 0x27f;
                case 0x1be: return 0x281;
                case 0x1bf: return 0x282;
                case 0x139: return 0x28f;
                default: return -1;
            }
        case 4: {
            switch (code) {
                case 0x172: return 0x267;
                case 0x173: return 0x25b;
                case 0x174: return 0x261;
                case 0x175: return 0x262;
                case 0x176: return 0x263;
                case 0x177: return 0x264;
                case 0x178: return 0x265;
                case 0x179: return 0x266;
                case 0x17a: return 0x255;
                case 0x17b: return 0x25c;
                case 0x17c: return 0x25d;
                case 0x17d: return 0x25e;
                case 0x17e: return 0x25f;
                case 0x17f: return 0x260;
                case 0x180: return 0x256;
                case 0x181: return 599;
                case 0x182: return 600;
                case 0x183: return 0x259;
                case 0x184: return 0x25a;
                case 0x185: return 0x26a;
                case 0x139: return 0x268;
                default: return -1;
            }
        }
        case 6: {
            switch (code) {
                case 0x172: return 0x230;
                case 0x173: return 0x2f6;
                case 0x174: return 0x22a;
                case 0x175: return 0x22b;
                case 0x176: return 0x22c;
                case 0x177: return 0x22d;
                case 0x178: return 0x22e;
                case 0x179: return 0x22f;
                case 0x17a: return 0x2f0;
                case 0x17b: return 0x225;
                case 0x17c: return 0x226;
                case 0x17d: return 0x227;
                case 0x17e: return 0x228;
                case 0x17f: return 0x229;
                case 0x180: return 0x2f1;
                case 0x181: return 0x2f2;
                case 0x182: return 0x2f3;
                case 0x183: return 0x2f4;
                case 0x184: return 0x2f5;
                case 0x185: return 0x2ef;
                case 0x139: return 0x231;
                default: return -1;
            }
        }
        case 7: {
            switch (code) {
                case 0x172: return 0x246;
                case 0x173: return 0x23a;
                case 0x174: return 0x240;
                case 0x175: return 0x241;
                case 0x176: return 0x242;
                case 0x177: return 0x243;
                case 0x178: return 0x244;
                case 0x179: return 0x245;
                case 0x17a: return 0x234;
                case 0x17b: return 0x23b;
                case 0x17c: return 0x23c;
                case 0x17d: return 0x23d;
                case 0x17e: return 0x23e;
                case 0x17f: return 0x23f;
                case 0x180: return 0x235;
                case 0x181: return 0x236;
                case 0x182: return 0x237;
                case 0x183: return 0x238;
                case 0x184: return 0x239;
                case 0x185: return 0x233;
                case 0x139: return 0x247;
                default: return -1;
            }
        }
        case 8:
            if ((unsigned) (code - 0x1b3) <= 5) {
                return code + 0x9b;
            }
            return -1;
        default:
            return -1;
    }
}

static void *const gPlanetRng = nullptr;

String Globals::getRandomPlanetName() {
    FileRead *f = (FileRead *) ::operator new(1);
    FileRead_ctor(f);
    int which = nextInt_71ad0((AbyssEngine::AERandom *) *(int *) gPlanetRng, 0x64);
    Station *st = f->loadStation(which);
    String name = st->getName();
    delete st;
    ::operator delete(FileRead_dtor(f));
    return name;
}

static void *const gGRN_guardHolder = nullptr;
static const char gGRN_noFirst[] = "";
static void *const gGRN_rng1 = nullptr;
static const char gGRN_noLast[] = "";
static void *const gGRN_rng2 = nullptr;
static const char gGRN_space[] = "";

String Globals::getRandomName(int kind, bool both) {
    int *guardP = *(int **) gGRN_guardHolder;
    volatile int saved = *guardP;

    void *fr = ::operator new(1);
    FileRead_ctor(fr);
    Array<String *> *first = ((FileRead *) fr)->loadNamesBinary(kind, both, 1);
    Array<String *> *last = ((FileRead *) fr)->loadNamesBinary(kind, both, 0);

    String firstStr, lastStr;
    if (first == 0) {
        firstStr.ctor_char(gGRN_noFirst, false);
    } else {
        int idx = nextInt_71aa4((AbyssEngine::AERandom *) **(int **) gGRN_rng1);
        firstStr.Set(((*first)[idx])->data);
    }
    if (last == 0) {
        lastStr.ctor_char(gGRN_noLast, false);
    } else {
        int idx = nextInt_71aa4((AbyssEngine::AERandom *) **(int **) gGRN_rng2);
        lastStr.Set(((*last)[idx])->data);
    }

    if (first != 0) {
        ArrayReleaseClasses(*first);
        delete first;
    }
    if (last != 0) {
        ArrayReleaseClasses(*last);
        delete last;
    }
    ::operator delete(FileRead_dtor(fr));

    String result;
    if (firstStr.size() == 0) {
        result.Set((firstStr).data);
    } else {
        String space, mid;
        space.ctor_char(gGRN_space, false);
        mid = firstStr + space;
        result = mid + lastStr;
    }

    return result;
}

static void *const gGL_canvas = nullptr;
static const char gGL_empty[] = "";

void Globals::getLine(unsigned font, String text, int maxWidth, String *out) {
    int lang = GameText::getLanguage();
    int width = 5;
    if (((unsigned) (lang | 1)) == 0xb) width = 0xf;
    if ((unsigned) lang == 0xf) width = 0xf;

    int *canvas = *(int **) gGL_canvas;
    unsigned lastSpace = 0;
    unsigned len = text.size();

    String tmp;
    for (unsigned i = 0; i < len;) {
        short ch = *text.index(i);
        width += ((PaintCanvas *) (long) *canvas)->GetTextWidth(font, text, i, i + 1);
        if (ch == 0x20) {
            lastSpace = i;
        }
        if (maxWidth <= width) {
            if (ch == 0x0a || ch == 0x0d) {
                tmp = text.SubString(0, i + 1);
            } else if ((int) lastSpace < 1) {
                tmp = text.SubString(0, i + 1);
            } else {
                tmp = text.SubString(0, lastSpace + 1);
            }
            *out = tmp;
            return;
        }
        i++;
        len = text.size();
    }

    if ((int) len < 2) {
        tmp.ctor_char(gGL_empty, false);
    } else {
        tmp = text.SubString(0, len);
    }
    *out = tmp;
}

#include "engine/core/AERandom.h"
