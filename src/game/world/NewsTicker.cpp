#include "game/world/NewsTicker.h"
#include "engine/core/AbyssEngine.h"
#include "game/world/SolarSystem.h"
#include "game/world/NewsItem.h"
#include "game/mission/Status.h"
#include "engine/core/GameText.h"
#include "engine/core/AERandom.h"
#include "engine/file/FileRead.h"
#include "game/core/String.h"
#include "engine/render/PaintCanvas.h"

static PaintCanvas **g_NewsTicker_draw_canvas = nullptr;
static int **g_NewsTicker_draw_fontHeight = nullptr;
static int *g_NewsTicker_draw_screenHeight = nullptr;
static void **g_NewsTicker_draw_font = nullptr;
static int *g_NewsTicker_touchMove_screenWidth = nullptr;
static int **g_NewsTicker_getHeight_font = nullptr;
static int *g_NewsTicker_getHeight_screen = nullptr;
static const char g_NewsTicker_ctor_empty[1] = {0};
static AbyssEngine::AERandom **g_NewsTicker_ctor_random = nullptr;
static Status **g_NewsTicker_ctor_status = nullptr;
static GameText **g_NewsTicker_ctor_text = nullptr;
static const char g_NewsTicker_ctor_separator[1] = {0};
static void **g_NewsTicker_ctor_font = nullptr;
static PaintCanvas **g_NewsTicker_ctor_canvas = nullptr;
static int **g_NewsTicker_touchBegin_font = nullptr;
static int *g_NewsTicker_touchBegin_screen = nullptr;

NewsTicker::NewsTicker(int x, int y, int width, int faction, int level) {
    { if (this->tickerText.data) delete[] this->tickerText.data; this->tickerText.data = nullptr; this->tickerText.length = 0; }
    this->x = x;
    this->y = y;
    this->width = width;
    this->tickerText = String(g_NewsTicker_ctor_empty);
    this->textWidth = 0;

    FileRead reader;
    Array<NewsItem *> *allItems = reader.loadTicker();

    Array<NewsItem *> *items = new Array<NewsItem *>();

    AbyssEngine::AERandom *random = *g_NewsTicker_ctor_random;
    int wanted = random->nextInt(1) + 2;

    for (uint32_t i = 0; i < allItems->size(); ++i) {
        NewsItem *item = (*allItems)[i];
        if (item->field_0x10 > 0 && item->field_0x10 <= level && level <= item->field_0x14) {
            if (((uint8_t *) item->data)[faction] != 0) {
                ArrayAdd((NewsItem *) item->clone(), *items);
            }
        }
    }

    Status *status = *g_NewsTicker_ctor_status;
    int added = 0;
    int attempts = 0;
    while (added < wanted && attempts < 100) {
        ++attempts;
        if (allItems->size() == 0) {
            break;
        }
        NewsItem *item = (*allItems)[random->nextInt(allItems->size())];
        if (status->getSystem() > 0x15 && item->id == 0x0d) {
            continue;
        }
        if (item->field_0x14 < 0xa1 || item->field_0x10 > level) {
            continue;
        }
        if (random->nextInt(100) > 0x31) {
            continue;
        }
        if (((uint8_t *) item->data)[faction] == 0 || item->field_0x18 != 0) {
            continue;
        }
        if (item->flag != 0) {
            int64_t now = status->getPlayingTime();
            int64_t *lastShown = (int64_t *) &status->field_160;
            if ((uint32_t)(now - *lastShown) <= 0x493e0) {
                continue;
            }
            *lastShown = now;
            status->field_174 = item->id;
        }
        ArrayAdd((NewsItem *) item->clone(), *items);
        item->field_0x18 = 1;
        ++added;
    }

    String separator(g_NewsTicker_ctor_separator);
    GameText *gameText = *g_NewsTicker_ctor_text;
    for (uint32_t i = 0; i < items->size(); ++i) {
        NewsItem *item = (*items)[i];
        String line(*gameText->getText(item->id + 0x0cbe));
        String replaced = replaceTokens(line);

        if (item->flag != 0) {
            if (item->field_0x18 == 0) {
                replaced = status->string_168;
            } else {
                status->string_168 = replaced;
            }
        }

        this->tickerText += replaced + separator;
    }

    PaintCanvas *canvas = *g_NewsTicker_ctor_canvas;
    unsigned int font = (unsigned int) (unsigned long) *g_NewsTicker_ctor_font;
    this->textWidth = canvas->GetTextWidth(font, this->tickerText);
    if (this->textWidth < width) {
        this->tickerText += String(this->tickerText);
        this->textWidth = canvas->GetTextWidth(font, this->tickerText);
    }

    if (GameText::getLanguage() == 9) {
        width = -this->textWidth;
    }
    this->lastTouchX = 0;
    this->touched = 0;
    this->scrollOffset = (float) width;

    ArrayReleaseClasses(*items);
    delete items;
    ArrayReleaseClasses(*allItems);
    delete allItems;
}

NewsTicker::~NewsTicker() {
    this->tickerText.~String();
}

void NewsTicker::draw() {
    PaintCanvas *canvas = *g_NewsTicker_draw_canvas;
    canvas->SetColor(0x6f);

    int *fontMetrics = *g_NewsTicker_draw_fontHeight;
    int screenHeight = *g_NewsTicker_draw_screenHeight;
    int fontHeight = fontMetrics[4];
    int fillHeight = (2 - this->y) + screenHeight - fontHeight;
    canvas->FillRectangle(this->x, this->y - 2, this->width, fillHeight);

    canvas->EnableClip(this->x, this->y, this->width, screenHeight);
    canvas->SetColor(0x777777ff);

    unsigned int font = (unsigned int) (unsigned long) *g_NewsTicker_draw_font;
    canvas->DrawString(font, this->tickerText,
                       (int) (this->scrollOffset + (float) this->x), this->y, false);

    float x = this->scrollOffset;
    if (GameText::getLanguage() == 9) {
        if (x > (float) this->textWidth) {
            canvas->DrawString(font, this->tickerText,
                               (int) (x + (float) this->x), this->y, false);
        }
    } else {
        if (x < (float) (this->textWidth - this->width)) {
            canvas->DrawString(font, this->tickerText,
                               (int) (x + (float) this->x + (float) this->textWidth),
                               this->y, false);
        }
    }

    canvas->DisableClip();
}

void NewsTicker::update(int dt) {
    if (this->touched != 0) {
        return;
    }

    float step = ((float) dt / 1000.0f) * 50.0f;
    if (GameText::getLanguage() == 9) {
        this->scrollOffset += step;
        if (this->scrollOffset > (float) this->width) {
            this->scrollOffset = (float) -this->textWidth;
        }
    } else {
        this->scrollOffset -= step;
        if (this->scrollOffset < (float) -this->textWidth) {
            this->scrollOffset = 0;
        }
    }
}

int NewsTicker::getHeight() {
    int fontHeight = (*g_NewsTicker_getHeight_font)[4];
    int bottom = (*g_NewsTicker_getHeight_screen + 2) - fontHeight;
    return bottom - this->y;
}

uint8_t NewsTicker::OnTouchBegin(int x, int y) {
    if (this->x <= x && x <= this->x + this->width && this->y <= y) {
        int fontHeight = (*g_NewsTicker_touchBegin_font)[4];
        int bottom = (*g_NewsTicker_touchBegin_screen + 2) - fontHeight;
        if (y <= bottom) {
            this->lastTouchX = x;
            this->touched = 1;
        }
    }
    return this->touched;
}

bool NewsTicker::OnTouchMove(int x, int) {
    if (this->touched != 0) {
        float delta = (float) (this->lastTouchX - x);
        this->scrollOffset -= delta;
        float maxX = (float) *g_NewsTicker_touchMove_screenWidth;
        if (maxX < this->scrollOffset) {
            this->scrollOffset = maxX;
        }
        this->lastTouchX = x;
    }
    return this->touched != 0;
}

uint32_t NewsTicker::OnTouchEnd(int, int) {
    if (this->touched == 0) {
        return 0;
    }
    this->touched = 0;
    return 1;
}

String NewsTicker::replaceTokens(String text) {
    return text;
}

