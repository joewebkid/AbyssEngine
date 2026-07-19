#ifndef GOF2_AEFILE_H
#define GOF2_AEFILE_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "FileInterface.h"

#include "game/core/String.h"

#include "AELowLevelFile.h"
#include "AENormalFile.h"
#include "AEPakFile.h"


#include "engine/file/AEPakFileEntry.h"
class AELowLevelFile;
class FileInterface;


using String = AbyssEngine::String;


void *OpenAppend(unsigned short *name, int size, bool append, unsigned int mode);

class AEFile {
public:
    enum FileOpenType : uint32_t {
        OPEN_READ = 0,
        OPEN_WRITE = 1,
        OPEN_APPEND = 2,
    };

    static void SetInterface(FileInterface *fileInterface);

    static void Release();

    static uint32_t Open(String &path, FileOpenType openType, uint32_t *handle);

    static uint32_t OpenRead(String &path, uint32_t *handle);

    static uint32_t OpenRead(const char *path, uint32_t *handle);

    static uint32_t OpenWrite(String &path, uint32_t *handle);

    static uint32_t OpenWrite(const char *path, uint32_t *handle);

    static uint32_t OpenAppend(String &path, uint32_t *handle);

    static uint32_t OpenAppend(const char *path, uint32_t *handle);

    static void Close(uint32_t handle);

    static uint32_t Read(uint32_t bytes, void *buffer, uint32_t handle);

    static uint32_t Read(bool &value, uint32_t handle);

    static uint32_t Read(char &value, uint32_t handle);

    static uint32_t Read(int8_t &value, uint32_t handle);

    static uint32_t Read(uint8_t &value, uint32_t handle);

    static uint32_t Read(int16_t &value, uint32_t handle);

    static uint32_t Read(uint16_t &value, uint32_t handle);

    static uint32_t Read(int32_t &value, uint32_t handle);

    static uint32_t Read(uint32_t &value, uint32_t handle);

    static uint32_t Read(int64_t &value, uint32_t handle);

    static uint32_t Read(float &value, uint32_t handle);

    static uint32_t Read(String &value, uint32_t handle, bool wide);

    static uint32_t ReadSwitched(int16_t &value, uint32_t handle);

    static uint32_t ReadSwitched(uint16_t &value, uint32_t handle);

    static uint32_t ReadSwitched(int32_t &value, uint32_t handle);

    static void ReadSwitched(String &value, uint32_t handle);

    static uint32_t Write(uint32_t bytes, void *buffer, uint32_t handle);

    static void Write(bool value, uint32_t handle);

    static void Write(char value, uint32_t handle);

    static void Write(int8_t value, uint32_t handle);

    static void Write(uint8_t value, uint32_t handle);

    static void Write(int16_t value, uint32_t handle);

    static void Write(uint16_t value, uint32_t handle);

    static void Write(int32_t value, uint32_t handle);

    static void Write(uint32_t value, uint32_t handle);

    static void Write(int64_t value, uint32_t handle);

    static void Write(float value, uint32_t handle);

    static uint32_t Write(const String &value, uint32_t handle, bool wide);

    static uint32_t Skip(uint32_t bytes, uint32_t handle);

    static uint32_t GetFileSize(uint32_t handle);

    static void RegisterPakFile(String path);

    static void collectPakFiles(const String &path);

    static void collectFilesInPakFiles(String &path);

    static void sortPakFileEntryList();

    static AELowLevelFile *findPakFile(const String &path);

    static uint32_t crc32_ccitt(const String &text);

    static uint32_t FileExist(const String &path);

    static uint32_t FileDelete(const String &path);

    static uint32_t GetDeviceFreeSpace();

    static const char *GetAppRootDir();

    static void SetAppRootDir(void *path);

    static void SetZipDirectory(void *path);

    static void SetSaveDirectory(String path);

    static void ResetSaveDirectory();

    // Static data members present in the original binary (defined for symbol parity).
    static void *fileInterface;
    static void *pakFileEntryList;
    static void *file;
    static void *appRoot;
};
#endif
