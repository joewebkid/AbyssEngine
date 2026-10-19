#include "game/core/String.h"
#include "engine/core/GameText.h"
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>

// Only unexecuted language/print services are traps. Formatting is production.
int GameText::getLanguage() { std::abort(); }
void String_printImpl(const char *) { std::abort(); }
namespace AbyssEngine {
#include "string_float_concat.inc"
}
namespace ProductionDigits {
#include "engine/core/FloatStringImpl.inc"
}

struct Allocation { void *pointer; size_t size; bool live; };
static Allocation allocations[2048];
static unsigned allocationCount, sequence, digitDelete, oldDelete, helperCalls;
static bool capture, scripted;
static void *oldData;
static int digitAllocation = -1, scriptExponent, scriptNegative;
static const char *scriptDigits;
static uint32_t inputBits;

void *operator new[](size_t size) {
    void *pointer = std::malloc(size);
    if (!pointer) throw std::bad_alloc();
    if (capture) {
        assert(allocationCount < 2048);
        allocations[allocationCount++] = {pointer, size, true};
    }
    return pointer;
}
void operator delete[](void *pointer) noexcept {
    if (capture && pointer) {
        ++sequence;
        bool tracked = false;
        for (unsigned i = allocationCount; i != 0; --i) {
            auto &entry = allocations[i - 1];
            if (entry.live && entry.pointer == pointer) {
                entry.live = false;
                if (int(i - 1) == digitAllocation) { assert(!digitDelete); digitDelete = sequence; }
                tracked = true;
                break;
            }
        }
        if (!tracked) { assert(pointer == oldData && !oldDelete); oldDelete = sequence; }
    }
    std::free(pointer);
}
void operator delete[](void *pointer, size_t) noexcept { ::operator delete[](pointer); }

namespace AbyssEngine {
unsigned short *computeFloatString(float value, int precision, int *exponent, int *negative, int mode) {
    uint32_t received;
    std::memcpy(&received, &value, sizeof(received));
    assert(received == inputBits && precision == 10 && mode == 1);
    assert(exponent && negative && exponent != negative && capture);
    ++helperCalls;
    unsigned short *digits;
    if (scripted) {
        *exponent = scriptExponent;
        *negative = scriptNegative;
        size_t length = std::strlen(scriptDigits);
        digits = new unsigned short[length + 1];
        for (size_t i = 0; i <= length; ++i) digits[i] = static_cast<unsigned char>(scriptDigits[i]);
    } else {
        // Nonfinite/out-of-int-range conversion is not a portable helper contract.
        assert(std::isfinite(value) && std::fabs(double(value)) < 2147483648.0);
        digits = ProductionDigits::computeFloatString(value, precision, exponent, negative, mode);
    }
    for (unsigned i = 0; i < allocationCount; ++i)
        if (allocations[i].live && allocations[i].pointer == digits) digitAllocation = int(i);
    assert(digitAllocation >= 0);
    return digits;
}
}

static unsigned liveCount() {
    unsigned count = 0;
    for (unsigned i = 0; i < allocationCount; ++i) count += allocations[i].live;
    return count;
}
static void begin(float value, void *old) {
    assert(!capture);
    std::memcpy(&inputBits, &value, sizeof(inputBits));
    allocationCount = sequence = digitDelete = oldDelete = helperCalls = 0;
    digitAllocation = -1;
    oldData = old;
    capture = true;
}
static void match(const String &text, const char *expected) {
    assert(text.data && text.length == int(std::strlen(expected)));
    for (int i = 0; i <= text.length; ++i)
        assert(text.data[i] == static_cast<unsigned char>(expected[i]));
    assert(helperCalls == 1 && digitDelete && liveCount() == 1);
}
static void check(float value, const char *expected) {
    {
        String destination("old allocation", false);
        begin(value, destination.data);
        destination.Set(value);
        match(destination, expected);
        assert(oldDelete > digitDelete);
    }
    assert(liveCount() == 0);
    capture = false;
    begin(value, nullptr);
    {
        String constructed(value);
        match(constructed, expected);
        assert(!oldDelete);
    }
    assert(liveCount() == 0);
    capture = false;
}

int main() {
    // Exact simple values establish the real helper route, not a decimal oracle
    // for every floating-point rounding boundary.
    check(0.0f, "0.0000000000");
    check(-0.0f, "0.0000000000");
    check(1.5f, "1.500000000");
    check(-1.5f, "-1.500000000");
    check(0.25f, "0.2500000000");
    check(0.025f, "0.02500000000");
    check(10.0f, "10.00000000");
    check(1000000.0f, "1000000.000");

    scripted = true;
    scriptDigits = "1234567890";
    scriptNegative = 0;
    scriptExponent = 0;
    check(0.125f, "0.1234567890");
    scriptExponent = -3;
    check(std::numeric_limits<float>::denorm_min(), "0.0001234567890");
    scriptExponent = 1;
    scriptNegative = -1; // ARM tests nonzero, not equality to one.
    check(-0.125f, "-1.234567890");
    scriptExponent = 10;
    scriptNegative = 0;
    check(2147483520.0f, "1234567890.");
    // Only bit-preserving call/assembly and ownership are tested for these values.
    scriptExponent = 1;
    check(std::numeric_limits<float>::infinity(), "1.234567890");
    scriptNegative = 1;
    check(-std::numeric_limits<float>::infinity(), "-1.234567890");
    scriptNegative = 0;
    check(std::numeric_limits<float>::quiet_NaN(), "1.234567890");
    check(std::numeric_limits<float>::max(), "1.234567890");
    std::puts("String float - real String bodies + production helper: eight finite format cases OK");
    std::puts("Eight controlled helper cases: input bits/mode/precision, exponent/sign assembly, ownership OK");
    std::puts("NaN/infinity/out-of-int-range numerical conversion remains unverified; no GPU/game claim");
}
