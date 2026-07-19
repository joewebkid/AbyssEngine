#include "game/ui/ScrollTouchBox.h"
#include "game/core/String.h"
#include "engine/render/PaintCanvas.h"
#include "engine/core/GameText.h"
#include "game/core/Globals.h"

struct FontMetrics {
    int field_0x0;
    int lineHeight;
    int field_0x08[16];
    int wrapMargin;
};

void ScrollTouchBox::setTextCentered(bool centered) {
    this->centered = centered;
}

float ScrollTouchBox::getRelativeScrollStartPos() {
    int pos = this->scrollOffset;
    if (pos > 0)
        return 0.0f;
    return -(float) pos / (float) this->contentHeight;
}

ScrollTouchBox::~ScrollTouchBox() {
    if (this->lines != 0) {
        ArrayReleaseClasses(*this->lines); ArrayRemoveAll(*(this->lines));
        delete this->lines;
        this->lines = 0;
    }
}

void ScrollTouchBox::setPosition(int x, int y) {
    this->x = x;
    this->y = y;
}

void ScrollTouchBox::OnTouchEnd(int x, int y) {
    if (this->dragging != 0) {
        int delta = this->lastDelta;
        float speed = 0.0f;
        if ((delta < 0 ? -delta : delta) > 3)
            speed = (float) delta;
        this->damping = 0.95f;
        this->dragging = 0;
        this->scrollOffset = delta + this->scrollOffset;
        this->velocity = speed;
    }
}

void ScrollTouchBox::OnTouchMove(int x, int y) {
    if (this->dragging != 0 && this->contentHeight > this->height) {
        int delta = y - this->lastTouchY;
        this->lastTouchY = y;
        this->lastDelta = delta;
        this->damping = 1.0f;
        this->scrollOffset = delta + this->scrollOffset;
    }
}

void ScrollTouchBox::OnTouchBegin(int x, int y) {
    if (touchIsInside(x, y)) {
        this->touchStartY = y;
        this->lastTouchY = y;
        this->lastDelta = 0;
        this->dragging = 1;
    }
}


static String **g_ScrollTouchBox_defaultFont_135598 = nullptr;

ScrollTouchBox::ScrollTouchBox(int x, int y, int width, int height) {
    this->touchStartY = 0;
    this->lines = 0;
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->textWidth = width;
    this->dragging = 0;
    this->scrollOffset = 0;
    this->centered = 0;
    this->contentHeight = 0;
    this->lastDelta = 0;
    this->damping = 0.0f;
    this->velocity = 0.0f;
    this->font = *g_ScrollTouchBox_defaultFont_135598;
}


static int **g_ScrollTouchBox_defaultWidth_13570c = nullptr;

void ScrollTouchBox::setText(AbyssEngine::String text) {
    String tmp(text);
    setText(tmp, **g_ScrollTouchBox_defaultWidth_13570c);
}

void ScrollTouchBox::update(int dt) {
    int height = this->height;
    int contentHeight = this->contentHeight;
    if (contentHeight < height)
        return;

    if (this->dragging == 0) {
        float velocity = this->damping * this->velocity;
        float absVelocity = -velocity;
        if (velocity > 0.0f)
            absVelocity = velocity;
        this->velocity = velocity;
        if (absVelocity > 1.5f)
            this->scrollOffset = (int) (velocity + (float) this->scrollOffset);
    }

    int pos = this->scrollOffset;
    int pull;
    if (pos < 1)
        goto negative;

    pull = -pos;
    goto apply;

negative: {
        int over = contentHeight - height;
        int min = -over;
        if (min <= pos)
            return;
        over += pos;
        pull = -over;
    }

apply:
    this->damping = 1.0f;
    this->velocity = (float) pull * 0.5f;
}


static void **g_ScrollTouchBox_canvas_135778 = nullptr;

static char *g_ScrollTouchBox_flag_135778 = nullptr;

static FontMetrics **g_ScrollTouchBox_font_135778 = nullptr;

static uint8_t *g_ScrollTouchBox_rtl_135778 = nullptr;

static uint8_t *g_ScrollTouchBox_dirty_135778 = nullptr;

void ScrollTouchBox::draw() {
    int language = GameText::getLanguage();
    unsigned special = 0;
    unsigned shifted = (unsigned) (language - 10);
    if ((unsigned short) shifted < 6)
        special = (0x33U >> (shifted & 0x3f)) & 1;

    void **canvasHolder = g_ScrollTouchBox_canvas_135778;
    void *canvas = *canvasHolder;
    ((PaintCanvas *) canvas)->SetColor((unsigned int) -1);

    Array<String *> *firstLineArray = this->lines;
    if (firstLineArray != 0) {
        unsigned notSpecial = special ^ (firstLineArray != 0);
        char *flag = g_ScrollTouchBox_flag_135778;
        FontMetrics **fontHolder = g_ScrollTouchBox_font_135778;
        uint8_t *rtl = g_ScrollTouchBox_rtl_135778;

        for (unsigned i = 0; i < this->lines->size(); ++i) {
            Array<String *> *lineArray = this->lines;
            unsigned count = (unsigned) lineArray->size();
            int lastOffset;
            if (i != count - 1 || notSpecial != 0) {
                lastOffset = 0;
            } else {
                lastOffset = -8;
                if (*flag == 0)
                    lastOffset = -4;
            }

            int yBase = this->y;
            int lineY = (*fontHolder)->lineHeight * (int) i + yBase + this->scrollOffset;
            if (count == 1 ||
                (yBase <= lineY &&
                 lineY + lastOffset <= (this->height + yBase) - ((PaintCanvas *) canvas)->GetTextHeight(
                     (unsigned int) (unsigned long) this->font))) {
                int x;
                String *font = this->font;
                String *line = lineArray->data()[i];
                canvas = *canvasHolder;
                if (GameText::getLanguage() == 9) {
                    *rtl = 0;
                    int left = this->x;
                    int width = this->width;
                    if (this->centered == 0) {
                        x = (left + width) - ((PaintCanvas *) canvas)->GetTextWidth(
                                (unsigned int) (unsigned long) font, *line);
                    } else {
                        x = (left + (width >> 1)) - (((PaintCanvas *) canvas)->GetTextWidth(
                                                         (unsigned int) (unsigned long) font, *line) >> 1);
                    }
                } else {
                    x = this->x;
                    if (this->centered != 0) {
                        int width = this->width;
                        x = (x + (width >> 1)) - (((PaintCanvas *) canvas)->GetTextWidth(
                                                      (unsigned int) (unsigned long) font, *line) >> 1);
                    }
                }
                ((PaintCanvas *) canvas)->DrawString((unsigned int) (unsigned long) font, *line, x, lineY, false);
            }
        }
    }

    *g_ScrollTouchBox_dirty_135778 = 1;
}


static void **g_ScrollTouchBox_globals_135600 = nullptr;

static FontMetrics **g_ScrollTouchBox_font_135600 = nullptr;

static char g_ScrollTouchBox_empty_135600[1] = {0};

void ScrollTouchBox::setText(AbyssEngine::String text, int font) {
    if (this->lines != 0) {
        ArrayReleaseClasses(*this->lines); ArrayRemoveAll(*(this->lines));
        delete this->lines;
        this->lines = 0;
    }

    this->font = (String *) (std::size_t) font;
    Array<String *> *lineArray = new Array<String *>();

    void **globals = g_ScrollTouchBox_globals_135600;
    int lineWidth = this->textWidth;
    this->lines = lineArray;
    static_cast<Globals *>(*globals)->getLineArray(font, text, lineWidth, lineArray);

    FontMetrics **fontHolder = g_ScrollTouchBox_font_135600;
    int boxHeight = this->height;
    Array<String *> *curLines = this->lines;
    FontMetrics *fontObj = *fontHolder;
    int contentHeight = fontObj->lineHeight * (int) curLines->size();
    this->contentHeight = contentHeight;
    if (contentHeight > boxHeight) {
        this->width = this->textWidth - fontObj->wrapMargin;
        if (curLines != 0) {
            ArrayReleaseClasses(*curLines);
            delete curLines;
            this->lines = 0;
        }

        lineArray = new Array<String *>();
        int fontArg = font;
        lineWidth = this->width;
        this->lines = lineArray;
        static_cast<Globals *>(*globals)->getLineArray(fontArg, text, lineWidth, lineArray);

        String *empty = new String(g_ScrollTouchBox_empty_135600, false);
        ArrayAdd(empty, *(this->lines));
        Array<String *> *finalLines = this->lines;
        FontMetrics *finalFont = *fontHolder;
        int finalCount = (int) finalLines->size();
        int finalLineHeight = finalFont->lineHeight;
        this->contentHeight = finalLineHeight * finalCount;
    }

    this->scrollOffset = 0;
    this->lastDelta = 0;
    this->damping = 0.0f;
    this->velocity = 0.0f;
    this->touchStartY = 0;
}

float ScrollTouchBox::getRelativeScrollHeight() {
    int height = this->height;
    int contentHeight = this->contentHeight;
    if (height > contentHeight)
        return 1.0f;

    int pos = this->scrollOffset;
    if (pos >= 1) {
        pos = height - pos;
    } else if (pos < height - contentHeight) {
        pos = pos + contentHeight;
    } else {
        pos = height;
    }

    return (float) pos / (float) contentHeight;
}

bool ScrollTouchBox::touchIsInside(int x, int y) {
    int left = this->x;
    int top = 0;
    if (left <= x)
        top = this->y;
    if (left > x || top > y)
        return false;

    bool inside = false;
    if (x < left + this->width)
        inside = y < top + this->height;
    return inside;
}

void ScrollTouchBox::setYPosition(int y) {
    this->y = y;
}
