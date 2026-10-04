#include "game/ui/ChoiceWindow.h"
#include "engine/audio/FModSound.h"
#include "game/ui/ScrollTouchWindow.h"
#include "game/mission/Status.h"
#include "game/mission/Achievements.h"
#include "game/core/Globals.h"
#include "engine/core/GameText.h"
#include "game/ui/Layout.h"
#include "game/ui/TouchButton.h"
#include "engine/render/PaintCanvas.h"

int ChoiceWindow::OnTouchMove(int x, int y) {
    if (this->buttonsVisible != 0) {
        if (this->leftButton != nullptr) this->leftButton->OnTouchMove(x, y);
        if (this->rightButton != nullptr) this->rightButton->OnTouchMove(x, y);
        if (this->miscButton != nullptr) this->miscButton->OnTouchMove(x, y);
    }
    if (this->scrollWindow != nullptr) this->scrollWindow->OnTouchMove(x, y);
    return 0;
}

uint8_t ChoiceWindow::hasChoice() {
    return this->hasButtons;
}

void ChoiceWindow::setWidth(int width) {
    this->width = width;
    this->x = Globals::w / 2 - width / 2;
}

void ChoiceWindow::set(String const &a, String const &b) {
    GameText *gameText = Globals::gameText;
    String *title = gameText->getText(0x186);
    String *left = gameText->getText(0x86);
    String *center = gameText->getText(0x87);
    set(*title, a, false, *left, *center, b, -1, -1);
}

void ChoiceWindow::set(String const &text, bool flag) {
    String *title = Globals::gameText->getText(0x186);
    set(*title, text, flag);
}

void ChoiceWindow::set(String const &title, String const &message, bool flag) {
    GameText *gameText = Globals::gameText;
    String *left = gameText->getText(0x86);
    String *center = gameText->getText(0x87);
    String *right = gameText->getText(0x20c);
    set(title, message, flag, *left, *center, *right, -1, -1);
}

int ChoiceWindow::OnTouchEnd(int x, int y) {
    if (this->buttonsVisible != 0) {
        if (this->leftButton != nullptr && this->leftButton->OnTouchEnd(x, y) != 0)
            return 0;
        if (this->rightButton != nullptr && this->rightButton->OnTouchEnd(x, y) != 0)
            return 1;
        if (this->miscButton != nullptr && this->miscButton->OnTouchEnd(x, y) != 0)
            return 2;
    }
    if (this->scrollWindow != nullptr) this->scrollWindow->OnTouchEnd(x, y);
    return -1;
}

void ChoiceWindow::set(String const &text) {
    String *title = Globals::gameText->getText(0x186);
    set(*title, text, false);
}

void ChoiceWindow::update(int dt) {
    this->scrollWindow->update(dt);
}

void ChoiceWindow::removeButtons() {
    this->buttonsVisible = 0;
}

int ChoiceWindow::OnTouchBegin(int x, int y) {
    if (this->buttonsVisible != 0) {
        if (this->leftButton != nullptr) this->leftButton->OnTouchBegin(x, y);
        if (this->rightButton != nullptr) this->rightButton->OnTouchBegin(x, y);
        if (this->miscButton != nullptr) this->miscButton->OnTouchBegin(x, y);
    }
    if (this->scrollWindow != nullptr) this->scrollWindow->OnTouchBegin(x, y);
    return 0;
}

ChoiceWindow::~ChoiceWindow() {
    delete this->leftButton;
    this->leftButton = nullptr;

    delete this->rightButton;
    this->rightButton = nullptr;

    delete this->miscButton;
    this->miscButton = nullptr;

    delete this->scrollWindow;
    this->scrollWindow = nullptr;
}

void ChoiceWindow::setHeight(int height) {
    Layout *layout = Globals::layout;
    int top = layout->field_0x8;
    this->height = top + height;

    TouchButton *button = this->leftButton;
    int yBase = Globals::h / 2 - height / 2;
    this->y = yBase - top;

    if (this->hasButtons == 0) {
        if (button == nullptr) return;
        button->setPosition(this->x + this->width / 2, height + yBase - this->padding, 0x24);
        return;
    }

    if (button != nullptr) {
        button->setPosition(this->x + this->width / 2 - layout->field_0x4c / 2,
                            height + yBase - this->padding, 0x22);
    }

    button = this->rightButton;
    if (button == nullptr) return;

    layout = Globals::layout;
    button->setPosition(this->x + this->width / 2 + layout->field_0x4c / 2,
                        this->y + this->height - this->padding, 0x21);
}

// Android 2.0.16 .rodata: 0x1fb898, 0x1fb888, and 0x202784.
static const uint32_t g_ChoiceWindow_medalImagesLow_146e8c[4] = {
    0x970, 0x96e, 0x96f, 0x96d,
};
static const uint32_t g_ChoiceWindow_medalImagesHigh_146e8c[4] = {
    0x1f6d, 0x1f63, 0x96f, 0x96d,
};
static const uint32_t g_ChoiceWindow_medalImages_146e8c[45] = {
    0x948, 0x949, 0x94a, 0x94b, 0x94c, 0x94d, 0x94e, 0x94f, 0x950,
    0x951, 0x952, 0x953, 0x954, 0x955, 0x956, 0x957, 0x958, 0x959,
    0x95a, 0x95b, 0x95c, 0x95d, 0x95e, 0x95f, 0x960, 0x961, 0x962,
    0x963, 0x964, 0x965, 0x966, 0x967, 0x968, 0x969, 0x96a, 0x96b,
    0x1f64, 0x1f65, 0x1f66, 0x1f67, 0x1f68, 0x1f69, 0x1f6a, 0x1f6b,
    0x1f6c,
};

void ChoiceWindow::setMedal(int medal, int count) {
    GameText *gameText = Globals::gameText;
    this->medalText = *gameText->getText(medal + 0x5e3);

    String finalText = String("\n\n\n\n\n\n") +
        Globals::status->replaceHash(
            String(*gameText->getText(medal + 0x610), false),
            String(String(static_cast<Achievements *>(Globals::achievements)->getValue(medal, count)), false)) +
        String("\n\n");

    set(*gameText->getText(0x161), finalText, false);
    this->scrollWindow->setTextCentered(true);

    if (count >= 0x24) {
        Globals::Canvas->Image2DCreate((unsigned short) g_ChoiceWindow_medalImagesHigh_146e8c[count],
                                       this->medalImage);
    } else {
        Globals::Canvas->Image2DCreate((unsigned short) g_ChoiceWindow_medalImagesLow_146e8c[count],
                                       this->medalImage);
    }
    Globals::Canvas->Image2DCreate((unsigned short) g_ChoiceWindow_medalImages_146e8c[medal],
                                   this->medalBgImage);

    this->count = count;
    this->medal = medal;
    this->buttonsVisible = 1;
}

ChoiceWindow::ChoiceWindow() {
    Layout *config = Globals::layout;
    int width = config->field_0x264;
    this->width = width;
    this->padding = config->field_0x268;
    this->padding2 = config->field_0x26c;
    this->padding3 = config->field_0x270;
    this->padding4 = config->field_0x274;

    int screenWidth = Globals::w;
    this->leftButton = nullptr;
    this->rightButton = nullptr;
    this->miscButton = nullptr;
    this->scrollWindow = nullptr;
    this->medalImage = -1;
    this->medalBgImage = -1;
    this->buttonsVisible = 1;
    this->x = screenWidth / 2 - width / 2;
}

void ChoiceWindow::set(String const &title, String const &message, bool hasButtons,
                       String const &left, String const &right, String const &single,
                       int y, int width) {
    this->hasButtons = hasButtons;

    GameText *gameText = Globals::gameText;
    bool defaultButtons = false;
    if (gameText->getText(0x86)->Compare_str((String *) &left) == 0)
        defaultButtons = gameText->getText(0x87)->Compare_str((String *) &right) == 0;

    if (width == -1)
        width = Globals::layout->field_0x264;
    if (!defaultButtons)
        width = (int) ((float) width * 0.85f);

    this->width = width;
    this->x = Globals::w / 2 - width / 2;
    Globals::sound->play(0x7e, nullptr, nullptr, 0.0f);
    this->title = title;

    delete this->scrollWindow;
    this->scrollWindow = nullptr;

    Array<String *> *lines = new Array<String *>();

    Layout *layout = Globals::layout;
    static_cast<Globals *>(Globals::globals)->getLineArray(
        Globals::font,
        message,
        (this->width - layout->field_0x4c * 2) - layout->field_0x48,
        lines);

    int contentHeight = layout->field_0x4 * (int) lines->size() + layout->field_0x8 +
                        layout->field_0x4c * 2 + this->padding * 2 +
                        layout->field_0x30;
    int maxHeight = layout->field_0x278;
    if ((unsigned) contentHeight < (unsigned) maxHeight)
        maxHeight = contentHeight;
    this->height = maxHeight;

    if (y == -1)
        y = Globals::h / 2 - maxHeight / 2;
    this->y = y;

    this->scrollWindow = new ScrollTouchWindow(
        this->x, layout->field_0x8 + y, this->width,
        (((maxHeight - this->padding * 2) - layout->field_0x8) - layout->field_0x30) +
        layout->field_0x2bc,
        false);

    this->scrollWindow->setText(title, message);

    ArrayReleaseClasses(*lines);
    delete lines;

    delete this->leftButton;
    this->leftButton = nullptr;

    delete this->rightButton;
    this->rightButton = nullptr;

    if (hasButtons) {
        int baseWidth = layout->field_0x40;
        float scaled = (float) baseWidth * 1.5f;
        if (defaultButtons) scaled = (float) baseWidth;
        int buttonWidth = (int) scaled;

        this->leftButton = new TouchButton(left, 0,
                                           this->x + this->width / 2 - layout->field_0x4c / 2,
                                           this->y + this->height - this->padding,
                                           buttonWidth, 0x22, 4);

        this->rightButton = new TouchButton(right, 0,
                                            this->x + this->width / 2 + layout->field_0x4c / 2,
                                            this->y + this->height - this->padding,
                                            buttonWidth, 0x21, 4);
    } else {
        this->rightButton = new TouchButton(single, 0,
                                            this->x + this->width / 2,
                                            this->y + this->height - this->padding,
                                            layout->field_0x40, 0x24, 4);
    }

    this->buttonsVisible = 1;
}

void ChoiceWindow::setMiscButton(String const &text) {
    Layout *layout;
    int padding = this->padding;
    int buttonWidth;

    if (this->leftButton == nullptr || this->rightButton == nullptr) {
        layout = Globals::layout;
        buttonWidth = padding + layout->field_0x40 * 2;
    } else {
        int leftWidth = this->leftButton->getWidth();
        int rightWidth = this->rightButton->getWidth();
        layout = Globals::layout;
        buttonWidth = leftWidth + rightWidth + padding;
    }

    this->miscButton = new TouchButton(text, 0,
                                       this->x + this->width / 2 - layout->field_0x4c / 2,
                                       this->y + this->height - padding, buttonWidth, 0x22, 4);

    layout = Globals::layout;
    setHeight((this->height - layout->field_0x8) + this->padding * 2 + layout->field_0x30);

    this->miscButton->setPosition(this->x + this->width / 2,
                                  this->y + this->height - this->padding, 0x24);

    if (this->leftButton != nullptr)
        this->leftButton->translate(0, -(Globals::layout->field_0x30 + this->padding));
    if (this->rightButton != nullptr)
        this->rightButton->translate(0, -(Globals::layout->field_0x30 + this->padding));

    this->scrollWindow->setYPosition(
        Globals::h / 2 - this->height / 2 + Globals::layout->field_0x8);
}

static Layout **g_ChoiceWindow_layout_1471bc = nullptr;
static int g_ChoiceWindow_medalColorsLow_1471bc[64];
static int g_ChoiceWindow_medalColorsHigh_1471bc[64];
static int g_ChoiceWindow_creditValues_1471bc[64];
static void **g_ChoiceWindow_font_1471bc_a = nullptr;
static void **g_ChoiceWindow_font_1471bc_b = nullptr;
static void *g_ChoiceWindow_posTargetA_1471bc = nullptr;
static void *g_ChoiceWindow_posTargetB_1471bc = nullptr;
static void *g_ChoiceWindow_posTargetC_1471bc = nullptr;
static void *g_ChoiceWindow_posTargetD_1471bc = nullptr;
static int *g_ChoiceWindow_dirtyFlag_1471bc = nullptr;

void ChoiceWindow::draw() {
    Vector pos;

    Layout *layout = *g_ChoiceWindow_layout_1471bc;
    layout->drawMask();

    layout->drawBox(7, this->x, this->y, this->width, this->height, this->title, 1u);

    PaintCanvas *canvas = PaintCanvas::gCanvas;
    canvas->SetColor(0xffffffff);

    if (this->medalImage != -1) {
        canvas->DrawImage2D(this->medalImage,
                            this->x + (this->width >> 1),
                            this->y + this->padding2 + 1, (unsigned char) 0x11);
        int color;
        if (this->count < 0x24)
            color = g_ChoiceWindow_medalColorsLow_1471bc[this->count];
        else
            color = g_ChoiceWindow_medalColorsHigh_1471bc[this->count];
        canvas->SetColor(color);
        canvas->DrawImage2D(this->medalBgImage,
                            this->x + (this->width >> 1),
                            this->y + this->padding2, (unsigned char) 0x11);

        if (Status::gStatus->hardCoreMode() == 0 &&
            Achievements::gAchievements->isEliteMedal(this->medal) == 0) {
            String creditsText = Globals::layout->formatCredits(
                g_ChoiceWindow_creditValues_1471bc[this->count]);

            void *font = *g_ChoiceWindow_font_1471bc_a;
            int textWidth = canvas->GetTextWidth((unsigned int) (unsigned long) font, creditsText);
            canvas->DrawString((unsigned int) (unsigned long) font, creditsText,
                               this->x + (this->width >> 1) - textWidth / 2,
                               this->y + this->height - this->padding3,
                               false);
        }

        canvas->SetColor(0xffffffff);
        void *font = *g_ChoiceWindow_font_1471bc_b;
        int textWidth = canvas->GetTextWidth((unsigned int) (unsigned long) font, this->medalText);
        canvas->DrawString((unsigned int) (unsigned long) font, this->medalText,
                           this->x + (this->width >> 1) - textWidth / 2,
                           this->y + this->padding4, false);
    }

    this->scrollWindow->draw();

    if (this->buttonsVisible != 0) {
        if (this->leftButton != nullptr) this->leftButton->draw();

        if (this->rightButton != nullptr) {
            this->rightButton->draw();
            pos = this->rightButton->getPosition();
            ((ChoiceWindowButtonPosCache *) g_ChoiceWindow_posTargetA_1471bc)->rightButtonPosX = (int) pos[0];
            pos = this->rightButton->getPosition();
            ((ChoiceWindowButtonPosCache *) g_ChoiceWindow_posTargetB_1471bc)->rightButtonPosY = (int) pos[1];
        }

        if (this->leftButton != nullptr) {
            pos = this->leftButton->getPosition();
            ((ChoiceWindowButtonPosCache *) g_ChoiceWindow_posTargetC_1471bc)->leftButtonPosX = (int) pos[0];
            pos = this->leftButton->getPosition();
            ((ChoiceWindowButtonPosCache *) g_ChoiceWindow_posTargetD_1471bc)->leftButtonPosY = (int) pos[1];

            if (this->rightButton == nullptr) {
                pos = this->leftButton->getPosition();
                ((ChoiceWindowButtonPosCache *) g_ChoiceWindow_posTargetC_1471bc)->singleButtonPosX = (int) pos[0];
                pos = this->leftButton->getPosition();
                ((ChoiceWindowButtonPosCache *) g_ChoiceWindow_posTargetD_1471bc)->singleButtonPosY = (int) pos[1];
            }
        }

        *g_ChoiceWindow_dirtyFlag_1471bc = 1;
        if (this->miscButton != nullptr) this->miscButton->draw();
    }
}

void ChoiceWindow::left() {
}

void ChoiceWindow::right() {
}

int ChoiceWindow::fire() {
    return 0;
}
