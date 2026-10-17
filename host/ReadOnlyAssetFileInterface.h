#ifndef GOF2_HOST_READ_ONLY_ASSET_FILE_INTERFACE_H
#define GOF2_HOST_READ_ONLY_ASSET_FILE_INTERFACE_H

#include "engine/file/FileInterface.h"
#include <cstdio>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

// Host-only bridge for loose, read-only resources. Not an Android ABI replacement.
class ReadOnlyAssetFileInterface final : public FileInterface {
public:
    ReadOnlyAssetFileInterface();
    ~ReadOnlyAssetFileInterface() override;
    ReadOnlyAssetFileInterface(const ReadOnlyAssetFileInterface &) = delete;
    ReadOnlyAssetFileInterface &operator=(const ReadOnlyAssetFileInterface &) = delete;

    bool addMount(const std::string &logicalPrefix,
                  const std::filesystem::path &physicalDirectory);
    unsigned opens() const;
    unsigned closes() const;

    void *OpenRead(String name, int size, bool windowed, int packedSize,
                   int rawSize, unsigned offset) override;
    void *OpenWrite(String, int, bool, unsigned) override;
    void *OpenAppend(String, int, bool, unsigned) override;
    uint32_t Read(uint32_t bytes, void *buffer) override;
    uint32_t Write(uint32_t, const void *) override;
    uint32_t Seek(uint32_t bytes) override;
    uint32_t GetFileSize() override;
    uint32_t FileExist(String name) override;
    uint32_t FileDelete(String) override;
    uint32_t GetDeviceFreeSpace() override;
    const char *GetAppRootDir() override;
    void SetAppRootDir(void *) override;
    void SetZipDirectory(void *) override;
    uint32_t FileEnumInit(char *, bool) override;
    uint32_t FileGetNextEnum(String &) override;
    void Close() override;
    String GetDirPreFix() override;
    char *Output(char *) override;
    void SetSaveDirectory(String) override;
    void ResetSaveDirectory() override;

private:
    struct Mount {
        std::string prefix;
        std::filesystem::path directory;
    };
    struct Shared {
        std::vector<Mount> mounts;
        unsigned opens = 0;
        unsigned closes = 0;
    };
    ReadOnlyAssetFileInterface(std::shared_ptr<Shared> shared, std::FILE *stream);
    std::filesystem::path resolve(const String &name) const;

    std::shared_ptr<Shared> shared_;
    std::FILE *stream_ = nullptr;
};

#endif
