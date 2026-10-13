#include "engine/core/Array.h"
#include <cassert>
#include <cstdio>

namespace AbyssEngine {
struct KeyFrame {
    static int destroyed;
    ~KeyFrame() { ++destroyed; }
};
int KeyFrame::destroyed = 0;

struct Transform {
    Array<KeyFrame *> keyFrames;
    bool keyFramesShared = false;
    ~Transform();
};

// PRODUCTION_DESTRUCTOR
}

int main() {
    using namespace AbyssEngine;
    {
        Transform owned;
        owned.keyFrames.data_[0] = new KeyFrame;
        owned.keyFrames.size_ = 1;
    }
    assert(KeyFrame::destroyed == 1);

    KeyFrame *sharedFrame = new KeyFrame;
    {
        Transform shared;
        shared.keyFramesShared = true;
        shared.keyFrames.data_[0] = sharedFrame;
        shared.keyFrames.size_ = 1;
    }
    assert(KeyFrame::destroyed == 1);
    delete sharedFrame;
    assert(KeyFrame::destroyed == 2);

    { Transform empty; }
    assert(KeyFrame::destroyed == 2);
    std::puts("Transform release ownership smoke: OK");
}
