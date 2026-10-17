#include "host/ReadOnlyAssetFileInterface.h"
#include <algorithm>
#include <limits>

namespace {
bool within(const std::filesystem::path &root, const std::filesystem::path &path) {
    auto parent = root.begin();
    auto child = path.begin();
    while (parent != root.end()) {
        if (child == path.end() || *parent != *child) return false;
        ++parent;
        ++child;
    }
    return true;
}
}

ReadOnlyAssetFileInterface::ReadOnlyAssetFileInterface()
    : shared_(std::make_shared<Shared>()) { enabled = 1; }

ReadOnlyAssetFileInterface::ReadOnlyAssetFileInterface(
    std::shared_ptr<Shared> shared, std::FILE *stream)
    : shared_(std::move(shared)), stream_(stream) { enabled = 1; }

ReadOnlyAssetFileInterface::~ReadOnlyAssetFileInterface() { Close(); }

bool ReadOnlyAssetFileInterface::addMount(
    const std::string &prefix, const std::filesystem::path &directory) {
    if (prefix.empty() || prefix.back() != '/' || prefix.front() == '/' ||
        prefix.find('\\') != std::string::npos || prefix.find(':') != std::string::npos ||
        prefix.find("//") != std::string::npos) return false;
    for (const auto &part : std::filesystem::path(prefix)) {
        if (part == "." || part == "..") return false;
    }
    for (const auto &mount : shared_->mounts) {
        if (mount.prefix == prefix) return false;
    }
    std::error_code error;
    auto root = std::filesystem::canonical(directory, error);
    if (error || !std::filesystem::is_directory(root)) return false;
    shared_->mounts.push_back({prefix, std::move(root)});
    std::sort(shared_->mounts.begin(), shared_->mounts.end(),
              [](const Mount &a, const Mount &b) {
                  return a.prefix.size() > b.prefix.size();
              });
    return true;
}

unsigned ReadOnlyAssetFileInterface::opens() const { return shared_->opens; }
unsigned ReadOnlyAssetFileInterface::closes() const { return shared_->closes; }

std::filesystem::path ReadOnlyAssetFileInterface::resolve(const String &name) const {
    char *raw = name.GetAEChar();
    if (!raw) return {};
    std::string logical(raw);
    delete[] raw;
    for (const auto &mount : shared_->mounts) {
        if (logical.compare(0, mount.prefix.size(), mount.prefix) != 0) continue;
        auto suffix = logical.substr(mount.prefix.size());
        if (suffix.empty() || suffix.find('\\') != std::string::npos ||
            suffix.find(':') != std::string::npos || suffix.find("//") != std::string::npos)
            return {};
        std::filesystem::path relative(suffix);
        if (relative.is_absolute()) return {};
        for (const auto &part : relative) {
            if (part == "." || part == "..") return {};
        }
        std::error_code error;
        auto candidate = std::filesystem::canonical(mount.directory / relative, error);
        if (!error && within(mount.directory, candidate) &&
            std::filesystem::is_regular_file(candidate)) return candidate;
        return {};
    }
    return {};
}

void *ReadOnlyAssetFileInterface::OpenRead(
    String name, int, bool windowed, int packedSize, int rawSize, unsigned offset) {
    if (windowed || packedSize || rawSize || offset) return nullptr;
    const auto path = resolve(name);
    if (path.empty()) return nullptr;
    std::FILE *stream = std::fopen(path.string().c_str(), "rb");
    if (!stream) return nullptr;
    ++shared_->opens;
    return new ReadOnlyAssetFileInterface(shared_, stream);
}

uint32_t ReadOnlyAssetFileInterface::FileExist(String name) {
    return !resolve(name).empty();
}

uint32_t ReadOnlyAssetFileInterface::Read(uint32_t bytes, void *buffer) {
    return stream_ && std::fread(buffer, 1, bytes, stream_) == bytes;
}

uint32_t ReadOnlyAssetFileInterface::Seek(uint32_t bytes) {
    return stream_ && bytes <= static_cast<uint32_t>(std::numeric_limits<long>::max()) &&
           std::fseek(stream_, static_cast<long>(bytes), SEEK_CUR) == 0;
}

uint32_t ReadOnlyAssetFileInterface::GetFileSize() {
    if (!stream_) return 0;
    const long position = std::ftell(stream_);
    if (position < 0 || std::fseek(stream_, 0, SEEK_END) != 0) return 0;
    const long size = std::ftell(stream_);
    if (std::fseek(stream_, position, SEEK_SET) != 0 || size < 0 ||
        static_cast<unsigned long>(size) > std::numeric_limits<uint32_t>::max()) return 0;
    return static_cast<uint32_t>(size);
}

void ReadOnlyAssetFileInterface::Close() {
    if (stream_) {
        std::fclose(stream_);
        stream_ = nullptr;
        ++shared_->closes;
    }
}

void *ReadOnlyAssetFileInterface::OpenWrite(String, int, bool, unsigned) { return nullptr; }
void *ReadOnlyAssetFileInterface::OpenAppend(String, int, bool, unsigned) { return nullptr; }
uint32_t ReadOnlyAssetFileInterface::Write(uint32_t, const void *) { return 0; }
uint32_t ReadOnlyAssetFileInterface::FileDelete(String) { return 0; }
uint32_t ReadOnlyAssetFileInterface::GetDeviceFreeSpace() { return 0; }
const char *ReadOnlyAssetFileInterface::GetAppRootDir() { return ""; }
void ReadOnlyAssetFileInterface::SetAppRootDir(void *) {}
void ReadOnlyAssetFileInterface::SetZipDirectory(void *) {}
uint32_t ReadOnlyAssetFileInterface::FileEnumInit(char *, bool) { return 0; }
uint32_t ReadOnlyAssetFileInterface::FileGetNextEnum(String &) { return 0; }
String ReadOnlyAssetFileInterface::GetDirPreFix() { return String(""); }
char *ReadOnlyAssetFileInterface::Output(char *line) { return line; }
void ReadOnlyAssetFileInterface::SetSaveDirectory(String) {}
void ReadOnlyAssetFileInterface::ResetSaveDirectory() {}
