#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>

struct Globals {
    static int mouseCursorActivated;
    static unsigned char options[100];
};
int Globals::mouseCursorActivated = 0;
unsigned char Globals::options[100]{};
struct PlayerEgo { int pitchAccumDir = 1; };

// PRODUCTION_HELPERS

static bool near(float a, float b) { return std::fabs(a - b) < 0.0001f; }
static void setSensitivity(unsigned offset, float value) {
    std::memcpy(Globals::options + offset, &value, sizeof(value));
}

int main() {
    PlayerEgo ego;
    Globals::mouseCursorActivated = 1;
    assert(near(PE_pitchRampDelta(&ego, 12.0f, 16), 16.0f));
    assert(near(PE_yawRampDelta(12.0f, 16), 50.0f));
    assert(near(PE_yawRampDelta(100.0f, 16), 280.0f));

    Globals::mouseCursorActivated = 0;
    Globals::options[0x11] = 1;
    setSensitivity(0x14, 1.0f);
    setSensitivity(0x18, 0.5f);
    assert(near(PE_yawRampDelta(4.0f, 20), 80.0f / 46.0f));
    assert(near(PE_pitchRampDelta(&ego, 4.0f, 20), 80.0f / 46.0f));

    Globals::options[0x11] = 0;
    assert(near(PE_yawRampDelta(4.0f, 20), 80.0f / 56.0f));
    ego.pitchAccumDir = -1;
    assert(near(PE_pitchRampDelta(&ego, 4.0f, 20), 80.0f / 51.5f));
    ego.pitchAccumDir = 1;
    assert(near(PE_pitchRampDelta(&ego, 4.0f, 20), 80.0f / 53.5f));
    std::puts("PlayerEgo axis ramp smoke: OK");
}
