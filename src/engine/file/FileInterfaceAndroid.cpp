#include "engine/file/FileInterfaceAndroid.h"
#include "game/core/String.h"

#include <jni.h>

#include <cstdio>
#include <new>

unsigned int JNI_CallIntMethod(void *env, void *m, void *arg0, void *arg1);

void JNI_CallVoidMethod(void *env, void *m, void *arg, ...);

// The opaque void* JNI handles used below are JNIEnv pointers: a pointer to a
// pointer to the JNINativeInterface function table.  Convert each raw offset
// load into a named function-table member access.  The byte offsets used by the
// decompiler match the standard JNINativeInterface layout (32-bit pointers):
//   0x7c=GetObjectClass 0x84=GetMethodID 0x2c0=NewByteArray
//   0x320=GetByteArrayRegion 0x340=SetByteArrayRegion
//   0x3c=ExceptionOccurred 0x40=ExceptionDescribe 0x44=ExceptionClear
//   0x5c=DeleteLocalRef
static inline const JNINativeInterface *JniTable(void *env) {
    return *reinterpret_cast<const JNINativeInterface *const *>(env);
}

#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(JNINativeInterface, GetObjectClass) == 0x7c, "JNI layout");
static_assert(offsetof(JNINativeInterface, GetMethodID) == 0x84, "JNI layout");
static_assert(offsetof(JNINativeInterface, NewByteArray) == 0x2c0, "JNI layout");
static_assert(offsetof(JNINativeInterface, GetByteArrayRegion) == 0x320, "JNI layout");
static_assert(offsetof(JNINativeInterface, SetByteArrayRegion) == 0x340, "JNI layout");
static_assert(offsetof(JNINativeInterface, ExceptionOccurred) == 0x3c, "JNI layout");
static_assert(offsetof(JNINativeInterface, ExceptionDescribe) == 0x40, "JNI layout");
static_assert(offsetof(JNINativeInterface, ExceptionClear) == 0x44, "JNI layout");
static_assert(offsetof(JNINativeInterface, DeleteLocalRef) == 0x5c, "JNI layout");
#endif

static inline const unsigned short *GetAEWChar(const String &s) {
    return reinterpret_cast<const unsigned short *>(s.text());
}

static int *gFIAInstCount = nullptr;

static const char kDirPreFix[] = "";

String FileInterfaceAndroid::GetDirPreFix() {
    return String(kDirPreFix);
}

FileInterfaceAndroid::FileInterfaceAndroid() {
    this->enabled = 1;
    this->appRootDir = 0;
    this->zipDirectory = 0;
    this->zipAppend = 0;
    this->zipReadLen = 0;
}

FileInterfaceAndroid::FileInterfaceAndroid(FILE *f, bool append) {
    this->file = f;
    this->zipFile = 0;
    this->jniStream = 0;
    this->modeFlag = append;
    ++*gFIAInstCount;
}

static void **gJniEnvObj = nullptr;

static void **gMidA_read = nullptr;
static const char *gNmA_read = nullptr;
static const char *gSgA_read = nullptr;
static void **gMidA_write = nullptr;
static const char *gNmA_write = nullptr;
static const char *gSgA_write = nullptr;

static void **gMidB_read = nullptr;
static void **gMidB_write = nullptr;
static const char *gNmB = nullptr;
static const char *gSgB = nullptr;

FileInterfaceAndroid::FileInterfaceAndroid(jobject stream, bool reading) {
    void *env = *gJniEnvObj;
    this->file = 0;
    this->zipFile = 0;
    this->jniStream = stream;
    this->modeFlag = reading;
    ++*gFIAInstCount;

    JNIEnv *jenv = reinterpret_cast<JNIEnv *>(env);
    const JNINativeInterface *jni = JniTable(env);
    jobject cls = jni->GetObjectClass(jenv, stream);

    void **selB;
    if (reading) {
        if (*gMidA_read == 0)
            *gMidA_read = jni->GetMethodID(jenv, reinterpret_cast<jclass>(cls), gNmA_read, gSgA_read);
        selB = gMidB_read;
    } else {
        if (*gMidA_write == 0)
            *gMidA_write = jni->GetMethodID(jenv, reinterpret_cast<jclass>(cls), gNmA_write, gSgA_write);
        selB = gMidB_write;
    }
    if (*selB == 0)
        *selB = jni->GetMethodID(jenv, reinterpret_cast<jclass>(cls), gNmB, gSgB);
}

FileInterfaceAndroid::FileInterfaceAndroid(zip_file *zf, bool append, int start, int p4, int p5) {
    this->file = 0;
    this->zipFile = zf;
    this->jniStream = 0;
    this->modeFlag = 0;
    ++*gFIAInstCount;
    this->zipReadPos = 0;
    this->zipReadLen = 0;
    this->Seek(start);
    this->zipAppend = append;
}

FileInterfaceAndroid::~FileInterfaceAndroid() {
    this->Close();
    if (*gFIAInstCount != 0)
        --*gFIAInstCount;
    else
        this->enabled = 0;
}

static void *gJniEnv = nullptr;
static void *gModeWrite = nullptr;
static void *gModeAppend = nullptr;

void FileInterfaceAndroid::Close() {
    if (this->file != 0) {
        fclose((FILE *) this->file);
        this->file = 0;
    }
    if (this->zipFile != 0) {
        zip_fclose((zip_file *) this->zipFile);
        this->zipFile = 0;
    }
    void *m = this->jniStream;
    if (m != 0) {
        void *env = *(void **) gJniEnv;
        void *modePtr = (this->modeFlag == 0) ? gModeWrite : gModeAppend;
        JNI_CallVoidMethod(env, m, *(void **) modePtr);
        this->jniStream = 0;
    }
}

uint32_t FileInterfaceAndroid::GetFileSize() {
    fseek((FILE *) this->file, 0, SEEK_END);
    int size = ftell((FILE *) this->file);
    fseek((FILE *) this->file, 0, SEEK_SET);
    return (uint32_t) size;
}

const char *FileInterfaceAndroid::GetAppRootDir() {
    return (const char *) this->appRootDir;
}

uint32_t FileInterfaceAndroid::GetDeviceFreeSpace() {
    return 0;
}

void FileInterfaceAndroid::SetZipDirectory(void *p) {
    if (p != 0)
        this->zipDirectory = p;
}

void FileInterfaceAndroid::SetAppRootDir(void *p) {
    if (p != 0)
        this->appRootDir = p;
}

static void **gEnvR = nullptr;
static void *gReadMidArg = nullptr;

uint32_t FileInterfaceAndroid::Read(uint32_t n, void *buf) {
    if (this->zipFile != 0)
        return zip_fread((zip_file *) this->zipFile, buf, n) == n;
    if (this->file != 0)
        return fread(buf, 1, n, (FILE *) this->file) == n;
    if (this->jniStream == 0)
        return false;

    void *r9 = *gEnvR;
    void *env = *(void **) r9;
    JNIEnv *jenv = reinterpret_cast<JNIEnv *>(env);
    jbyteArray arr = JniTable(env)->NewByteArray(jenv, (jsize) n);
    unsigned int got = JNI_CallIntMethod(*(void **) r9, this->jniStream, *(void **) gReadMidArg, arr);

    bool ok;
    if (JniTable(env)->ExceptionOccurred(jenv) == 0 && got == n) {
        JniTable(env)->GetByteArrayRegion(jenv, arr, 0, (jsize) n, (jbyte *) buf);
        ok = true;
    } else {
        JniTable(env)->ExceptionDescribe(jenv);
        JniTable(env)->ExceptionClear(jenv);
        ok = false;
    }
    JniTable(env)->DeleteLocalRef(jenv, arr);
    return ok;
}

uint32_t FileInterfaceAndroid::Seek(uint32_t n) {
    if (n == 0)
        return true;
    void *zf = this->zipFile;
    int delta;
    if (zf != 0) {
        void *tmp = malloc(n);
        if (tmp == 0)
            return false;
        unsigned int got = zip_fread((zip_file *) zf, tmp, n);
        free(tmp);
        delta = got - n;
    } else {
        void *f = this->file;
        if (f == 0)
            return false;
        delta = fseek((FILE *) f, n, SEEK_CUR);
    }
    return delta == 0;
}

static void **gEnvW = nullptr;
static void *gWriteMidArg = nullptr;

uint32_t FileInterfaceAndroid::Write(uint32_t n, const void *buf) {
    if (this->file != 0)
        return fwrite(buf, 1, n, (FILE *) this->file) == n;
    if (this->jniStream == 0)
        return true;

    void *r9 = *gEnvW;
    void *envObj = *(void **) r9;
    JNIEnv *jenv = reinterpret_cast<JNIEnv *>(envObj);
    jbyteArray arr = JniTable(envObj)->NewByteArray(jenv, (jsize) n);
    JniTable(envObj)->SetByteArrayRegion(jenv, arr, 0, (jsize) n, (const jbyte *) buf);
    JNI_CallVoidMethod(envObj, this->jniStream, *(void **) gWriteMidArg, arr);
    bool ok = JniTable(envObj)->ExceptionOccurred(jenv) == 0;
    if (!ok)
        JniTable(envObj)->ExceptionClear(jenv);
    JniTable(envObj)->DeleteLocalRef(jenv, arr);
    return ok;
}

void *FileInterfaceAndroid::OpenAppend(String, int, bool, unsigned int) {
    return 0;
}

char *FileInterfaceAndroid::Output(char *line) {
    return line;
}

uint32_t FileInterfaceAndroid::FileDelete(String name) {
    String *src = *reinterpret_cast<String **>(&name.data);
    String discard(*src, false);
    return 0;
}

uint32_t FileInterfaceAndroid::FileEnumInit(char *, bool) {
    return 0;
}

uint32_t FileInterfaceAndroid::FileGetNextEnum(String &) {
    return 0;
}

void FileInterfaceAndroid::SetSaveDirectory(String) {
}

void FileInterfaceAndroid::ResetSaveDirectory() {
}

void **FileInterfaceAndroid::gZipMain = nullptr;
void **FileInterfaceAndroid::gZipPatch = nullptr;
static const char *gZipPrefixA = nullptr;
static const char *gZipPrefixB = nullptr;
static const char *gModeRb = nullptr;

uint32_t FileInterfaceAndroid::FileExist(String name) {
    String a(gZipPrefixA);
    a += name;
    String b(gZipPrefixB);
    b += name;

    void *z1 = zip_fopen((struct zip *) *gZipMain, a.GetAEChar(), 0);
    void *z2 = zip_fopen((struct zip *) *FileInterfaceAndroid::gZipPatch, b.GetAEChar(), 0);

    bool exists;
    if (z1 != 0) {
        zip_fclose((zip_file *) z1);
        exists = true;
    } else if (z2 != 0) {
        zip_fclose((zip_file *) z2);
        exists = true;
    } else {
        String dir((const char *) this->appRootDir);
        String full = dir + name;
        FILE *f = fopen(full.GetAEChar(), gModeRb);
        if (f != 0) {
            fclose(f);
            exists = true;
        } else {
            exists = false;
        }
    }
    return exists;
}

static void **gZipMainR = nullptr;
static void **gZipPatchR = nullptr;
static const char *gPrefixSlash = nullptr;
static const char *gPrefixPlain = nullptr;
static const char *gOpenReadFmt = nullptr;
static char *gStderrBase = nullptr;

// gStderrBase points to a pointer to the Android I/O state block; the stderr
// FILE* lives at byte offset 0xa8 within that block.  Model it as a named field.
struct AndroidIoState {
    char pad_00[0xa8];
    FILE *stderrFile;
};
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(AndroidIoState, stderrFile) == 0xa8, "AndroidIoState layout");
#endif
static const char *gModeRbR = nullptr;

void *FileInterfaceAndroid::OpenRead(String name, int p2, bool p3, int p4, int p5, unsigned int p6) {
    const unsigned short *w = GetAEWChar(name);
    if (this->enabled == 0)
        return 0;

    const unsigned short *body = (*w == '/') ? w + 1 : w;

    String a(gPrefixSlash);
    String wide;
    wide.Set((const unsigned short *) (body));
    a += wide;
    String b(gPrefixPlain);
    String wide2;
    wide2.Set((const unsigned short *) (body));
    b += wide2;

    AndroidIoState *ioState = *reinterpret_cast<AndroidIoState **>(gStderrBase);
    fprintf(ioState->stderrFile, gOpenReadFmt, b.GetAEChar(), p3, p4, p5, p6, p2);

    zip_file *z1 = zip_fopen((struct zip *) *gZipMainR, a.GetAEChar(), 0);
    zip_file *z2 = (*gZipPatchR != 0) ? zip_fopen((struct zip *) *gZipPatchR, b.GetAEChar(), 0) : 0;

    FileInterfaceAndroid * result = 0;
    if (z1 != 0) {
        result = new FileInterfaceAndroid(z1, (bool) p4, p2, p5, p4);
    } else if (z2 != 0) {
        result = new FileInterfaceAndroid(z2, (bool) p4, p2, p5, p4);
    } else if (this->appRootDir != 0) {
        String dir((const char *) this->appRootDir);
        String full = dir + a;
        FILE *f = fopen(full.GetAEChar(), gModeRbR);
        if (f != 0)
            result = new FileInterfaceAndroid(f, false);
    }
    return result;
}

static const char *gModeWb = nullptr;

void *FileInterfaceAndroid::OpenWrite(String name, int, bool, unsigned int) {
    const unsigned short *w = GetAEWChar(name);
    while (*w)
        ++w;

    String dir((const char *) this->appRootDir);
    String wide;
    wide.Set((const unsigned short *) (GetAEWChar(name)));
    String full = dir + wide;

    FILE *f = fopen(full.GetAEChar(), gModeWb);
    if (f == 0)
        return 0;
    return new FileInterfaceAndroid(f, true);
}

char *logi(char *message) {
    return message;
}

char *loge(char *message) {
    return message;
}

// Static data members present in the original binary (defined for symbol parity).
void *FileInterfaceAndroid::methodRead;
int FileInterfaceAndroid::fileCounter;
void *FileInterfaceAndroid::methodWrite;
void *FileInterfaceAndroid::methodCloseRead;
void *FileInterfaceAndroid::methodFileExist;
void *FileInterfaceAndroid::methodCloseWrite;
void *FileInterfaceAndroid::env;
void *FileInterfaceAndroid::clazz;
void *FileInterfaceAndroid::context;
