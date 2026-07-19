#ifndef GOF2_KEYFRAME_H
#define GOF2_KEYFRAME_H

#include "engine/core/Array.h"
#include "engine/math/Vector.h"

#include "engine/math/AEMath.h"

#include <cstdint>

namespace AbyssEngine {
    class KeyFrame {
    public:
        AEMath::Vector translation;
        AEMath::Vector scale;
        AEMath::Vector rotation;
        AEMath::Vector localTranslation;
        AEMath::Vector localScale;
        AEMath::Vector localRotation;
        float alpha;
        uint32_t field_0x4c;
        union {
            int64_t timestamp;             // 0x50 (spans timestampLo/timestampHi)
            struct {
                uint32_t timestampLo;      // 0x50
                uint32_t timestampHi;      // 0x54
            };
        };
        uint32_t channelFlags;             // 0x58 (channel-present bitmask, low dword)
        uint32_t channelFlagsHi;           // 0x5c (channel-present bitmask, high dword)

        KeyFrame();
    };

#if defined(GOF2_MATCH) && __SIZEOF_POINTER__ == 4
    static_assert(__builtin_offsetof(KeyFrame, translation) == 0x00, "KeyFrame::translation");
    static_assert(__builtin_offsetof(KeyFrame, scale) == 0x0c, "KeyFrame::scale");
    static_assert(__builtin_offsetof(KeyFrame, rotation) == 0x18, "KeyFrame::rotation");
    static_assert(__builtin_offsetof(KeyFrame, localTranslation) == 0x24, "KeyFrame::localTranslation");
    static_assert(__builtin_offsetof(KeyFrame, localScale) == 0x30, "KeyFrame::localScale");
    static_assert(__builtin_offsetof(KeyFrame, localRotation) == 0x3c, "KeyFrame::localRotation");
    static_assert(__builtin_offsetof(KeyFrame, alpha) == 0x48, "KeyFrame::alpha");
    static_assert(__builtin_offsetof(KeyFrame, timestamp) == 0x50, "KeyFrame::timestamp");
    static_assert(__builtin_offsetof(KeyFrame, channelFlags) == 0x58, "KeyFrame::channelFlags");
    static_assert(__builtin_offsetof(KeyFrame, channelFlagsHi) == 0x5c, "KeyFrame::channelFlagsHi");
#endif
}

#endif
