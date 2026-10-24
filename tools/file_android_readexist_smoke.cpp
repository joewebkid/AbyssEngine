#include "game/core/String.h"
#include "engine/core/GameText.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

int GameText::getLanguage() { std::abort(); }
void String_printImpl(const char *) { std::abort(); }
namespace AbyssEngine {
unsigned short *computeFloatString(float, int, int *, int *, int) { std::abort(); }
}

struct zip {};
struct zip_file {};
static zip apkArchive, patchArchive;
static zip_file apkFile, patchFile;
static bool hasApk = false, hasPatch = false;
static std::vector<std::string> lookups;
static std::vector<unsigned> seeks;
static int closes = 0;

zip *zip_open(const char *path, int flags, int *error) {
    assert(flags == 0 && error == nullptr);
    return std::string(path) == "apk" ? &apkArchive :
           std::string(path) == "patch" ? &patchArchive : nullptr;
}
zip_file *zip_fopen(zip *archive, const char *name, int flags) {
    assert(flags == 0);
    lookups.emplace_back(name);
    if (archive == &apkArchive && hasApk && lookups.back() == "assets/save.bin")
        return &apkFile;
    if (archive == &patchArchive && hasPatch &&
        lookups.back() == "47947_GOF2CONTENT_ETC/assets/save.bin")
        return &patchFile;
    return nullptr;
}
int zip_fclose(zip_file *) { ++closes; return 0; }
unsigned zip_fread(zip_file *, void *, unsigned n) {
    seeks.push_back(n);
    return n;
}

class FileInterfaceAndroid {
public:
    int enabled = 1;
    void *appRootDir = nullptr;
    FILE *file = nullptr;
    zip_file *zipFile = nullptr;
    void *jniStream = nullptr;
    bool modeFlag = false;
    int zipReadPos = 0;
    int zipReadLen = 0;
    bool zipAppend = false;
    static int fileCounter;
    static void **gZipMain;
    static void **gZipPatch;
    FileInterfaceAndroid() = default;
    FileInterfaceAndroid(FILE *stream, bool append);
    FileInterfaceAndroid(zip_file *stream, bool append, int offset, int, int);
    ~FileInterfaceAndroid() { if (file) std::fclose(file); }
    uint32_t Seek(uint32_t n);
    uint32_t FileExist(String name);
    void *OpenRead(String name, int p2, bool p3, int p4, int p5, unsigned p6);
    void *OpenWrite(String name, int size, bool append, unsigned mode);
};
int FileInterfaceAndroid::fileCounter = 0;

#include "file_android_readexist_under_test.inc"

static void checkLookups() {
    assert(lookups.size() == 2);
    assert(lookups[0] == "assets/save.bin");
    assert(lookups[1] == "47947_GOF2CONTENT_ETC/assets/save.bin");
    lookups.clear();
}

int main(int argc, char **argv) {
    assert(argc == 2);
    const std::filesystem::path root = argv[1];
    std::filesystem::create_directories(root);
    std::string prefix = root.string() + "/";
    FileInterfaceAndroid reader;
    reader.appRootDir = prefix.data();
    assert(FileInterfaceAndroid::gZipMain && FileInterfaceAndroid::gZipPatch);
    assert(loadAPKAndZip("apk", "patch"));
    assert(*FileInterfaceAndroid::gZipMain == &apkArchive);
    assert(*FileInterfaceAndroid::gZipPatch == &patchArchive);

    assert(!reader.FileExist(String("save.bin")));
    checkLookups();
    assert(reader.OpenRead(String("save.bin"), 8, false, 0, 0, 0) == nullptr);
    checkLookups();
    assert(!loadAPKAndZip("apk", "missing"));
    assert(*FileInterfaceAndroid::gZipPatch == nullptr);
    reader.appRootDir = nullptr;
    assert(reader.OpenRead(String("save.bin"), 8, false, 0, 0, 0) == nullptr);
    assert(lookups.size() == 1 && lookups[0] == "assets/save.bin");
    lookups.clear();
    assert(loadAPKAndZip("apk", "patch"));
    reader.appRootDir = prefix.data();

    const auto path = root / "save.bin";
    FILE *created = std::fopen(path.string().c_str(), "wb");
    assert(created && std::fwrite("ok", 1, 2, created) == 2);
    std::fclose(created);
    assert(reader.FileExist(String("save.bin")));
    checkLookups();
    auto *disk = static_cast<FileInterfaceAndroid *>(
        reader.OpenRead(String("/save.bin"), 9, false, 0, 0, 0));
    assert(disk && disk->file && !disk->zipFile && !disk->modeFlag);
    checkLookups();
    char bytes[2];
    assert(std::fread(bytes, 1, 2, disk->file) == 2 &&
           bytes[0] == 'o' && bytes[1] == 'k');
    delete disk;

    hasApk = true;
    assert(reader.FileExist(String("save.bin")));
    checkLookups();
    auto *fromApk = static_cast<FileInterfaceAndroid *>(
        reader.OpenRead(String("save.bin"), 8, true, 17, 23, 42));
    assert(fromApk && fromApk->zipFile == &apkFile && !fromApk->file);
    assert(fromApk->zipAppend && seeks.size() == 1 && seeks[0] == 42);
    checkLookups();
    delete fromApk;

    hasPatch = true;
    auto *both = static_cast<FileInterfaceAndroid *>(
        reader.OpenRead(String("save.bin"), 8, true, 17, 23, 42));
    assert(both && both->zipFile == &apkFile);
    assert(both->zipAppend && seeks.size() == 2 && seeks[1] == 42);
    checkLookups();
    delete both;

    hasApk = false;
    assert(reader.FileExist(String("save.bin")));
    checkLookups();
    auto *fromPatch = static_cast<FileInterfaceAndroid *>(
        reader.OpenRead(String("save.bin"), 8, true, 17, 23, 42));
    assert(fromPatch && fromPatch->zipFile == &patchFile && !fromPatch->file);
    assert(fromPatch->zipAppend && seeks.size() == 3 && seeks[2] == 42);
    checkLookups();
    delete fromPatch;

    reader.enabled = 0;
    assert(reader.OpenRead(String("save.bin"), 8, false, 0, 0, 0) == nullptr);
    assert(lookups.empty() && closes == 2);
    std::puts("Android read/exist archive and filesystem paths: OK");
}
