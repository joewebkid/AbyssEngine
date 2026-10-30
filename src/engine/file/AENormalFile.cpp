#include "engine/file/AENormalFile.h"

AENormalFile::AENormalFile(FileInterface *file)
    : file(file) {
}

AENormalFile::~AENormalFile() {
    Release();
}

uint32_t AENormalFile::Read(uint32_t bytes, void *buffer) {
    if (file != nullptr) {
        return file->Read(bytes, buffer);
    }
    return 0;
}

uint32_t AENormalFile::Write(uint32_t bytes, void *buffer) {
    if (file != nullptr) {
        return file->Write(bytes, buffer);
    }
    return 0;
}

uint32_t AENormalFile::Skip(uint32_t bytes) {
    if (file != nullptr) {
        return file->Seek(bytes);
    }
    return 0;
}

uint32_t AENormalFile::GetFileSize() {
    if (file != nullptr) {
        return file->GetFileSize();
    }
    return 0;
}

uint32_t AENormalFile::Release() {
    if (file != nullptr) {
        // Original ARM closes the stream before invoking its deleting destructor.
        file->Close();
        delete file;
    }
    file = nullptr;
    return 1;
}
