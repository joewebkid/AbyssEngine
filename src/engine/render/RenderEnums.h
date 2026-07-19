#ifndef GOF2_RENDERENUMS_H
#define GOF2_RENDERENUMS_H

namespace AbyssEngine {
    enum FogMode { FogMode_dummy = 0, FogMode_1 = 1, FogMode_linear = 0x2601 };
    enum BlendMode { BlendMode_dummy = 0, BlendMode_1 = 1, BlendMode_2 = 2, BlendMode_8 = 8, BlendMode_0x15 = 0x15 };
    enum ResourceType { ResourceType_dummy };
    enum LandscapeMode : int { LandscapeMode_dummy = 0, LandscapeMode_1 = 1, LandscapeMode_2 = 2, LandscapeMode_3 = 3 };
}

#endif
