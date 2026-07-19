#include "engine/audio/AESound.h"

namespace AbyssEngine {
    char *AESound::loadSound(const char *name) {
        return const_cast<char *>(name);
    }

    void AESound::unloadSound() {
    }

    void AESound::play() {
    }

    void AESound::play(float volume) {
        (void) volume;
    }

    void AESound::playLoop() {
    }

    void AESound::pause() {
    }

    void AESound::resume() {
    }

    void AESound::stop() {
    }

    int AESound::isPlaying() {
        return 1;
    }

    int AESound::loaded() {
        return 1;
    }

    void AESound::setGain(int gain) {
        (void) gain;
    }

    void AESound::setVolume(int volume) {
        (void) volume;
    }

    void AESound::setSoundVolume(int volume) {
        (void) volume;
    }

    void AESound::setMusicVolume(int volume) {
        (void) volume;
    }

    void AESound::release() {
    }
}
