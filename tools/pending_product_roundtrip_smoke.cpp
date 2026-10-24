#include "engine/file/AEFile.h"
#include "engine/core/GameText.h"
#include "game/mission/PendingProduct.h"
#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <vector>

// Item lookup is outside this gate; the getters and all downstream bodies are production.
BluePrint::BluePrint(int item) : stationName("Kalter", false) {
    ingredientCounters = nullptr;
    itemIndex = item;
    stationIndex = 17;
    remainingBatch = 5;
}
int GameText::getLanguage() { std::abort(); }
void String_printImpl(const char *) { std::abort(); }
namespace AbyssEngine {
String operator+(const String &, const String &) { std::abort(); }
unsigned short *computeFloatString(float, int, int *, int *, int) { std::abort(); }
}

struct PendingStatus {
    Array<PendingProduct *> *pendingProducts = nullptr;
    void addPendingProduct(BluePrint *bp);
};

struct PendingRecord {
    Array<PendingProduct *> *pendingProducts = nullptr;
    void setPendingProducts(Array<PendingProduct *> *value) { pendingProducts = value; }
};

static std::vector<uint8_t> tape;
static size_t cursor = 0;

uint32_t AEFile::Write(uint32_t bytes, void *buffer, uint32_t handle) {
    assert(handle == 1 && buffer && bytes);
    const auto *first = static_cast<const uint8_t *>(buffer);
    tape.insert(tape.end(), first, first + bytes);
    return bytes;
}

uint32_t AEFile::Read(uint32_t bytes, void *buffer, uint32_t handle) {
    assert(handle == 1 && buffer && bytes && cursor + bytes <= tape.size());
    std::memcpy(buffer, tape.data() + cursor, bytes);
    cursor += bytes;
    return bytes;
}

#include "pending_product_under_test.inc"

static int32_t word(size_t offset) {
    int32_t result;
    assert(offset + sizeof(result) <= tape.size());
    std::memcpy(&result, tape.data() + offset, sizeof(result));
    return result;
}

static void checkName(const String &actual, const String &expected) {
    assert(actual.size() == expected.size());
    for (uint32_t i = 0; i < actual.size(); ++i)
        assert(actual.data[i] == expected.data[i]);
}

int main() {
    PendingStatus status;
    PendingRecord record;
    writePending(&status, 1);
    assert(tape.size() == 4 && word(0) == -1);
    readPending(&record, 1);
    assert(record.pendingProducts == nullptr && cursor == tape.size());

    tape.clear();
    cursor = 0;
    status.pendingProducts = new Array<PendingProduct *>();
    ArrayAdd(static_cast<PendingProduct *>(nullptr), *status.pendingProducts);
    writePending(&status, 1);
    assert(tape.size() == 4 && word(0) == -1);
    readPending(&record, 1);
    assert(record.pendingProducts == nullptr && cursor == tape.size());

    tape.clear();
    cursor = 0;
    auto *first = new PendingProduct(42, String("Kalter", false), 17, 5);
    const uint16_t cyrillicName[] = {0x0410, 0x0440, 0x043a, 0x0430,
                                     0x0434, 0x0438, 0x044f, 0};
    auto *second = new PendingProduct(211, String(cyrillicName, false), 103, 9);
    ArrayAdd(first, *status.pendingProducts);
    ArrayAdd(static_cast<PendingProduct *>(nullptr), *status.pendingProducts);
    ArrayAdd(second, *status.pendingProducts);
    writePending(&status, 1);
    assert(tape.size() == 62 && word(0) == 2);
    assert(word(4) == 42 && word(8) == 5 && word(12) == 17 && word(16) == 6);
    assert(word(32) == 211 && word(36) == 9 && word(40) == 103 && word(44) == 7);
    readPending(&record, 1);
    assert(cursor == tape.size() && record.pendingProducts->size() == 2);
    for (unsigned i = 0; i < 2; ++i) {
        const PendingProduct *expected = i == 0 ? first : second;
        const PendingProduct *actual = (*record.pendingProducts)[i];
        assert(actual && actual->blueprintIndex == expected->blueprintIndex);
        assert(actual->stationIndex == expected->stationIndex);
        assert(actual->quantity == expected->quantity);
        checkName(actual->stationName, expected->stationName);
    }

    delete first;
    delete second;
    delete status.pendingProducts;
    for (auto *item : *record.pendingProducts) delete item;
    delete record.pendingProducts;

    tape.clear();
    cursor = 0;
    PendingStatus created;
    PendingRecord reloaded;
    BluePrint blueprint(42);
    created.addPendingProduct(&blueprint);
    created.addPendingProduct(&blueprint);
    assert(created.pendingProducts->size() == 1);
    assert((*created.pendingProducts)[0]->quantity == 10);
    blueprint.stationIndex = 103;
    blueprint.stationName = String("Arcadia", false);
    blueprint.remainingBatch = 9;
    created.addPendingProduct(&blueprint);
    assert(created.pendingProducts->size() == 2);
    writePending(&created, 1);
    readPending(&reloaded, 1);
    assert(cursor == tape.size() && reloaded.pendingProducts->size() == 2);
    const PendingProduct *merged = (*reloaded.pendingProducts)[0];
    const PendingProduct *separate = (*reloaded.pendingProducts)[1];
    assert(merged->blueprintIndex == 42 && merged->stationIndex == 17);
    assert(merged->quantity == 10);
    checkName(merged->stationName, String("Kalter", false));
    assert(separate->blueprintIndex == 42 && separate->stationIndex == 103);
    assert(separate->quantity == 9);
    checkName(separate->stationName, blueprint.stationName);
    for (auto *item : *created.pendingProducts) delete item;
    delete created.pendingProducts;
    for (auto *item : *reloaded.pendingProducts) delete item;
    delete reloaded.pendingProducts;
    std::puts("PendingProduct add/save/load round trips: OK");
}
