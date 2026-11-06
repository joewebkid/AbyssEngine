#include "engine/audio/FModSound.h"
#include "engine/core/GameText.h"
#include "engine/file/AEFile.h"

#include <cstring>

static inline int FMOD_Event_stop(FMOD::Event *event, int immediate) { return event->stop(immediate); }

static inline int FMOD_Event_setPaused(FMOD::Event *event, int paused) { return event->setPaused(paused); }

void FMOD_fade(FModSound *self, int a, int s, float v);

static inline void FMOD_EventSystem_unload(FMOD::EventSystem *system) { system->unload(); }

static inline void FMOD_EventSystem_release(FMOD::EventSystem *system) { system->release(); }

FModSound *FMOD_Event_stop_p(FMOD::Event *event, int immediate);

static inline int FMOD_Event_setPitch(FMOD::Event *event, float pitch, int mode) { return event->setPitch(pitch, (FMOD_EVENT_PITCHUNITS) mode); }

static inline int FMOD_Event_setVolume(FMOD::Event *event, float vol) { return event->setVolume(vol); }

static inline int FMOD_Event_getProperty(FMOD::Event *event, unsigned char *prop, unsigned char *out, int b = 1) { return event->getProperty((const char *) prop, (void *) out, b); }

static inline int FMOD_EventSystem_getNumReverbPresets(FMOD::EventSystem *system, int *out) { return system->getNumReverbPresets(out); }

static inline int FMOD_Event_getParameterByIndex(FMOD::Event *event, int idx, FMOD::EventParameter **out) { return event->getParameterByIndex(idx, out); }

static inline int FMOD_EventParameter_setValue(FMOD::EventParameter *p, float v) { return p->setValue(v); }

static inline int FMOD_Event_getState(FMOD::Event *event, unsigned *out) { return event->getState(out); }

static inline int FMOD_Event_getParameter(FMOD::Event *event, const char *name, FMOD::EventParameter **out) { return event->getParameter(name, out); }

static inline int FMOD_EventSystem_getProject(FMOD::EventSystem *system, const char *name, FMOD::EventProject **out) { return system->getProject(name, out); }

static inline int FMOD_Event_getParentGroup(FMOD::Event *event, FMOD::EventGroup **out) { return event->getParentGroup(out); }
static inline int FMOD_Event_getCategory(FMOD::Event *event, FMOD::EventCategory **out = 0) { return event->getCategory(out); }

static inline int FMOD_Event_start(FMOD::Event *event) { return event->start(); }

char *property_name_pause = (char *) "pause";
char *property_name_purge = (char *) "purge";
static float FModSound_defaultPitch = 0.0f;

void FModSound::setAudioLanguage(int p1) {
#if defined(GOF2_ARM64_SILENT_AUDIO)
    (void) p1;
#else
    static const char *const langTable[2] = {"english", "deutsch"};
    if (!this->system)
        return;
    this->system->setLanguage(langTable[p1 == 1]);
#endif
}

void FModSound::updateEvent3DAttributes(int idx, Vector *a, Vector *b, bool c) {
    this->events[idx] = updateEvent3DAttributes(this->events[idx], idx, a, b, c);
}

void FModSound::stopAll() {
    for (unsigned i = 0; i < 0x8f5u; ++i) {
        if (this->system && this->events[i])
            FMOD_Event_stop(this->events[i], 1);
    }
}

void FModSound::resumeAll() {
    for (unsigned i = 0; i < 0x8f5u; ++i) {
        if (this->system && this->events[i])
            FMOD_Event_setPaused(this->events[i], 0);
    }
}

void FModSound::promptMusicCue(int p1) {
    if (!this->music)
        return;
    this->music->promptCue(p1);
}

static const float kFade = 0.1f;

void FModSound::fadeOutNow() {
    int s = this->currentMusicEvent;
    if (s == -1)
        return;
    if (this->fadeTargetMusicEvent == s)
        return;
    FMOD_fade(this, 0, s, kFade);
}

void FModSound::release() {
    if (this->system != 0) {
        FMOD_EventSystem_unload(this->system);
        FMOD_EventSystem_release(this->system);
        this->system = 0;
    }
    for (unsigned i = 0; i < 0x8f5u; ++i)
        this->events[i] = 0;
    for (int i = 0; i != 4; i++)
        this->category[i] = 0;
}

FModSound::FModSound() {
    this->system = 0;
    this->music = 0;
    this->project = nullptr;
    this->appRootDir = const_cast<char *>(AEFile::GetAppRootDir());
    this->lowMemory = 1;
    for (int i = 0; i != 4; i++)
        this->categoryEnabled[i] = 1;
    for (int i = 0; i != 0x8f5; i++)
        this->events[i] = 0;
    this->eventVel = 0;
    this->eventPos = 0;
    this->listenerPos = 0;
    this->listenerVel = 0;
    this->listenerForward = 0;
    this->listenerUp = 0;
}

FModSound *FModSound::stop(FMOD::Event *e) {
    if (e == 0)
        return this;
    return FMOD_Event_stop_p(e, 0);
}

void FModSound::setVolume(int p1, float vol) {
    if (this->system == 0)
        return;
    FMOD::EventCategory *h = this->category[p1];
    if (!h)
        return;
    h->setVolume(vol);
}

void FModSound::pauseAll() {
    for (unsigned i = 0; i < 0x8f5u; ++i) {
        if (this->system && this->events[i])
            FMOD_Event_setPaused(this->events[i], 1);
    }
}

FModSound::~FModSound() {
    release();
}

void FModSound::setDownPitch(bool down) {
    static const float kUp = 0.1f;
    float pitch = down ? -1.0f : kUp;
    this->downPitch = down ? 1 : 0;
    for (unsigned i = 0; i < 0x8f5u; ++i) {
        if (this->system && this->events[i] && isPlaying(i))
            FMOD_Event_setPitch(this->events[i], pitch, 1);
    }
}

void FModSound::stop(int p1) {
    if (p1 < 0)
        return;
    FMOD::Event *h = this->events[p1];
    if (!h)
        return;
    FMOD_Event_stop(h, 0);
}

void FModSound::setMusicParamValue(int p1, float p2) {
    if (!this->music)
        return;
    this->music->setParameterValue(p1, p2);
}

void FModSound::setSoundVolume(int p1, float vol) {
    if (this->system != 0) {
        FMOD::Event *h = this->events[p1];
        if (h)
            FMOD_Event_setVolume(h, vol);
    }
}

void FModSound::pauseAllPlaying() {
    for (unsigned i = 0; i < 0x8f5u; ++i) {
        if (this->system) {
            FMOD::Event *slot = this->events[i];
            if (slot && isPlaying(i))
                FMOD_Event_setPaused(slot, 1);
        }
    }
}

void FModSound::resume(int p1) {
    if (this->system != 0) {
        FMOD::Event *h = this->events[p1];
        if (h)
            FMOD_Event_setPaused(h, 0);
    }
}

int FModSound::getEventPauseLength(int idx) {
    int out = 0;
    if (this->project != 0 && this->system != 0 && this->categoryEnabled[0] != 0) {
        FMOD::Event *h = this->events[idx];
        if (h != 0)
            FMOD_Event_getProperty(h, (unsigned char *) property_name_pause, (unsigned char *) &out, 1);
    }
    return out;
}

void FModSound::enableReverb(int p1) {
#if defined(GOF2_ARM64_SILENT_AUDIO)
    (void) p1;
#else
    if (!this->system)
        return;
    FMOD_REVERB_PROPERTIES props;
    int count;
    if (FMOD_EventSystem_getNumReverbPresets(this->system, &count) != 0)
        return;
    if (count <= p1)
        return;
    if (this->reverbPreset == p1)
        return;
    this->reverbPreset = p1;
    if (this->system->getReverbPresetByIndex(p1, &props, 0) == 0)
        this->system->setReverbProperties(&props);
#endif
}

float FModSound::getPlayingProgress(int idx) {
    float progress = 0.0f;
    if (this->project != 0 && this->system != 0 &&
        this->categoryEnabled[0] != 0 &&
        this->events[idx] != 0) {
        FMOD_EVENT_INFO info;
        int eventIndex;
        char *name;
        this->events[idx]->getInfo(&eventIndex, &name, &info);
        progress = static_cast<float>(info.positionms) /
                   static_cast<float>(info.lengthms);
    }
    return progress;
}

void FModSound::play(int idx, Vector *pos, Vector *vel, float pitch) {
#if defined(GOF2_ARM64_SILENT_AUDIO)
    (void) idx;
    (void) pos;
    (void) vel;
    (void) pitch;
    return;
#else
    if (this->project == nullptr || (unsigned int) idx >= 0x8f5 || this->system == nullptr)
        return;

    FMOD::Event *&slot = this->events[idx];
    bool alreadyLoaded = slot != nullptr;
    FMOD::Event *event = slot;
    if (!alreadyLoaded)
        this->system->getEventBySystemID(idx, FMOD_EVENT_INFOONLY, &event);
    if (event == nullptr)
        return;

    float basePitch = this->downPitch == 0 ? FModSound_defaultPitch : -1.0f;
    event->setPitch(basePitch, (FMOD_EVENT_PITCHUNITS) 1);
    if (pitch != 0.0f)
        event->setPitch(pitch, (FMOD_EVENT_PITCHUNITS) 0);

    FMOD::EventCategory *category = nullptr;
    if (event->getCategory(&category) != FMOD_OK) {
        if (slot != nullptr) {
            FMOD::EventGroup *group = nullptr;
            if (slot->getParentGroup(&group) == FMOD_OK) {
                group->freeEventData(slot, false);
                slot = nullptr;
            }
        }
        return;
    }

    int categoryIndex = 0;
    char *categoryName = nullptr;
    category->getInfo(&categoryIndex, &categoryName);
    if (categoryIndex == 0)
        this->currentMusicEvent = idx;
    else if (categoryIndex > 2)
        return;
    if (this->categoryEnabled[categoryIndex + 1] == 0)
        return;

    bool havePos = false;
    bool haveVel = false;
    if (pos != nullptr) {
        if (this->eventPos == nullptr)
            this->eventPos = new Vector();
        *this->eventPos = *pos;
        havePos = true;
    }
    if (vel != nullptr) {
        if (this->eventVel == nullptr)
            this->eventVel = new Vector();
        *this->eventVel = *vel;
        haveVel = true;
    }

    if (!alreadyLoaded) {
        FMOD::Event *ready = nullptr;
        if (this->system->getEventBySystemID(idx, FMOD_EVENT_DEFAULT, &ready) != FMOD_OK)
            return;
        slot = ready;
    }

    if (havePos || haveVel) {
        const FMOD_VECTOR *eventPosition = havePos
                ? reinterpret_cast<const FMOD_VECTOR *>(this->eventPos) : nullptr;
        const FMOD_VECTOR *eventVelocity = haveVel
                ? reinterpret_cast<const FMOD_VECTOR *>(this->eventVel) : nullptr;
        slot->set3DAttributes(eventPosition, eventVelocity, nullptr);
    }

    int purge = 0;
    if (slot->getProperty(property_name_purge, &purge, true) == FMOD_OK && purge == 1) {
        for (unsigned i = 0; i < 5; ++i) {
            if (this->fxSlots[i] == -1) {
                this->fxSlots[i] = idx;
                break;
            }
        }
    }
    slot->start();
#endif
}

bool FModSound::resume(FMOD::Event *e) {
    bool ok = false;
    if (e != 0 && this->system != 0)
        ok = FMOD_Event_setPaused(e, 0) == 0;
    return ok;
}

void FModSound::setParamValue(FMOD::Event *e, int paramIdx, float val) {
    if (e != 0 && this->system != 0) {
        FMOD::EventParameter *p;
        FMOD_Event_getParameterByIndex(e, paramIdx, &p);
        FMOD_EventParameter_setValue(p, val);
    }
}

void FModSound::stopAllSoundFXEvents() {
    if (this->system == 0)
        return;
    FMOD::EventCategory **cats = &this->category[1];
    unsigned j = 0;
    while (true) {
        unsigned i;
        do {
            i = j;
            j = i + 1;
        } while ((j & 0x7fffffff) == 1);
        if ((j & 0x7fffffff) == 4)
            break;
        FMOD::EventCategory *c = cats[i];
        c->stopAllEvents();
    }
}

int FModSound::pause(int p1) {
    uintptr_t self = (uintptr_t) this;
    if (this->system) {
        FMOD::Event *ev = this->events[p1];
        self = (uintptr_t) ev;
        if (ev)
            return FMOD_Event_setPaused(ev, 1);
    }
    return (int) (long) self;
}

void FModSound::disableReverb() {
#if !defined(GOF2_ARM64_SILENT_AUDIO)
    static const FMOD_REVERB_PROPERTIES kOff = FMOD_PRESET_OFF;
    if (this->system) {
        FMOD_REVERB_PROPERTIES props = kOff;
        this->system->setReverbProperties(&props);
    }
#endif
    this->reverbPreset = -1;
}

int FModSound::init() {
#if defined(GOF2_ARM64_SILENT_AUDIO)
    // First arm64 startup milestone: do not open the original FMOD event bank.
    // The normal ARMv7 path below remains the comparison target.
    for (int i = 0; i != 5; ++i)
        this->fxSlots[i] = -1;
    this->system = nullptr;
    this->music = nullptr;
    this->project = nullptr;
    this->propSlot = 0;
    this->reverbPreset = -1;
    this->currentMusicEvent = -1;
    this->fadeTargetMusicEvent = -1;
    this->downPitch = 0;
    return 0;
#else
    static const char kNormalPath[25] = "data/audio/FMOD_GOF2.fev";
    static const char kLowMemoryPath[14] = "FMOD_GOF2.fev";
    static const char *const kCategories[4] = {"master", "music", "sfx", "voice"};

    for (int i = 0; i != 5; i++)
        this->fxSlots[i] = -1;
    FMOD_EventSystem_Create((FMOD_EVENTSYSTEM **) &this->system);
    this->system->init(0x20, (FMOD_INITFLAGS) 0x82, nullptr);
    setAudioLanguage(GameText::getLanguage());

    char path[0x400];
    memset(path, 0, 0x400);
    strcpy(path, this->appRootDir);
    uint8_t lowFlag = this->lowMemory;
    char *end = path + strlen(path);
    if (lowFlag == 0)
        memcpy(end, kNormalPath, sizeof(kNormalPath));
    else
        memcpy(end, kLowMemoryPath, sizeof(kLowMemoryPath));
    this->system->load(path, nullptr, nullptr);
    for (int i = 0; i != 4; i++) {
        this->category[i] = 0;
        this->system->getCategory(kCategories[i], &this->category[i]);
    }
    this->system->getProjectByIndex(0, &this->project);
    this->propSlot = 0;
    this->reverbPreset = -1;
    this->currentMusicEvent = -1;
    this->fadeTargetMusicEvent = -1;
    this->downPitch = 0;
    return 0;
#endif
}

void FModSound::setParamValue(int paramIdx, int idx, float val) {
    if (idx >= 0 && this->system != 0 && this->events[idx] != 0) {
        FMOD::EventParameter *p;
        FMOD_Event_getParameterByIndex(this->events[idx], paramIdx, &p);
        FMOD_EventParameter_setValue(p, val);
    }
}

unsigned FModSound::isChannelActive(int p1) {
    if (this->system != 0) {
        FMOD::Event *h = this->events[p1];
        if (h != 0) {
            unsigned s;
            FMOD_Event_getState(h, &s);
            return (s >> 4) & 1;
        }
    }
    return 0;
}

void FModSound::playMusicFadeOutCurrent(int p1) {
    int s = this->currentMusicEvent;
    if (s == p1)
        return;
    if (s == -1) {
        this->currentMusicEvent = p1;
        s = p1;
    }
    setParamValue(0, s, kFade);
    this->fadeTargetMusicEvent = p1;
}

void FModSound::getParam(const char *name, int idx) {
    if (this->system != 0) {
        FMOD::Event *h = this->events[idx];
        if (h != 0) {
            FMOD::EventParameter *out;
            FMOD_Event_getParameter(h, name, &out);
        }
    }
}

void FModSound::updateAll(Vector *pos, Vector *vel, Vector *forward, Vector *up) {
#if defined(GOF2_ARM64_SILENT_AUDIO)
    (void) pos;
    (void) vel;
    (void) forward;
    (void) up;
    return;
#else
    FMOD::EventSystem *system = this->system;
    if (system == 0)
        return;

    unsigned int havePos = 0;
    if (pos != 0) {
        if (this->listenerPos == 0)
            this->listenerPos = new Vector();
        *this->listenerPos = *pos;
        havePos = 1;
    }
    unsigned int haveVel = 0;
    if (vel != 0) {
        if (this->listenerVel == 0)
            this->listenerVel = new Vector();
        *this->listenerVel = *vel;
        haveVel = 1;
    }
    unsigned int haveFwd = 0;
    if (forward != 0) {
        if (this->listenerForward == 0)
            this->listenerForward = new Vector();
        *this->listenerForward = *forward;
        haveFwd = 1;
    }

    bool haveUp = false;
    if (up == 0) {
        if ((havePos | haveVel | haveFwd) == 1)
            goto afterListener;
        haveUp = false;
    } else {
        if (this->listenerUp == 0)
            this->listenerUp = new Vector();
        *this->listenerUp = *up;
        haveUp = true;
    }

    {
        Vector *pPos = havePos ? this->listenerPos : 0;
        Vector *pUp = haveUp ? this->listenerUp : 0;
        Vector *pVel = haveVel ? this->listenerVel : 0;
        Vector *pFwd = haveFwd ? this->listenerForward : 0;
        system->set3DListenerAttributes(
                0, reinterpret_cast<const FMOD_VECTOR *>(pPos),
                reinterpret_cast<const FMOD_VECTOR *>(pUp),
                reinterpret_cast<const FMOD_VECTOR *>(pVel),
                reinterpret_cast<const FMOD_VECTOR *>(pFwd));
    }

afterListener:
    system->update();

    for (int i = 0; i != 5; i++) {
        int slotIdx = this->fxSlots[i];
        FMOD::Event **evp = 0;
        FMOD::Event *ev = 0;
        if (slotIdx != -1) {
            evp = &this->events[slotIdx];
            ev = *evp;
        }
        if (slotIdx != -1 && ev != 0 && isPlaying(slotIdx) == 0) {
            FMOD::EventGroup *group = 0;
            if (FMOD_Event_getParentGroup(*evp, &group) == 0) {
                group->freeEventData(*evp, false);
                *evp = 0;
                this->fxSlots[i] = -1;
            }
        }
    }
#endif
}

unsigned FModSound::isPlaying(int p1) {
    if (this->system != 0) {
        FMOD::Event *h = this->events[p1];
        if (h != 0) {
            unsigned s;
            FMOD_Event_getState(h, &s);
            return (s >> 3) & 1;
        }
    }
    return 0;
}

void FModSound::freeAllEvents() {
    static const char kProjName[8] = "GoF2";
    if (this->system != 0) {
        FMOD::EventProject *proj = 0;
        FMOD_EventSystem_getProject(this->system, kProjName, &proj);
        if (proj != 0) {
            for (unsigned i = 0; i < 0x8f5u; ++i) {
                FMOD::Event *e = this->events[i];
                if (e) {
                    FMOD::EventGroup *grp;
                    if (FMOD_Event_getParentGroup(e, &grp) == 0) {
                        unsigned st;
                        FMOD_Event_getState(this->events[i], &st);
                        if ((st & 0x0a) == 0) {
                            FMOD::Event *ee = this->events[i];
                            FMOD_Event_stop(ee, 0);
                            this->events[i] = 0;
                        }
                    }
                }
            }
        }
    }
    delete this->listenerPos;
    this->listenerPos = 0;
    delete this->listenerVel;
    this->listenerVel = 0;
    delete this->listenerForward;
    this->listenerForward = 0;
    delete this->listenerUp;
    this->listenerUp = 0;
    delete this->eventPos;
    this->eventPos = 0;
    delete this->eventVel;
    this->eventVel = 0;
}

uint8_t FModSound::IsCategoryEnabled(int p1) {
    uint8_t r = 0;
    if (p1 <= 3 && this->system != 0 && this->categoryEnabled[p1] != 0)
        r = 1;
    return r;
}

void FModSound::setParamValue(const char *name, int idx, float val) {
    if (idx >= 0 && this->system != 0 && this->events[idx] != 0) {
        FMOD::EventParameter *p;
        FMOD_Event_getParameter(this->events[idx], name, &p);
        FMOD_EventParameter_setValue(p, val);
    }
}

int FModSound::pause(FMOD::Event *e) {
    unsigned r = 0;
    if (e != 0 && this->system != 0) {
        unsigned s;
        int st = FMOD_Event_getState(e, &s);
        if (st == 0 && (int) (s << 0x1c) < 0)
            r = FMOD_Event_setPaused(e, 1) == 0;
    }
    return r;
}

FMOD::Event *FModSound::updateEvent3DAttributes(FMOD::Event *event, int idx, Vector *pos, Vector *vel, bool restart) {
#if defined(GOF2_ARM64_SILENT_AUDIO)
    (void) idx;
    (void) pos;
    (void) vel;
    (void) restart;
    return event;
#else
    if (this->project == nullptr || this->categoryEnabled[0] == 0)
        return event;

    FMOD::EventCategory *category = nullptr;
    int categoryIndex = 0;
    char *categoryName = nullptr;

    if (event != nullptr) {
        FMOD_RESULT result = event->getCategory(&category);
        if (result != FMOD_ERR_INVALID_HANDLE) {
            if (result != FMOD_OK)
                return nullptr;
            if (category->getInfo(&categoryIndex, &categoryName) != FMOD_OK)
                return event;
            if (this->categoryEnabled[categoryIndex + 1] == 0)
                return event;

            bool havePos = false;
            bool haveVel = false;
            if (pos != nullptr) {
                if (this->eventPos == nullptr)
                    this->eventPos = new Vector();
                *this->eventPos = *pos;
                havePos = true;
            }
            if (vel != nullptr) {
                if (this->eventVel == nullptr)
                    this->eventVel = new Vector();
                *this->eventVel = *vel;
                haveVel = true;
            }
            if (!havePos && !haveVel)
                return nullptr;

            const FMOD_VECTOR *eventPosition = havePos
                    ? reinterpret_cast<const FMOD_VECTOR *>(this->eventPos) : nullptr;
            const FMOD_VECTOR *eventVelocity = haveVel
                    ? reinterpret_cast<const FMOD_VECTOR *>(this->eventVel) : nullptr;
            if (event->set3DAttributes(eventPosition, eventVelocity, nullptr) != FMOD_OK)
                return nullptr;
            if (restart) {
                unsigned state = 0;
                event->getState(&state);
                if ((state & 8u) != 0)
                    event->stop(true);
                event->start();
            }
            return event;
        }
    }

    FMOD::Event *info = nullptr;
    if (this->system->getEventBySystemID(
            idx, FMOD_EVENT_INFOONLY, &info) != FMOD_OK)
        return nullptr;
    if (info->getCategory(&category) != FMOD_OK)
        return nullptr;
    if (category->getInfo(&categoryIndex, &categoryName) != FMOD_OK)
        return nullptr;
    if (this->categoryEnabled[categoryIndex + 1] == 0)
        return nullptr;

    bool havePos = false;
    bool haveVel = false;
    if (pos != nullptr) {
        if (this->eventPos == nullptr)
            this->eventPos = new Vector();
        *this->eventPos = *pos;
        havePos = true;
    }
    if (vel != nullptr) {
        if (this->eventVel == nullptr)
            this->eventVel = new Vector();
        *this->eventVel = *vel;
        haveVel = true;
    }
    if (!havePos && !haveVel)
        return nullptr;

    const FMOD_VECTOR *eventPosition = havePos
            ? reinterpret_cast<const FMOD_VECTOR *>(this->eventPos) : nullptr;
    const FMOD_VECTOR *eventVelocity = haveVel
            ? reinterpret_cast<const FMOD_VECTOR *>(this->eventVel) : nullptr;
    if (info->set3DAttributes(eventPosition, eventVelocity, nullptr) != FMOD_OK)
        return nullptr;

    FMOD::Event *ready = nullptr;
    if (this->system->getEventBySystemID(
            idx, FMOD_EVENT_DEFAULT, &ready) != FMOD_OK)
        return nullptr;
    ready->start();
    return ready;
#endif
}

void FModSound::pauseAllPlayingSoundFXEvents() {
    for (unsigned i = 0; i < 0x8f5u; ++i) {
        if (this->system) {
            FMOD::Event *h = this->events[i];
            if (h && isPlaying(i)) {
                FMOD::EventCategory *cat;
                FMOD_Event_getCategory(h, &cat);
                int categoryIndex = 0;
                char *categoryName = 0;
                cat->getInfo(&categoryIndex, &categoryName);
                if (categoryIndex == 1)
                    FMOD_Event_setPaused(this->events[i], 1);
            }
        }
    }
}

void FModSound::enableCategory(int p1, bool enable) {
    if (p1 > 3)
        return;
    if (this->system == 0)
        return;
    FMOD::EventCategory *cat = this->category[p1];
    if (cat == 0)
        return;

    this->categoryEnabled[p1] = enable ? 1 : 0;
    if (!enable) {
        cat->stopAllEvents();
    } else if (p1 == 1 && this->currentMusicEvent >= 0) {
        this->play(this->currentMusicEvent, nullptr, nullptr, 0.0f);
    }

    uint8_t any = 0;
    int i = 1;
    do {
        if ((unsigned) i > 3)
            break;
        uint8_t v = this->categoryEnabled[i];
        i++;
        any = (any || v) ? 1 : 0;
    } while (this->categoryEnabled[i - 1] == 0);
    this->categoryEnabled[0] = any;
}

int FModSound::tryToStopMusicForBGMusic() {
    return 0;
}

bool FModSound::isHeadsetPluggedIn() {
    return false;
}

void FModSound::ERRCHECK(FMOD_RESULT /*result*/) {
}
