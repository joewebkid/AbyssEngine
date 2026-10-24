#include "engine/core/GameText.h"
#include "engine/file/AEFile.h"

#include <archive.h>
#include <archive_entry.h>
#include <cassert>
#include <codecvt>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <locale>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
std::vector<unsigned char> input;
size_t offset = 0;
bool closed = false;

// Android ARM String::getWCharFromUtf8 maps these homoglyphs after decoding.
char16_t armGlyph(char16_t c) {
    switch (c) {
        case 0x410: return 'A'; case 0x412: return 'B';
        case 0x415: return 'E'; case 0x41a: return 'K';
        case 0x41c: return 'M'; case 0x41d: return 'H';
        case 0x41e: return 'O'; case 0x420: return 'P';
        case 0x421: return 'C'; case 0x422: return 'T';
        case 0x425: return 'X'; case 0x43e: case 0xba: return 'o';
        case 0x440: return 'p'; case 0x441: return 'c';
        case 0x445: return 'x'; case 0x435: return 'e';
        case 0xaa: case 0x430: return 'a'; case 0x60: return '\'';
        default: return c;
    }
}

std::vector<unsigned char> asset(const char *apk, const char *name) {
    archive *reader = archive_read_new();
    archive_read_support_format_zip(reader);
    if (archive_read_open_filename(reader, apk, 10240) != ARCHIVE_OK) {
        archive_read_free(reader);
        throw std::runtime_error("cannot open APK");
    }
    archive_entry *entry;
    while (archive_read_next_header(reader, &entry) == ARCHIVE_OK) {
        if (std::strcmp(archive_entry_pathname(entry), name) != 0) {
            archive_read_data_skip(reader);
            continue;
        }
        std::vector<unsigned char> bytes(archive_entry_size(entry));
        size_t pos = 0;
        while (pos < bytes.size()) {
            auto n = archive_read_data(reader, bytes.data() + pos, bytes.size() - pos);
            if (n <= 0) {
                archive_read_free(reader);
                throw std::runtime_error("short APK entry");
            }
            pos += static_cast<size_t>(n);
        }
        archive_read_free(reader);
        return bytes;
    }
    archive_read_free(reader);
    throw std::runtime_error("missing language asset");
}

void check(const char *apk, const char *name, unsigned short language) {
    input = asset(apk, name);
    offset = 0;
    closed = false;
    GameText text;
    text.textCount = 3401;
    text.textTable = new String *[3401]();
    GameText::currentLang = language;
    text.ReadLangFile(1, 3401);
    assert(closed);

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf8;
    size_t source = 0;
    for (int i = 0; i < 3401; ++i) {
        assert(source + 2 <= input.size());
        unsigned n = (unsigned(input[source]) << 8) | input[source + 1];
        source += 2;
        assert(source + n <= input.size());
        std::u16string expected = utf8.from_bytes(
            reinterpret_cast<const char *>(input.data() + source),
            reinterpret_cast<const char *>(input.data() + source + n));
        for (char16_t &c : expected) c = armGlyph(c);
        source += n;
        String *actual = text.getText(i);
        assert(actual != nullptr && actual != &text.fallbackText);
        if (actual->length != static_cast<int>(expected.size()) ||
            std::u16string(actual->text(), actual->length) != expected) {
            std::cerr << name << " record " << i << ": decoded " << expected.size()
                      << " UTF-16 units, recovered " << actual->length << '\n';
            for (char16_t c : expected) std::cerr << std::hex << unsigned(c) << ' ';
            std::cerr << " versus ";
            for (int j = 0; j < actual->length; ++j)
                std::cerr << std::hex << unsigned(actual->text()[j]) << ' ';
            std::cerr << std::dec << '\n';
            std::abort();
        }
    }
    assert(source == input.size() && offset == input.size());
    assert(std::u16string(text.getText(3401)->text()) == u"Error Language");
    std::cout << name << ": 3401 records match\n";

    input.resize(3);
    offset = 0;
    closed = false;
    GameText truncated;
    truncated.textCount = 3401;
    truncated.textTable = new String *[3401]();
    GameText::currentLang = language;
    truncated.ReadLangFile(1, 3401);
    assert(closed);
    for (int i = 0; i < 3401; ++i) assert(truncated.textTable[i] == nullptr);
}
} // namespace

uint32_t AEFile::Read(uint32_t n, void *buffer, uint32_t handle) {
    assert(handle == 1);
    if (n > input.size() - offset) return 0;
    std::memcpy(buffer, input.data() + offset, n);
    offset += n;
    return 1;
}

void AEFile::Close(uint32_t handle) {
    assert(handle == 1 && !closed);
    closed = true;
}

uint32_t AEFile::FileExist(const String &) { std::abort(); }
uint32_t AEFile::OpenRead(String &, uint32_t *) { std::abort(); }

void String_printImpl(const char *) { std::abort(); }
namespace AbyssEngine {
unsigned short *computeFloatString(float, int, int *, int *, int) { std::abort(); }
String operator+(const String &, const String &) { std::abort(); }
}

int main(int argc, char **argv) {
    assert(argc == 2);
    check(argv[1], "assets/gb.lang", 0);
    check(argv[1], "assets/ru.lang", 5);
}
