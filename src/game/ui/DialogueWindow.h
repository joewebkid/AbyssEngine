#ifndef GOF2_DIALOGUEWINDOW_H
#define GOF2_DIALOGUEWINDOW_H
#include "ChoiceWindow.h"
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "ScrollTouchWindow.h"
#include "TouchButton.h"
#include "engine/render/ImagePart.h"
#include "game/mission/Mission.h"
#include "game/world/Level.h"


#include "game/ui/DialogueWindowGermanTextTable.h"
class Agent;
class ChoiceWindow;
class ImagePart;
class Level;
class Mission;
class ScrollTouchWindow;
class TouchButton;



class DialogueWindow {
public:
    TouchButton *prevButton;
    TouchButton *nextButton;
    TouchButton *moreButton;
    Array<ImagePart *> *faceParts;
    int campaignMission;
    int frameX;
    int frameY;
    int frameWidth;
    int frameHeight;
    void *clientImage;
    String bodyText;
    String agentName;
    ScrollTouchWindow *scrollWindow;
    int kind;
    int page;
    Mission *mission;
    ChoiceWindow *choiceWindow;
    uint8_t choiceActive;
    Level *level;
    int *briefingOffsets;
    int *successOffsets;
    int voiceSound;
    int autoAdvanceTimer;
    int pauseLength;
    uint8_t mirrorFace;

    DialogueWindow();

    DialogueWindow(Mission *mission, Level *level, int kind);

    DialogueWindow(String *text, String *agentName, int *parts);

    ~DialogueWindow();

    int OnTouchBegin(int x, int y);

    int OnTouchEnd(int x, int y);

    int OnTouchMove(int x, int y);

    void draw();

    int getMode();

    bool hasLevel();

    int init();

    bool isFirstPage();

    bool isLastPage();

    int length();

    void loadContent();

    int nextPage();

    int pickGermanGenericTextBecauseWeSaved100EurosWithThat(int kind, Agent *agent);

    int previousPage();

    void set(Mission *mission, int kind, int campaign);

    void setLevel(Level *level);

    void update(int dt);

    static bool hasBriefingDialogue(int id);

    static bool hasSuccessDialogue(int id);
};
#endif
