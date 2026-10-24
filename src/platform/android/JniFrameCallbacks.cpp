#include <jni.h>

extern "C" void ndk23_newrender(long long now);
extern "C" void ndk23_resize(int width, int height);

extern "C" void ndk23_setDisplayHeightAndWidth(int /*width*/, int /*height*/) {
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_renderstep(
    JNIEnv * /*env*/, jclass /*clazz*/, jlong now) {
    ndk23_newrender(now);
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_ToJNI_resize(
    JNIEnv * /*env*/, jclass /*clazz*/, jint width, jint height) {
    ndk23_resize(width, height);
}
