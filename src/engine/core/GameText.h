#ifndef GOF2_GAMETEXT_H
#define GOF2_GAMETEXT_H
#include "engine/core/Array.h"
#include "AEString.h"
#include "game/core/String.h"

class GameText {
public:
    Array<int> substitutes;

    AbyssEngine::String **textTable;

    AbyssEngine::String fallbackText;
    int textCount;

    GameText();

    ~GameText();

    static int getLanguage();

    static AbyssEngine::String getRegionCode();

    static int isNonArabicString(const unsigned short *str, unsigned int count);

    AbyssEngine::String convertStringFromArabic(AbyssEngine::String in);

    void ReadLangFile(unsigned int file, int count);

    AbyssEngine::String *getText(int key);

    void release();

    void setLanguage(int langId);

    void setSubstituteArray(int *pairs, unsigned count);

    void setLanguage(short stringCount, int langId);

    inline void setLanguage_i(int langId) { setLanguage(langId); }

    static GameText *gGameText;

    // Static data members present in the original binary (defined for symbol parity).
    static unsigned short currentLang;
};

#endif
