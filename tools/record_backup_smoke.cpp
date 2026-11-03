#include <cassert>
#include <cstdint>
#include <vector>

template <class T> struct Array {
    std::vector<T> values;
    T &operator[](unsigned i) { return values[i]; }
};
template <class T> void ArraySetLength(unsigned n, Array<T> &array) {
    array.values.assign(n, T{});
}
template <class T> void ArrayReleaseArrays(Array<T> &array) {
    for (T value : array.values) delete[] value;
    array.values.clear();
}

struct Globals { static void *recordSlots; };
void *Globals::recordSlots = nullptr;
namespace AbyssEngine { struct Engine { static unsigned char backupSaveGames; }; }
unsigned char AbyssEngine::Engine::backupSaveGames = 0;

struct RecordHandler {
    struct Write {
        int slot;
        bool backup;
        int size;
        int firstByte;
        unsigned char backupEnabled;
    };
    std::vector<Write> writes;
    std::vector<int> hashes;
    int reads = 0;

    int readRecordAsByteArray(signed char **out, int slot, bool backup) {
        ++reads;
        if (!backup && slot == 1) {
            *out = nullptr;
            return 0;
        }
        *out = new signed char[2]{static_cast<signed char>(slot + (backup ? 10 : 1)), 0};
        return 2;
    }
    void writeByteArrayAsRecord(signed char *data, int size, int slot, bool backup) {
        writes.push_back({slot, backup, size, data ? data[0] : -1,
                          AbyssEngine::Engine::backupSaveGames});
    }
    void addHash(int slot) { hashes.push_back(slot); }
    void changeSaveDirectoryToBackupDirectory();
};

// PRODUCTION_FUNCTION

int main() {
    Globals::recordSlots = reinterpret_cast<void *>(static_cast<std::uintptr_t>(3));
    RecordHandler handler;
    handler.changeSaveDirectoryToBackupDirectory();
    assert(handler.reads == 6);
    assert(AbyssEngine::Engine::backupSaveGames == 1);
    assert((handler.hashes == std::vector<int>{1, 3}));
    assert(handler.writes.size() == 4);
    assert(handler.writes[0].slot == 0 && !handler.writes[0].backup && handler.writes[0].firstByte == 1);
    assert(handler.writes[1].slot == 0 && handler.writes[1].backup && handler.writes[1].firstByte == 10);
    assert(handler.writes[2].slot == 2 && !handler.writes[2].backup && handler.writes[2].firstByte == 3);
    assert(handler.writes[3].slot == 2 && handler.writes[3].backup && handler.writes[3].firstByte == 12);
    for (const auto &write : handler.writes)
        assert(write.size == 2 && write.backupEnabled == 1);

    Globals::recordSlots = nullptr;
    AbyssEngine::Engine::backupSaveGames = 0;
    RecordHandler empty;
    empty.changeSaveDirectoryToBackupDirectory();
    assert(empty.reads == 0 && empty.writes.empty() && empty.hashes.empty());
    assert(AbyssEngine::Engine::backupSaveGames == 1);
}
