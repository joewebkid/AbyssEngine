// Temporary no-output FMOD Ex adapter for the arm64 Android startup build.
// No event bank is loaded and no sound is produced. Keep this outside src/ so
// the Android ARMv7 comparison corpus continues to use the original path.
#include "engine/audio/FModSound.h"
#include "fmod_event.h"

namespace {
FMOD::EventSystem silentSystem;
}

extern "C" FMOD_RESULT F_API FMOD_EventSystem_Create(FMOD_EVENTSYSTEM **out) {
    if (out) *out = reinterpret_cast<FMOD_EVENTSYSTEM *>(&silentSystem);
    return FMOD_OK;
}

namespace FMOD {
FMOD_RESULT EventSystem::init(int, FMOD_INITFLAGS, void *, FMOD_EVENT_INITFLAGS) { return FMOD_OK; }
FMOD_RESULT EventSystem::release() { return FMOD_OK; }
FMOD_RESULT EventSystem::update() { return FMOD_OK; }
FMOD_RESULT EventSystem::unload() { return FMOD_OK; }
FMOD_RESULT EventSystem::getProject(const char *, EventProject **out) {
    if (out) *out = nullptr;
    return FMOD_ERR_EVENT_NOTFOUND;
}
FMOD_RESULT EventSystem::getNumReverbPresets(int *out) {
    if (out) *out = 0;
    return FMOD_OK;
}

FMOD_RESULT Event::start() { return FMOD_OK; }
FMOD_RESULT Event::stop(bool) { return FMOD_OK; }
FMOD_RESULT Event::setPaused(bool) { return FMOD_OK; }
FMOD_RESULT Event::setPitch(float, FMOD_EVENT_PITCHUNITS) { return FMOD_OK; }
FMOD_RESULT Event::setVolume(float) { return FMOD_OK; }
FMOD_RESULT Event::getState(FMOD_EVENT_STATE *out) {
    if (out) *out = 0;
    return FMOD_OK;
}
FMOD_RESULT Event::getCategory(EventCategory **out) {
    if (out) *out = nullptr;
    return FMOD_ERR_EVENT_NOTFOUND;
}
FMOD_RESULT Event::getParentGroup(EventGroup **out) {
    if (out) *out = nullptr;
    return FMOD_ERR_EVENT_NOTFOUND;
}
FMOD_RESULT Event::getParameter(const char *, EventParameter **out) {
    if (out) *out = nullptr;
    return FMOD_ERR_EVENT_NOTFOUND;
}
FMOD_RESULT Event::getParameterByIndex(int, EventParameter **out) {
    if (out) *out = nullptr;
    return FMOD_ERR_EVENT_NOTFOUND;
}
FMOD_RESULT Event::getProperty(const char *, void *, bool) { return FMOD_ERR_EVENT_NOTFOUND; }
FMOD_RESULT EventParameter::setValue(float) { return FMOD_OK; }
} // namespace FMOD

void FMOD_setLanguage(FMOD::EventSystem *, uint32_t) {}
void FMOD_fade(FModSound *, int, int, float) {}
void FMOD_play(FModSound *, int, unsigned char *, float) {}

int FMOD_EventSystem_freeEventData(FMOD::EventSystem *, FMOD::Event *, int) { return FMOD_OK; }
FModSound *FMOD_Event_stop_p(FMOD::Event *, int) { return nullptr; }
int FMOD_EventSystem_getReverbPresetByIndex(FMOD::EventSystem *, int, unsigned char *, char **) {
    return FMOD_ERR_EVENT_NOTFOUND;
}
int FMOD_EventSystem_setReverbProperties(FMOD::EventSystem *, unsigned char *) { return FMOD_OK; }
int FMOD_EventSystem_load(FMOD::EventSystem *, const char *, FMOD::EventProject *) {
    return FMOD_OK;
}
void FMOD_EventSystem_getCategory(FMOD::EventSystem *, FMOD::EventCategory *) {}
void FMOD_EventSystem_getProjectByIndex(FMOD::EventSystem *, FMOD::EventProject *) {}
int FMOD_EventSystem_getEventBySystemID(unsigned int, int, FMOD::Event **out) {
    if (out) *out = nullptr;
    return FMOD_ERR_EVENT_NOTFOUND;
}
int FMOD_Event_set3DAttributes(FMOD::Event *, Vector *, Vector *) { return FMOD_OK; }
int FMOD_EventSystem_set3DListenerAttributes(int, unsigned char *, Vector *, Vector *, Vector *) {
    return FMOD_OK;
}
void FMOD_MusicSystem_promptCue(FMOD::MusicSystem *, int) {}
void FMOD_MusicSystem_setParameterValue(FMOD::MusicSystem *, int, float) {}
void FMOD_EventCategory_setVolume(FMOD::EventCategory *, float) {}
void FMOD_EventCategory_stopAllEvents(FMOD::EventCategory *) {}
int FMOD_EventCategory_getInfo(FMOD::EventCategory *, int *index, char **name) {
    if (index) *index = 0;
    if (name) *name = nullptr;
    return FMOD_ERR_EVENT_NOTFOUND;
}
int FMOD_Event_getInfo(FMOD::Event *, char **name, unsigned char *) {
    if (name) *name = nullptr;
    return FMOD_ERR_EVENT_NOTFOUND;
}
