#include "game/world/NewsTicker.h"
#include "engine/core/AbyssEngine.h"
#include "game/world/SolarSystem.h"
#include "game/world/NewsItem.h"
#include "game/mission/Status.h"
#include "engine/core/GameText.h"
#include "engine/core/AERandom.h"
#include "engine/file/FileRead.h"
#include "game/core/String.h"
#include "game/core/Globals.h"
#include "game/ui/Layout.h"
#include "engine/render/PaintCanvas.h"

static const char g_NewsTicker_ctor_empty[1] = {0};

NewsTicker::NewsTicker(int x, int y, int width, int faction, int level) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->tickerText = String(g_NewsTicker_ctor_empty);
    this->textWidth = 0;

    FileRead reader;
    Array<NewsItem *> *allItems = reader.loadTicker();

    Array<NewsItem *> *items = new Array<NewsItem *>();

    AbyssEngine::AERandom *random = static_cast<AbyssEngine::AERandom *>(Globals::rnd);
    int wanted = random->nextInt(1) + 2;

    for (uint32_t i = 0; i < allItems->size(); ++i) {
        NewsItem *item = (*allItems)[i];
        if (item->field_0x10 > 0 && item->field_0x10 <= level && level <= item->field_0x14) {
            if (((uint8_t *) item->data)[faction] != 0) {
                ArrayAdd((NewsItem *) item->clone(), *items);
            }
        }
    }

    Status *status = Globals::status;
    int added = 0;
    int attempts = 0;
    while (added < wanted && attempts < 100) {
        ++attempts;
        if (allItems->size() == 0) {
            break;
        }
        NewsItem *item = (*allItems)[random->nextInt(allItems->size())];
        if (status->getSystem()->getIndex() > 0x15 && item->id == 0x0d) {
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

    String separator("    +++    ");
    GameText *gameText = Globals::gameText;
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

    PaintCanvas *canvas = Globals::Canvas;
    unsigned int font = Globals::font;
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
}

void NewsTicker::draw() {
    PaintCanvas *canvas = Globals::Canvas;
    canvas->SetColor(0x6f);

    int screenHeight = Globals::h;
    int fontHeight = Globals::layout->field_0x10;
    int fillHeight = (2 - this->y) + screenHeight - fontHeight;
    canvas->FillRectangle(this->x, this->y - 2, this->width, fillHeight);

    canvas->EnableClip(this->x, this->y, this->width, screenHeight);
    canvas->SetColor(0x777777ff);

    unsigned int font = Globals::font;
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
    return Globals::h + 2 - Globals::layout->field_0x10 - this->y;
}

uint8_t NewsTicker::OnTouchBegin(int x, int y) {
    if (this->x <= x && x <= this->x + this->width && this->y <= y) {
        int bottom = Globals::h + 2 - Globals::layout->field_0x10;
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
        float maxX = (float) Globals::w;
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
