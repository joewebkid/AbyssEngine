"""Original ARM anchors for the atlas upload ownership and manager ABI repair."""
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
class TextureAtlasArmEvidence(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.data = SO.read_bytes()
        assert hashlib.sha256(cls.data).hexdigest() == "6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58"
        cls.asm = subprocess.check_output(delink.tool_argv(str(ROOT / "tools/verify/orbobjdump"), [
            "-d", "--start-address=0x6f7f4", "--stop-address=0x7000c", str(SO)]), text=True)
        cls.calls = {int(address, 16): symbol for address, symbol in re.findall(
            r"^\s*([0-9a-f]+):.*\bblx?\s+[0-9a-f]+ <([^>]+)>", cls.asm, re.M)}

    def test_manager_canvas_indirection_and_reacquisition(self):
        # .text virtual address equals file offset in this pinned ELF.
        for address, instruction in (
            (0x6ff8a, 0x6b31),  # ldr r1, [r6, #48]: Engine::appManager
            (0x6ff90, 0x6808),  # ldr r0, [r1, #0]: manager::paintCanvas
            (0x6ff9c, 0x6b30),  # reacquire manager after ArrayAdd
            (0x6ff9e, 0x6800),  # reacquire canvas
            (0x6ffa0, 0x6900),  # canvas +16: managed array count
            (0x6ffa2, 0x3801),  # managed output is count - 1
        ):
            self.assertEqual(struct.unpack_from("<H", self.data, address)[0], instruction)
        self.assertEqual(self.calls[0x6ff98], "_Z8ArrayAddIPN11AbyssEngine15AELoadedTextureEEvT_R5ArrayIS3_E@plt")

    def test_first_gl_query_and_unmanaged_error_cleanup(self):
        # These two compatibility bodies really return immediately in Android;
        # their reproduction is not a replacement for any GLES2 upload call.
        for address in (0x6dad2, 0x6daf0):
            self.assertEqual(self.data[address:address + 2], bytes.fromhex("7047"))
        self.assertEqual(self.calls[0x6f820], "glGetError@plt")
        for address, symbol in (
            (0x6feac, "_ZN11AbyssEngine6StringC1EPKcb"),
            (0x6feb6, "_ZN11AbyssEngine6StringaSERKS0_"),
            (0x6fec0, "_ZN11AbyssEngine6StringD1Ev"),
            (0x6fec8, "glDeleteTextures"),
            (0x6fece, "_ZN11AbyssEngine12ImageReleaseEPPNS_5ImageE"),
        ):
            self.assertEqual(self.calls[address], symbol + "@plt")

    def test_managed_allocation_and_publication_targets(self):
        self.assertEqual(struct.unpack_from("<H", self.data, 0x6fed8)[0], 0x201c) # sizeof(record)=28
        for address, symbol in (
            (0x6feda, "_Znwj"), (0x6fee2, "_ZN11AbyssEngine6StringC1Ev"),
            (0x6ff02, "_ZN11AbyssEngine6String3SetEPKc"),
            (0x6ff06, "glGetError"), (0x6ff14, "glDeleteTextures"),
            (0x6ff6c, "_Z13AELabelObjectjjPKc"),
            (0x6ffaa, "_ZN11AbyssEngine12ImageReleaseEPPNS_5ImageE"),
        ):
            self.assertEqual(self.calls[address], symbol + "@plt")


if __name__ == "__main__":
    unittest.main()
