#ifndef GOF2_LISTITEMWINDOW_H
#define GOF2_LISTITEMWINDOW_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "ListItem.h"
#include "ScrollTouchWindow.h"

#include "engine/math/Matrix.h"
#include "engine/render/AEGeometry.h"

#include "engine/math/AEMath.h"


class AEGeometry;
class ListItem;
class ScrollTouchWindow;


class ListItemWindow {
public:
    Array<AbyssEngine::String *> *labels;
    Array<AbyssEngine::String *> *values;
    Array<int> *statsCur;
    Array<int> *statsPrev;

    AEGeometry *previewGeometry;
    ListItem *item;
    ScrollTouchWindow *scrollWindow;
    int textHalfHeight;
    int previewHeight;

    int scrollBarX;
    int scrollBarY;
    int scrollBarOffsetX;
    int scrollBarTrackLength;

    uint32_t param2;
    uint32_t param3;
    uint32_t param4;
    uint32_t param5;

    unsigned int scrollThumbImage;
    unsigned int arrowUpImage;
    unsigned int arrowDownImage;
    unsigned int arrowEqualImage;
    uint8_t shows3DShipFlag;

    int arrowSeparator;
    int field_0x5c;
    int x;
    int y;
    int width;
    int height;

    AbyssEngine::String str74;
    AbyssEngine::String str80;

    unsigned int previewTransformId;
    unsigned int previewSecondaryTransformId;
    unsigned int previewCameraId;
    AbyssEngine::AEMath::Matrix previewCameraLocal;
    AbyssEngine::AEMath::Matrix previewScratch;
    unsigned int previewLightingTexture;
    float previewScaleBias;
    float previewAngle;
    int dragAccum;
    int dragLastX;
    int dragSettled;
    int dragDelta;
    float spinDamping;
    float spinVelocity;
    int dragStartX;
    uint8_t dragging;

    ListItemWindow();

    ~ListItemWindow();

    void OnTouchBegin(int x, int y);

    void OnTouchMove(int x, int y);

    void OnTouchEnd(int x, int y);

    uint8_t shows3DShip();

    void render();

    void set(ListItem *item, unsigned p2, unsigned p3, unsigned p4, unsigned p5, bool p6);

    void draw();

    void update(int frameTime);
};

#endif
