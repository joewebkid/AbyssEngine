#include "game/core/String.h"
#include "engine/core/GameText.h"
#include <jni.h>

#include <cassert>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using AbyssEngine::String;

int GameText::getLanguage() { std::abort(); }
void String_printImpl(const char *) { std::abort(); }
namespace AbyssEngine {
unsigned short *computeFloatString(float, int, int *, int *, int) { std::abort(); }
}

struct zip_file {};
uint32_t zip_fread(zip_file *, void *, uint32_t) { std::abort(); }

class FileInterfaceAndroid {
public:
    void *file = nullptr;
    zip_file *zipFile = nullptr;
    jobject jniStream = nullptr;
    static void *env;
    static void *methodRead;
    uint32_t Read(uint32_t n, void *buf);
};

void *FileInterfaceAndroid::env = nullptr;
void *FileInterfaceAndroid::methodRead = nullptr;

#include "file_android_jni_read_under_test.inc"

static struct {
    jint got;
    bool exception;
    int copied;
    int described;
    int cleared;
    int deleted;
} state;
static int arrayToken;
static int streamToken;
static int methodToken;

static jbyteArray NewByteArray(JNIEnv *, jsize n) {
    assert(n == 4);
    return reinterpret_cast<jbyteArray>(&arrayToken);
}

static jint CallIntMethodV(JNIEnv *, jobject stream, jmethodID method, va_list args) {
    assert(stream == reinterpret_cast<jobject>(&streamToken));
    assert(method == reinterpret_cast<jmethodID>(&methodToken));
    assert(va_arg(args, jbyteArray) == reinterpret_cast<jbyteArray>(&arrayToken));
    return state.got;
}

static jthrowable ExceptionOccurred(JNIEnv *) {
    return state.exception ? reinterpret_cast<jthrowable>(&arrayToken) : nullptr;
}
static void ExceptionDescribe(JNIEnv *) { ++state.described; }
static void ExceptionClear(JNIEnv *) { ++state.cleared; state.exception = false; }
static void GetByteArrayRegion(JNIEnv *, jbyteArray array, jsize start, jsize len, jbyte *out) {
    assert(array == reinterpret_cast<jbyteArray>(&arrayToken) && start == 0 && len == 4);
    const jbyte input[4] = {1, 2, 3, 4};
    std::memcpy(out, input, sizeof input);
    ++state.copied;
}
static void DeleteLocalRef(JNIEnv *, jobject ref) {
    assert(ref == reinterpret_cast<jobject>(&arrayToken));
    ++state.deleted;
}

int main() {
    JNINativeInterface table{};
    table.NewByteArray = NewByteArray;
    table.CallIntMethodV = CallIntMethodV;
    table.ExceptionOccurred = ExceptionOccurred;
    table.ExceptionDescribe = ExceptionDescribe;
    table.ExceptionClear = ExceptionClear;
    table.GetByteArrayRegion = GetByteArrayRegion;
    table.DeleteLocalRef = DeleteLocalRef;
    JNIEnv jenv{};
    jenv.functions = &table;
    FileInterfaceAndroid::env = &jenv;
    FileInterfaceAndroid::methodRead = &methodToken;
    FileInterfaceAndroid file;
    file.jniStream = reinterpret_cast<jobject>(&streamToken);
    jbyte output[4] = {-1, -1, -1, -1};

    state = {4, false, 0, 0, 0, 0};
    assert(file.Read(4, output) == 1);
    assert(output[0] == 1 && output[1] == 2 && output[2] == 3 && output[3] == 4);
    assert(state.copied == 1 && state.described == 0 && state.cleared == 0 && state.deleted == 1);

    state = {2, false, 0, 0, 0, 0};
    assert(file.Read(4, output) == 0);
    assert(state.copied == 0 && state.described == 0 && state.cleared == 0 && state.deleted == 1);

    state = {4, true, 0, 0, 0, 0};
    assert(file.Read(4, output) == 0);
    assert(state.copied == 0 && state.described == 1 && state.cleared == 1 && state.deleted == 1);
}
