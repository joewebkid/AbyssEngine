#ifndef GOF2_STATUSWINDOW_H
#define GOF2_STATUSWINDOW_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "TouchButton.h"
#include "engine/render/ImagePart.h"

class ImagePart;
class TouchButton;


typedef AbyssEngine::String String;

class StatusWindow {
public:
    int medalCount;
    Array<TouchButton *> *tabButtons;
    Array<TouchButton *> *medalButtons;
    Array<ImagePart *> *imageParts;
    Array<String *> *detailLines;
    unsigned rankImage0;
    unsigned rankImage1;
    unsigned rankImage2;
    unsigned rankImage3;
    unsigned standingEmblemImage;
    unsigned standingBarImage;
    unsigned standingFrameImage;
    unsigned activeTab;
    int selectedMedal;
    int scrollOffset;
    int lastTouchY;
    int scrollTarget;
    int scrollVelocity;
    float scrollDamping;
    float scrollVelocityF;
    int touchStartY;
    unsigned char isDragging;
    int contentHeight;
    int viewportHeight;
    int charImageWidth;
    int charImageHeight;
    int *tabHeights;
    int boxWidth;
    int standingBarWidth;
    int standingBarHeight;
    int medalRowHeight;

    StatusWindow();

    ~StatusWindow();

    int OnTouchBegin(int x, int y);

    void OnTouchEnd(int x, int y);

    int OnTouchMove(int x, int y);

    void draw();

    float getRelativeScrollHeight();

    float getRelativeScrollStartPos();

    String getMedalHintText(int medalIndex);

    void reInit();

    void update(int frameTime);
};
#endif
