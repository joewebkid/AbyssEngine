#include <jni.h>

// Keep these small JNI entrypoints separate from RecordHandler's larger
// 64-bit port so Android can load the startup path while it is recovered.
const char *g_android_origami_super_club = nullptr;

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_GOF2HD2012_SetOrigamiSuperClub(
    JNIEnv *env, jobject /*thiz*/, jstring value) {
    jboolean isCopy;
    g_android_origami_super_club = env->GetStringUTFChars(value, &isCopy);
}

extern "C" void Java_net_fishlabs_gof2hdallandroid2012_GOF2HD2012_ReleaseOrigamiSuperClub(
    JNIEnv *env, jobject /*thiz*/, jstring value) {
    env->ReleaseStringUTFChars(value, g_android_origami_super_club);
}
