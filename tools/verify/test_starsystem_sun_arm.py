"""Original ARM sun texture call, table stride and ETC1 mip-upload evidence."""
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
class StarSystemSunArmEvidence(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data = SO.read_bytes()
        assert hashlib.sha256(cls.data).hexdigest() == "6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58"
        cls.calls = {}
        for begin, end in ((0x6fbe8, 0x6fc1c), (0x6fd84, 0x6fdf0), (0x79a34, 0x79ad0), (0x133eb0, 0x13402a)):
            asm = subprocess.check_output(delink.tool_argv(str(ROOT / "tools/verify/orbobjdump"), [
                "-d", f"--start-address={begin}", f"--stop-address={end}", str(SO)]), text=True)
            cls.calls.update({int(address, 16): symbol for address, symbol in re.findall(
                r"^\s*([0-9a-f]+):.*\bblx?\s+[0-9a-f]+ <([^>]+)>", asm, re.M)})

    def test_sun_table_and_next_station_lookup(self):
        self.assertEqual(self.data[0x133ec6:0x133eca], bytes.fromhex("31f82010")) # ldrh [r1,r0,lsl #2]
        self.assertEqual(struct.unpack_from("<I", self.data, 0x202260 + 2 * 4)[0], 10033)
        self.assertEqual(struct.unpack_from("<I", self.data, 0x2023a0 + 3 * 4)[0], 10014)
        self.assertEqual(self.calls[0x133ece], "_ZN11AbyssEngine11PaintCanvas13TextureCreateEtRjb@plt")
        self.assertEqual(self.calls[0x133ed6], "_ZN6Status9getSystemEv@plt")
        self.assertEqual(self.calls[0x133eda], "_ZN11SolarSystem8getIndexEv@plt")

    def test_canvas_publishes_only_after_success(self):
        self.assertEqual(self.calls[0x79aa6], "_ZN11AbyssEngine27TextureCreateFromFileInternEPNS_6EngineEPKcPFvPNS_5ImageEPvES6_PjfPNS_15AELoadedTextureEb@plt")
        self.assertEqual(self.data[0x79aaa:0x79ab6], bytes.fromhex("012803d10498a060caf80000"))
        self.assertEqual(self.data[0x79a72:0x79a76], bytes.fromhex("90ed010a")) # float scale, payload +4

    def test_etc1_levels_size_and_real_gl_entry(self):
        for address, opcode in ((0x6fda2, "082c"), (0x6fda6, "48f66452"),
                                (0x6fdac, "4fea5105"), (0x6fdb4, "0825"),
                                (0x6fde2, "09f10109"), (0x6fdea, "0169")):
            self.assertEqual(self.data[address:address + len(bytes.fromhex(opcode))], bytes.fromhex(opcode))
        self.assertEqual(self.calls[0x6fdc2], "glCompressedTexImage2D@plt")

    def test_station_loop_branch_calls(self):
        self.assertEqual(self.calls[0x133f1c], "_ZN6Status25getCurrentCampaignMissionEv@plt")
        self.assertEqual(self.data[0x133f22:0x133f2a], bytes.fromhex("032908bf002866d0"))
        for address in (0x133f34, 0x133f8e):
            self.assertEqual(self.calls[address], "_ZN7Station15getTextureIndexEv@plt")
        self.assertEqual(self.data[0x133f9a:0x133f9e], bytes.fromhex("3bf82010"))
        self.assertEqual(self.calls[0x133fa4], "_ZN11AbyssEngine11PaintCanvas13TextureCreateEtRjb@plt")
        self.assertEqual(self.calls[0x133fba], "_ZN6Status18orbitHasPlanetRingEi@plt")
        self.assertEqual(self.data[0xabcd0:0xabce0], bytes.fromhex("a1f178004fea7001062984bf00207047"))
        self.assertEqual(struct.unpack_from("<I", self.data, 0x2023a0 + 10 * 4)[0], 10021)

    def test_rgba_upload_and_generated_mips(self):
        self.assertEqual(self.data[0x6fc02:0x6fc06], bytes.fromhex("41f60812")) # GL_RGBA
        self.assertEqual(self.calls[0x6fc06], "glTexImage2D@plt")
        self.assertEqual(self.data[0x6fc0c:0x6fc10], bytes.fromhex("007a0028")) # hasMipmaps
        self.assertEqual(self.calls[0x6fc18], "glGenerateMipmap@plt")


if __name__ == "__main__":
    unittest.main()
