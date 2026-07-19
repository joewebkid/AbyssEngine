#include "engine/file/AEPakFile.h"

AEPakFile::AEPakFile(FileInterface *fileInterface, int sizeLimit, int baseOffset)
    : fileInterface(fileInterface),
      sizeLimit(static_cast<uint32_t>(sizeLimit)),
      baseOffset(static_cast<uint32_t>(baseOffset)),
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
        delete fileInterface;
        fileInterface = nullptr;
    }
    return 1;
}
