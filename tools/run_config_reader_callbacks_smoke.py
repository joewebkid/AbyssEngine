"""Compile the recovered ConfigReader callbacks against a small host seam."""

import subprocess
import sys
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "src/engine/file/ConfigReader.cpp").read_text()
CALLBACKS = SOURCE[SOURCE.index("namespace AbyssEngine {"):SOURCE.index("    ConfigReader::~ConfigReader()")]

HOST = r'''
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
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
    String() = default;
    String(const char *s, bool = false) : value(s) {}
    int Compare(const char *s) const { return value.compare(s); }
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
class ConfigReader {
public:
    Engine *engine;
    std::vector<Registration> registered;
    std::vector<std::string> lines;
    size_t next = 0;
    explicit ConfigReader(Engine *engine);
    String GetNewLine() {
        if (next == lines.size()) return String("EOF");
        return String(lines[next++].c_str());
    }
    void RegisterTokenReadFunction(String name, ConfigTokenReadFunction read, void *context) {
        registered.push_back({name.value, read, context});
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
    reader.lines = timer;
    reader.registered[0].read(&reader, reader.registered[0].context);
    assert(engine.field_0x28 == 20);
    reader.lines = keys;
    reader.next = 0;
    reader.registered[1].read(&reader, reader.registered[1].context);
    assert(manager.mapping.size() == 64);
    assert(manager.mapping[0].code == 48 && manager.mapping[0].name.value == "0");
    assert(manager.mapping[63].code == -1 && manager.mapping[63].name.value == "NA");
    reader.lines = {"ON"};
    reader.next = 0;
    reader.registered[2].read(&reader, reader.registered[2].context);
    assert(engine.vibrationSupported);

    AbyssEngine::ApplicationManager shortManager;
    AbyssEngine::Engine shortEngine;
    shortEngine.field_0x28 = 99;
    shortEngine.appManager = &shortManager;
    AbyssEngine::ConfigReader shortReader(&shortEngine);
    shortReader.registered[0].read(&shortReader, shortReader.registered[0].context);
    assert(shortEngine.field_0x28 == 99);
    shortReader.lines.assign(keys.begin(), keys.end() - 1);
    shortReader.registered[1].read(&shortReader, shortReader.registered[1].context);
    assert(shortManager.mapping.empty());
    shortReader.lines = {"OFF"};
    shortReader.next = 0;
    shortReader.registered[2].read(&shortReader, shortReader.registered[2].context);
    assert(!shortEngine.vibrationSupported);
    std::cout << "config callbacks: TIMER/KEYMAP/VIBRATION and incomplete input OK\n";
}
'''


def main() -> None:
    if len(sys.argv) != 2:
        raise SystemExit("usage: run_config_reader_callbacks_smoke.py <original config.cfg>")
    with tempfile.TemporaryDirectory(prefix="config-reader-", dir=ROOT / "_work") as temp:
        source = Path(temp) / "smoke.cpp"
        source.write_text(HOST + CALLBACKS + MAIN)
        for optimization in ("-O0", "-O2"):
            binary = Path(temp) / f"smoke-{optimization[2:]}.exe"
            subprocess.run(["g++", "-std=c++17", optimization, str(source), "-o", str(binary)], check=True)
            subprocess.run([str(binary), sys.argv[1]], check=True)


if __name__ == "__main__":
    main()
