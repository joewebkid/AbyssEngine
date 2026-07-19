#ifndef GOF2_GAME_GAMESETTINGS_H
#define GOF2_GAME_GAMESETTINGS_H

#include <cstdint>

struct GameSettings {
    uint8_t field_0x0[0x35];

    union {
        uint8_t blackMarketUnlockedFlag;
        char settingSkipCampaignFlag;
        uint8_t field_0x35;
    };

    union {
        uint8_t hardCoreFlag;
        uint8_t field_0x36;
    };

    char settingSkipIntroFlag;

    uint8_t field_0x38[0x1c];

    int steerAnchorX;
    int fireAnchorX;
};

#endif
