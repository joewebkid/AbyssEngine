"""Pinned original ARM evidence for station reading and the next StarSystem prefix."""
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
class StationBinaryArmEvidence(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data = SO.read_bytes()
        assert hashlib.sha256(cls.data).hexdigest() == "6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58"
        cls.calls = {}
        for start, end in ((0x12014c, 0x1202b8), (0x133b68, 0x133eea)):
            asm = subprocess.check_output(delink.tool_argv(str(ROOT / "tools/verify/orbobjdump"), [
                "-d", f"--start-address={start}", f"--stop-address={end}", str(SO)]), text=True)
            cls.calls.update({int(address, 16): symbol for address, symbol in re.findall(
                r"^\s*([0-9a-f]+):.*\bblx?\s+[0-9a-f]+ <([^>]+)>", asm, re.M)})

    def call(self, address, symbol):
        self.assertEqual(self.calls[address], symbol + "@plt")

    def test_solar_array_and_record_abi(self):
        self.assertEqual(self.data[0x1558a2:0x1558a6], bytes.fromhex("806b7047")) # ldr r0,[r0,#56]; bx lr
        for address, instruction in ((0x120162, 0x200c), (0x12019e, 0x6800),
                                     (0x1201f0, 0x6868), (0x1201fa, 0x2034)):
            self.assertEqual(struct.unpack_from("<H", self.data, address)[0], instruction)

    def test_path_lifetime_and_record_publication(self):
        for address, symbol in (
            (0x12017a, "_ZN6AEFile9FileExistERKN11AbyssEngine6StringE"),
            (0x120182, "_ZN11AbyssEngine6StringD1Ev"),
            (0x120192, "_ZN6AEFile8OpenReadEPKcPj"),
            (0x120198, "_ZN11SolarSystem11getStationsEv"),
            (0x1201be, "_ZN6AEFile12ReadSwitchedERN11AbyssEngine6StringEj"),
            (0x1201c4, "_ZN11AbyssEngine6String15ConvertFromUTF8Ev"),
            (0x12021c, "_ZN7StationC1EN11AbyssEngine6StringEiiii"),
            (0x120228, "_ZN11AbyssEngine6StringD1Ev"),
            (0x120256, "_ZN6AEFile5CloseEj"),
            (0x12025c, "_ZN11AbyssEngine6StringD1Ev"),
        ):
            self.call(address, symbol)
        self.assertEqual(self.data[0x120222:0x120226], bytes.fromhex("40f828a0")) # publish before String D1
        self.assertEqual(self.data[0x120242:0x120246], bytes.fromhex("b9f1870f")) # 135 records

    def test_heap_reader_destroyed_before_star_arrays(self):
        for address, symbol in (
            (0x133df8, "_ZN11SolarSystem11getStationsEv"), (0x133e00, "_Znwj"),
            (0x133e06, "_ZN8FileReadC1Ev"), (0x133e10, "_ZN6Status9getSystemEv"),
            (0x133e18, "_ZN8FileRead18loadStationsBinaryEP11SolarSystem"),
            (0x133e20, "_ZN8FileReadD1Ev"), (0x133e24, "_ZdlPv"),
            (0x133e30, "_ZN5ArrayIjEC2Ev"), (0x133e40, "_Z14ArraySetLengthIjEvjR5ArrayIT_E"),
            (0x133e4c, "_ZN5ArrayIiEC2Ev"), (0x133e5a, "_Z14ArraySetLengthIiEvjR5ArrayIT_E"),
            (0x133eb8, "_ZN11SolarSystem15getTextureIndexEv"),
            (0x133ece, "_ZN11AbyssEngine11PaintCanvas13TextureCreateEtRjb"),
        ):
            self.call(address, symbol)
        self.assertEqual(struct.unpack_from("<H", self.data, 0x133dfe)[0], 0x2001)
        # Literal loads resolve to GOT entries, not the similarly named class statics.
        relocations = subprocess.check_output(delink.tool_argv(str(ROOT / "tools/verify/orbobjdump"), ["-R", str(SO)]), text=True)
        for pool, pc, symbol in ((0x133f50, 0x133b88, "_ZN7Globals6statusE"),
                                 (0x133f54, 0x133bd0, "_ZN7Globals6CanvasE")):
            got = struct.unpack_from("<I", self.data, pool)[0] + pc
            self.assertRegex(relocations, rf"(?m)^{got:08x}\s+R_ARM_GLOB_DAT\s+{symbol}")

    def test_all_star_texture_table_values(self):
        source = (ROOT / "src/game/world/StarSystem.cpp").read_text()
        for name, address, count in (("systemPlanetTextures", 0x202260, 19),
                                     ("systemColors", 0x2022ac, 34),
                                     ("stationPlanetTextures", 0x202334, 27),
                                     ("stationTextures", 0x2023a0, 27)):
            text = re.search(r"g_StarSystem_ctor_" + name + r"\[\d+\] = \{(.*?)\};", source, re.S)[1]
            current = tuple(int(v.strip(), 0) for v in text.split(",") if v.strip())
            self.assertEqual(current, struct.unpack_from("<" + "I" * count, self.data, address))


if __name__ == "__main__":
    unittest.main()
