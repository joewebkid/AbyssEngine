"""Run the same bounded Level/GLES gate on an attached Android ARMv7 device.

Pushes only the compiled test. Original assets are streamed read-only through
ADB reverse into process memory; no original file is written onto the device.
Requires host fixtures prepared by run_gles_atlas_asset_gate.py.
"""
import argparse
from pathlib import Path
import socketserver
import struct
import subprocess
import threading

from run_gles_atlas_asset_gate import ROOT, SOURCES, prepare_gles
from run_particle_control_smoke import function


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--asset-root", type=Path, required=True)
    p.add_argument("--out-dir", type=Path, required=True)
    p.add_argument("--ndk", type=Path, required=True)
    p.add_argument("--adb", required=True)
    p.add_argument("--serial", required=True)
    args = p.parse_args()
    root, out = args.asset_root.resolve(), args.out_dir.resolve()
    prepare_gles(out, None, root, reuse=True)
    # The repo emits six operators twice on ARM. Select the existing Vector/Matrix
    # definitions explicitly for this bounded executable; do not hide link errors.
    math = (ROOT / "src/engine/math/AEMath.cpp").read_text()
    for signature in (
        "__attribute__((noinline)) Vector &Vector::operator=(",
        "Vector &Vector::operator-=(", "Vector &Vector::operator*=(const Vector &",
        "Vector &Vector::operator/=(const Vector &", "Matrix &Matrix::operator=(",
        "Matrix &Matrix::operator*=(",
    ):
        math = math.replace(function(math, signature), "", 1)
    math_file = out / "gles_atlas_math.cpp"
    math_file.write_text(math)
    sources = [str(math_file) if s == "engine/math/AEMath" else f"src/{s}.cpp" for s in SOURCES]
    adb = [args.adb, "-s", args.serial]

    class Assets(socketserver.StreamRequestHandler):
        def handle(self):
            name = self.rfile.readline(512).decode("utf-8").rstrip("\n")
            file = (root / name).resolve()
            if not file.is_relative_to(root) or not file.is_file():
                raise ValueError("request is not an existing file inside the asset root")
            data = file.read_bytes()
            self.wfile.write(struct.pack("!I", len(data)))
            self.wfile.write(data)

    compiler = args.ndk / "toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe"
    for opt in ("-O0", "-O2"):
        exe = out / f"gles_atlas_android_{opt[1:]}"
        subprocess.run([str(compiler), "--target=armv7a-linux-androideabi21", "-std=gnu++17",
            opt, "-g0", "-fPIE", "-pie", "-static-libstdc++", "-ffunction-sections", "-fdata-sections",
            "-DGOF2_ATLAS_REMOTE_FILES", "-Isrc", "-Ithird_party/jni", "-Ithird_party/gl", "-Itools",
            "-I", str(out), "tools/gles_atlas_asset_gate.cpp", *sources,
            "-lEGL", "-lGLESv2", "-Wl,--gc-sections", "-o", str(exe)], cwd=ROOT, check=True)
        remote = f"/data/local/tmp/gof2-gles-atlas-20261019-{opt[1:]}"
        subprocess.run([*adb, "push", str(exe), remote], check=True)
        subprocess.run([*adb, "shell", "chmod", "700", remote], check=True)
        with socketserver.ThreadingTCPServer(("127.0.0.1", 0), Assets) as server:
            port = str(server.server_address[1])
            thread = threading.Thread(target=server.serve_forever, daemon=True)
            thread.start()
            subprocess.run([*adb, "reverse", "--no-rebind", f"tcp:{port}", f"tcp:{port}"], check=True)
            try:
                print(f"Android ARMv7 real EGL/GLES gate {opt}; assets stay on host", flush=True)
                subprocess.run([*adb, "shell", remote, port], check=True, timeout=600)
            finally:
                subprocess.run([*adb, "reverse", "--remove", f"tcp:{port}"], check=True)
                server.shutdown()
                thread.join()


if __name__ == "__main__":
    main()
