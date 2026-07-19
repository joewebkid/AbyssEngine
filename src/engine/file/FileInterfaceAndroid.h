#ifndef GOF2_FILEINTERFACEANDROID_H
#define GOF2_FILEINTERFACEANDROID_H
#include <cstdio>
#include <zip.h>

#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/file/AEFile.h"

using AbyssEngine::String;

typedef struct _jobject *jobject;

class FileInterfaceAndroid : public FileInterface {
public:
    static void **gZipMain;
    static void **gZipPatch;

    void *file;
    void *zipFile;
    void *jniStream;
    uint8_t modeFlag;
    char pad_15[7];
    int32_t zipReadPos;
    char pad_20[4];
    uint8_t zipAppend;
    char pad_25[3];
    int32_t zipReadLen;
    char pad_2c[4];
    void *appRootDir;
    void *zipDirectory;

    FileInterfaceAndroid();

    FileInterfaceAndroid(FILE *f, bool append);

    FileInterfaceAndroid(jobject stream, bool reading);

    FileInterfaceAndroid(zip_file *zf, bool append, int start, int p4, int p5);

    ~FileInterfaceAndroid() override;

    void *OpenRead(String name, int p2, bool p3, int p4, int p5, unsigned int p6) override;

    void *OpenWrite(String name, int p2, bool p3, unsigned int p4) override;

    uint32_t Read(uint32_t n, void *buf) override;

    uint32_t Write(uint32_t n, const void *buf) override;

    uint32_t Seek(uint32_t n) override;

    uint32_t GetFileSize() override;

    uint32_t FileExist(String name) override;

    const char *GetAppRootDir() override;

    uint32_t GetDeviceFreeSpace() override;

    void SetAppRootDir(void *p) override;

    void SetZipDirectory(void *p) override;

    void ResetSaveDirectory() override;

    void Close() override;

    void *OpenAppend(String name, int p2, bool p3, unsigned int p4) override;

    char *Output(char *line) override;

    uint32_t FileDelete(String name) override;

    uint32_t FileEnumInit(char *pattern, bool recurse) override;

    uint32_t FileGetNextEnum(String &out) override;

    void SetSaveDirectory(String dir) override;

    String GetDirPreFix() override;

    // Static data members present in the original binary (defined for symbol parity).
    static void *methodRead;
    static int fileCounter;
    static void *methodWrite;
    static void *methodCloseRead;
    static void *methodFileExist;
    static void *methodCloseWrite;
    static void *env;
    static void *clazz;
    static void *context;
};

char *logi(char *message);

char *loge(char *message);

#endif
