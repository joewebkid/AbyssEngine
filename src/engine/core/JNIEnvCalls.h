#ifndef GOF2_JNIENVCALLS_H
#define GOF2_JNIENVCALLS_H

#include <jni.h>

namespace gof2 {
    struct JNIEnvCallAnchors {
        jint (_JNIEnv::*callIntMethod)(jobject, jmethodID, ...);

        void (_JNIEnv::*callVoidMethod)(jobject, jmethodID, ...);

        jint (_JNIEnv::*callStaticIntMethod)(jclass, jmethodID, ...);

        void (_JNIEnv::*callStaticVoidMethod)(jclass, jmethodID, ...);

        jboolean (_JNIEnv::*callStaticBooleanMethod)(jclass, jmethodID, ...);
    };

    extern const JNIEnvCallAnchors kJNIEnvCallAnchors;
}

#endif
