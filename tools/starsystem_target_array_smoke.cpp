#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <initializer_list>
static bool tracking;
static size_t objectBytes, initialBytes, resizedBytes;
static void *checkedRealloc(void *p, size_t size) {
    if (tracking) resizedBytes = size;
    return std::realloc(p, size);
}
#define realloc checkedRealloc
#include "engine/core/Array.h"
#undef realloc
void *operator new(size_t size) {
    if (tracking) objectBytes = size;
    if (void *p = std::malloc(size)) return p;
    throw std::bad_alloc();
}
void *operator new[](size_t size) {
    if (tracking) initialBytes = size;
    if (void *p = std::malloc(size)) return p;
    throw std::bad_alloc();
}
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
void operator delete(void *p, size_t) noexcept { std::free(p); }
void operator delete[](void *p, size_t) noexcept { std::free(p); }
struct KIPlayer { int marker; };
struct TargetAllocation {
    void *playerTargets = nullptr;
    __attribute__((noinline)) void allocate(unsigned count) {
#include "starsystem_target_array_under_test.inc"
    }
};
int main() {
    static_assert(sizeof(Array<KIPlayer *>) == (sizeof(void *) == 4 ? 12 : 24));
    static_assert(offsetof(Array<KIPlayer *>, data_) == sizeof(void *));
    for (unsigned count : {0u,1u,5u}) {
        TargetAllocation system;
        objectBytes = initialBytes = resizedBytes = 0;
        tracking = true; system.allocate(count); tracking = false;
        auto *targets = static_cast<Array<KIPlayer *> *>(system.playerTargets);
        unsigned capacity = count ? count : 1;
        assert(objectBytes == sizeof(*targets) && initialBytes == sizeof(KIPlayer *));
        assert((resizedBytes ? resizedBytes : initialBytes) == capacity * sizeof(KIPlayer *));
        assert(targets->size() == count && targets->capacity_ == capacity);
        for (unsigned i = 0; i < capacity; ++i) assert(!targets->data()[i]);
        KIPlayer first{1}, last{2};
        if (count) {
            (*targets)[0] = &first; (*targets)[count - 1] = &last;
            assert((*targets)[count - 1]->marker == 2);
            if (count > 1) assert((*targets)[0]->marker == 1 && !(*targets)[1]);
        }
        delete targets; // Array storage only; full StarSystem lifetime is outside this check.
        std::printf("Target array count %u: object %zu bytes, initial element %zu, final storage %zu\n", count, objectBytes, initialBytes, capacity * sizeof(KIPlayer *));
    }
}
