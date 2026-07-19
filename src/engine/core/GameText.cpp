#include "engine/core/GameText.h"
GameText *GameText::gGameText = nullptr;
#include "engine/file/AEFile.h"


static short g_GameText_language_storage = 0;
static short *g_GameText_language = &g_GameText_language_storage;
static unsigned short g_GameText_langReset_storage = 0;
static unsigned short *g_GameText_langReset = &g_GameText_langReset_storage;

void GameText::release() {
    if (this->textTable == nullptr)
        return;
    for (int i = 0; i < this->textCount; ++i) {
        delete this->textTable[i];
        this->textTable[i] = nullptr;
    }
}

int GameText::getLanguage() {
    return *g_GameText_language;
}

static const char gRegionCodeStr[] = "";

String GameText::getRegionCode() {
    return String(gRegionCodeStr);
}

void GameText::setSubstituteArray(int *pairs, unsigned count) {
    if (count != 0) {
        if ((count & 1) != 0) return;
        ArrayRemoveAll<int>(this->substitutes);
    }
    for (; count != 0; --count) {
        int v = *pairs++;
        ArrayAdd(v, this->substitutes);
    }
}

static unsigned short g_langCode_storage = 0;
static unsigned short *g_langCode = &g_langCode_storage;

static const char *gLangPaths[17] = {0};
static const char gLangPathDefault[] = "";
static const char gLangPathEnglish[] = "";

void GameText::setLanguage(short stringCount, int langId) {
    if ((unsigned int) *g_langCode == ((unsigned int) langId & 0xffff))
        return;

    this->release();
    this->textCount = stringCount;
    this->textTable = new String *[stringCount]();

    String path(gLangPathDefault);

    unsigned short code;
    const char *p;
    switch (langId) {
        case 0: p = gLangPaths[0];
            code = 0;
            break;
        case 1: p = gLangPaths[1];
            code = 1;
            break;
        case 2: p = gLangPaths[2];
            code = 2;
            break;
        case 3: p = gLangPaths[3];
            code = 3;
            break;
        case 4: p = gLangPaths[4];
            code = 4;
            break;
        case 5: p = gLangPaths[5];
            code = 5;
            break;
        case 6: p = gLangPaths[6];
            code = 6;
            break;
        case 7: p = gLangPaths[7];
            code = 7;
            break;
        case 8: p = gLangPaths[8];
            code = 8;
            break;
        case 9: p = gLangPaths[9];
            code = 9;
            break;
        case 10: p = gLangPaths[10];
            code = 0;
            break;
        case 11: p = gLangPaths[11];
            code = 0;
            break;
        case 12: p = gLangPaths[12];
            code = 12;
            break;
        case 13: p = gLangPaths[13];
            code = 13;
            break;
        case 14: p = gLangPaths[14];
            code = 14;
            break;
        case 15: p = gLangPaths[15];
            code = 15;
            break;
        default: p = gLangPaths[16];
            code = 0;
            break;
    }
    path = String(p);
    *g_langCode = code;

    if (AEFile::FileExist(path) == 0) {
        path = String(gLangPathEnglish);
        *g_langCode = 0;
    }

    unsigned int file = 0;
    AEFile::OpenRead(path, &file);
    this->ReadLangFile(0, stringCount);
}

GameText::~GameText() {
    this->release();
    delete[] this->textTable;
    this->textTable = nullptr;
}

static unsigned gArabicTable[0x29 * 5] = {0};

int GameText::isNonArabicString(const unsigned short *str, unsigned int count) {
    unsigned short i = 0;
    bool keep;
    do {
        if (count <= i) return 1;
        keep = true;
        unsigned short ch = str[i];
        unsigned *row = gArabicTable;
        for (int r = 0; r != 0x29; row += 5, ++r) {
            for (int c = 0; c != 5; ++c)
                keep = keep & (row[c] != ch);
        }
        ++i;
    } while (keep);
    return 0;
}

static const char gInitLangStr[] = "";

GameText::GameText() {
    { if (this->fallbackText.data) delete[] this->fallbackText.data; this->fallbackText.data = nullptr; this->fallbackText.length = 0; }
    *g_GameText_langReset = 0xffff;
    this->textTable = nullptr;
    this->textCount = 0;
    String tmp(gInitLangStr);
}

static const unsigned int gArabForms[(0x334 / 20) * 5] = {0};
static const unsigned int gLamAlef[10 * 4] = {0};
static const unsigned int gLamAlefForms[0x29 * 5] = {0};

static inline bool isJoiner(unsigned short c) {
    return c >= 0x600 && c != 0x60c && c != 0x61f;
}

String GameText::convertStringFromArabic(String in) {
    String work;
    work.Set((const unsigned short *) (in.index(0)));
    unsigned int len = work.size();

    unsigned short *buf = new unsigned short[len + 1];
    memcpy(buf, work.index(0), (unsigned long) (len * 2 + 2));

    int inLen = (int) in.size();
    unsigned int hi = (unsigned int) (inLen - 1);
    unsigned int i = hi;

    while (true) {
        if (i > 0x7fffffff) {
            String result;
            result.Set((const unsigned short *) (buf));
            delete[] buf;
            return result;
        }

        if (i != 0) {
            unsigned short prev = *in.index(i - 1);
            unsigned short cur = *in.index(i);
            if (prev == 0x644) {
                unsigned short form = 2;
                switch (cur) {
                    case 0x622: form = 5;
                        break;
                    case 0x623: form = 3;
                        break;
                    case 0x627: form = 2;
                        break;
                    default: break;
                }
                buf[i] = form;

                String merged;
                merged.Set((const unsigned short *) (buf));
                String head, tail;
                head = merged.SubString(0, i);
                tail = merged.SubString(i + 1, merged.size());
                head += tail;

                delete[] buf;

                unsigned int mLen = head.size();
                buf = new unsigned short[mLen + 1];
                memcpy(buf, head.index(0), (unsigned long) (mLen * 2 + 2));
                --i;
            }

            unsigned int k = 0;
            for (; k < 10; ++k)
                if (gLamAlef[k * 4] == (unsigned int) prev)
                    break;
            if (k < 10) {
                for (unsigned int r = 0; r < 0x29; ++r) {
                    const unsigned int *row = &gLamAlefForms[r * 5];
                    if (row[0] == (unsigned int) buf[i]) {
                        int form = 1;
                        if ((int) i < (int) hi && isJoiner(buf[i + 1]))
                            form = isJoiner(buf[i + 1]) ? 4 : 1;
                        buf[i] = (unsigned short) row[form];
                        break;
                    }
                }
            }
        } else {
            for (unsigned int r = 0; r < 0x334 / 20; ++r) {
                const unsigned int *row = &gArabForms[r * 5];
                if (row[0] == (unsigned int) buf[0]) {
                    int form;
                    if (inLen == 1 || buf[1] < 0x600) {
                        form = 1;
                    } else {
                        unsigned short nxt = buf[1];
                        form = (nxt == 0x60c || nxt == 0x61f) ? 1 : 4;
                    }
                    buf[0] = (unsigned short) row[form];
                    break;
                }
            }
        }

        --i;
    }
}

void GameText::setLanguage(int langId) {
    this->setLanguage(static_cast<short>(0), langId);
}

static const char gLang5000Primary[] = "";
static const char gLang5000Fallback[] = "";
static const char gLang5001Primary[] = "";
static const char gLang5001Fallback[] = "";

AbyssEngine::String *GameText::getText(int key) {
    if (key == 5000) {
        static String s5000;
        const char *txt = (*g_langCode != 1) ? gLang5000Fallback : gLang5000Primary;
        s5000.ctor_char(txt, false);
        return &s5000;
    }
    if (key == 5001) {
        static String s5001;
        const char *txt = (*g_langCode != 1) ? gLang5001Fallback : gLang5001Primary;
        s5001.ctor_char(txt, false);
        return &s5001;
    }

    uint32_t pairCount = (uint32_t) this->substitutes.size();
    int *pairs = this->substitutes.data();
    for (uint32_t i = 0; i < pairCount; i += 2) {
        if (pairs[i] == key) {
            key = pairs[i + 1];
            break;
        }
    }

    if (key >= 0 && key < this->textCount && this->textTable != nullptr) {
        String *s = this->textTable[key];
        if (s != nullptr)
            return s;
    }
    return &this->fallbackText;
}

void GameText::ReadLangFile(unsigned int file, int count) {
    if (file == 0)
        return;

    short lang = *g_langCode;
    for (int i = 0; i < count; ++i) {
        unsigned short len;
        if (AEFile::Read(2, &len, file) == 0) {
            this->release();
            break;
        }

        unsigned int byteLen = (unsigned int) ((len >> 8) | (len << 8)) & 0xffff;

        char *utf8 = new char[byteLen + 1];
        if (AEFile::Read(byteLen, utf8, file) == 0) {
            delete[] utf8;
            this->release();
            break;
        }
        utf8[byteLen] = '\0';

        unsigned short *wide = String::getWCharFromUtf8(utf8, (int) byteLen);

        String *s = new String;
        if (lang == 9) {
            String tmp;
            tmp.Set((const unsigned short *) (wide));
            *s = this->convertStringFromArabic(tmp);
            this->textTable[i] = s;
        } else {
            s->Set((const unsigned short *) (wide));
            this->textTable[i] = s;
        }

        delete[] wide;
        delete[] utf8;
    }

    AEFile::Close(file);
}

// Static data members present in the original binary (defined for symbol parity).
unsigned short GameText::currentLang;
