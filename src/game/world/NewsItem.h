#ifndef GOF2_NEWSITEM_H
#define GOF2_NEWSITEM_H
#include <cstdint>

class NewsItem {
public:
    int id;
    uint8_t flag;
    void *data;
    int length;

    union {
        int minLevel;
        int field_0x10;
    };

    union {
        int maxLevel;
        int field_0x14;
    };

    union {
        uint8_t used;
        uint8_t field_0x18;
    };

    NewsItem(int id, bool flag, bool *data, int length, int field_0x10, int field_0x14);

    ~NewsItem();

    NewsItem *clone();
};
#endif
