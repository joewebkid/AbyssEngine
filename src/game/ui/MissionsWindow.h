#ifndef GOF2_MISSIONSWINDOW_H
#define GOF2_MISSIONSWINDOW_H
#include "ChoiceWindow.h"
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "ScrollTouchWindow.h"
#include "WantedWindow.h"
#include "engine/render/ImagePart.h"
#include "game/world/StarMap.h"

class ChoiceWindow;
class ImagePart;
class ScrollTouchWindow;
class StarMap;
class TouchButton;
class WantedWindow;


class MissionsWindow {
public:
    ScrollTouchWindow *m_pCampaignWindow;
    ScrollTouchWindow *m_pFreelanceWindow;
    StarMap *m_pStarMap;
    ChoiceWindow *m_pChoiceWindow;
    WantedWindow *m_pWantedWindow;
    Array<TouchButton *> *m_pTabButtons;
    Array<ImagePart *> *m_pAgentImageParts;
    int m_textHalfHeight;
    uint8_t m_choiceActive;
    uint8_t m_field_0x21;
    uint8_t m_starMapActive;
    uint8_t m_hangarNeedsUpdate;
    TouchButton *m_pAcceptButton;
    TouchButton *m_pRejectButton;
    TouchButton *m_pMapButton;
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    int m_mode;

    MissionsWindow();

    ~MissionsWindow();

    int OnTouchMove(int x, int y);

    int OnTouchBegin(int x, int y);

    void OnTouchEnd(int x, int y);

    void setHangarUpdate(bool v);

    uint8_t hangarNeedsUpdate();

    void render3D();

    int init();

    void draw();

    void update(int dt);
};
#endif
