#ifndef GOF2_FMODSOUND_H
#define GOF2_FMODSOUND_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/math/Vector.h"
#include "fmod_event.hpp"

#include "engine/math/AEMath.h"


using AbyssEngine::AEMath::Vector;

class FModSound {
public:
    int currentMusicEvent;
    int fadeTargetMusicEvent;
    int downPitch;
    char *appRootDir;
    uint8_t lowMemory;
    uint8_t categoryEnabled[4];
    FMOD::Event *events[0x8f5];
    FMOD::EventCategory *category[4];
    FMOD::EventSystem *system;
    FMOD::MusicSystem *music;
    int initialized;
    int reverbPreset;
    int propSlot;
    int fxSlots[5];
    Vector *listenerPos;
    Vector *listenerVel;
    Vector *listenerForward;
    Vector *listenerUp;
    Vector *eventPos;
    Vector *eventVel;

    FModSound();

    ~FModSound();

    void setAudioLanguage(int p1);

    void updateEvent3DAttributes(int idx, Vector *a, Vector *b, bool c);

    FMOD::Event *updateEvent3DAttributes(FMOD::Event *event, int idx, Vector *pos, Vector *vel, bool restart);

    void stopAll();

    void resumeAll();

    void promptMusicCue(int p1);

    void fadeOutNow();

    void release();

    FModSound *stop(FMOD::Event *e);

    void stop(int p1);

    void setVolume(int p1, float vol);

    void pauseAll();

    void setDownPitch(bool down);

    void setMusicParamValue(int p1, float p2);

    void setSoundVolume(int p1, float vol);

    void pauseAllPlaying();

    void resume(int p1);

    bool resume(FMOD::Event *e);

    int getEventPauseLength(int idx);

    void enableReverb(int p1);

    float getPlayingProgress(int idx);

    void play(int idx, Vector *pos, Vector *vel, float pitch);

    void setParamValue(FMOD::Event *e, int paramIdx, float val);

    void setParamValue(int paramIdx, int idx, float val);

    void setParamValue(const char *name, int idx, float val);

    void stopAllSoundFXEvents();

    int pause(int p1);

    int pause(FMOD::Event *e);

    void disableReverb();

    int init();

    unsigned isChannelActive(int p1);

    void playMusicFadeOutCurrent(int p1);

    void getParam(const char *name, int idx);

    void updateAll(Vector *pos, Vector *vel, Vector *forward, Vector *up);

    unsigned isPlaying(int p1);

    void freeAllEvents();

    uint8_t IsCategoryEnabled(int p1);

    void pauseAllPlayingSoundFXEvents();

    void enableCategory(int p1, bool enable);

    int tryToStopMusicForBGMusic();

    bool isHeadsetPluggedIn();

    void ERRCHECK(FMOD_RESULT result);
};
#endif
