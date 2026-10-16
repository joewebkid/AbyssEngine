#include "engine/math/AEMath.h"
#include "engine/math/Vector.h"

int main() {
    AbyssEngine::AEMath::Vector value{2.0f, 3.0f, 4.0f};
    const AbyssEngine::AEMath::Vector rhs{5.0f, 6.0f, 7.0f};
    value *= rhs;
    value -= rhs;
    value /= rhs;
    AbyssEngine::AEMath::Vector copy{0.0f, 0.0f, 0.0f};
    copy = value;
    if (copy.x != 1.0f || copy.y != 2.0f || copy.z != 3.0f) return 1;

    AbyssEngine::AEMath::Matrix matrix;
    const float identity[15] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1};
    for (int i = 0; i < 15; ++i) {
        if (matrix.m[i] != identity[i]) return 2;
    }

    const AbyssEngine::AEMath::Matrix left(1, 0, 0, 10, 0, 1, 0, 20,
                                            0, 0, 1, 0, 1, 1, 1);
    const AbyssEngine::AEMath::Matrix right(2, 0, 0, 5, 0, 3, 0, 7,
                                             0, 0, 1, 0, 2, 3, 4);
    const AbyssEngine::AEMath::Matrix product = AbyssEngine::AEMath::operator*(left, right);
    const float expected[15] = {2, 0, 0, 15, 0, 3, 0, 27,
                                0, 0, 1, 0, 2, 3, 4};
    for (int i = 0; i < 15; ++i) {
        if (product.m[i] != expected[i]) return 3;
    }

    AbyssEngine::AEMath::Matrix zero(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    matrix = zero;
    matrix *= zero;
    for (float element : matrix.m) {
        if (element != 0.0f) return 4;
    }

    const AbyssEngine::AEMath::Vector direction{2.0f, 0.0f, 0.0f};
    const AbyssEngine::AEMath::Vector up{0.0f, 3.0f, 0.0f};
    const auto facingRight = AbyssEngine::AEMath::VectorNormalize(
        AbyssEngine::AEMath::VectorCross(up, direction));
    const auto facingUp = AbyssEngine::AEMath::VectorNormalize(
        AbyssEngine::AEMath::VectorCross(direction, facingRight));
    if (facingRight.x != 0.0f || facingRight.y != 0.0f || facingRight.z != -1.0f ||
        facingUp.x != 0.0f || facingUp.y != 1.0f || facingUp.z != 0.0f) return 5;

    const auto fallback = AbyssEngine::AEMath::VectorNormalize({0.0f, 0.0f, 0.0f});
    if (fallback.x != 0.0f || fallback.y != 1.0f || fallback.z != 0.0f) return 6;

    AbyssEngine::AEMath::Matrix scaled;
    scaled.m[3] = 11.0f;
    scaled.m[7] = 22.0f;
    scaled.m[11] = 33.0f;
    AbyssEngine::AEMath::MatrixSetScaling(scaled, 2.0f, 3.0f, 4.0f);
    if (scaled.m[0] != 2.0f || scaled.m[5] != 3.0f || scaled.m[10] != 4.0f ||
        scaled.m[3] != 11.0f || scaled.m[7] != 22.0f || scaled.m[11] != 33.0f) return 7;
    return 0;
}
