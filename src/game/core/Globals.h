#ifndef GOF2_GLOBALS_H
#define GOF2_GLOBALS_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/core/ApplicationManager.h"
#include "engine/math/BoundingVolume.h"
#include "engine/render/AEGeometry.h"
#include "game/ship/Agent.h"
#include "game/ui/Layout.h"
#include "game/world/Station.h"

class AEGeometry;
class Agent;
class BoundingVolume;
class Station;
class Status;
class FModSound;
namespace AbyssEngine { 
    class ApplicationManager;
    class Engine;
 }



class Globals {
public:
    Array<int> *soundResources;
    int field_0x34;
    void *field_0x3c;
    void *field_0x40;
    void *field_0x48;
    unsigned int *field_0x54;
    int field_0xac;
    unsigned short field_0x110;
    int field_0x114;
    int field_0x14c;

    Globals();

    ~Globals();

    unsigned getRandomEnemyFighter(int kind);

    void resetHints();

    String getItemName(int item);

    AEGeometry *getShipGroup(int type, int variant, bool wireframe);

    int init(AbyssEngine::ApplicationManager *app, AbyssEngine::Engine *engine);

    float sqrt(float x);

    void startNewSoundResourceList();

    void playMusicAndFadeOutCurrent(int mode);

    void addSoundResourceToList(int snd);

    void loadFont(int kind);

    void getLineArray(unsigned int font, const String &text, int maxWidth, Array<String *> *out);

    void getLine(unsigned int font, String text, int maxWidth, String *out);

    void releaseResources();

    void reportLeaderboards();

    String getKeyActionName(int action);

    void getRandomSystemForDrinks();

    String replaceKeyBindingTokens(const String &src);

    Station *getRandomStation();

    void longToTimeString(long long ms, String &out);

    void longToTimeStringNoSeconds(long long ms, String &out);

    String getBoundedString(const String &text, int width);

    String getAgentMissionText(Agent *agent);

    void setCoordsSteer(int p1, int p2, int p3, int p4,
                        unsigned short &o5, unsigned short &o6, unsigned short &o7,
                        unsigned short &o8, unsigned short &o9, unsigned short &o10,
                        unsigned short &o11, unsigned short &o12, unsigned short &o13,
                        unsigned short &o14);

    void setCoordsFire(int p1, int p2, unsigned int p3, unsigned int p4,
                       unsigned int &o5, unsigned short &o6, unsigned short &o7,
                       unsigned short &o8, unsigned short &o9, unsigned short &o10,
                       unsigned short &o11, unsigned short &o12, unsigned short &o13,
                       unsigned short &o14, unsigned short &o15, unsigned short &o16,
                       unsigned short &o17);

    unsigned int createBillBoard(int p1, int height, float u0, float v0, float u1, float v1, int width);

    Array<BoundingVolume *> *getWreckCollision(int kind, AEGeometry *geom);

    int getDialogueSoundId(int code, Agent *agent);

    String getRandomPlanetName();

    String getRandomName(int kind, bool both);

    Array<int> *getSoundResourceList();

    int getInAppPurchaseArrayIndex(int productCode, Array<String *> *list);

    String getKeyBindingReplaceString(int key);

    void reportSupernovaChallengeScore();

    void drawLines(unsigned int font, Array<String *> *lines, int baseX, int startY);

    void drawLines(unsigned int font, Array<String *> *lines, int baseX, int startY, bool centered);

    void drawLines(unsigned int font, Array<String *> *lines, int baseX, int startY,
                   unsigned int rightX, bool centered);

    static int is_dialogue_window_visible;
    static int is_choice_window_visible;
    static int is_menu_visible;
    static int is_hacking_visible;
    static unsigned char isStarMapVisible;
    static unsigned char isCinematicModeActive;
    static int mouseCursorActivated;
    static unsigned char showMouseDuringGameOver;
    static unsigned char keyBindings[8];

    static int left_edge;
    static int right_edge;
    static int top_edge;
    static int bottom_edge;
    static int resetKeyboard;
    static int rotateShipInStation;
    static int translateStarMapInXDirection;
    static int translateStarMapInYDirection;
    static int smallButton_dim;
    static int touch_stick_x;
    static int touch_stick_y;

    static unsigned char iPad;
    static unsigned char iPadHD;
    static unsigned char retinaDisplay;
    static unsigned char n9;
    static unsigned char iPadLarge;
    static unsigned char iPadLargePossible;
    static unsigned char iPadAssetsWithLowerRes;
    static unsigned char enterSpaceLounge;
    static int switch_to_target_setting;

    static Status *status;
    static unsigned char options[100];
    static FModSound *sound;
    static int logoIsShown;
    static int isInMainMenu;

    static char *cItemListID_00;
    static char *cItemListID_01;
    static char *cItemListID_02;
    static char *cItemListID_03;
    static char *cItemListID_04;
    static char *cItemListID_05;
    static char *cItemListID_06;
    static char *cItemListID_07;
    static char *cItemListID_08;
    static char *cItemListID_09;
    static char *cItemListID_10;
    static char *cItemListID_11;
    static char *cItemListID_12;
    static char *cItemListID_13;
    static char *cItemListID_14;
    static char *cItemListID_15;
    static char *cItemListID_16;
    static char *cItemListID_17;
    static char *cItemListID_18;
    static char *cItemListID_19;
    static char *cItemListID_20;
    static char *cItemListID_21;
    static char *cItemListID_22;
    static char *cItemListID_23;
    static char *cItemListID_24;
    static char *cItemListName_00;
    static char *cItemListName_01;
    static char *cItemListName_02;
    static char *cItemListName_03;
    static char *cItemListName_04;
    static char *cItemListDescription_00;
    static char *cItemListDescription_01;
    static char *cItemListDescription_02;
    static char *cItemListDescription_03;
    static char *cItemListDescription_04;
    static char *cItemListCurrency_00;
    static char *cItemListCurrency_01;
    static char *cItemListCurrency_02;
    static char *cItemListCurrency_03;
    static char *cItemListCurrency_04;
    static char *cItemListPrice_00;
    static char *cItemListPrice_01;
    static char *cItemListPrice_02;
    static char *cItemListPrice_03;
    static char *cItemListPrice_04;

    static Globals *gGlobals;
    static Layout *gLayout;
    static void *gFont;
    static int gScreenWidth;
    static int gScreenHeight;

    // Static data members present in the original binary (defined for symbol parity).
    static void *appManager;
    static unsigned char gameLoaded;
    static unsigned char gameSaving;
    static float sec_fire_x;
    static float sec_fire_y;
    static float sec_fire_z;
    static float autopilot_x;
    static float autopilot_y;
    static float autopilot_z;
    static int mouseDeltaX;
    static int mouseDeltaY;
    static int mouse_wheel;
    static void *recordSlots;
    static void *achievements;
    static void *imageFactory;
    static int mouse_wheelX;
    static int mouse_wheelY;
    static int qualityLevel;
    static void *shipTemplate;
    static unsigned char showBestDeal;
    static int simulateFire;
    static int subMenuIndex;
    static int topMenuIndex;
    static float action_menu_x;
    static float action_menu_y;
    static float action_menu_z;
    static void *recordHandler;
    static float touch_stick_z;
    static float turret_view_x;
    static float turret_view_y;
    static float turret_view_z;
    static float fast_forward_x;
    static float fast_forward_y;
    static float fast_forward_z;
    static int fontLangSelect;
    static int keyboard_swipe;
    static unsigned char iCloudSupported;
    static unsigned char isIOS6Installed;
    static int other_buttons_x[10];
    static int other_buttons_y[10];
    static unsigned char showWingmanMenu;
    static unsigned char first_start_ever;
    static int globalPriceRaise;
    static unsigned char initMemoryWarning;
    static int instantActionType;
    static int instantActionWave;
    static unsigned char isTelekomCustomer;
    static int lastRecordWritten;
    static int instantActionScore;
    static unsigned char showNewCreditsMenu;
    static int sub_menu_buttons_x[10];
    static int sub_menu_buttons_y[10];
    static unsigned char iap_hack_dlc1Bought;
    static unsigned char iap_hack_dlc2Bought;
    static unsigned char iap_hack_dlc3Bought;
    static int instantActionPoints;
    static unsigned char useRefractionShader;
    static int h;
    static int w;
    static int lastSpaceMusicPlayed;
    static int energyCellsProbChange;
    static int sub_menu_button_count;
    static unsigned char inAppPurchaseSupported;
    static int lastStationMusicPlayed;
    static int menu_touch_window_type;
    static unsigned char useLowResTexturesForHD;
    static void *instantActionPlayerName;
    static unsigned char isRunningHDonWeakDevice;
    static float quickmenu_button_start_x;
    static float quickmenu_button_start_y;
    static int lastCampaignMissionFailed;
    static int secondaryWeaponsProbChange;
    static int lastCampaignMissionFailCount;
    static unsigned char startLiteVersionWithMoreCredits;
    static void *rnd;
    static void *font;
    static unsigned char keys[1020];
    static void *bankZ;
    static unsigned char hints[59];
    static void *items;
    static void *ships;
    static void *Canvas;
    static float fire_x;
    static float fire_y;
    static float fire_z;
    static void *galaxy;
    static void *layout;
    static float boost_x;
    static float boost_y;
    static float boost_z;
    static void *globals;
    static float pause_x;
    static float pause_y;
    static float pause_z;
    static void *gameText;
    static void *fontAlien;
    static void *generator;
};
// ApplicationManager::gAppManager is declared in engine/core/ApplicationManager.h (included above)

#endif
