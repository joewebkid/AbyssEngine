"""Check the production camera pointer accessors and host field width."""

from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def extract(source, start, end):
    begin = source.index(start)
    return source[begin:source.index(end, begin)]


def main():
    compiler = shutil.which("g++")
    if compiler is None:
        raise SystemExit("g++ is required")
    source = (ROOT / "src/game/ship/PlayerEgo.cpp").read_text()
    methods = "\n".join((
        extract(source, "TargetFollowCamera *PlayerEgo::getTargetFollowCamera() {",
                "unsigned char PlayerEgo::isCloaked()"),
        extract(source, "void PlayerEgo::hitCamera() {", "int PlayerEgo::hackingWon()"),
        extract(source, "void PlayerEgo::setTargetFollowCamera(TargetFollowCamera *cam) {",
                "void PlayerEgo::calcCollision("),
    ))
    fixture = (ROOT / "tools/player_ego_camera_pointer_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_METHODS", methods)
    with tempfile.TemporaryDirectory(prefix="ego-camera-", dir=ROOT / "_work") as temp:
        temp = Path(temp)
        header_check = temp / "header.cpp"
        header_check.write_text(
            '#include <cstddef>\n#include <type_traits>\n'
            '#include "game/ship/PlayerEgo.h"\n'
            'static_assert(std::is_same<decltype(PlayerEgo::targetFollowCamera), '
            'TargetFollowCamera *>::value, "camera field must hold a pointer");\n'
            'static_assert(offsetof(PlayerEgo, explosion) >= '
            'offsetof(PlayerEgo, targetFollowCamera) + sizeof(void *), '
            '"camera pointer must not overlap explosion");\n'
        )
        subprocess.run([compiler, "-std=gnu++17", "-I", str(ROOT / "src"),
                        "-c", str(header_check), "-o", str(temp / "header.o")],
                       cwd=ROOT, check=True)
        subject = temp / "subject.cpp"
        subject.write_text(fixture)
        exe = temp / "smoke.exe"
        for opt in ("-O0", "-O2"):
            subprocess.run([compiler, "-std=gnu++17", opt, str(subject), "-o", str(exe)],
                           cwd=ROOT, check=True)
            subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
