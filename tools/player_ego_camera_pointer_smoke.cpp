#include <cassert>
#include <cstdint>

struct TargetFollowCamera {
    float handling = 0.0f;
    float rumble = 0.0f;
    int rumbleDuration = 0;
    void setShipHandling(float value) { handling = value; }
    void setRumblePercentage(float value, int duration) {
        rumble = value;
        rumbleDuration = duration;
    }
};

struct PlayerEgo {
    TargetFollowCamera *targetFollowCamera = nullptr;
    uintptr_t adjacent = 0x12345678u;
    float handling = 3.5f;
    unsigned char hitShake = 0;
    int hitShakeTimer = 4;
    TargetFollowCamera *getTargetFollowCamera();
    void setTargetFollowCamera(TargetFollowCamera *cam);
    void hitCamera();
};

// PRODUCTION_METHODS

int main() {
    PlayerEgo ego;
    TargetFollowCamera camera;
    ego.setTargetFollowCamera(&camera);
    assert(ego.getTargetFollowCamera() == &camera);
    assert(ego.adjacent == 0x12345678u && camera.handling == 3.5f);
    ego.hitCamera();
    assert(ego.hitShake == 1 && ego.hitShakeTimer == 0);
    assert(camera.rumble == 1.0f && camera.rumbleDuration == 250);
}
