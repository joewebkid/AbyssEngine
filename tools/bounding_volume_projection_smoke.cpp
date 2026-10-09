#include <cassert>

#include "engine/math/BoundingVolume.h"

// Link-only stubs for a non-exercised BoundingVolume helper in this focused smoke.
namespace AbyssEngine::AEMath {
Vector operator-(const Vector &a, const Vector &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector VectorNormalize(const Vector &v) { return v; }
}

class ProbeVolume : public BoundingVolume {
public:
    int checks = 0;
    int projections = 0;

    ProbeVolume() : BoundingVolume(0, 0, 0, 0, 0, 0) {}

    int outerCollide(float, float, float) override {
        ++checks;
        return 1;
    }

    Vector projectCollisionOnSurface(const Vector &point) override {
        ++projections;
        return {point.x + 1, point.y + 2, point.z + 3};
    }
};

int main() {
    const Vector input{1, 2, 3};
    const Vector copied = BoundingVolume::staticProjectCollisionOnSurface(input, nullptr);
    assert(copied.x == 1 && copied.y == 2 && copied.z == 3);

    ProbeVolume probe;
    Array<BoundingVolume *> volumes;
    volumes.data_[0] = &probe;
    volumes.size_ = 1;
    const Vector projected = BoundingVolume::staticProjectCollisionOnSurface(input, &volumes);
    assert(projected.x == 3 && projected.y == 6 && projected.z == 9);
    assert(input.x == 1 && input.y == 2 && input.z == 3);
    assert(probe.checks == 2 && probe.projections == 2);
}
