"""Exercise the production Level::update flash-fade prefix at O0 and O2."""

from pathlib import Path
import argparse
import subprocess
import tempfile
import uuid


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cxx", default="C:/msys64/ucrt64/bin/g++.exe")
    parser.add_argument("--ndk-root", type=Path)
    parser.add_argument("--adb", type=Path)
    args = parser.parse_args()
    if bool(args.ndk_root) != bool(args.adb):
        parser.error("--ndk-root and --adb must be supplied together")
    source = (ROOT / "src/game/world/Level.cpp").read_text()
    begin = source.index("void Level::update(long long time, bool param) {")
    end = source.index("    bool didMission = false;", begin)
    prefix = source[begin:end] + "}\n"
    fixture = '''#include <assert.h>
struct Vector { float x, y, z; };
struct Level {
    static float i_r, i_g, i_b, i_a;
    Vector flashColor{};
    float flashField = 0;
    int flashDurationA = 0, flashDurationB = 0;
    unsigned char flashActive = 0;
    void update(long long time, bool param);
};
float Level::i_r = 10, Level::i_g = 20, Level::i_b = 30, Level::i_a = 40;
// SOURCE_PREFIX
int main() {
    Level level{};
    level.flashColor = {100, 200, 50};
    level.flashField = 255;
    level.flashDurationA = level.flashDurationB = 100;
    level.flashActive = 1;
    level.update(25, false);
    assert(level.flashActive == 1 && level.flashDurationA == 75);
    assert(level.flashColor.x == 75 && level.flashColor.y == 150);
    assert(level.flashColor.z == 37.5f && level.flashField == 191.25f);
    level.update(95, false);
    assert(level.flashActive == 0 && level.flashDurationA == -20);
    assert(level.flashColor.x == 10 && level.flashColor.y == 20);
    assert(level.flashColor.z == 30 && level.flashField == 40);
    level.update(1, false);
    assert(level.flashDurationA == -20 && level.flashField == 40);
}
'''.replace("// SOURCE_PREFIX", prefix)
    with tempfile.TemporaryDirectory(prefix="level-fade-", dir=ROOT / "_work") as path:
        source_path = Path(path) / "fade.cpp"
        source_path.write_text(fixture)
        for opt in ("-O0", "-O2"):
            executable = Path(path) / ("fade" + opt[1:] + ".exe")
            subprocess.run([args.cxx, "-std=gnu++14", opt, str(source_path), "-o", str(executable)],
                           cwd=ROOT, check=True)
            subprocess.run([str(executable)], cwd=ROOT, check=True)
        if args.ndk_root:
            ndk = args.ndk_root.resolve()
            compiler = ndk / "toolchains/llvm/prebuilt/windows-x86_64/bin/clang.exe"
            gcc = ndk / "toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64"
            platform_lib = ndk / "platforms/android-21/arch-arm/usr/lib"
            executable = Path(path) / "fade-arm"
            subprocess.run([str(compiler), "-x", "c++", "-target", "armv7-none-linux-androideabi21",
                            "-march=armv7-a", "-mthumb", "-mfpu=neon", "-mfloat-abi=softfp",
                            f"--sysroot={ndk / 'sysroot'}", f"--gcc-toolchain={gcc}",
                            f"-B{platform_lib}", f"-L{platform_lib}",
                            "-isystem", str(ndk / "sysroot/usr/include/arm-linux-androideabi"),
                            "-D__ANDROID_API__=21", "-fPIE", "-pie", "-O2",
                            str(source_path), "-o", str(executable)], cwd=ROOT, check=True)
            remote = "/data/local/tmp/gof2-level-flash-fade-" + uuid.uuid4().hex
            subprocess.run([str(args.adb), "push", str(executable), remote], check=True)
            try:
                subprocess.run([str(args.adb), "shell", "chmod", "700", remote], check=True)
                subprocess.run([str(args.adb), "shell", remote], check=True)
            finally:
                subprocess.run([str(args.adb), "shell", "rm", "-f", remote], check=True)
    print("Level fade prefix: active, expired and inactive cases pass at O0/O2"
          + (" and ARMv7 device" if args.ndk_root else ""))


if __name__ == "__main__":
    main()
