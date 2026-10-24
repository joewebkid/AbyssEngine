"""Compile the recovered ConfigReader callbacks against a small host seam."""

import subprocess
import sys
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "src/engine/file/ConfigReader.cpp").read_text()
CALLBACKS = SOURCE[SOURCE.index("namespace AbyssEngine {"):SOURCE.index("    ConfigReader::~ConfigReader()")]
GET_NEW_LINE = SOURCE[SOURCE.index("    String ConfigReader::GetNewLine()"):SOURCE.index("    void ConfigReader::ParseFile(")]
PARSER = SOURCE[SOURCE.index("    void ConfigReader::ParseFile("):SOURCE.rfind("\n}")]

HOST = r'''
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

template<class T> class Array {
public:
    std::vector<T> items;
    size_t size() const { return items.size(); }
    T &operator[](size_t i) { return items[i]; }
};
template<class T> void ArrayAdd(T item, Array<T> &a) { a.items.push_back(item); }

namespace AbyssEngine {
class String {
public:
    std::string value;
    std::u16string data;
    String() = default;
    String(const char *s, bool = false) : value(s), data(value.begin(), value.end()) {}
    void sync() { data.assign(value.begin(), value.end()); }
    void Set(const char *s) { value = s; sync(); }
    String &operator+=(const char &c) { value += c; sync(); return *this; }
    void Trim() {
        size_t first = value.find_first_not_of(" \t");
        value = first == std::string::npos ? "" : value.substr(first, value.find_last_not_of(" \t") - first + 1);
        sync();
    }
    unsigned int IndexOf(const String &needle) const {
        size_t found = value.find(needle.value);
        return found == std::string::npos ? UINT32_MAX : static_cast<unsigned int>(found);
    }
    int Compare(const char *s) const { return value.compare(s); }
    int Compare(const String &s) const { return value.compare(s.value); }
    size_t size() const { return data.size(); }
    String SubString(size_t start, size_t end) const {
        return String(value.substr(start, end - start).c_str());
    }
    int ValueOf() const { return std::atoi(value.c_str()); }
    void *Split(String separator) {
        if (separator.value.empty() || value.find(separator.value) == std::string::npos)
            return nullptr;
        auto *parts = new Array<String *>();
        size_t start = 0;
        while (true) {
            size_t end = value.find(separator.value, start);
            if (end == std::string::npos) end = value.size();
            if (end > start) ArrayAdd(new String(value.substr(start, end - start).c_str()), *parts);
            if (end == value.size()) break;
            start = end + separator.value.size();
        }
        return parts;
    }
};
class KeyCode {
public:
    int code = 0;
    String name;
};
class ApplicationManager {
public:
    std::vector<KeyCode> mapping;
    void KeyCodeSetMapping(Array<KeyCode *> *items) {
        for (size_t i = 0; i < items->size(); ++i) mapping.push_back(*(*items)[i]);
    }
};
class Engine {
public:
    int field_0x28 = 0;
    bool vibrationSupported = false;
    ApplicationManager *appManager = nullptr;
};
class ConfigReader;
using ConfigTokenReadFunction = void (*)(ConfigReader *, void *);
struct Registration { std::string name; ConfigTokenReadFunction read; void *context; };
struct TokenStruct { String name; ConfigTokenReadFunction read; void *context; };
class AEFile {
public:
    inline static std::string content;
    inline static size_t position = 0;
    static void Use(std::string bytes) { content = bytes; position = 0; }
    static int OpenRead(String name, uint32_t *handle) {
        std::ifstream input(name.value, std::ios::binary);
        if (!input) return 0;
        Use(std::string(std::istreambuf_iterator<char>(input), {}));
        *handle = 1;
        return 1;
    }
    static uint32_t Read(char &c, uint32_t) {
        if (position == content.size()) return 0;
        c = content[position++];
        return 1;
    }
    static void Close(uint32_t) { Use(""); }
};
class ConfigReader {
public:
    Engine *engine;
    Array<TokenStruct *> tokens;
    uint32_t file_handle = 0;
    std::vector<Registration> registered;
    explicit ConfigReader(Engine *engine);
    void ParseFile(String name);
    String GetNewLine();
    void RegisterTokenReadFunction(String name, ConfigTokenReadFunction read, void *context) {
        registered.push_back({name.value, read, context});
        ArrayAdd(new TokenStruct{name, read, context}, tokens);
    }
};
}
'''

MAIN = r'''
}
static std::string trim(std::string s) {
    size_t begin = s.find_first_not_of(" \t\r");
    if (begin == std::string::npos) return "";
    return s.substr(begin, s.find_last_not_of(" \t\r") - begin + 1);
}
int main(int argc, char **argv) {
    assert(argc == 2);
    std::ifstream input(argv[1]);
    assert(input);
    std::vector<std::string> timer, keys;
    std::string line, section;
    while (std::getline(input, line)) {
        line = trim(line.substr(0, line.find("//")));
        if (line.empty()) continue;
        if (line.front() == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
        } else if (section == "TIMER") {
            timer.push_back(line);
        } else if (section == "KEYMAP") {
            keys.push_back(line);
        }
    }
    assert(timer.size() == 1 && keys.size() == 64);
    AbyssEngine::ApplicationManager manager;
    AbyssEngine::Engine engine;
    engine.appManager = &manager;
    AbyssEngine::ConfigReader reader(&engine);
    assert(reader.registered.size() == 3);
    assert(reader.registered[0].name == "TIMER");
    assert(reader.registered[1].name == "KEYMAP");
    assert(reader.registered[2].name == "VIBRATION");
    reader.file_handle = 1;
    AbyssEngine::AEFile::Use(timer[0] + "\n");
    reader.registered[0].read(&reader, reader.registered[0].context);
    assert(engine.field_0x28 == 20);
    std::string keyBytes;
    for (const auto &key : keys) keyBytes += key + "\n";
    AbyssEngine::AEFile::Use(keyBytes);
    reader.registered[1].read(&reader, reader.registered[1].context);
    assert(manager.mapping.size() == 64);
    assert(manager.mapping[0].code == 48 && manager.mapping[0].name.value == "0");
    assert(manager.mapping[63].code == -1 && manager.mapping[63].name.value == "NA");
    AbyssEngine::AEFile::Use("ON\n");
    reader.registered[2].read(&reader, reader.registered[2].context);
    assert(engine.vibrationSupported);

    AbyssEngine::AEFile::Use("\t // skip\r\n\t 20 \t // tail\r\n");
    assert(reader.GetNewLine().value == "20");
    assert(reader.GetNewLine().value == "EOF");

    AbyssEngine::ApplicationManager parsedManager;
    AbyssEngine::Engine parsedEngine;
    parsedEngine.appManager = &parsedManager;
    AbyssEngine::ConfigReader parsedReader(&parsedEngine);
    parsedReader.ParseFile(AbyssEngine::String(argv[1]));
    assert(parsedEngine.field_0x28 == 20 && parsedManager.mapping.size() == 64);
    assert(parsedManager.mapping[63].code == -1 && parsedManager.mapping[63].name.value == "NA");

    AbyssEngine::ApplicationManager shortManager;
    AbyssEngine::Engine shortEngine;
    shortEngine.field_0x28 = 99;
    shortEngine.appManager = &shortManager;
    AbyssEngine::ConfigReader shortReader(&shortEngine);
    shortReader.file_handle = 1;
    AbyssEngine::AEFile::Use("");
    shortReader.registered[0].read(&shortReader, shortReader.registered[0].context);
    assert(shortEngine.field_0x28 == 99);
    AbyssEngine::AEFile::Use(keyBytes.substr(0, keyBytes.size() - keys.back().size() - 1));
    shortReader.registered[1].read(&shortReader, shortReader.registered[1].context);
    assert(shortManager.mapping.empty());
    AbyssEngine::AEFile::Use("OFF\n");
    shortReader.registered[2].read(&shortReader, shortReader.registered[2].context);
    assert(!shortEngine.vibrationSupported);
    std::cout << "config parser/callbacks: TIMER/KEYMAP/VIBRATION and incomplete input OK\n";
}
'''


def main() -> None:
    if len(sys.argv) != 2:
        raise SystemExit("usage: run_config_reader_callbacks_smoke.py <original config.cfg>")
    with tempfile.TemporaryDirectory(prefix="config-reader-", dir=ROOT / "_work") as temp:
        source = Path(temp) / "smoke.cpp"
        source.write_text(HOST + CALLBACKS + GET_NEW_LINE + PARSER + MAIN)
        for optimization in ("-O0", "-O2"):
            binary = Path(temp) / f"smoke-{optimization[2:]}.exe"
            subprocess.run(["g++", "-std=c++17", optimization, str(source), "-o", str(binary)], check=True)
            subprocess.run([str(binary), sys.argv[1]], check=True)


if __name__ == "__main__":
    main()
