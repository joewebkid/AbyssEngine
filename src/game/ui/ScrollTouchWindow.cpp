#include "game/ui/ScrollTouchWindow.h"
#include "game/ui/ScrollTouchBox.h"
#include "game/ui/Layout.h"
#include "game/core/String.h"
#include "engine/render/PaintCanvas.h"

void ScrollTouchWindow::OnTouchEnd(int x, int y) {
    this->scrollBox->OnTouchEnd(x, y);
    this->touchActive = 0;
}

void ScrollTouchWindow::OnTouchMove(int x, int y) {
    this->scrollBox->OnTouchMove(x, y);
    this->touchActive = 1;
}

void ScrollTouchWindow::setTextCentered(bool centered) {
    this->scrollBox->setTextCentered(centered);
}

void ScrollTouchWindow::setYPosition(int y) {
    this->scrollBox->setYPosition(y);
}

ScrollTouchWindow::~ScrollTouchWindow() {
    delete this->scrollBox;
    this->scrollBox = nullptr;
}

void ScrollTouchWindow::update(int dt) {
    this->scrollBox->update(dt);
}

void ScrollTouchWindow::OnTouchBegin(int x, int y) {
    this->scrollBox->OnTouchBegin(x, y);
}

static Layout **g_STW_canvas_draw = nullptr;
static Layout **g_STW_layout_draw_plain = nullptr;
static Layout **g_STW_layout_draw_window = nullptr;
static Layout **g_STW_layout_draw_scrollbar = nullptr;

void ScrollTouchWindow::draw() {
    PaintCanvas **canvasHolder = reinterpret_cast<PaintCanvas **>(g_STW_canvas_draw);
    PaintCanvas *canvas = *canvasHolder;
    int color = canvas->GetColor();

    int scrollOffset;
    int contentHeight;
    if (this->hasFrame == 0) {
        Layout *layout = *g_STW_layout_draw_plain;
        contentHeight = this->height - layout->field_0x2c * 2;
        scrollOffset = 0;
    } else {
        Layout *layout = *g_STW_layout_draw_window;
        {
            String windowTitle(this->title);
            layout->drawWindow(windowTitle, this->x, this->y,
                               this->width, this->height, true);
        }
        layout = *g_STW_layout_draw_window;
        contentHeight = this->height - layout->field_0x2c * 2;
        scrollOffset = -layout->field_0x8;
    }

    this->scrollBox->draw();
    int scrollHeight = scrollOffset + contentHeight;
    float scale = (float) scrollHeight;
    float start = this->scrollBox->getRelativeScrollStartPos();
    float height = this->scrollBox->getRelativeScrollHeight();
    int startPx = (int) (start * scale);
    int heightPx = (int) (height * scale);

    if (startPx > 0 || heightPx >= 1) {
        Layout *layout = *g_STW_layout_draw_scrollbar;
        int yOffset = (this->hasFrame == 0) ? 0 : layout->field_0x8;
        layout->drawScrollBar((this->x + this->width) - layout->field_0x48 - layout->field_0x2c,
                              this->y + layout->field_0x2c + yOffset,
                              scrollHeight, startPx, heightPx);
    }

    (*canvasHolder)->SetColor((unsigned int) color);
}

static Layout **g_STW_layout_drawTextBG = nullptr;
static const char g_STW_empty_drawTextBG[] = "";

void ScrollTouchWindow::drawTextBG() {
    Layout *layout = *g_STW_layout_drawTextBG;
    int x = this->x;
    int y = this->y;
    int w = this->width;
    int pad = layout->field_0x2c;
    float relHeight = this->scrollBox->getRelativeScrollHeight();
    int widthInset;
    int heightInset;
    if (relHeight > 0.0f) {
        widthInset = layout->field_0x48 + pad * 2;
        heightInset = pad * 2;
    } else {
        widthInset = pad;
        heightInset = pad * 2;
    }
    int h = this->height;
    String text(g_STW_empty_drawTextBG);
    layout->drawBox(5, x, pad + y, w - widthInset, h - heightInset, text, 0);
}

void ScrollTouchWindow::setText(AbyssEngine::String title, AbyssEngine::String text) {
    this->scrollBox->setText(text);
    this->title = title;
}

static Layout **g_STW_layout_174128 = nullptr;

ScrollTouchWindow::ScrollTouchWindow(int x, int y, int w, int h, bool hasFrame) {
    this->x = x;
    this->y = y;
    this->width = w;
    this->height = h;

    Layout *layout = *g_STW_layout_174128;
    int border = layout->field_0x4c;
    int extra;
    int boxY;
    if (hasFrame) {
        int top = layout->field_0x8;
        boxY = border + y + top;
        extra = -top;
    } else {
        boxY = border + y;
        extra = 0;
    }
    this->scrollBox = new ScrollTouchBox(border + x, boxY, w - border * 2,
                                         extra + h - border * 2);
    this->touchActive = 0;
    this->hasFrame = hasFrame;
}

void ScrollTouchWindow::setText(AbyssEngine::String title, AbyssEngine::String text, int color) {
    this->scrollBox->setText(text, color);
    this->title = title;
}

static Layout **g_STW_layout_1741c0 = nullptr;

ScrollTouchWindow::ScrollTouchWindow(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->width = w;
    this->height = h;

    Layout *layout = *g_STW_layout_1741c0;
    int top = layout->field_0x8;
    int border = layout->field_0x4c;
    this->scrollBox = new ScrollTouchBox(border + x, top + border + y, w - border * 2,
                                         (h - top) - border * 2);
    this->touchActive = 0;
    this->hasFrame = 1;
}
