#include "game/core/String.h"
#include "engine/core/GameText.h"
#include "engine/render/Engine.h"
#include "engine/file/AEFile.h"
#include <jni.h>
#include <archive.h>
#include <archive_entry.h>
#include <openssl/sha.h>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <tuple>
#include <vector>

void String_printImpl(const char *) { std::abort(); }
namespace AbyssEngine {
unsigned short *computeFloatString(float, int, int *, int *, int) { std::abort(); }
}

struct zip { std::string path; };
struct zip_file { archive *reader; };
static std::vector<zip_file *> archiveHandles;
static unsigned closedStreams = 0;
static unsigned closedFiles = 0;

zip *zip_open(const char *path, int flags, int *error) {
    assert(flags == 0 && error == nullptr);
    return std::filesystem::is_regular_file(path) ? new zip{path} : nullptr;
}
zip_file *zip_fopen(zip *source, const char *name, int flags) {
    assert(flags == 0);
    if (!source) return nullptr;
    archive *reader = archive_read_new();
    archive_read_support_format_zip(reader);
    if (archive_read_open_filename(reader, source->path.c_str(), 10240) != ARCHIVE_OK) {
        archive_read_free(reader);
        return nullptr;
    }
    archive_entry *entry;
    while (archive_read_next_header(reader, &entry) == ARCHIVE_OK) {
        if (std::strcmp(archive_entry_pathname(entry), name) == 0) {
            auto *file = new zip_file{reader};
            archiveHandles.push_back(file);
            return file;
        }
        archive_read_data_skip(reader);
    }
    archive_read_free(reader);
    return nullptr;
}
long long zip_fread(zip_file *file, void *buffer, unsigned n) {
    unsigned total = 0;
    while (total < n) {
        const auto got = archive_read_data(file->reader,
                                           static_cast<char *>(buffer) + total, n - total);
        if (got <= 0) return got < 0 ? -1 : total;
        total += static_cast<unsigned>(got);
    }
    return total;
}
int zip_fclose(zip_file *file) {
    if (file->reader) {
        archive_read_free(file->reader);
        ++closedStreams;
    }
    file->reader = nullptr;
    return 0;
}

class FileInterfaceAndroid : public FileInterface {
public:
    FILE *file = nullptr;
    zip_file *zipFile = nullptr;
    void *jniStream = nullptr;
    bool modeFlag = false;
    int zipReadPos = 0;
    int zipReadLen = 0;
    bool zipAppend = false;
    void *appRootDir = nullptr;
    static int fileCounter;
    static void **gZipMain;
    static void **gZipPatch;
    FileInterfaceAndroid() { enabled = 1; }
    FileInterfaceAndroid(FILE *stream, bool append);
    FileInterfaceAndroid(zip_file *stream, bool append, int offset, int, int);
    ~FileInterfaceAndroid() override {
        Close();
        if (fileCounter) --fileCounter;
    }
    uint32_t Seek(uint32_t n) override;
    uint32_t FileExist(String name) override;
    void *OpenRead(String name, int p2, bool p3, int p4, int p5, unsigned p6) override;
    void *OpenWrite(String, int, bool, unsigned) override;
    void *OpenAppend(String, int, bool, unsigned) override { return nullptr; }
    uint32_t Read(uint32_t n, void *buf) override {
        if (zipFile) return zip_fread(zipFile, buf, n) == n;
        if (file) return std::fread(buf, 1, n, file) == n;
        return 0;
    }
    uint32_t Write(uint32_t n, const void *buf) override {
        return file && std::fwrite(buf, 1, n, file) == n;
    }
    uint32_t GetFileSize() override { return 0; }
    uint32_t FileDelete(String) override;
    uint32_t GetDeviceFreeSpace() override { return 0; }
    const char *GetAppRootDir() override { return static_cast<const char *>(appRootDir); }
    void SetAppRootDir(void *path) override { appRootDir = path; }
    void SetZipDirectory(void *) override {}
    uint32_t FileEnumInit(char *, bool) override { return 0; }
    uint32_t FileGetNextEnum(String &) override { return 0; }
    void Close() override {
        if (file) {
            std::fclose(file);
            ++closedFiles;
        }
        if (zipFile) zip_fclose(zipFile);
        file = nullptr;
        zipFile = nullptr;
    }
    String GetDirPreFix() override { return String(""); }
    void Output(char *) override {}
    void SetSaveDirectory(String) override {}
    void ResetSaveDirectory() override {}
};
int FileInterfaceAndroid::fileCounter = 0;
unsigned int AbyssEngine::Engine::countryCode = 0;

#include "file_android_readexist_under_test.inc"
#include "jni_country_under_test.inc"

static std::vector<unsigned char> readBytes(FileInterfaceAndroid *file, size_t size) {
    assert(file && file->zipFile);
    std::vector<unsigned char> bytes(size);
    assert(zip_fread(file->zipFile, bytes.data(), size) == static_cast<long long>(size));
    delete file;
    return bytes;
}
static void checkSha256(const std::vector<unsigned char> &bytes, const char *expected) {
    uint64_t context[0x70 / sizeof(uint64_t)];
    unsigned char digest[32];
    SHA256_Init(context);
    SHA256_Update(context, bytes.data(), bytes.size());
    SHA256_Final(digest, context);
    char hex[65];
    for (int i = 0; i < 32; ++i) std::sprintf(hex + 2 * i, "%02x", digest[i]);
    assert(std::strcmp(hex, expected) == 0);
}

int main(int argc, char **argv) {
    assert(argc == 4);
    assert(loadAPKAndZip(argv[1], argv[2]));
    FileInterfaceAndroid reader;
    assert(reader.FileExist(String("data/bin/items.bin")));
    auto *items = static_cast<FileInterfaceAndroid *>(
        reader.OpenRead(String("data/bin/items.bin"), 18, false, 0, 0, 0));
    checkSha256(readBytes(items, 27628),
                "639b1da183594b9ca024a528d95082152591e49c9a685b6e42419bff931d7f4c");

    assert(reader.FileExist(String("data/bin/stations.bin")));
    auto *stations = static_cast<FileInterfaceAndroid *>(
        reader.OpenRead(String("data/bin/stations.bin"), 21, false, 0, 0, 0));
    checkSha256(readBytes(stations, 3408),
                "761e84019f4c0ae6f4c8e160252be56add2203d1325af2571c94fa7e45c7baa3");

    auto *offsetItems = static_cast<FileInterfaceAndroid *>(
        reader.OpenRead(String("data/bin/items.bin"), 18, true, 0, 0, 4));
    assert(offsetItems && offsetItems->zipFile && offsetItems->zipAppend);
    auto suffix = readBytes(offsetItems, 27624);
    auto *fullItems = static_cast<FileInterfaceAndroid *>(
        reader.OpenRead(String("data/bin/items.bin"), 18, false, 0, 0, 0));
    auto full = readBytes(fullItems, 27628);
    assert(std::memcmp(suffix.data(), full.data() + 4, suffix.size()) == 0);

    // Exercise the production handle table and read/close path above the
    // Android archive selection, using both an APK and a patch-only entry.
    AEFile::SetInterface(&reader);
    for (const auto &[path, size, digest] : {
             std::tuple{"data/bin/items.bin", 27628u,
                        "639b1da183594b9ca024a528d95082152591e49c9a685b6e42419bff931d7f4c"},
             std::tuple{"data/bin/stations.bin", 3408u,
                        "761e84019f4c0ae6f4c8e160252be56add2203d1325af2571c94fa7e45c7baa3"}}) {
        uint32_t handle = 0;
        assert(AEFile::OpenRead(path, &handle) == 1 && handle == 1);
        std::vector<unsigned char> bytes(size);
        assert(AEFile::Read(size, bytes.data(), handle) == 1);
        checkSha256(bytes, digest);
        const unsigned closesBefore = closedStreams;
        AEFile::Close(handle);
        assert(closedStreams == closesBefore + 1);
        AEFile::Close(handle);
        assert(closedStreams == closesBefore + 1);
    }
    uint32_t missingHandle = 99;
    assert(AEFile::OpenRead("data/bin/not_present.bin", &missingHandle) == 0);
    assert(missingHandle == 99);

    const std::filesystem::path saveRoot(argv[3]);
    std::string appRoot = saveRoot.string() + "/";
    reader.appRootDir = appRoot.data();
    const int languageFilesBefore = FileInterfaceAndroid::fileCounter;
    GameText text;
    ndk23_setCountryCode(0);
    text.setLanguage(static_cast<short>(AbyssEngine::Engine::countryCode), 3401);
    assert(GameText::getLanguage() == 0 && text.textCount == 3401);
    assert(std::u16string(text.getText(0)->text()) == u"Language");
    for (int i = 0; i < 3401; ++i)
        assert(text.getText(i) != &text.fallbackText);

    Java_net_fishlabs_gof2hdallandroid2012_ToJNI_setCountryCodeOfDevice(nullptr, nullptr, 5);
    assert(AbyssEngine::Engine::countryCode == 5);
    text.setLanguage(static_cast<short>(AbyssEngine::Engine::countryCode), 3401);
    assert(GameText::getLanguage() == 5 && text.textCount == 3401);
    assert(std::u16string(text.getText(0)->text()) == u"Язык");
    for (int i = 0; i < 3401; ++i)
        assert(text.getText(i) != &text.fallbackText);
    assert(std::u16string(text.getText(3401)->text()) == u"Error Language");

    // No cz.lang exists in this APK or OBB; selection must fall back to gb.lang.
    assert(!reader.FileExist(String("cz.lang")));
    Java_net_fishlabs_gof2hdallandroid2012_ToJNI_setCountryCodeOfDevice(nullptr, nullptr, 8);
    text.setLanguage(static_cast<short>(AbyssEngine::Engine::countryCode), 3401);
    assert(GameText::getLanguage() == 0);
    assert(std::u16string(text.getText(0)->text()) == u"Language");
    assert(FileInterfaceAndroid::fileCounter == languageFilesBefore);
    Java_net_fishlabs_gof2hdallandroid2012_ToJNI_setCountryCodeOfDevice(nullptr, nullptr, 16);
    assert(AbyssEngine::Engine::countryCode == 0);
    Java_net_fishlabs_gof2hdallandroid2012_ToJNI_setCountryCodeOfDevice(nullptr, nullptr, -1);
    assert(AbyssEngine::Engine::countryCode == 0);

    // Save-file bytes stay in the runner's temporary directory.
    const int countBefore = FileInterfaceAndroid::fileCounter;
    const unsigned fileClosesBefore = closedFiles;
    uint32_t saveHandle = 99;
    String saveName("save_probe.bin");
    assert(AEFile::OpenWrite(saveName, &saveHandle) == 1 && saveHandle == 1);
    assert(FileInterfaceAndroid::fileCounter == countBefore + 1);
    char payload[] = {'G', 'O', 'F', '2', '\n'};
    assert(AEFile::Write(sizeof(payload), payload, saveHandle) == 1);
    AEFile::Close(saveHandle);
    assert(FileInterfaceAndroid::fileCounter == countBefore);
    assert(closedFiles == fileClosesBefore + 1);
    FILE *saved = std::fopen((saveRoot / "save_probe.bin").string().c_str(), "rb");
    assert(saved);
    char diskBytes[sizeof(payload)] = {};
    assert(std::fread(diskBytes, 1, sizeof(diskBytes), saved) == sizeof(diskBytes));
    assert(std::fclose(saved) == 0);
    assert(std::memcmp(diskBytes, payload, sizeof(payload)) == 0);

    uint32_t readBack = 99;
    assert(AEFile::OpenRead(saveName, &readBack) == 1 && readBack == 1);
    char roundTrip[sizeof(payload)] = {};
    assert(AEFile::Read(sizeof(roundTrip), roundTrip, readBack) == 1);
    assert(std::memcmp(roundTrip, payload, sizeof(payload)) == 0);
    AEFile::Close(readBack);
    String missingParent("missing/save_probe.bin");
    uint32_t failedWrite = 99;
    assert(AEFile::OpenWrite(missingParent, &failedWrite) == 0);
    assert(failedWrite == 99);
    AEFile::Release();

    delete static_cast<zip *>(*FileInterfaceAndroid::gZipMain);
    delete static_cast<zip *>(*FileInterfaceAndroid::gZipPatch);
    for (auto *handle : archiveHandles) {
        zip_fclose(handle);
        delete handle;
    }
    std::puts("Original APK/OBB, JNI country and GameText language stack: OK");
}
