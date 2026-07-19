#ifndef GOF2_CONFIGREADER_H
#define GOF2_CONFIGREADER_H
#include "engine/core/Array.h"
#include "engine/file/TokenStruct.h"
#include "../core/AEString.h"
#include "engine/file/AEFile.h"

namespace AbyssEngine {
    class Engine;
 }


namespace AbyssEngine {

    class ConfigReader {
    public:
        Array<TokenStruct *> tokens;
        Engine *engine;
        uint32_t file_handle;

        ConfigReader(Engine *engine);

        ~ConfigReader();

        void RegisterTokenReadFunction(String name, ConfigTokenReadFunction read, void *context);

        String GetNewLine();

        void ParseFile(String name);
    };
}
#endif
