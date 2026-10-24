#include "engine/file/ConfigReader.h"
#include "engine/core/ApplicationManager.h"
#include "engine/core/KeyCode.h"
#include "engine/render/Engine.h"

namespace AbyssEngine {
    namespace {
        void read_timer(ConfigReader *reader, void *context) {
            String line = reader->GetNewLine();
            if (line.Compare("EOF") != 0)
                static_cast<Engine *>(context)->field_0x28 = line.ValueOf();
        }

        void read_keymap(ConfigReader *reader, void *context) {
            Array<KeyCode *> mappings;
            for (unsigned int i = 0; i < 64; ++i) {
                String line = reader->GetNewLine();
                Array<String *> *parts = static_cast<Array<String *> *>(line.Split(String(",", false)));
                bool valid = parts != nullptr && parts->size() == 2;
                if (valid) {
                    KeyCode *mapping = new KeyCode();
                    mapping->code = (*parts)[0]->ValueOf();
                    mapping->name = *(*parts)[1];
                    ArrayAdd(mapping, mappings);
                }
                if (parts != nullptr) {
                    for (unsigned int j = 0; j < parts->size(); ++j)
                        delete (*parts)[j];
                    delete parts;
                }
                if (!valid)
                    break;
            }
            if (mappings.size() == 64)
                static_cast<Engine *>(context)->appManager->KeyCodeSetMapping(&mappings);
            for (unsigned int i = 0; i < mappings.size(); ++i)
                delete mappings[i];
        }

        void read_vibration(ConfigReader *reader, void *context) {
            String line = reader->GetNewLine();
            if (line.Compare("EOF") != 0 && line.Compare("ON") == 0)
                static_cast<Engine *>(context)->vibrationSupported = true;
        }
    }

    ConfigReader::ConfigReader(Engine *engine) {
        this->engine = engine;
        RegisterTokenReadFunction(String("TIMER", false), read_timer, engine);
        RegisterTokenReadFunction(String("KEYMAP", false), read_keymap, engine);
        RegisterTokenReadFunction(String("VIBRATION", false), read_vibration, engine);
    }

    ConfigReader::~ConfigReader() {
        for (uint32_t i = 0; i < tokens.size(); i++) {
            TokenStruct *token = tokens[i];
            if (token != nullptr) {
                delete token;
            }
            tokens[i] = nullptr;
        }
    }

    void ConfigReader::RegisterTokenReadFunction(
        String name, ConfigTokenReadFunction read, void *context) {
        TokenStruct *token = new TokenStruct();
        token->name = name;
        token->read = read;
        token->context = context;
        ArrayAdd(token, tokens);
    }

    String ConfigReader::GetNewLine() {
        String line;
        char c = 0;

        while (line.size() == 0) {
            uint32_t read;
            while (true) {
                read = AEFile::Read(c, this->file_handle);
                bool newline = (c == '\n');
                if (read == 0 || newline) {
                    break;
                }
                if (c != '\r') {
                    line += c;
                }
            }

            line.Trim();

            String commentNeedle("//", false);
            unsigned int commentPos = line.IndexOf(commentNeedle);
            int32_t commentIndex = commentPos == 0xffffffffu ? -1 : static_cast<int32_t>(commentPos);
            if (commentIndex != -1) {
                line = line.SubString(0, static_cast<uint32_t>(commentIndex));
                line.Trim();
            }

            if (line.size() == 0 && read == 0) {
                line.Set("EOF");
            }
        }

        return line;
    }

    void ConfigReader::ParseFile(String name) {
        if (AEFile::OpenRead(name, &this->file_handle) != 0) {
            String line = GetNewLine();
            while ((line.Compare("EOF") == 0 ? 0 : 1) != 0) {
                uint16_t *first = reinterpret_cast<uint16_t *>(&line.data[0]);
                if (*first == '[') {
                    uint16_t *last = reinterpret_cast<uint16_t *>(&line.data[line.size() - 1]);
                    if (*last == ']') {
                        for (uint32_t i = 0; i < tokens.size(); i++) {
                            TokenStruct *token = tokens[i];
                            String section = line.SubString(1, line.size() - 1);
                            if (token->name.Compare(section) == 0) {
                                token->read(this, token->context);
                                break;
                            }
                        }
                    }
                }
                line = GetNewLine();
            }
            AEFile::Close(this->file_handle);
        }
    }
}
