#ifndef GOF2_STARMAP_H
#define GOF2_STARMAP_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "SystemPathFinder.h"
#include "engine/core/AbyssEngine.h"
#include "engine/math/EaseInOut.h"
#include "engine/render/AEGeometry.h"
#include "game/ui/ChoiceWindow.h"
#include "game/ui/TouchButton.h"

#include "engine/math/Vector.h"

class SystemPathFinder;

class AEGeometry;
class ChoiceWindow;
class Mission;
class SolarSystem;
class Station;
class TouchButton;
namespace AbyssEngine { 
    class EaseInOut;
 }


class StarMap {
public:
    uint8_t exitRequested;
    uint8_t field_0x01;
    int32_t mode;
    float cameraBaseX;
    float cameraBaseZ;
    int32_t field_0x10;
    int32_t field_0x14;
    int32_t lastDt;
    uint32_t field_0x1c;
    uint32_t keyImageRetreat;
    uint32_t keyImageWanted;
    uint32_t keyImageMission;
    uint32_t keyImageCurrent;
    uint32_t keyImageDiscovered;
    uint32_t systemNameImage;
    uint32_t field_0x38;
    uint32_t field_0x3c;
    uint32_t selectIcon;
    uint32_t cursorIcon;
    uint32_t currentMarkerIcon;
    TouchButton *backButton;
    SystemPathFinder *pathFinder;
    Array<SolarSystem *> *systems;
    Array<Station *> *stations;
    ChoiceWindow *choiceWindow;
    int32_t selectedSystem;
    int32_t selectedStation;
    Array<AEGeometry *> *systemGeoms;
    AEGeometry *systemRoot;
    uint32_t camera;
    uint32_t prevCamera;
    AbyssEngine::Vector scratchVector;
    AbyssEngine::Vector scratchVector2;
    Array<AEGeometry *> *stationGeoms;
    Array<AEGeometry *> *ringGeoms;
    int *stationAngles;
    int *stationDistances;
    Array<int> *systemPath;
    AEGeometry *starSystemRoot;
    uint8_t pad_0xa8_a;
    uint8_t choiceVisible;
    uint8_t jumpMapModeA;
    uint8_t jumpMapModeB;
    AbyssEngine::EaseInOut field_0xac;
    AbyssEngine::EaseInOut field_0xbc;
    AbyssEngine::EaseInOut field_0xcc;
    uint8_t missionChangedFlag;
    int32_t field_0xe0;
    int32_t field_0xe4;
    int32_t field_0xe8;
    float field_0xec;
    int32_t field_0xf0;
    uint8_t isGalaxyMode;
    AEGeometry *markerGeom;
    int *iconBuffer;
    // Android uses Array<bool>; byte storage has the same 0/1 layout here.
    Array<uint8_t> *usedFlags;
    int32_t targetSystem;
    uint8_t showKey;
    int32_t keyBoxWidth;
    int32_t keyBoxHeight;
    int32_t pulseSystem;
    uint8_t autoMode;
    int32_t autoTimer;
    uint8_t alienJumpPending;
    uint32_t raceImageNeutral;
    uint32_t raceImageA;
    uint32_t raceImageB;
    uint32_t raceImageDefault;
    uint32_t image_0x134;
    uint8_t transitionIn;
    uint8_t transitionOut;
    uint8_t pathAnim;
    uint8_t stationCenterAnim;
    int32_t panX;
    int32_t panY;
    float touchStartX;
    float touchStartY;
    float field_0x14c;
    float touchDeltaX;
    float touchDeltaY;
    float field_0x158;
    float lastTouchX;
    float lastTouchY;
    int32_t field_0x164;
    float momentumFactor;
    float velocityX;
    float velocityY;
    uint8_t dragging;
    uint32_t planetTexture;
    AbyssEngine::EaseInOut *easeX;
    AbyssEngine::EaseInOut *easeY;
    AbyssEngine::EaseInOut *easeZ;
    float yaw;
    float pitch;
    int32_t field_0x190;
    Array<AbyssEngine::Vector *> *systemPositions;
    Array<AbyssEngine::Vector *> *stationPositions;
    int32_t lastSelectedSystem;
    int32_t lastSelectedStation;
    int32_t alpha;
    int32_t iconWidth;
    int32_t missionIconWidth;
    AEGeometry *bgLayer0;
    AEGeometry *bgLayer1;
    AEGeometry *bgLayer2;
    AEGeometry *planetGeom;
    float spin;
    int32_t centeredStation;
    int32_t hitRadius;
    float dragScale;
    int32_t jumpCost;
    uint8_t noRoute;
    int32_t cargoAmount;
    int32_t routeStart;
    int32_t routeTarget;
    uint8_t suppressNextClose;

    StarMap(bool jumpMapMode, Mission *mission, bool param3, int param4);

    ~StarMap();

    int init(bool jumpMapMode, Mission *mission, bool param3, int param4);

    void initLights();

    void initStarSystem();

    void render();

    void renderBG();

    void draw();

    void drawKey();

    void drawOnScreenInfo(int index, bool stationMode);

    void update(int dt);

    uint32_t OnTouchBegin(int x, int y);

    void OnTouchMove(int x, int y);

    int OnTouchEnd(int x, int y);

    void depart(bool jump);

    void setStart(int start, int target);

    void setJumpMapMode(bool enabled, bool value);

    void askForJumpIntoAlienWorld();

    bool isInPlanetMode();

    uint8_t missionChanged();
};
#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(StarMap, systems) == 0x54, "StarMap systems offset");
static_assert(__builtin_offsetof(StarMap, scratchVector) == 0x78, "StarMap scratch offset");
static_assert(__builtin_offsetof(StarMap, stationGeoms) == 0x90, "StarMap station geometry offset");
static_assert(__builtin_offsetof(StarMap, alpha) == 0x1a4, "StarMap alpha offset");
static_assert(__builtin_offsetof(StarMap, jumpCost) == 0x1d0, "StarMap jump cost offset");
static_assert(__builtin_offsetof(StarMap, field_0xbc) == 0xbc, "StarMap inline easing offset");
static_assert(__builtin_offsetof(StarMap, missionChangedFlag) == 0xdc, "StarMap mission flag offset");
static_assert(__builtin_offsetof(StarMap, easeZ) == 0x184, "StarMap camera easing offset");
static_assert(__builtin_offsetof(StarMap, suppressNextClose) == 0x1e4, "StarMap final flag offset");
static_assert(sizeof(StarMap) == 0x1e8, "StarMap native allocation size");
#endif
#endif
