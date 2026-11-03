#include "engine/core/Array.h"
#include "engine/core/GameText.h"
#include "engine/file/AEFile.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>

int GameText::getLanguage() { std::abort(); }
void String_printImpl(const char *) { std::abort(); }
namespace AbyssEngine {
unsigned short *computeFloatString(float, int, int *, int *, int) { std::abort(); }
struct Engine { static unsigned char backupSaveGames; };
unsigned char Engine::backupSaveGames = 0;
}
struct Globals { static void *recordSlots; };
void *Globals::recordSlots = nullptr;

struct RecordMigration {
    String recordDir = String("save/", false);
    String backupDir = String("preview/", false);
    int readRecordAsByteArray(signed char **out, int slot, bool backup);
    int writeByteArrayAsRecord(signed char *data, int size, int slot, bool backup);
    void addHash(int slot);
    void convertSDVersionSaves();
    void changeSaveDirectoryToBackupDirectory();
};

struct WriteEvent { int sourceSlot, targetSlot; bool backup; };
static std::vector<WriteEvent> writes;
static std::vector<int> hashes;
static std::vector<std::string> deletedPaths;
static int reads = 0;
static int deletes = 0;

uint32_t AEFile::FileDelete(const String &path) {
    std::string name;
    for (uint32_t i = 0; i < path.size(); ++i)
        name.push_back(static_cast<char>(path.text()[i]));
    deletedPaths.push_back(name);
    ++deletes;
    return 1;
}

int RecordMigration::readRecordAsByteArray(signed char **out, int slot, bool backup) {
    assert(slot >= 0 && slot < 2);
    *out = new signed char[2]{static_cast<signed char>(slot), static_cast<signed char>(backup)};
    ++reads;
    return 2;
}

int RecordMigration::writeByteArrayAsRecord(signed char *data, int size, int slot, bool backup) {
    assert(data && size == 2 && data[1] == backup);
    writes.push_back({data[0], slot, backup});
    return size;
}

void RecordMigration::addHash(int slot) { hashes.push_back(slot); }

#include "record_migration_under_test.inc"

int main() {
    Globals::recordSlots = reinterpret_cast<void *>(std::uintptr_t{2});
    RecordMigration migration;

    migration.convertSDVersionSaves();
    assert(reads == 4 && deletes == 4 && writes.size() == 4);
    assert((deletedPaths == std::vector<std::string>{"save/", "preview/", "save/1", "preview/1"}));
    assert(writes[0].sourceSlot == 0 && writes[0].targetSlot == 1 && !writes[0].backup);
    assert(writes[1].sourceSlot == 0 && writes[1].targetSlot == 1 && writes[1].backup);
    assert(writes[2].sourceSlot == 1 && writes[2].targetSlot == 0 && !writes[2].backup);
    assert(writes[3].sourceSlot == 1 && writes[3].targetSlot == 0 && writes[3].backup);
    assert(hashes.size() == 2 && hashes[0] == 1 && hashes[1] == 0);

    writes.clear();
    hashes.clear();
    reads = 0;
    deletes = 0;
    deletedPaths.clear();
    migration.changeSaveDirectoryToBackupDirectory();
    assert(reads == 4 && deletes == 0 && writes.size() == 4 && AbyssEngine::Engine::backupSaveGames == 1);
    assert(writes[0].sourceSlot == 0 && writes[0].targetSlot == 0 && !writes[0].backup);
    assert(writes[1].sourceSlot == 0 && writes[1].targetSlot == 0 && writes[1].backup);
    assert(writes[2].sourceSlot == 1 && writes[2].targetSlot == 1 && !writes[2].backup);
    assert(writes[3].sourceSlot == 1 && writes[3].targetSlot == 1 && writes[3].backup);
    assert(hashes.size() == 2 && hashes[0] == 1 && hashes[1] == 2);
    std::puts("Record migration and cleanup: OK");
}
