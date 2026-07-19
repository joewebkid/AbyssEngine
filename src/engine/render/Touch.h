#ifndef GOF2_TOUCH_H
#define GOF2_TOUCH_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/render/AEGeometry.h"
#include "engine/math/AEMath.h"
#include "engine/math/Vector.h"

struct Touch {
    int x;
    int y;
    int id;
    int action;
};
#endif
