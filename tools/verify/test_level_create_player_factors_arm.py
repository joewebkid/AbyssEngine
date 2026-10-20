"""Original ARM passes float bit patterns through Ship's integer getters."""
from pathlib import Path
import hashlib
import shutil
import subprocess
import tempfile
import unittest

import delink

ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)
OBJDUMP = str(ROOT / "tools/verify/orbobjdump")


@unittest.skipUnless(SO.is_file(), "original Android binary unavailable")
class LevelCreatePlayerFactorsArm(unittest.TestCase):
    def test_getter_results_are_float_bits(self):
        self.assertEqual(hashlib.sha256(SO.read_bytes()).hexdigest(),
                         "6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58")
        asm = subprocess.check_output(delink.tool_argv(OBJDUMP, [
            "-d", "--start-address=0xaf474", "--stop-address=0xaf4f8", str(SO)]), text=True)
        for address in ("af480", "af49e", "af4dc"):
            self.assertRegex(asm, address + r":.*vmov\s+s0, r0")
        self.assertRegex(asm, r"af488:.*vcmpe\.f32\s+s0, #0\.0")
        self.assertRegex(asm, r"af4a2:.*vmul\.f32\s+s0, s0, s16")
        body = (ROOT / "src/game/world/Level.cpp").read_text()
        self.assertIn("bitsToFloat(ship->getDamageFactor())", body)
        self.assertEqual(body.count("bitsToFloat(ship->getFireRateFactor())"), 2)

    def test_extracted_weapon_factor_branch(self):
        compiler = shutil.which("g++") or "C:/msys64/ucrt64/bin/g++.exe"
        if not Path(compiler).is_file() and not shutil.which(compiler):
            self.skipTest("host C++ compiler unavailable")
        level = (ROOT / "src/game/world/Level.cpp").read_text()
        ship = (ROOT / "src/game/ship/Ship.cpp").read_text()
        helper = level[level.index("static inline float bitsToFloat("):
                       level.index("struct PlayerEgoVtable", level.index("static inline float bitsToFloat("))]
        start = level.index("float dmgFactor = bitsToFloat(ship->getDamageFactor());")
        end = level.index("rate = (int) (fireFactor * (float) rate);", start) + len(
            "rate = (int) (fireFactor * (float) rate);")
        branch = level[start:end]
        getters = "\n".join(next(line for line in ship.splitlines() if line.startswith(signature))
                            for signature in ("int Ship::getFireRateFactor()", "int Ship::getDamageFactor()"))
        source = ("#include <cassert>\n#include <cstring>\n"
                  "struct Ship { float fireRateFactor, damageFactor; "
                  "int getFireRateFactor(); int getDamageFactor(); };\n"
                  + getters + "\n" + helper + "\n"
                  "static void check(float damage, float fire, int dmg, int rate, int wantDmg, int wantRate) {\n"
                  "    Ship fixture{fire, damage}; Ship *ship = &fixture;\n"
                  + branch + "\n    assert(dmg == wantDmg && rate == wantRate);\n}\n"
                  "int main() { check(1.25f, 0.8f, 20, 100, 25, 80); "
                  "check(-0.5f, 0.8f, 9, 100, 9, 56); }\n")
        with tempfile.TemporaryDirectory(prefix="level-player-factors-") as directory:
            path = Path(directory) / "factors.cpp"
            path.write_text(source)
            for opt in ("-O0", "-O2"):
                exe = Path(directory) / ("factors_" + opt[1:] + ".exe")
                subprocess.run([compiler, "-std=gnu++17", opt, str(path), "-o", str(exe)], check=True)
                subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    unittest.main()
