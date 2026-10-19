#include "engine/core/Array.h"
#include "engine/core/IApplicationModule.h"
#include "engine/core/AEString.h"
#include "engine/core/GameText.h"
#include "engine/math/Vector.h"
#include "game/weapons/AbstractGun.h"

#include <cstdio>
#include <cstdlib>
#include <limits>

// String.cpp contains unrelated methods that this isolated target does not call.
// Fail loudly if a new test path reaches one of their external dependencies.
void String_printImpl(const char *) { std::abort(); }
int GameText::getLanguage() { std::abort(); }
namespace AbyssEngine {
#include "engine/core/FloatStringImpl.inc"
String operator+(const String &a, const String &b) {
    String result(a);
    result += b;
    return result;
}
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

bool matchesAscii(const AbyssEngine::String &value, const char *expected) {
    int i = 0;
    while (expected[i] != 0) {
        if (i >= value.length || value.data[i] != (unsigned char) expected[i])
            return false;
        ++i;
    }
    return i == value.length;
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
        AbyssEngine::String emptySlice = text.SubString(4, 4);
        check(emptySlice.length == 0 && emptySlice.data == nullptr,
              "String empty substring");
        text.data[2] = 0;
        AbyssEngine::String embeddedZeroSlice = text.SubString(1, 4);
        check(embeddedZeroSlice.length == 1 && embeddedZeroSlice.data[0] == 'l',
              "String substring copy-constructor truncation");
        AbyssEngine::String leadingZeroSlice = text.SubString(2, 4);
        check(leadingZeroSlice.length == 0 && leadingZeroSlice.data != nullptr &&
                  leadingZeroSlice.data[0] == 0,
              "String leading-zero substring ownership");
        text.data[2] = 'p';
        AbyssEngine::String emptyChar;
        emptyChar.Set("");
        check(emptyChar.length == 0 && emptyChar.data != nullptr && emptyChar.data[0] == 0,
              "String empty char allocation");
        emptyChar.Set(static_cast<const char *>(nullptr));
        check(emptyChar.length == 0 && emptyChar.data != nullptr && emptyChar.data[0] == 0,
              "String null char allocation");
        const unsigned short emptyWide[] = {0};
        AbyssEngine::String emptyUtf16;
        emptyUtf16.Set(emptyWide);
        check(emptyUtf16.length == 0 && emptyUtf16.data != nullptr && emptyUtf16.data[0] == 0,
              "String empty UTF-16 allocation");
        emptyUtf16.Set(static_cast<const unsigned short *>(nullptr));
        check(emptyUtf16.length == 0 && emptyUtf16.data != nullptr && emptyUtf16.data[0] == 0,
              "String null UTF-16 allocation");
        copy.Set("beta");
        check(text.length == 5 && text.data[0] == 'a', "String independent copy");
        AbyssEngine::String assignedText;
        assignedText = copy;
        check(assignedText.length == 4 && assignedText.data[0] == 'b',
              "String assignment");
        AbyssEngine::String appendSource("abc");
        appendSource.data[1] = 0;
        AbyssEngine::String appended("xy");
        appended += appendSource;
        check(appended.length == 3 && appended.data[0] == 'x' &&
                  appended.data[1] == 'y' && appended.data[2] == 'a',
              "String append copies zero-terminated source");
        appended += appended;
        check(appended.length == 6 && appended.data[3] == 'x' &&
                  appended.data[5] == 'a', "String self append");
        AbyssEngine::String medalValue(1234);
        check(medalValue.length == 4 && medalValue.data[0] == '1' &&
                  medalValue.data[3] == '4', "String integer value");
        AbyssEngine::String minimumNarrowInt(std::numeric_limits<int>::min());
        check(matchesAscii(minimumNarrowInt, "-2147483648"),
              "String signed int minimum");
        AbyssEngine::String highByte((char) 0xff);
        check(matchesAscii(highByte, "255"), "String unsigned char value");
        AbyssEngine::String negativeInt(-1234LL);
        AbyssEngine::String minimumInt(std::numeric_limits<long long>::min());
        AbyssEngine::String maximumInt(std::numeric_limits<long long>::max());
        AbyssEngine::String zeroInt(0LL);
        check(matchesAscii(negativeInt, "-1234"), "String negative integer");
        check(matchesAscii(minimumInt, "-9223372036854775808"), "String minimum integer");
        check(matchesAscii(maximumInt, "9223372036854775807"), "String maximum integer");
        check(zeroInt.length == 0 && zeroInt.data != nullptr && zeroInt.data[0] == 0,
              "String zero integer");
        AbyssEngine::String reusedInt("old");
        reusedInt.Set(-42LL);
        check(matchesAscii(reusedInt, "-42"), "String replace integer");
        AbyssEngine::String appendedInt("n=");
        appendedInt += std::numeric_limits<long long>::min();
        check(matchesAscii(appendedInt, "n=-9223372036854775808"),
              "String append minimum integer");
        check(appendedInt.data[appendedInt.length] == 0,
              "String append integer terminator");
        AbyssEngine::String appendedToEmpty;
        appendedToEmpty += 123LL;
        check(matchesAscii(appendedToEmpty, "123") &&
                  appendedToEmpty.data[appendedToEmpty.length] == 0,
              "String append integer to empty");
        AbyssEngine::String appendedZero("keep");
        AbyssEngine::String *zeroResult = &(appendedZero += 0LL);
        check(zeroResult == &appendedZero && matchesAscii(appendedZero, "keep"),
              "String append zero returns self without changing text");
        AbyssEngine::String appendedNarrowInt("i=");
        appendedNarrowInt += std::numeric_limits<int>::min();
        check(matchesAscii(appendedNarrowInt, "i=-2147483648"),
              "String append signed int minimum");
        AbyssEngine::String positiveFloat(1.5f);
        AbyssEngine::String negativeFloat(-1.5f);
        AbyssEngine::String fractionalFloat(0.25f);
        AbyssEngine::String smallFloat(0.025f);
        AbyssEngine::String zeroFloat(0.0f);
        check(matchesAscii(positiveFloat, "1.500000000"), "String positive float");
        check(matchesAscii(negativeFloat, "-1.500000000"), "String negative float");
        check(matchesAscii(fractionalFloat, "0.2500000000"), "String fractional float");
        check(matchesAscii(smallFloat, "0.02500000000"), "String small float");
        check(matchesAscii(zeroFloat, "0.0000000000"), "String zero float");
        AbyssEngine::String appendedFloat("value=");
        appendedFloat += 1.5f;
        check(matchesAscii(appendedFloat, "value=1.500000000"),
              "String append float");

        Array<AbyssEngine::String *> owned;
        auto *member = new AbyssEngine::String;
        member->Set("owned");
        ArrayAdd(member, owned);
        ArrayReleaseClasses(owned);
        check(owned.data() == nullptr, "Array<String*> element release");
    }

    int exponent = 0;
    int negative = 0;
    unsigned short *digits = AbyssEngine::computeFloatString(1.5f, 2,
                                                            &exponent, &negative, 0);
    check(digits[0] == '1' && digits[1] == '5' && digits[2] == '0' &&
              digits[3] == 0 && exponent == 1 && negative == 0,
          "float digit helper fixed mode");
    delete[] digits;

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
