#include "game/ui/ScrollTouchBox.h"
#include "game/core/String.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/Engine.h"
#include "engine/core/GameText.h"
#include "game/core/Globals.h"
#include "game/ui/Layout.h"

#include <cstdint>

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
    this->font = reinterpret_cast<String *>(static_cast<std::uintptr_t>(Globals::font));
}

void ScrollTouchBox::setText(AbyssEngine::String text) {
    String tmp(text);
    setText(tmp, Globals::font);
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


void ScrollTouchBox::draw() {
    int language = GameText::getLanguage();
    unsigned special = 0;
    unsigned shifted = (unsigned) (language - 10);
    if ((unsigned short) shifted < 6)
        special = (0x33U >> (shifted & 0x3f)) & 1;

    PaintCanvas *canvas = Globals::Canvas;
    canvas->SetColor((unsigned int) -1);

    Array<String *> *firstLineArray = this->lines;
    if (firstLineArray != 0) {
        unsigned notSpecial = special ^ (firstLineArray != 0);
        for (unsigned i = 0; i < this->lines->size(); ++i) {
            Array<String *> *lineArray = this->lines;
            unsigned count = (unsigned) lineArray->size();
            int lastOffset;
            if (i != count - 1 || notSpecial != 0) {
                lastOffset = 0;
            } else {
                lastOffset = -8;
                if (Globals::retinaDisplay == 0)
                    lastOffset = -4;
            }

            int yBase = this->y;
            int lineY = Globals::layout->field_0x4 * (int) i + yBase + this->scrollOffset;
            if (count == 1 ||
                (yBase <= lineY &&
                 lineY + lastOffset <= (this->height + yBase) - canvas->GetTextHeight(
                     static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(this->font))))) {
                int x;
                String *font = this->font;
                String *line = lineArray->data()[i];
                canvas = Globals::Canvas;
                if (GameText::getLanguage() == 9) {
                    Engine::enableReverseFlag = 0;
                    int left = this->x;
                    int width = this->width;
                    if (this->centered == 0) {
                        x = (left + width) - canvas->GetTextWidth(
                                static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(font)), *line);
                    } else {
                        x = (left + (width >> 1)) - (canvas->GetTextWidth(
                                                         static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(font)), *line) >> 1);
                    }
                } else {
                    x = this->x;
                    if (this->centered != 0) {
                        int width = this->width;
                        x = (x + (width >> 1)) - (canvas->GetTextWidth(
                                                      static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(font)), *line) >> 1);
                    }
                }
                canvas->DrawString(static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(font)), *line, x, lineY, false);
            }
        }
    }

    Engine::enableReverseFlag = 1;
}

void ScrollTouchBox::setText(AbyssEngine::String text, int font) {
    if (this->lines != 0) {
        ArrayReleaseClasses(*this->lines); ArrayRemoveAll(*(this->lines));
        delete this->lines;
        this->lines = 0;
    }

    this->font = (String *) (std::size_t) font;
    Array<String *> *lineArray = new Array<String *>();

    int lineWidth = this->textWidth;
    this->lines = lineArray;
    static_cast<Globals *>(Globals::globals)->getLineArray(font, text, lineWidth, lineArray);

    int boxHeight = this->height;
    Array<String *> *curLines = this->lines;
    int contentHeight = Globals::layout->field_0x4 * (int) curLines->size();
    this->contentHeight = contentHeight;
    if (contentHeight > boxHeight) {
        this->width = this->textWidth - Globals::layout->field_0x48;
        if (curLines != 0) {
            ArrayReleaseClasses(*curLines);
            delete curLines;
            this->lines = 0;
        }

        lineArray = new Array<String *>();
        int fontArg = font;
        lineWidth = this->width;
        this->lines = lineArray;
        static_cast<Globals *>(Globals::globals)->getLineArray(fontArg, text, lineWidth, lineArray);

        String *empty = new String("\n", false);
        ArrayAdd(empty, *(this->lines));
        Array<String *> *finalLines = this->lines;
        int finalCount = (int) finalLines->size();
        int finalLineHeight = Globals::layout->field_0x4;
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
