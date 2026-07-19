#ifndef GOF2_AESOUND_H
#define GOF2_AESOUND_H
#include "engine/audio/AESoundInterface.h"

namespace AbyssEngine {
    // Concrete AESoundInterface implementation.
    class AESound : public AESoundInterface {
    public:
        char *loadSound(const char *name) override;

        void unloadSound() override;

        void play() override;

        void play(float volume) override;

        void playLoop() override;

        void pause() override;

        void resume() override;

        void stop() override;

        int isPlaying() override;

        int loaded() override;

        void setGain(int gain) override;

        void setVolume(int volume) override;

        void setSoundVolume(int volume) override;

        void setMusicVolume(int volume) override;

        void release() override;
    };
}

#endif
