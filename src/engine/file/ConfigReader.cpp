#include "engine/file/ConfigReader.h"

namespace AbyssEngine {
    namespace {
        void config_reader_read_file_callback(void *, void *) {
        }

        void config_reader_keys_for_action_callback(void *, void *) {
        }

        void config_reader_register_token_callback(void *, void *) {
        }
    }

    ConfigReader::ConfigReader(Engine *engine) {
        this->engine = engine;

        String readFileName;
        for (const char *p = "ConfigReadFile"; p && *p; ++p)
            { int _nl = readFileName.length + 1; unsigned short *_nd = new unsigned short[_nl + 1]; for (int _i = 0; _i < readFileName.length; _i++) _nd[_i] = readFileName.data[_i]; _nd[readFileName.length] = (unsigned short) (static_cast<char16_t>(static_cast<unsigned char>(*p))); _nd[_nl] = 0; if (readFileName.data) delete[] readFileName.data; readFileName.data = _nd; readFileName.length = _nl; }
        RegisterTokenReadFunction(
            readFileName,
            (ConfigTokenReadFunction) config_reader_read_file_callback, engine);

        String keysForActionName;
        for (const char *p = "ConfigGetKeysForAction"; p && *p; ++p)
            { int _nl = keysForActionName.length + 1; unsigned short *_nd = new unsigned short[_nl + 1]; for (int _i = 0; _i < keysForActionName.length; _i++) _nd[_i] = keysForActionName.data[_i]; _nd[keysForActionName.length] = (unsigned short) (static_cast<char16_t>(static_cast<unsigned char>(*p))); _nd[_nl] = 0; if (keysForActionName.data) delete[] keysForActionName.data; keysForActionName.data = _nd; keysForActionName.length = _nl; }
        RegisterTokenReadFunction(
            keysForActionName,
            (ConfigTokenReadFunction) config_reader_keys_for_action_callback, engine);

        String registerTokenName;
        for (const char *p = "ConfigRegisterTokenReadFunction"; p && *p; ++p)
            { int _nl = registerTokenName.length + 1; unsigned short *_nd = new unsigned short[_nl + 1]; for (int _i = 0; _i < registerTokenName.length; _i++) _nd[_i] = registerTokenName.data[_i]; _nd[registerTokenName.length] = (unsigned short) (static_cast<char16_t>(static_cast<unsigned char>(*p))); _nd[_nl] = 0; if (registerTokenName.data) delete[] registerTokenName.data; registerTokenName.data = _nd; registerTokenName.length = _nl; }
        RegisterTokenReadFunction(
            registerTokenName,
            (ConfigTokenReadFunction) config_reader_register_token_callback, engine);
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
                    { int _nl = line.length + 1; unsigned short *_nd = new unsigned short[_nl + 1]; for (int _i = 0; _i < line.length; _i++) _nd[_i] = line.data[_i]; _nd[line.length] = (unsigned short) (static_cast<char16_t>(static_cast<unsigned char>(c))); _nd[_nl] = 0; if (line.data) delete[] line.data; line.data = _nd; line.length = _nl; }
                }
            }

            {
                int b = 0, e = line.length;
                while (b < line.length && line.data[b] == static_cast<unsigned short>(' ')) b++;
                while (e > b && line.data[e - 1] == static_cast<unsigned short>(' ')) e--;
                if (b >= e) {
                    { if (line.data) delete[] line.data; line.data = nullptr; line.length = 0; }
                } else {
                    line = line.SubString(static_cast<unsigned int>(b), static_cast<unsigned int>(e));
                }
            }

            String commentNeedle;
            for (const char *p = "//"; p && *p; ++p)
                { int _nl = commentNeedle.length + 1; unsigned short *_nd = new unsigned short[_nl + 1]; for (int _i = 0; _i < commentNeedle.length; _i++) _nd[_i] = commentNeedle.data[_i]; _nd[commentNeedle.length] = (unsigned short) (static_cast<char16_t>(static_cast<unsigned char>(*p))); _nd[_nl] = 0; if (commentNeedle.data) delete[] commentNeedle.data; commentNeedle.data = _nd; commentNeedle.length = _nl; }
            unsigned int commentPos = line.IndexOf(commentNeedle);
            int32_t commentIndex = commentPos == 0xffffffffu ? -1 : static_cast<int32_t>(commentPos);
            if (commentIndex != -1) {
                line = line.SubString(0, 0 + static_cast<uint32_t>(commentIndex));
                {
                    int b = 0, e = line.length;
                    while (b < line.length && line.data[b] == static_cast<unsigned short>(' ')) b++;
                    while (e > b && line.data[e - 1] == static_cast<unsigned short>(' ')) e--;
                    if (b >= e) {
                        { if (line.data) delete[] line.data; line.data = nullptr; line.length = 0; }
                    } else {
                        line = line.SubString(static_cast<unsigned int>(b), static_cast<unsigned int>(e));
                    }
                }
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
                            String section = line.SubString(1, 1 + (line.size() - 1));
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
