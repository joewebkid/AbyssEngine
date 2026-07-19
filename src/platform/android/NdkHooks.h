#ifndef GOF2_NDKHOOKS_H
#define GOF2_NDKHOOKS_H

#include <jni.h>

extern "C" {

int setBaughtCredits(int amount);

void checkFirstCreditPackBoughtWriteAction();

const char *getStringUTFChars(JNIEnv *env, jstring str);

void releaseStringUTFChars(JNIEnv *env, jstring str, const char *chars);

char *pConstToNonConst(const char *s);

void ndk23_sendingPauseSignal();

void ndk23_sendingResumeSignal();

void ndk_autosave();

int ndk_getCurrentApplicationModule();

int ndk_getLogoShown();

int ndk_isInMainMenu();

bool ndk_getDLC_1_BOUGHT();

bool ndk_getDLC_2_BOUGHT();

bool ndk_getDLC_3_BOUGHT();

bool ndk_getDLC_4_BOUGHT();

bool ndk_getDLC_5_BOUGHT();

void ndk_iapBoughtConsumable(unsigned int consumable);

void ndk_iapBoughtPremium(unsigned int pack, unsigned int bought);

void ndk_setNativeItemInformationList(JNIEnv *env, jclass clazz,
                                      jobjectArray ids, jobjectArray names,
                                      jobjectArray descriptions,
                                      jobjectArray currencies,
                                      jobjectArray prices);

} // extern "C"

#endif
