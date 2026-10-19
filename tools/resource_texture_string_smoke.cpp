// Link the real String.cpp and ResourceTexture.cpp; no String_GetAEChar shim.
#include "game/core/String.h"
#include "engine/core/GameText.h"
#include "engine/render/ResourceTexture.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>

// Constructors below pass false. Language-dependent Reverse is outside this test.
int GameText::getLanguage() { std::abort(); }
// Unexecuted dependencies of other String methods retained by the Windows linker.
void String_printImpl(const char *) { std::abort(); }
namespace AbyssEngine {
unsigned short *computeFloatString(float, int, int *, int *, int) { std::abort(); }
String operator+(const String &, const String &) { std::abort(); }
}

struct Allocation { void *pointer; size_t size; unsigned frees; };
static Allocation allocations[4];
static unsigned allocationCount;
static bool capture;
void *operator new[](size_t size) {
    void *pointer = std::malloc(size);
    if (!pointer) throw std::bad_alloc();
    if (capture) {
        assert(allocationCount < 4);
        allocations[allocationCount++] = {pointer, size, 0};
    }
    return pointer;
}
void operator delete[](void *pointer) noexcept {
    for (unsigned i = 0; i < allocationCount; ++i)
        if (pointer == allocations[i].pointer) assert(++allocations[i].frees == 1);
    std::free(pointer);
}
void operator delete[](void *pointer, size_t) noexcept { ::operator delete[](pointer); }

static void check(String &source, const unsigned char *expected, unsigned size, uint32_t bits) {
    assert(size == static_cast<unsigned>(source.length) + 1);
    auto *originalData = source.data;
    const int originalLength = source.length;
    char *conversion = source.GetAEChar();
    assert(std::memcmp(conversion, expected, size) == 0);
    delete[] conversion;

    float value;
    std::memcpy(&value, &bits, sizeof(value));
    allocationCount = 0;
    capture = true;
    {
        AbyssEngine::ResourceTexture texture(source, value);
        capture = false;
        assert(allocationCount == 2);
        assert(allocations[0].size == size && allocations[1].size == size);
        assert(allocations[0].pointer != allocations[1].pointer);
        assert(allocations[0].frees == 1 && allocations[1].frees == 0);
        assert(texture.name == allocations[1].pointer);
        assert(std::memcmp(texture.name, expected, size) == 0);
        uint32_t stored;
        std::memcpy(&stored, &texture.value, sizeof(stored));
        assert(stored == bits);
        assert(source.data == originalData && source.length == originalLength);
        if (source.length) {
            unsigned short saved = source.data[0];
            source.data[0] ^= 0xff;
            assert(std::memcmp(texture.name, expected, size) == 0);
            source.data[0] = saved;
        }
    }
    assert(allocations[0].frees == 1 && allocations[1].frees == 1);
    allocationCount = 0;
}

int main() {
    static_assert(sizeof(float) == sizeof(uint32_t));
    const unsigned char zero[] = {0};
    String empty;
    assert(empty.data == nullptr && empty.length == 0);
    check(empty, zero, sizeof(zero), 0);

    String ascii("data/x.aei", false);
    const unsigned char asciiBytes[] = "data/x.aei";
    check(ascii, asciiBytes, sizeof(asciiBytes), 0x3fa00000); // 1.25f

    String embedded("A", false);
    embedded += char(0);
    embedded += char('B');
    const unsigned char embeddedBytes[] = {'A', 0, 'B', 0};
    check(embedded, embeddedBytes, sizeof(embeddedBytes), 0x80000000); // -0

    const unsigned short units[] = {'A', 0x0100, 0x20ac, 0x00e9, 0xd83d, 0xde80, 'Z', 0};
    String nonAscii(units, false);
    const unsigned char lowBytes[] = {'A', 0, 0xac, 0xe9, 0x3d, 0x80, 'Z', 0};
    check(nonAscii, lowBytes, sizeof(lowBytes), 0x7fc12345); // quiet NaN payload

    // Raw stored-state probes, not claims that these are canonical text strings:
    // ARM copies data[length]'s low byte only in the nonempty branch.
    String terminal("Q", false);
    terminal.data[1] = 0x12ef;
    const unsigned char terminalBytes[] = {'Q', 0xef};
    check(terminal, terminalBytes, sizeof(terminalBytes), 0xff800000); // -infinity
    String zeroLength("X", false);
    zeroLength.length = 0;
    zeroLength.data[0] = 0x12ef;
    check(zeroLength, zero, sizeof(zero), 0x3f800000);

    {
        AbyssEngine::ResourceTexture control("control.aei", 2.0f);
        assert(std::strcmp(control.name, "control.aei") == 0 && control.value == 2.0f);
    }
    std::puts("Real String/ResourceTexture link: six length/low-byte cases, allocation/copy/ownership/value OK");
    std::puts("No String_GetAEChar shim, GPU service or source-extracted replacement supplied");
}
