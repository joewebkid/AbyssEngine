#include "engine/core/Array.h"
#include "engine/core/IApplicationModule.h"
#include "engine/core/AEString.h"
#include "engine/core/GameText.h"
#include "engine/math/Vector.h"
#include "game/weapons/AbstractGun.h"

#include <cstdio>
#include <cstdlib>

// String.cpp contains unrelated methods that this isolated target does not call.
// Fail loudly if a new test path reaches one of their external dependencies.
void String_printImpl(const char *) { std::abort(); }
uint16_t *String_computeFloatString(float, int, int *, int *) { std::abort(); }
int GameText::getLanguage() { std::abort(); }
namespace AbyssEngine {
String operator+(const String &, const String &) { std::abort(); }
}

namespace {
int failures = 0;
int moduleDestructions = 0;
int gunDestructions = 0;

void check(bool ok, const char *message) {
    if (!ok) {
        std::fprintf(stderr, "ABI smoke failed: %s\n", message);
        ++failures;
    }
}

struct ProbeModule : AbyssEngine::IApplicationModule {
    int updateCount = 0;
    int render3DCount = 0;
    int render2DCount = 0;

    ~ProbeModule() override { ++moduleDestructions; }
    int OnInitialize() override { return 17; }
    void OnRelease() override {}
    long long OnKeyPress(long long, long long) override { return 0; }
    long long OnKeyRelease(long long, long long) override { return 0; }
    void OnTouchBegin(int, int) override {}
    void OnTouchMove(int, int) override {}
    void OnTouchEnd(int, int) override {}
    void OnTouchBegin(int, int, void *) override {}
    void OnTouchMove(int, int, void *) override {}
    void OnTouchEnd(int, int, void *) override {}
    void OnUpdate() override { ++updateCount; }
    void OnRender3D() override { ++render3DCount; }
    void OnRender2D() override { ++render2DCount; }
    void OnSuspend() override {}
    void OnResume() override {}
    int ShowLoadingScreen() override { return 23; }
};

struct ProbeGun : AbstractGun {
    ~ProbeGun() override { ++gunDestructions; }
    void setEnemies(Array<Player *> *) override {}
    void setEnemy(Player *) override {}
    void update(int) override {}
    void render() override {}
    void translate(const AbyssEngine::AEMath::Vector &) override {}
    void replaceGun(unsigned int, int) override {}
    int isRocketGun() override { return 1; }
    int isBombGun() override { return 0; }
    int isMineGun() override { return 0; }
    int isAutoTurret() override { return 0; }
};

#if defined(__GNUC__)
__attribute__((noinline))
#endif
AbyssEngine::AEMath::Vector vectorReturn(float x, float y, float z) {
    return {x, y, z};
}
}

int main() {
    for (int iteration = 0; iteration < 100; ++iteration) {
        Array<bool> bits;
        for (int i = 0; i < 32; ++i) ArrayAdd((i & 1) != 0, bits);
        Array<bool> copied(bits);
        check(copied.size() == 32 && copied[1] && !copied[2], "Array<bool> grow/copy");
        Array<bool> assigned;
        assigned = copied;
        ArrayRemove(true, assigned);
        check(assigned.size() == 16 && !assigned[0], "Array<bool> assign/remove");

        AbyssEngine::String text;
        text.Set("alpha");
        AbyssEngine::String copy(text);
        AbyssEngine::String slice = text.SubString(1, 4);
        check(slice.length == 3 && slice.data[0] == 'l' && slice.data[2] == 'h',
              "String hidden return");
        copy.Set("beta");
        check(text.length == 5 && text.data[0] == 'a', "String independent copy");
        AbyssEngine::String assignedText;
        assignedText = copy;
        check(assignedText.length == 4 && assignedText.data[0] == 'b',
              "String assignment");

        Array<AbyssEngine::String *> owned;
        auto *member = new AbyssEngine::String;
        member->Set("owned");
        ArrayAdd(member, owned);
        ArrayReleaseClasses(owned);
        check(owned.data() == nullptr, "Array<String*> element release");
    }

    const auto vec = vectorReturn(1.0f, 2.0f, 3.0f);
    check(sizeof(vec) == 12 && vec.x == 1.0f && vec.y == 2.0f && vec.z == 3.0f,
          "Vector by-value return");

    auto *module = new ProbeModule;
    // Itanium ABI address point: two destructor entries, then declared virtual methods.
    auto **slots = *reinterpret_cast<void ***>(module);
    using IntCall = int (*)(void *);
    using VoidCall = void (*)(void *);
    check(reinterpret_cast<IntCall>(slots[2])(module) == 17, "module initialize slot");
    reinterpret_cast<VoidCall>(slots[12])(module);
    reinterpret_cast<VoidCall>(slots[13])(module);
    reinterpret_cast<VoidCall>(slots[14])(module);
    check(module->updateCount == 1 && module->render3DCount == 1 &&
              module->render2DCount == 1, "module update/render slots");
    check(reinterpret_cast<IntCall>(slots[17])(module) == 23, "module loading slot");
    delete static_cast<AbyssEngine::IApplicationModule *>(module);
    check(moduleDestructions == 1, "module virtual destruction");

    AbstractGun *gun = new ProbeGun;
    check(gun->isRocketGun() == 1, "gun virtual dispatch");
    delete gun;
    check(gunDestructions == 1, "gun virtual destruction");

    if (failures) return 1;
    std::puts("GOF2 native ABI/lifetime smoke: OK");
    return 0;
}
