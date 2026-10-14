#include <cassert>
#include <cstdint>

namespace AbyssEngine {
struct Material { int pad[8] = {}; int materialMode = 0; };
struct Mesh { void *materialBlock = nullptr; };
struct Transform {
    struct Vector { float x, y, z; } boundingCenter;
    float boundingRadius = 0;
};
}
using Vector = AbyssEngine::Transform::Vector;

struct PaintCanvas {
    static PaintCanvas *gCanvas;
    AbyssEngine::Transform transform;
    unsigned clonedMesh = 0;
    unsigned cloneCount = 0;
    AbyssEngine::Mesh mesh;
    AbyssEngine::Mesh *MeshGetPointer(unsigned handle) {
        assert(handle == 42);
        return &mesh;
    }
    AbyssEngine::Transform *TransformGetTransform(unsigned handle) {
        assert(handle == 7);
        return &transform;
    }
    void MeshCloneMaterial(unsigned mesh, uint32_t &out) {
        clonedMesh = mesh;
        cloneCount++;
        out = 321;
    }
};
PaintCanvas *PaintCanvas::gCanvas = nullptr;
struct Globals { static PaintCanvas *Canvas; };
PaintCanvas *Globals::Canvas = nullptr;

struct AEGeometry {
    static int constructed;
    unsigned transform = 7, meshId = 42;
    AEGeometry(uint16_t resource, PaintCanvas *, bool enabled) {
        assert(resource == 0x3826 && !enabled);
        constructed++;
    }
    AEGeometry() = default;
};
int AEGeometry::constructed = 0;
struct Item {
    int value = 0, queried = -1;
    int getAttribute(int attribute) { queried = attribute; return value; }
};
struct Ship {
    Item *emergency = nullptr;
    bool hasSystem = false;
    Item *getFirstEquipmentOfSort(int sort) { assert(sort == 27); return emergency; }
    bool hasEmergencySystem() { return hasSystem; }
};
struct Status { Ship *ship; Ship *getShip() { return ship; } };
Status *currentStatus = nullptr;
Status *PE_status() { return currentStatus; }
void Vec_assign(void *out, const void *in) {
    *static_cast<Vector *>(out) = *static_cast<const Vector *>(in);
}
static const float g_PE_ss_emDiv = 500.0f;
static const float g_PE_ss_emBias = 0.1f;

struct PlayerEgo {
    AEGeometry *field_0x4 = nullptr;
    void *field_0xac = nullptr;
    int emergencyVal1 = 0;
    Vector emergencyVec = {};
    float emergencyVal2 = 0;
    void *cloak = nullptr;
    uint32_t cloakMaterial1 = 0;
    int originalMaterialMode = -1;
    void testMaterialMode();
    void testTail();
};
void PlayerEgo::testMaterialMode() {
    AEGeometry *grp = field_0x4;
// PRODUCTION_MATERIAL_MODE
}
void PlayerEgo::testTail() {
// PRODUCTION_TAIL
}

int main() {
    PaintCanvas canvas;
    PaintCanvas::gCanvas = &canvas;
    Globals::Canvas = &canvas;
    canvas.transform.boundingCenter = {1, 2, 3};
    canvas.transform.boundingRadius = 1000;
    Ship ship;
    Status status{&ship};
    currentStatus = &status;
    AEGeometry hull;
    PlayerEgo ego;
    ego.field_0x4 = &hull;
    AbyssEngine::Material originalMaterial;
    originalMaterial.materialMode = 7;
    canvas.mesh.materialBlock = &originalMaterial;
    ego.testMaterialMode();
    assert(ego.originalMaterialMode == 7);

    ego.testTail();
    assert(AEGeometry::constructed == 0 && canvas.cloneCount == 0);

    Item emergency;
    emergency.value = 13;
    ship.emergency = &emergency;
    ship.hasSystem = true;
    ego.cloak = &ego;
    ego.testTail();
    assert(AEGeometry::constructed == 1 && ego.field_0xac != nullptr);
    assert(emergency.queried == 41 && ego.emergencyVal1 == 13);
    assert(ego.emergencyVec.x == 1 && ego.emergencyVec.y == 2 && ego.emergencyVec.z == 3);
    assert(ego.emergencyVal2 > 2.09f && ego.emergencyVal2 < 2.11f);
    assert(canvas.cloneCount == 1 && canvas.clonedMesh == 42 && ego.cloakMaterial1 == 321);

    ship.hasSystem = false;
    ego.field_0xac = nullptr;
    ego.cloak = nullptr;
    ego.testTail();
    assert(ego.field_0xac == nullptr && canvas.cloneCount == 1);
}
