"""Compare recovered ship-group resource tables with Android 2.0.16 bytes."""

import hashlib
from pathlib import Path
import re
import shutil
import struct
import subprocess
import tempfile
import unittest

import delink


ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)
SOURCE = ROOT / "src/game/core/Globals.cpp"
SHA256 = "6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58"
TABLES = {
    "res": (0x1FE3A0, "H", 64),
    "mesh": (0x1FE420, "H", 64),
    "extra": (0x1FE4A0, "H", 64),
    "lod": (0x1FE520, "I", 192),
    "child": (0x1FE820, "I", 192),
}


@unittest.skipUnless(SO.is_file(), "original Android binary unavailable")
class ShipGroupTablesArm(unittest.TestCase):
    def test_recovered_tables_equal_original_bytes(self):
        binary = SO.read_bytes()
        self.assertEqual(hashlib.sha256(binary).hexdigest(), SHA256)
        source = SOURCE.read_text()
        for name, (offset, width, count) in TABLES.items():
            with self.subTest(table=name):
                body = re.search(
                    rf"static const [\w ]+ gGSG_{name}Table\[{count}\] = \{{([^}}]*)\}};",
                    source, re.S)
                self.assertIsNotNone(body)
                values = [int(token, 16) for token in re.findall(r"0x[0-9a-f]+", body.group(1))]
                self.assertEqual(len(values), count)
                original = struct.unpack_from("<" + width * count, binary, offset)
                self.assertEqual(values, list(original))

    def test_tail_mesh_ids_and_exceptions(self):
        asm = subprocess.check_output(delink.tool_argv(str(ROOT / "tools/verify/orbobjdump"), [
            "-d", "--start-address=0xe518a", "--stop-address=0xe523e", str(SO)]), text=True)
        for pattern in (r"e51be:.*cmp\s+r6, #39", r"e51c2:.*cmpne\s+r6, #41",
                        r"e51a8:.*mov\s+r2, r4", r"e51ac:.*movs\s+r3, #0",
                        r"e51d6:.*movw\s+r2, #17900", r"e5224:.*movw\s+r2, #18000",
                        r"e5228:.*add\s+r2, r6", r"e522e:.*movs\s+r3, #0"):
            self.assertRegex(asm, pattern)

        compiler = shutil.which("g++")
        if not compiler:
            self.skipTest("host C++ compiler unavailable")
        source = SOURCE.read_text()
        start = source.index("        unsigned short extra = gGSG_extraTable[kind];", source.index(
            "AEGeometry *Globals::getShipGroup("))
        end = source.index("        unsigned lastVisibleDist", start)
        branch = source[start:end]
        extra_table = re.search(
            r"static const unsigned short gGSG_extraTable\[64\] = \{[^}]*\};", source, re.S)
        self.assertIsNotNone(extra_table)
        fixture = '''#include <cassert>
#include <initializer_list>
#include <vector>
EXTRA_TABLE
struct Canvas {
    std::vector<unsigned> ids;
    unsigned next = 1;
    void TransformCreate(unsigned &out) { out = next++; }
    void TransformAddMesh(unsigned, unsigned short mesh, bool flag) {
        assert(!flag); ids.push_back(mesh);
    }
};
struct Geometry { int children = 0; void addChild(unsigned) { ++children; } };
void check(int kind, bool wireframe, std::initializer_list<unsigned> expected) {
    Canvas fixture; Canvas *canvas = &fixture;
    Geometry value; Geometry *geom = &value;
BRANCH
    assert(fixture.ids == std::vector<unsigned>(expected));
    assert(value.children == int(expected.size()));
}
int main() {
    check(0, true, {0x490c, 17900}); check(4, true, {17904});
    check(39, true, {}); check(41, true, {});
    check(0, false, {0x490c, 18000}); check(63, false, {0x498a, 18063});
}
'''.replace("EXTRA_TABLE", extra_table.group(0)).replace("BRANCH", branch)
        with tempfile.TemporaryDirectory(prefix="ship-group-tail-") as directory:
            path = Path(directory) / "ship_group_tail.cpp"
            path.write_text(fixture)
            for optimization in ("-O0", "-O2"):
                exe = Path(directory) / ("ship_group_tail_" + optimization[1:] + ".exe")
                subprocess.run([compiler, "-std=gnu++17", optimization, str(path), "-o", str(exe)], check=True)
                subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    unittest.main()
