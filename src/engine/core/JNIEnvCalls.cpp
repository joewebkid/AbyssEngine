#include "engine/core/JNIEnvCalls.h"

namespace gof2 {
    const JNIEnvCallAnchors kJNIEnvCallAnchors = {
        &_JNIEnv::CallIntMethod,
        &_JNIEnv::CallVoidMethod,
        &_JNIEnv::CallStaticIntMethod,
        &_JNIEnv::CallStaticVoidMethod,
        &_JNIEnv::CallStaticBooleanMethod,
    };
}
