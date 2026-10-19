"""Check the source-backed Level player allocation and field layout contract."""

from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    compiler = shutil.which("g++")
    if compiler is None:
        raise SystemExit("g++ is required")
    source = (ROOT / "src/game/world/Level.cpp").read_text()
    begin = source.index("void Level::createPlayer()")
    end = source.index("void Level::wingmanDied(", begin)
    body = source[begin:end]
    assert "::operator new(sizeof(Player))" in body
    assert "::operator new(sizeof(PlayerEgo))" in body
    assert "pl->field_69 = 1;" in body
    assert "((uint8_t *) pl + 0x69)" not in body
    with tempfile.TemporaryDirectory(prefix="level-player-layout-", dir=ROOT / "_work") as temp:
        exe = Path(temp) / "level_player_allocation_smoke.exe"
        subprocess.run([
            compiler, "-std=gnu++17", "-Isrc",
            "tools/level_player_allocation_smoke.cpp", "-o", str(exe),
        ], cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)
    print("Level player allocation/layout smoke: OK")


if __name__ == "__main__":
    main()
