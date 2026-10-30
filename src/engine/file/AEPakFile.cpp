#include "engine/file/AEPakFile.h"

AEPakFile::AEPakFile(FileInterface *fileInterface, int sizeLimit, int rawSize)
    : fileInterface(fileInterface),
      sizeLimit(static_cast<uint32_t>(sizeLimit)),
      rawSize(static_cast<uint32_t>(rawSize)),
      position(0) {
}

AEPakFile::~AEPakFile() {
    Release();
}

uint32_t AEPakFile::Read(uint32_t bytes, void *buffer) {
    FileInterface *file;
    if (bytes != 0 && (file = fileInterface) != nullptr) {
        if (static_cast<int>(position + bytes) > static_cast<int>(sizeLimit)) {
            bytes = static_cast<uint32_t>(static_cast<int>(sizeLimit) - static_cast<int>(position));
        }
        if (bytes != 0) {
            position += bytes;
            return file->Read(bytes, buffer);
        }
    }
    return 0;
}

uint32_t AEPakFile::Write(uint32_t, void *) {
    return 0;
}

uint32_t AEPakFile::Skip(uint32_t bytes) {
    char *buffer = new char[bytes];
    Read(bytes, buffer);
    delete[] buffer;
    return 1;
}

uint32_t AEPakFile::GetFileSize() {
    return sizeLimit;
}

uint32_t AEPakFile::Release() {
    if (fileInterface != nullptr) {
        // Original ARM closes the stream before invoking its deleting destructor.
        fileInterface->Close();
        delete fileInterface;
        fileInterface = nullptr;
    }
    return 1;
}
