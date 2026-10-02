#include "game/weapons/Gun.h"
#include <cstdio>

// Rotation is an operand-recording test double. This does not test the renderer.
static const Vector *rotationOperand;
static int rotations;
namespace AbyssEngine { namespace AEMath {
Matrix::Matrix() : m{} {}
Vector MatrixRotateVector(const Matrix &, const Vector &vector) {
    rotationOperand = &vector;
    ++rotations;
    return vector;
}
}}
void exerciseGunMount(Gun &, const Matrix &);

int checkGunMountContracts() {
    Gun gun(0, 1, 1, 0, 10, 1, 1.0f, Vector{10,20,30}, Vector{0,0,0});
    Matrix matrix;
    gun.weaponType = ITEM_SORT_LASER;
    gun.field_0xa4_b0 = gun.field_0xa5 = gun.field_0xa6 = gun.field_0xa7 = 0;
    gun.targetDir = Vector{101,102,103};
    gun.positionArray[0] = Vector{0,0,0};
    exerciseGunMount(gun, matrix);
    if (rotations != 1 || rotationOperand != &gun.offset ||
        gun.targetDir.x != 101 || gun.targetDir.y != 102 || gun.targetDir.z != 103 ||
        gun.positionArray[0].x != 10 || gun.positionArray[0].y != 20) {
        std::fputs("Plain mount must rotate offset directly and preserve targetDir\n", stderr);
        return 1;
    }
    rotations = 0;
    gun.field_0xa4_b0 = gun.field_0xa6 = 1;
    exerciseGunMount(gun, matrix);
    if (rotations != 1 || rotationOperand != &gun.targetDir ||
        gun.targetDir.x != -10 || gun.targetDir.y != 20 || gun.field_0xa6 != 0) return 1;
    rotations = 0;
    gun.field_0xa4_b0 = 0;
    gun.field_0xa5 = gun.field_0xa6 = gun.field_0xa7 = 1;
    exerciseGunMount(gun, matrix);
    if (rotations != 1 || rotationOperand != &gun.targetDir ||
        gun.targetDir.x != 10 || gun.targetDir.y != -20 || gun.field_0xa7 != 0) return 1;
    rotations = 0;
    gun.weaponType = ITEM_SORT_SHOCK_BLAST;
    exerciseGunMount(gun, matrix);
    if (rotations != 0) return 1;
    gun.weaponType = ITEM_SORT_LASER;
    gun.offset = Vector{0,0,0};
    exerciseGunMount(gun, matrix);
    if (rotations != 0) return 1;
    std::puts("Gun extracted mount block operand/side-effect smoke: OK");
    return 0;
}
