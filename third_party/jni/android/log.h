#ifndef GOF2_STUB_ANDROID_LOG_H
#define GOF2_STUB_ANDROID_LOG_H
/* Minimal stub of <android/log.h> for the native (non-Android) dev build.
   The real NDK header is used in the match/.so build via the NDK sysroot. */
#ifdef __cplusplus
extern "C" {
#endif
enum android_LogPriority {
    ANDROID_LOG_UNKNOWN = 0, ANDROID_LOG_DEFAULT, ANDROID_LOG_VERBOSE,
    ANDROID_LOG_DEBUG, ANDROID_LOG_INFO, ANDROID_LOG_WARN,
    ANDROID_LOG_ERROR, ANDROID_LOG_FATAL, ANDROID_LOG_SILENT
};
int __android_log_print(int prio, const char *tag, const char *fmt, ...);
int __android_log_write(int prio, const char *tag, const char *text);
#ifdef __cplusplus
}
#endif
#endif
