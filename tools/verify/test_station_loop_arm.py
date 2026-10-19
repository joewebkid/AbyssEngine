"""Original ARM evidence for station cleanup, pointer arrays and texture 10056."""
import hashlib
from pathlib import Path
import re
import struct
import subprocess
import unittest

import delink

ROOT = Path(__file__).resolve().parents[2]
SO = Path(delink.DEFAULT_SO)


@unittest.skipUnless(SO.is_file(), "original Android ELF unavailable")
class StationLoopArmEvidence(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data = SO.read_bytes()
        assert hashlib.sha256(cls.data).hexdigest() == "6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58"
        cls.calls = {}
        for begin, end in ((0xa8e28, 0xa8e8e), (0x11a314, 0x11a34c), (0x134030, 0x1340bc)):
            asm = subprocess.check_output(delink.tool_argv(str(ROOT / "tools/verify/orbobjdump"), [
                "-d", f"--start-address={begin}", f"--stop-address={end}", str(SO)]), text=True)
            cls.calls.update({int(address, 16): symbol for address, symbol in re.findall(
                r"^\s*([0-9a-f]+):.*\bblx?\s+[0-9a-f]+ <([^>]+)>", asm, re.M)})

    def test_station_cleanup_before_typed_target_allocation(self):
        for address, symbol in (
            (0x134038, "_Z19ArrayReleaseClassesIP7StationEvR5ArrayIT_E"),
            (0x13403e, "_ZN5ArrayIP7StationED2Ev"), (0x134042, "_ZdlPv"),
            (0x13404e, "_ZN5ArrayIP8KIPlayerEC2Ev"),
            (0x13405c, "_Z14ArraySetLengthIP8KIPlayerEvjR5ArrayIT_E"),
            (0x134068, "_ZN5ArrayIP10AEGeometryEC2Ev"),
            (0x134078, "_Z14ArraySetLengthIP10AEGeometryEvjR5ArrayIT_E"),
            (0x134084, "_ZN5ArrayIN11AbyssEngine6AEMath6VectorEEC2Ev"),
            (0x134092, "_Z14ArraySetLengthIN11AbyssEngine6AEMath6VectorEEvjR5ArrayIT_E"),
            (0x1340b8, "_ZN11AbyssEngine8AERandom7setSeedEx"),
        ):
            self.assertEqual(self.calls[address], symbol + "@plt")
        self.assertEqual(self.data[0x134046:0x134048], bytes.fromhex("0c20"))
        self.assertEqual(self.data[0x134056:0x13405a], bytes.fromhex("c8f81850"))
        self.assertEqual(self.data[0x134076:0x134078], bytes.fromhex("0130"))
        self.assertEqual(self.data[0x134090:0x134092], bytes.fromhex("0130"))

    def test_station_release_owns_records_and_pointer_storage(self):
        for address, symbol in ((0xa8e42, "_ZN7StationD1Ev"), (0xa8e46, "_ZdlPv"),
                                (0xa8e68, "_ZdaPv"), (0xa8e82, "_ZdaPv")):
            self.assertEqual(self.calls[address], symbol + "@plt")
        self.assertEqual(self.data[0xa8e5e:0xa8e62], bytes.fromhex("d9e90101"))
        self.assertEqual(self.data[0xa8e6e:0xa8e72], bytes.fromhex("c9f80400"))

    def test_big_texture_id_path_and_zero_scale(self):
        self.assertEqual(self.data[0x11a328:0x11a32a], bytes.fromhex("0022"))
        self.assertEqual(self.calls[0x11a32a], "_ZN11AbyssEngine15ResourceTextureC2EPKcf@plt")
        self.assertEqual(self.data[0x11a334:0x11a338], bytes.fromhex("42f24871"))
        path = struct.unpack_from("<I", self.data, 0x11a330)[0] + 0x11a32a
        original = self.data[path:self.data.index(b"\0", path)].decode()
        self.assertEqual(original, "data/assets/main/3d/textures/low/etc/planets/planet_014_big.aei")
        low = (ROOT / "src/engine/render/LowResourceTable.cpp").read_text()
        block = low[low.index("res->id = 0x2748;"):]
        block = block[:block.index("canvas->AddResource(res);")]
        self.assertIn('"' + original + '", 0.0f)', block)


if __name__ == "__main__":
    unittest.main()
