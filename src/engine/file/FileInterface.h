#ifndef GALAXYONFIRE2_FILEINTERFACE_H
#define GALAXYONFIRE2_FILEINTERFACE_H


#include "engine/core/AEString.h"

class FileInterface {
public:
    uint8_t enabled;

    virtual ~FileInterface() {
    }

    virtual void *OpenRead(String name, int size, bool windowed, int packedSize, int rawSize, unsigned int offset) = 0;

    virtual void *OpenWrite(String name, int size, bool append, unsigned int mode) = 0;

    virtual void *OpenAppend(String name, int size, bool append, unsigned int mode) = 0;

    virtual uint32_t Read(uint32_t bytes, void *buffer) = 0;

    virtual uint32_t Write(uint32_t bytes, const void *buffer) = 0;

    virtual uint32_t Seek(uint32_t bytes) = 0;

    virtual uint32_t GetFileSize() = 0;

    virtual uint32_t FileExist(String name) = 0;

    virtual uint32_t FileDelete(String name) = 0;

    virtual uint32_t GetDeviceFreeSpace() = 0;

    virtual const char *GetAppRootDir() = 0;

    virtual void SetAppRootDir(void *path) = 0;

    virtual void SetZipDirectory(void *path) = 0;

    virtual uint32_t FileEnumInit(char *pattern, bool recurse) = 0;

    virtual uint32_t FileGetNextEnum(String &out) = 0;

    virtual void Close() = 0;

    virtual String GetDirPreFix() = 0;

    virtual char *Output(char *line) = 0;

    virtual void SetSaveDirectory(String dir) = 0;

    virtual void ResetSaveDirectory() = 0;
};

#endif //GALAXYONFIRE2_FILEINTERFACE_H
