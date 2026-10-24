"""Exercise source-extracted RecordHandler pending-product save/load blocks."""

import argparse
from pathlib import Path
import subprocess

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def between(source, start, end):
    return source[source.index(start):source.index(end, source.index(start))]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--cxx", required=True)
    args = parser.parse_args()
    out = args.out_dir.resolve()
    out.mkdir(parents=True, exist_ok=True)

    record = (ROOT / "src/game/mission/RecordHandler.cpp").read_text()
    file_source = (ROOT / "src/engine/file/AEFile.cpp").read_text()
    status_source = (ROOT / "src/game/mission/Status.cpp").read_text()
    blueprint_source = (ROOT / "src/game/mission/BluePrint.cpp").read_text()
    helpers = [
        "inline void AEStr_set(String &self, const char *s) {",
        "inline void AEStr_set(String &self, const uint16_t *s) {",
        "inline uint16_t *AEStr_wchar(const String &self) {",
        "inline char *AEStr_char(const String &self) {",
    ]
    wrappers = [
        "static inline void AEFile_ReadInt(void *out, unsigned int fd) {",
        "static inline void AEFile_ReadString(void *out, unsigned int fd, int flag) {",
        "static inline void AEFile_WriteInt(int v, unsigned int fd) {",
        "static inline void AEFile_WriteString(void *s, unsigned int fd, int flag) {",
    ]
    write_block = between(
        record, "        Array<PendingProduct *> *pending = status->pendingProducts;",
        "        Array<String *> *wingmen = status->getWingmen();")
    read_block = between(record, "                int ppN = 0;", "                int wmN = 0;")
    (out / "pending_product_under_test.inc").write_text(
        "namespace {\n" + "\n".join(function(file_source, h) for h in helpers) + "\n}\n"
        + function(file_source, "uint32_t AEFile::Read(String &value, uint32_t handle, bool wide) {")
        + function(file_source, "uint32_t AEFile::Write(const String &value, uint32_t handle, bool wide) {")
        + "\n".join(function(record, h) for h in wrappers)
        + "\n".join(function(blueprint_source, h) for h in [
            "BluePrint::~BluePrint() {", "int BluePrint::getIndex() {",
            "int BluePrint::getStationIndex() {", "int BluePrint::getQuantity() {",
            "String BluePrint::getStationName() {",
        ])
        + function(status_source, "void Status::addPendingProduct(BluePrint *bp) {").replace(
            "void Status::addPendingProduct", "void PendingStatus::addPendingProduct", 1)
        + "\nstatic void writePending(PendingStatus *status, unsigned int fd) {\n"
        + write_block + "\n}\n"
        + "static void readPending(PendingRecord *rec, unsigned int fd) {\n"
        + read_block + "\n}\n"
    )
    for opt in ("-O0", "-O2"):
        exe = out / f"pending_product_{opt[1:]}.exe"
        subprocess.run([
            args.cxx, "-std=gnu++17", opt, "-ffunction-sections", "-fdata-sections",
            "-Isrc", "-I", str(out), "tools/pending_product_roundtrip_smoke.cpp",
            "src/game/mission/PendingProduct.cpp", "src/game/core/String.cpp",
            "-Wl,--gc-sections", "-o", str(exe),
        ], cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
