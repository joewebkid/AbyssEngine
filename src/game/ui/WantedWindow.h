#ifndef GOF2_WANTEDWINDOW_H
#define GOF2_WANTEDWINDOW_H

#include <cstdint>
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "ScrollTouchWindow.h"
#include "TouchButton.h"
#include "engine/render/ImagePart.h"
#include "game/mission/Mission.h"
#include "game/world/StarMap.h"
#include "game/world/Wanted.h"

class ImagePart;
class Mission;
class ScrollTouchWindow;
class StarMap;
class TouchButton;
class Wanted;


class WantedWindow {
public:
    uint32_t lastButtonHit;
    StarMap *starMap;
    Array<ImagePart *> *imageParts;
    Array<TouchButton *> *buttons;
    int halfTextHeight;
    uint8_t showingMap;
    uint8_t hangarUpdate;
    TouchButton *detailButton;
    int windowX;
    int windowY;
    int windowWidth;
    int windowHeight;
    ScrollTouchWindow *scrollWindow;
    uint32_t selectedWanted;
    uint32_t highlightedWanted;
    Array<Wanted *> *wantedList;
    String fromText;
    String toText;
    String nameText;
    String detailText;
    String atText;
    String rewardText;
    int scrollOffset;
    int lastDragY;
    int scrollOffsetSnapshot;
    int dragDelta;
    float scrollDamping;
    float scrollVelocity;
    int touchStartY;
    uint8_t dragging;
    int contentHeight;
    int visibleHeight;
    int bgImage;
    Mission *mission;

    WantedWindow();

    ~WantedWindow();

    void setHangarUpdate(bool needsUpdate);

    bool hangarNeedsUpdate();

    int OnTouchBegin(int x, int y);

    void OnTouchEnd(int x, int y);

    int OnTouchMove(int x, int y);

    void draw();

    float getRelativeScrollHeight();

    float getRelativeScrollStartPos();

    uint32_t getWantedAtPosition(int x, int y);

    int init();

    void render3D();

    void selectWanted(int idx);

    void update(int dt);
};
#endif
