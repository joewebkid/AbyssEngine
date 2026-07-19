#include "engine/core/KeyFrame.h"

namespace AbyssEngine {
    KeyFrame::KeyFrame() {
        const AEMath::Vector one = {1.0f, 1.0f, 1.0f};

        timestamp = 0;
        __builtin_memset(this, 0, 0x48);
        scale = one;
        localScale = one;
        channelFlags = 0;
        channelFlagsHi = 0;
        alpha = 1.0f;
    }
}
