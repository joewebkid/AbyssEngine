#include "game/world/NewsTicker.h"
#include "engine/core/AbyssEngine.h"
#include "game/world/SolarSystem.h"
#include "game/world/Galaxy.h"
#include "game/world/Station.h"
#include "game/world/NewsItem.h"
#include "game/mission/Status.h"
#include "game/mission/Item.h"
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
    Status *status = Globals::status;
    Globals *globals = static_cast<Globals *>(Globals::globals);
    Galaxy *galaxy = static_cast<Galaxy *>(Globals::galaxy);
    AbyssEngine::AERandom *random = static_cast<AbyssEngine::AERandom *>(Globals::rnd);
    GameText *gameText = Globals::gameText;

    if (status->stringHasToken(text, "#PLANET_NAME")) {
        Station *station = globals->getRandomStation();
        text = status->replaceHash(text, station->getName(), "#PLANET_NAME");
        if (status->stringHasToken(text, "#SYSTEM_NAME")) {
            SolarSystem *system = galaxy->getSystem(station->getSystem());
            text = status->replaceHash(text, system->getName(), "#SYSTEM_NAME");
        }
        delete station;
    }

    if (status->stringHasToken(text, "#SYSTEM_NAME")) {
        SolarSystem *system;
        if (status->stringHasToken(text, "#DRINK_NAME")) {
            system = status->getSystem();
            text = status->replaceHash(text,
                                       *gameText->getText(system->getIndex() + 1406),
                                       "#DRINK_NAME");
        } else {
            system = globals->getRandomSystemForDrinks();
        }
        text = status->replaceHash(text, system->getName(), "#SYSTEM_NAME");
    }

    if (status->stringHasToken(text, "#CHILD_NAME")) {
        String name = globals->getRandomName(status->getSystem()->getRace(), true);
        int split = static_cast<int>(name.IndexOf(String(" ")));
        if (split >= 0) name = name.SubString(0, split);
        text = status->replaceHash(text, name, "#CHILD_NAME");
    }

    if (status->stringHasToken(text, "#SHIP_NAME")) {
        int race = random->nextInt(4);
        int ship = globals->getRandomEnemyFighter(race);
        text = status->replaceHash(text, *gameText->getText(ship + 913), "#SHIP_NAME");
    }

    if (status->stringHasToken(text, "#PLATFORM_NUMBER")) {
        static const char *letters[4] = {"A", "B", "C", "D"};
        unsigned letterIndex = random->nextInt(6);
        const char *letter = letterIndex < 4 ? letters[letterIndex]
                                             : (letterIndex == 4 ? "E" : "F");
        String platform = String(letter) + String(random->nextInt(10));
        text = status->replaceHash(text, platform, "#PLATFORM_NUMBER");
    }

    if (status->stringHasToken(text, "#CATASTROPHE")) {
        text = status->replaceHash(text,
                                   *gameText->getText(random->nextInt(5) + 3237),
                                   "#CATASTROPHE");
    }

    if (status->stringHasToken(text, "#VICTIMS")) {
        int victims = random->nextInt(99000) + 100000;
        victims -= victims % 1000;
        text = status->replaceHash(text, Globals::layout->formatNumber(victims), "#VICTIMS");
    }

    if (status->stringHasToken(text, "#BERGER_LASER")) {
        text = status->replaceHash(text,
                                   *gameText->getText(random->nextInt(4) + 1279),
                                   "#BERGER_LASER");
    }

    if (status->stringHasToken(text, "#VOSSK_SHIP_OR_ITEM")) {
        int choice = random->nextInt(100);
        String subject;
        if (choice > 39) {
            Array<Item *> *eligible = new Array<Item *>();
            Array<Item *> *items = static_cast<Array<Item *> *>(Globals::items);
            for (uint32_t i = 0; i < items->size(); ++i) {
                Item *item = (*items)[i];
                if (item->getAttribute(60) == 1 && !item->getIngredients() &&
                    item->getType() != 4) {
                    ArrayAdd(item, *eligible);
                }
            }
            Item *item = (*eligible)[random->nextInt(eligible->size())];
            subject = *gameText->getText(item->getIndex() + 1274);
            delete eligible;
        } else {
            subject = *gameText->getText(922);
        }

        unsigned revenue = static_cast<unsigned>(status->getPlayingTime() / 1000000) +
                           (choice < 40 ? 140 : 12);
        String formatted = Globals::layout->formatNumber(revenue);
        text = status->replaceHash(text, subject, "#VOSSK_SHIP_OR_ITEM");
        text = status->replaceHash(text,
                                   formatted + String(" ") + *gameText->getText(3242),
                                   "#VOSSK_REVENUE");
    }

    if (status->stringHasToken(text, "#PROFESSION")) {
        text = status->replaceHash(text,
                                   *gameText->getText(random->nextInt(4) + 3243),
                                   "#PROFESSION");
        text = status->replaceHash(text, globals->getRandomName(random->nextInt(8), true),
                                   "#NAME");
        text = status->replaceHash(text,
                                   *gameText->getText(random->nextInt(4) + 3247),
                                   "#ACTIVITY");
    }

    if (status->stringHasToken(text, "#RACE_NAME")) {
        text = status->replaceHash(text,
                                   *gameText->getText(random->nextInt(4) + 406),
                                   "#RACE_NAME");
        text = status->replaceHash(text, globals->getRandomName(random->nextInt(8), true),
                                   "#NAME");
        text = status->replaceHash(text,
                                   *gameText->getText(random->nextInt(5) + 3251),
                                   "#CRIME");
    }

    if (status->stringHasToken(text, "#NIVELIAN_PLANET")) {
        Array<SolarSystem *> *systems = galaxy->getSystems();
        int systemIndex = -1;
        while (systemIndex < 0) {
            SolarSystem *candidate = (*systems)[random->nextInt(systems->size())];
            if (candidate->getRace() == 2) systemIndex = candidate->getIndex();
        }
        Array<int> *stationIds = (*systems)[systemIndex]->stationIds;
        Station *station = galaxy->getStation(
            (*stationIds)[random->nextInt(stationIds->size())]);
        text = status->replaceHash(text, station->getName(), "#NIVELIAN_PLANET");
        int demonstrators = random->nextInt(990000) + 1000000;
        demonstrators -= demonstrators % 1000;
        text = status->replaceHash(text, Globals::layout->formatNumber(demonstrators),
                                   "#DEMONSTRATORS");
        delete station;
    }

    if (status->stringHasToken(text, "#POLL_PERCENTAGE")) {
        text = status->replaceHash(text, String(random->nextInt(80) + 10),
                                   "#POLL_PERCENTAGE");
        text = status->replaceHash(text,
                                   *gameText->getText(random->nextInt(6) + 3256),
                                   "#POLL_TOPIC");
    }

    if (status->stringHasToken(text, "#CASUALTIES")) {
        text = status->replaceHash(text, 2 + String(random->nextInt(48)), "#CASUALTIES");
    }

    return text;
}
