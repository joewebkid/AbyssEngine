#ifndef GOF2_AESOUNDRESSOURCE_H
#define GOF2_AESOUNDRESSOURCE_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/audio/AESoundInfo.h"
#include "engine/audio/AESoundInterface.h"
namespace AbyssEngine {
    
    

    class AESoundRessource {
    public:
        const AESoundInfo *soundInfoTable;
        Array<AESoundInterface *> *sounds;
        volatile int numSounds;

        AESoundRessource();

        ~AESoundRessource();

        void freeAllRessources();

        void SetSound(const AESoundInfo *info, int count);

        void getSoundInfo(int id, AESoundInfo &info, int &index);

        void init(int id);

        void checkLooping();

        void initWithoutLoading(int id);

        void play(int id, float volume);

        void play(int id);

        void playLoop(int id);

        void playMusic(int id);

        void playMusicLoop(int id);

        void stop(int id);

        void stop();

        void pause(int id);

        int pause();

        void resume(int id);

        void resume();

        void supend();

        void release(int id);

        bool isPlaying(int id);

        void setVolume(int id, int volume);

        void setVolume(int volume);

        void setSoundVolume(int volume);

        void setMusicVolume(int volume);
    };
}

#endif
