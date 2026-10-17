"""Run the extracted weapon-position loader against a local original asset."""
import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--asset", type=Path, required=True)
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.asset.is_file():
        parser.error("--asset must name a local weapons_hd.bin")
    if not args.cxx:
        parser.error("specify --cxx")

    production = (ROOT / "src/engine/file/FileRead.cpp").read_text()
    start = production.index("Array<Array<Vector *> *> *FileRead::loadWeaponPositions(")
    end = production.index("Array<SpacePoint *> *FileRead::loadSpacePoints(", start)
    fragment = production[start:end]
    source = """#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <string>
#include "engine/core/Array.h"

struct Vector { float x, y, z; };
struct String {
    static int live;
    std::string value;
    String(const char *text) : value(text) { ++live; }
    ~String() { --live; }
};
int String::live = 0;
struct AEFile {
    static const char *asset;
    static bool present;
    static int opens, closes;
    static std::ifstream input;
    static uint32_t FileExist(const String &path) {
        assert(path.value == "data/bin/weapons_hd.bin");
        return present;
    }
    static uint32_t OpenRead(const char *path, uint32_t *handle) {
        assert(String::live == 0);
        assert(std::string(path) == "data/bin/weapons_hd.bin");
        input.open(asset, std::ios::binary);
        assert(input.good());
        *handle = 1;
        ++opens;
        return 1;
    }
    template<class T> static uint32_t Read(T &value, uint32_t handle) {
        assert(handle == 1);
        input.read(reinterpret_cast<char *>(&value), sizeof(value));
        assert(input.gcount() == sizeof(value));
        return sizeof(value);
    }
    static void Close(uint32_t handle) {
        assert(handle == 1);
        input.close();
        ++closes;
    }
};
const char *AEFile::asset = nullptr;
bool AEFile::present = false;
int AEFile::opens = 0;
int AEFile::closes = 0;
std::ifstream AEFile::input;
struct FileRead {
    Array<Array<Vector *> *> *loadWeaponPositions(int32_t id);
};
""" + fragment + """
void release(Array<Array<Vector *> *> *positions) {
    for (unsigned i = 0; i < positions->size(); ++i) {
        if ((*positions)[i]) {
            ArrayReleaseClasses(*(*positions)[i]);
            delete (*positions)[i];
            (*positions)[i] = nullptr;
        }
    }
    ArrayReleaseClasses(*positions);
    delete positions;
}
int main(int argc, char **argv) {
    assert(argc == 2);
    AEFile::asset = argv[1];
    FileRead reader;
    assert(reader.loadWeaponPositions(63) == nullptr);
    assert(String::live == 0 && AEFile::opens == 0);
    AEFile::present = true;
    auto *first = reader.loadWeaponPositions(63);
    assert(first && first->size() == 4);
    assert((*first)[0]->size() == 2 && (*first)[1]->size() == 4);
    assert((*first)[2] == nullptr && (*first)[3]->size() == 6);
    Vector *v = (*(*first)[0])[0];
    assert(v->x == 488 && v->y == -56 && v->z == 519);
    release(first);
    auto *later = reader.loadWeaponPositions(1);
    assert(later && later->size() == 4);
    assert((*later)[0]->size() == 2 && (*later)[1]->size() == 4);
    assert((*later)[2]->size() == 1 && (*later)[3]->size() == 6);
    v = (*(*later)[2])[0];
    assert(v->x == 0 && v->y == 155 && v->z == -239);
    v = (*(*later)[3])[0];
    assert(v->x == 231 && v->y == -42 && v->z == -533);
    v = (*(*later)[3])[1];
    assert(std::fabs(v->x - 0.8f) < 0.00001f);
    release(later);
    assert(String::live == 0 && AEFile::opens == 2 && AEFile::closes == 2);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="weapon-asset-smoke-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "weapon_asset.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, "-I", str(ROOT / "src"),
                            str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe), str(args.asset.resolve())], check=True)


if __name__ == "__main__":
    main()
