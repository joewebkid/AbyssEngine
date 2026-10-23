"""Original station binary reader and continuous Level/GLES -> sun request gate.

Catalog decoding below supplies independent expectations/bootstrap, never the
production reader's result. Originals stay in place. Earlier scene fixtures
remain explicit; reaching a sun request is not completing Level stage zero.
"""
import argparse
import hashlib
import os
from pathlib import Path
import socketserver
import struct
import subprocess
import threading

from run_gles_atlas_asset_gate import ROOT, SOURCES, prepare_gles
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once
from run_particle_control_smoke import function


def oracle(root, out):
    records = {}
    for name, count in (("stations", 135), ("systems", 34)):
        data = (root / f"data/bin/{name}.bin").read_bytes()
        pos = 0

        def integer(fmt=">i"):
            nonlocal pos
            value = struct.unpack_from(fmt, data, pos)[0]
            pos += struct.calcsize(fmt)
            return value

        rows = []
        for _ in range(count):
            size = integer(">H")
            text = data[pos:pos + size].decode("utf-8")
            pos += size
            fields = [integer() for _ in range(4 if name == "stations" else 8)]
            arrays = [[integer() for _ in range(integer())] for _ in range(4)] if name == "systems" else []
            rows.append((text, fields, arrays, pos))
        assert pos == len(data)
        records[name] = rows
        print(f"Original {name}.bin: {len(data)} bytes SHA256 {hashlib.sha256(data).hexdigest()}", flush=True)
    lines = ["struct ExpectedStation { std::vector<uint16_t> name; int system, tech, texture; size_t end; };",
             "static const ExpectedStation expectedStations[] = {"]
    for i, (name, fields, _, end) in enumerate(records["stations"]):
        assert fields[0] == i
        chars = ",".join(str(v) for v in struct.unpack("<" + "H" * (len(name.encode("utf-16le")) // 2), name.encode("utf-16le")))
        lines.append("{{" + chars + "}," + ",".join(map(str, fields[1:] + [end])) + "},")
    lines += ["};", "struct ExpectedSystem { const char *name; int fields[8]; int rgb[3]; std::vector<int> stations; };",
              "static const ExpectedSystem expectedSystems[] = {"]
    for name, fields, arrays, _ in records["systems"]:
        assert len(arrays[0]) == 3
        # Bootstrap name is not the string-decoder oracle; station UTF-16 is.
        escaped = "".join(f"\\{v:03o}" for v in name.encode("utf-8"))
        lines.append('{"' + escaped + '",{' + ",".join(map(str, fields)) + "},{" +
                     ",".join(map(str, arrays[0])) + "},{" + ",".join(map(str, arrays[1])) + "}},")
    (out / "station_binary_oracle.inc").write_text("\n".join(lines + ["};"]))


def prepare(out, cxx, root, reuse):
    prepare_gles(out, cxx, root, reuse)
    oracle(root, out)
    # Preserve all inherited production bodies. Refresh the changed constructor
    # on reused builds, rather than silently testing stale extracted source.
    chain = (out / "station_chain_under_test.inc").read_text()
    ctor = extract("src/game/world/StarSystem.cpp", ["StarSystem::StarSystem(int mode) {"])
    chain = replace_once(chain, function(chain, "StarSystem::StarSystem(int mode) {"),
                         "#define ArraySetLength stationArraySetLength\n" + ctor + "\n#undef ArraySetLength\n")
    chain += "\n" + extract("src/game/world/SolarSystem.cpp", ["uint32_t *SolarSystem::getStations()", "SolarSystem::~SolarSystem()"])
    chain += "\n" + extract("src/game/world/Station.cpp", ["Station::Station(String name,"])
    chain += "\ninline void AEStr_set(String &self, const char *s) { self.Set_char(s); }\n"
    chain += extract("src/engine/file/AEFile.cpp", ["uint32_t AEFile::ReadSwitched(uint16_t &", "uint32_t AEFile::ReadSwitched(int32_t &", "void AEFile::ReadSwitched(String &"])
    chain += "\n#define ArraySetLength stationArraySetLength\n" + extract("src/engine/file/FileRead.cpp", ["Array<Station *> *FileRead::loadStationsBinary(SolarSystem *"]) + "\n#undef ArraySetLength\n"
    (out / "station_binary_chain.inc").write_text(chain)
    services = (ROOT / "tools/gles_atlas_services.inc").read_text()
    services = replace_once(services, function(services, "uint32_t *SolarSystem::getStations()"), "")
    (out / "station_binary_gles.inc").write_text(services)
    traps = (out / "lensflare_traps.inc").read_text().replace("TRAP(_ZN8FileRead18loadStationsBinaryEP11SolarSystem)", "")
    (out / "station_binary_traps.inc").write_text(traps)
    gate = (out / "gles_atlas_station_under_test.inc").read_text()
    gate = replace_once(gate, '#include "station_chain_under_test.inc"', '')
    gate = replace_once(gate, '#include "gles_atlas_services.inc"',
        '#include "station_binary_gles.inc"\n#include "station_binary_services.inc"\n#include "station_binary_chain.inc"')
    gate = gate.replace('"lensflare_traps.inc"', '"station_binary_traps.inc"')
    gate = replace_once(gate, 'Status *Globals::status;', 'Status *Globals::status;\nstatic bool missingStationFile;')
    # Fixture failure flag must precede FileExist.
    gate = replace_once(gate, 'char *name = path.GetAEChar();',
        'char *name = path.GetAEChar();\n    if (missingStationFile && std::strcmp(name, "data/bin/stations.bin") == 0) { delete[] name; return 0; }')
    start = gate.index('    int rgb[] = {31, 60, 90};')
    end = gate.index('    registerResources', start)
    gate = gate[:start] + '''    status->system = fixtureSystem(1); // Pan catalog, texture 2, stations 5..9.
    // Inherited Level fixture keeps both jumpgates at Binon (original gate is 5).
    status->system->jumpgateStationId = 7;
''' + gate[end:]
    gate = gate.replace('opened == 18', 'opened == 19')
    marker = '    assert(stationOpenedPaths == expectedFiles);'
    gate = replace_once(gate, marker, '    expectedFiles.push_back("data/bin/stations.bin");\n' + marker)
    gate = replace_once(gate, 'const std::vector<std::string> expectedFiles', 'std::vector<std::string> expectedFiles')
    gate = gate.replace('SolarSystem::getStations boundary', 'station binary completed -> StarSystem sun TextureCreate boundary')
    texture = (out / "level_texture_canvas_under_test.inc").read_text()
    signature = 'void PaintCanvas::TextureCreate(unsigned short id, unsigned int &out, bool flag) {'
    texture = replace_once(texture, signature, signature + '\n    stationTextureBoundary(id, out, flag);')
    (out / "station_binary_texture.inc").write_text('static void stationTextureBoundary(unsigned short, unsigned &, bool);\n' + texture)
    host = (out / "station_level_host.inc").read_text().replace('"level_texture_canvas_under_test.inc"', '"station_binary_texture.inc"')
    (out / "station_binary_host.inc").write_text(host)
    gate = gate.replace('"station_level_host.inc"', '"station_binary_host.inc"')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--asset-root", type=Path, required=True)
    p.add_argument("--out-dir", type=Path, required=True)
    p.add_argument("--cxx", default="C:/msys64/ucrt64/bin/g++.exe")
    p.add_argument("--reuse-inputs", action="store_true")
    p.add_argument("--angle-dir", type=Path)
    p.add_argument("--egl-include", type=Path)
    p.add_argument("--ndk", type=Path)
    p.add_argument("--adb")
    p.add_argument("--serial")
    args = p.parse_args()
    root, out = args.asset_root.resolve(), args.out_dir.resolve()
    out.mkdir(parents=True, exist_ok=True)
    prepare(out, args.cxx, root, args.reuse_inputs)
    sources = [f"src/{s}.cpp" for s in SOURCES]
    flags = ["-std=gnu++17", "-g0", "-ffunction-sections", "-fdata-sections", "-Isrc", "-Ithird_party/jni", "-Ithird_party/gl", "-Itools", "-I", str(out)]
    env = os.environ.copy()
    if args.ndk:
        assert args.adb and args.serial
        math = (ROOT / "src/engine/math/AEMath.cpp").read_text()
        for signature in ("__attribute__((noinline)) Vector &Vector::operator=(", "Vector &Vector::operator-=(", "Vector &Vector::operator*=(const Vector &", "Vector &Vector::operator/=(const Vector &", "Matrix &Matrix::operator=(", "Matrix &Matrix::operator*=("):
            math = replace_once(math, function(math, signature), "")
        (out / "station_binary_math.cpp").write_text(math)
        sources[sources.index("src/engine/math/AEMath.cpp")] = str(out / "station_binary_math.cpp")
        compiler = str(args.ndk / "toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe")
        flags += ["--target=armv7a-linux-androideabi21", "-fPIE", "-pie", "-DGOF2_ATLAS_REMOTE_FILES"]
        libraries = ["-lEGL", "-lGLESv2"]
        legacy_stl = args.ndk / "sources/cxx-stl/llvm-libc++/libs/armeabi-v7a"
        if legacy_stl.is_dir():
            gcc = args.ndk / "toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64"
            flags += [f"--gcc-toolchain={gcc}", "-B", str(gcc / "bin"),
                      f"--sysroot={args.ndk / 'platforms/android-21/arch-arm'}"]
            for include in ("sources/cxx-stl/llvm-libc++/include",
                            "sources/cxx-stl/llvm-libc++abi/include",
                            "sources/android/support/include", "sysroot/usr/include",
                            "sysroot/usr/include/arm-linux-androideabi"):
                flags += ["-isystem", str(args.ndk / include)]
            flags += ["-L", str(legacy_stl), "-nostdlib++"]
            libraries += ["-lc++_static", "-lc++abi", "-lunwind"]
        else:
            flags += ["-static-libstdc++"]
    else:
        assert args.angle_dir and args.egl_include
        compiler = args.cxx
        flags += ["-idirafter", str(args.egl_include)]
        libraries = [str(args.angle_dir / name) for name in ("libEGL.dll", "libGLESv2.dll")]
        env["PATH"] = str(args.angle_dir) + os.pathsep + env["PATH"]

    class Assets(socketserver.StreamRequestHandler):
        def handle(self):
            file = (root / self.rfile.readline(512).decode().rstrip("\n")).resolve()
            if not file.is_relative_to(root) or not file.is_file():
                raise ValueError("asset request outside original root")
            data = file.read_bytes()
            self.wfile.write(struct.pack("!I", len(data)))
            self.wfile.write(data)

    for opt in ("-O0", "-O2"):
        exe = out / (f"station_binary_android_{opt[1:]}" if args.ndk else f"station_binary_{opt[1:]}.exe")
        subprocess.run([compiler, *flags, opt, "tools/station_binary_asset_gate.cpp", *sources, *libraries, "-Wl,--gc-sections", "-o", str(exe)], cwd=ROOT, check=True)
        if not args.ndk:
            subprocess.run([str(exe), str(root)], cwd=ROOT, env=env, check=True)
            continue
        adb = [args.adb, "-s", args.serial]
        remote = f"/data/local/tmp/gof2-station-binary-20261019-{opt[1:]}"
        subprocess.run([*adb, "push", str(exe), remote], check=True)
        subprocess.run([*adb, "shell", "chmod", "700", remote], check=True)
        with socketserver.ThreadingTCPServer(("127.0.0.1", 0), Assets) as server:
            port = str(server.server_address[1])
            thread = threading.Thread(target=server.serve_forever, daemon=True)
            thread.start()
            subprocess.run([*adb, "reverse", "--no-rebind", f"tcp:{port}", f"tcp:{port}"], check=True)
            try:
                subprocess.run([*adb, "shell", remote, port], check=True, timeout=600)
            finally:
                subprocess.run([*adb, "reverse", "--remove", f"tcp:{port}"], check=True)
                server.shutdown()
                thread.join()


if __name__ == "__main__":
    main()
