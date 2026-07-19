#include "engine/core/GameData.h"
#include "engine/core/ApplicationManager.h"
#include "game/core/Globals.h"

GameData::GameData() {
}

GameData::~GameData() {
}

void OnDestroyApplication(AbyssEngine::Engine *engine) {
    AbyssEngine::ApplicationManager *app =
            *reinterpret_cast<AbyssEngine::ApplicationManager **>(
                reinterpret_cast<char *>(engine) + 0x30);

    GameData * data = static_cast<GameData *>(app->GetApplicationData());

    if (data->globals != nullptr) {
        delete data->globals;
    }
    data->globals = nullptr;

    if (data != nullptr) {
        delete data;
    }
}
