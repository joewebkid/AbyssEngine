#ifndef GOF2_SPACELOUNGE_H
#define GOF2_SPACELOUNGE_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/core/AbyssEngine.h"
#include "engine/math/EaseInOut.h"
#include "engine/math/EaseInOutMatrix.h"
#include "engine/math/Vector.h"
#include "engine/render/ImagePart.h"
#include "game/core/CutScene.h"
#include "game/ship/Agent.h"
#include "game/ui/ChoiceWindow.h"
#include "game/ui/ListItemWindow.h"
#include "game/ui/ScrollTouchWindow.h"
#include "game/ui/TouchButton.h"
#include "game/world/StarMap.h"

#include "engine/math/AEMath.h"
#include "engine/math/Matrix.h"


class Agent;
class ChoiceWindow;
class CutScene;
class ImagePart;
class ListItemWindow;
class ScrollTouchWindow;
class StarMap;
class TouchButton;
namespace AbyssEngine { 
    class EaseInOut;
    class EaseInOutMatrix;
 }


class SpaceLounge {
public:
    StarMap *starMap;
    ChoiceWindow *choiceWindow;
    ListItemWindow *listWindow;
    int mode;
    int field_0x18;
    uint8_t chatActive;
    uint8_t popupActive;
    uint8_t choiceVisible;
    uint8_t listVisible;
    int selectedAgent;
    Array<Agent *> *agents;
    Array<String *> *agentTexts;
    int chatScroll;
    int chatAnswer;
    uint8_t mapVisible;
    uint8_t hangarUpdate;
    uint8_t singleOffer;
    Array<Array<ImagePart *> *> *silhouetteGrid;
    Array<ImagePart *> *agentImageParts;
    Array<AbyssEngine::AEMath::Vector *> *agentRects;
    CutScene *cutScene;
    AbyssEngine::EaseInOutMatrix *cameraEase;
    AbyssEngine::Vector silhouettePos;
    char *agentVisited;
    Array<TouchButton *> *buttons;
    ScrollTouchWindow *scrollWindow;
    int buttonsHeight;
    int visibleButtonCount;
    int panelWidth;
    int panelX;
    int panelY;
    int buttonPanelY;
    int buttonY1;
    int buttonY0;
    int buttonX;
    int hoverAgent;
    int field_0x8c;
    int field_0x90;
    int field_0x94;
    int field_0x98;
    int field_0x9c;
    int field_0xa0;
    String title;
    uint8_t initialized;
    uint8_t touchDown;
    int touchX;
    int touchY;
    uint8_t cameraAnimating;
    uint8_t introDone;
    AbyssEngine::EaseInOut *headEase;
    uint8_t headBobReverse;
    AbyssEngine::Matrix baseMatrix;
    float headBobPhase;
    int headBobSteps;

    SpaceLounge();

    virtual ~SpaceLounge();

    void OnRender3D();

    void OnRenderBG();

    int OnTouchBegin(int x, int y);

    void OnTouchEnd(int x, int y);

    int OnTouchMove(int x, int y);

    bool checkLocationMode();

    void draw();

    void draw3DShip();

    void drawLounge();

    int getSoundId(Agent *agent);

    int getSpecificSoundForRace(int soundId, int race, bool alternate);

    unsigned char hangarNeedsUpdate();

    int init();

    unsigned char introFinished();

    bool listMode();

    unsigned char mapMode();

    void onKeyPress(int key);

    void refresh();

    void setHangarUpdate(bool value);

    void startChat();

    void update(int dt);

    void updateScreenPositions();
};
#endif
