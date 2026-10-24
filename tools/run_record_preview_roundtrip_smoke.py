"""Exercise source-extracted record preview write/read with a native GameRecord."""

import argparse
from pathlib import Path
import subprocess

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--cxx", required=True)
    args = parser.parse_args()
    out = args.out_dir.resolve()
    out.mkdir(parents=True, exist_ok=True)

    record = (ROOT / "src/game/mission/RecordHandler.cpp").read_text()
    file_source = (ROOT / "src/engine/file/AEFile.cpp").read_text()
    android_file_source = (ROOT / "src/engine/file/FileInterfaceAndroid.cpp").read_text()
    engine = (ROOT / "src/engine/core/AbyssEngine.cpp").read_text()
    helpers = [
        "inline void AEStr_set(String &self, const char *s) {",
        "inline void AEStr_set(String &self, const uint16_t *s) {",
        "inline uint16_t *AEStr_wchar(const String &self) {",
        "inline char *AEStr_char(const String &self) {",
    ]
    wrappers = [
        "static inline void AEFile_Read_i64(void *out, unsigned int fd) {",
        "static inline void AEFile_Read_i32(void *out, unsigned int fd) {",
        "static inline void AEFile_Read_bool(void *out, unsigned int fd, int wide) {",
        "static inline void AEFile_Read_f32(void *out, unsigned int fd) {",
        "static inline void AEFile_Write_i64(long long v, unsigned int fd) {",
        "static inline void AEFile_Write_i32(int v, unsigned int fd) {",
        "static inline void AEFile_Write_str(void *s, unsigned int fd, int flag) {",
        "static inline void AEFile_Write_f32(int bits, unsigned int fd) {",
    ]
    (out / "record_preview_under_test.inc").write_text(
        "namespace {\n" + "\n".join(function(file_source, h) for h in helpers) + "\n}\n"
        + "namespace AbyssEngine {\n"
        + function(engine, "String operator+(const String &a, const String &b) {")
        + "\n}\n"
        + function(file_source, "uint32_t AEFile::Read(String &value, uint32_t handle, bool wide) {")
        + function(file_source, "uint32_t AEFile::ReadSwitched(uint16_t &value, uint32_t handle) {")
        + function(file_source, "uint32_t AEFile::ReadSwitched(String &value, uint32_t handle) {")
        + function(file_source, "uint32_t AEFile::Write(const String &value, uint32_t handle, bool wide) {")
        + function(android_file_source, "static inline const unsigned short *GetAEWChar(const String &s) {")
        + next(line + "\n" for line in android_file_source.splitlines()
               if line.startswith("static int *gFIAInstCount = "))
        + next(line + "\n" for line in android_file_source.splitlines()
               if line.startswith("static const char *gModeWb = "))
        + function(android_file_source, "FileInterfaceAndroid::FileInterfaceAndroid(FILE *f, bool append) {")
        + function(android_file_source, "void *FileInterfaceAndroid::OpenWrite(String name, int, bool, unsigned int) {")
        + "\n".join(function(record, h) for h in wrappers)
        + function(record, "void *RecordHandler::recordStoreReadPreview(int slot) {").replace(
            "void *RecordHandler::recordStoreReadPreview", "void *PreviewWriter::recordStoreReadPreview", 1)
        + function(record, "int RecordHandler::recordStoreWritePreview(GameRecord *rec, int slot) {").replace(
            "int RecordHandler::recordStoreWritePreview(GameRecord *rec, int slot)",
            "int PreviewWriter::recordStoreWritePreview(PreviewData *rec, int slot)", 1)
    )
    for opt in ("-O0", "-O2"):
        exe = out / f"record_preview_{opt[1:]}.exe"
        subprocess.run([
            args.cxx, "-std=gnu++17", opt, "-DGOF2_RECORD_POINTER_SMOKE",
            "-ffunction-sections", "-fdata-sections", "-Isrc", "-I", str(out),
            "tools/record_preview_roundtrip_smoke.cpp",
            "src/game/mission/GameRecord.cpp", "src/game/core/String.cpp",
            "-Wl,--gc-sections", "-o", str(exe),
        ], cwd=ROOT, check=True)
        subprocess.run([str(exe), str(out)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
