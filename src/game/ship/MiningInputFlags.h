#ifndef GOF2_MININGINPUTFLAGS_H
#define GOF2_MININGINPUTFLAGS_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "TargetFollowCamera.h"
#include "engine/math/Vector.h"
#include "engine/math/Matrix.h"
#include "engine/render/AEGeometry.h"
#include "game/weapons/Radar.h"
#include "game/weapons/RepairBeam.h"
#include "engine/math/AEMath.h"
#include "game/ship/ExplosionEmitterHolder.h"

struct MiningInputFlags {
    uint8_t field_0x00[0x10];
    uint8_t invertAxisFlag;
};
#endif
