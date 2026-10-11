"""Guard the dialogue sound map recovered from Android 2.0.16 .rodata."""

import hashlib
from pathlib import Path
import re
import struct
import unittest


TABLE = Path(__file__).resolve().parents[2] / "src/game/core/DialogueSoundPairs.inc"
ARM_SHA256 = "24201ffc5c374c33aeec4660c20bd8af5788878246a0e6f0b1cae22c09895b52"


class DialogueSoundPairsTest(unittest.TestCase):
    def test_full_arm_table(self):
        values = [int(value) for value in re.findall(r"\d+", TABLE.read_text())]
        self.assertEqual(len(values), 3008)
        self.assertEqual(len(set(values[::2])), 1504)
        raw = struct.pack("<3008I", *values)
        self.assertEqual(hashlib.sha256(raw).hexdigest(), ARM_SHA256)


if __name__ == "__main__":
    unittest.main()
