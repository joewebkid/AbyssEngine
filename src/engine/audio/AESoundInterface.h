#ifndef GOF2_AESOUNDINTERFACE_H
#define GOF2_AESOUNDINTERFACE_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/audio/AESoundInfo.h"

namespace AbyssEngine {
class AESoundInterface {
    public:
        virtual char *loadSound(const char *name) = 0;

        virtual void unloadSound() = 0;

        virtual void play() = 0;

        virtual void play(float volume) = 0;

        virtual void playLoop() = 0;

        virtual void pause() = 0;

        virtual void resume() = 0;

        virtual void stop() = 0;

        virtual int isPlaying() = 0;

        virtual int loaded() = 0;

        virtual void setGain(int gain) = 0;

        virtual void setVolume(int volume) = 0;

        virtual void setSoundVolume(int volume) = 0;

        virtual void setMusicVolume(int volume) = 0;

        virtual void release() = 0;
    };
}
#endif
