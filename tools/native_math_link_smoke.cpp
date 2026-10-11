#include "engine/math/Matrix.h"
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

    AbyssEngine::AEMath::Matrix zero(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    AbyssEngine::AEMath::Matrix matrix;
    matrix = zero;
    matrix *= zero;
    for (float element : matrix.m) {
        if (element != 0.0f) return 2;
    }
    return 0;
}
