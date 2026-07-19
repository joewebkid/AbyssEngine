#ifndef GOF2_IMAGEPART_H
#define GOF2_IMAGEPART_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
class ImagePart {
public:
    int id;
    int f_4;
    int pos_y;
    int scale_x;
    int scale_y;

    ImagePart(unsigned id, int field04, int posY);

    ~ImagePart();

    void draw(int x, int y, bool b);
};
#endif
