"""Exercise source-extracted record migration paths and their array cleanup."""

import argparse
from pathlib import Path
import subprocess

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def before(source, start, end):
    return source[source.index(start):source.index(end, source.index(start))]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out-dir", type=Path, required=True)
    parser.add_argument("--cxx", required=True)
    args = parser.parse_args()
    out = args.out_dir.resolve()
    out.mkdir(parents=True, exist_ok=True)

    record = (ROOT / "src/game/mission/RecordHandler.cpp").read_text()
    engine = (ROOT / "src/engine/core/AbyssEngine.cpp").read_text()
    (out / "record_migration_under_test.inc").write_text(
        "namespace AbyssEngine {\n"
        + function(engine, "String operator+(const String &a, const String &b) {")
        + "\n}\n"
        + before(record, "static int *g_CSV_count = nullptr;", "void RecordHandler::convertSDVersionSaves() {")
        + function(record, "void RecordHandler::convertSDVersionSaves() {").replace(
            "void RecordHandler::convertSDVersionSaves", "void RecordMigration::convertSDVersionSaves", 1)
        + before(record, "static int *g_RH_csd_count = nullptr;", "void RecordHandler::changeSaveDirectoryToBackupDirectory() {")
        + function(record, "void RecordHandler::changeSaveDirectoryToBackupDirectory() {").replace(
            "void RecordHandler::changeSaveDirectoryToBackupDirectory",
            "void RecordMigration::changeSaveDirectoryToBackupDirectory", 1)
    )
    for opt in ("-O0", "-O2"):
        exe = out / f"record_migration_{opt[1:]}.exe"
        subprocess.run([
            args.cxx, "-std=gnu++17", opt, "-ffunction-sections", "-fdata-sections",
            "-Isrc", "-I", str(out), "tools/record_migration_cleanup_smoke.cpp",
            "src/game/core/String.cpp", "-Wl,--gc-sections", "-o", str(exe),
        ], cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
