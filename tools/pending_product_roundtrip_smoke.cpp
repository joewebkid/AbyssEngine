#include "engine/file/AEFile.h"
#include "engine/core/GameText.h"
#include "game/mission/PendingProduct.h"
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
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

struct RecordWriter {
    String optionsPath = String("options", false);
    String recordDir = String("save/", false);
    String backupDir = String("backup/", false);
    void writeByteArrayAsOptionsFile(signed char *buf, int n);
    int writeByteArrayAsRecord(signed char *buf, int n, int slot, bool toBackup);
    int readRecordAsByteArray(signed char **out, int slot, bool fromBackup);
    int readOptionsFileAsByteArray(signed char **out);
    void addHash(int slot);
    void addHashToOptions();
    bool checkHash(unsigned int fd);
};

static std::vector<uint8_t> tape;
static size_t cursor = 0;
static std::string openedPath;
const char *g_android_origami_super_club = "0123456789abcdef";

static std::string pathText(const String &path) {
    std::string result;
    for (uint32_t i = 0; i < path.size(); ++i)
        result.push_back(static_cast<char>(path.data[i]));
    return result;
}

uint32_t AEFile::FileExist(const String &path) {
    return pathText(path) == openedPath && !tape.empty();
}
uint32_t AEFile::FileDelete(const String &path) {
    assert(pathText(path) == openedPath);
    tape.clear();
    return 1;
}
uint32_t AEFile::OpenWrite(String &path, uint32_t *handle) {
    openedPath = pathText(path);
    tape.clear();
    cursor = 0;
    *handle = 1;
    return 1;
}
uint32_t AEFile::OpenRead(String &path, uint32_t *handle) {
    assert(pathText(path) == openedPath);
    cursor = 0;
    *handle = 1;
    return 1;
}
uint32_t AEFile::GetFileSize(uint32_t handle) {
    assert(handle == 1);
    return tape.size();
}
void AEFile::Close(uint32_t handle) { assert(handle == 1); }
void AEFile::Write(bool value, uint32_t handle) {
    uint8_t byte = value;
    Write(1, &byte, handle);
}

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

int main(int argc, char **argv) {
    assert(argc == 1 || argc == 3);
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

    RecordWriter writer;
    signed char payload[] = {0, 1, 2, 3, 4, 5, 6};
    const uint8_t expectedHash[32] = {
        0xdf, 0xe8, 0x59, 0x95, 0x73, 0x78, 0x60, 0x81,
        0x82, 0x94, 0x14, 0xda, 0x50, 0xfb, 0x09, 0xe5,
        0xf1, 0x6c, 0xfa, 0xf4, 0xc5, 0x15, 0xe8, 0x7c,
        0x7e, 0x00, 0x3d, 0x88, 0x67, 0x7f, 0xdd, 0x55,
    };
    writer.writeByteArrayAsOptionsFile(payload, sizeof(payload));
    assert(openedPath == "options" && tape.size() == sizeof(payload));
    assert(std::memcmp(tape.data(), payload, sizeof(payload)) == 0);
    writer.addHashToOptions();
    assert(openedPath == "options" && tape.size() == sizeof(payload) + 32);
    assert(std::memcmp(tape.data() + sizeof(payload), expectedHash, 32) == 0);
    cursor = 0;
    assert(writer.checkHash(1));
    assert(writer.writeByteArrayAsRecord(payload, sizeof(payload), 4, false) == sizeof(payload));
    assert(openedPath == "save/4" && tape.size() == sizeof(payload));
    assert(std::memcmp(tape.data(), payload, sizeof(payload)) == 0);
    writer.addHash(4);
    assert(tape.size() == sizeof(payload) + 32);
    assert(std::memcmp(tape.data() + sizeof(payload), expectedHash, 32) == 0);
    cursor = 0;
    assert(writer.checkHash(1));
    tape[2] ^= 0x40;
    cursor = 0;
    assert(!writer.checkHash(1));
    tape[2] ^= 0x40;
    tape.back() ^= 1;
    cursor = 0;
    assert(!writer.checkHash(1));
    tape.back() ^= 1;
    g_android_origami_super_club = "fedcba9876543210";
    cursor = 0;
    assert(!writer.checkHash(1));
    g_android_origami_super_club = "0123456789abcdef";
    cursor = 0;
    assert(writer.checkHash(1));
    tape.resize(33);
    cursor = 0;
    assert(!writer.checkHash(1));
    assert(writer.writeByteArrayAsRecord(payload, sizeof(payload), 4, true) == sizeof(payload));
    assert(openedPath == "backup/4" && tape.size() == sizeof(payload));
    assert(std::memcmp(tape.data(), payload, sizeof(payload)) == 0);
    if (argc == 3) {
        const size_t expectedSizes[] = {13054, 15757};
        const uint64_t expectedTimes[] = {80559, 850954};
        for (int i = 0; i < 2; ++i) {
            std::ifstream fixture(argv[i + 1], std::ios::binary);
            assert(fixture);
            tape.assign(std::istreambuf_iterator<char>(fixture), {});
            assert(tape.size() == expectedSizes[i]);
            const size_t payloadSize = tape.size() - 32;
            const auto readLe = [](size_t offset, unsigned bytes) {
                uint64_t value = 0;
                for (unsigned j = 0; j < bytes; ++j)
                    value |= uint64_t(tape[offset + j]) << (8 * j);
                return value;
            };
            const size_t visitedCount = readLe(0, 4);
            assert(visitedCount == 635 && 4 + visitedCount + 16 <= payloadSize);
            size_t visitedOnes = 0;
            for (size_t j = 0; j < visitedCount; ++j) {
                assert(tape[4 + j] <= 1);
                visitedOnes += tape[4 + j];
            }
            assert(visitedOnes == (i == 1 ? 2u : 0u));
            assert(tape[4 + 77] == (i == 1));
            assert(tape[4 + 135] == (i == 1));
            assert(readLe(4 + visitedCount, 4) == 0); // credits
            assert(readLe(8 + visitedCount, 4) == 0); // rating
            assert(readLe(12 + visitedCount, 8) == expectedTimes[i]);
            unsigned char digest[32];
            uint64_t context[0x70 / sizeof(uint64_t)];
            SHA256_Init(context);
            SHA256_Update(context, tape.data(), payloadSize);
            SHA256_Update(context, RH_hash_salt, sizeof(RH_hash_salt));
            SHA256_Final(digest, context);
            assert(std::memcmp(digest, tape.data() + payloadSize, 32) == 0);
            cursor = 0;
            assert(!writer.checkHash(1));
            assert(cursor == tape.size());
        }
        std::puts("iOS full-save prefixes: 2/2 have 635 visited flags; Android keyed check rejects");
    }
    std::puts("PendingProduct round trip, full byte writes and record hashes: OK");
}
