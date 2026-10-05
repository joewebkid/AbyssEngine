import unittest

import delink


class DelinkPaddingTests(unittest.TestCase):
    def test_preserves_zero_literal_referenced_by_thumb_vldr(self):
        # VLDR s0, [pc, #32] at offset zero reads the word at offset 36.
        body = bytearray(bytes.fromhex("9f ed 08 0a") + b"\x70\x47" * 16 + b"\0" * 6)
        trimmed, pool = delink.trim_zero_padding(body, True)
        self.assertEqual((len(trimmed), pool), (40, 36))

    def test_removes_unreferenced_zero_padding(self):
        body = bytearray(b"\x70\x47" + b"\0" * 4)
        self.assertEqual(delink.trim_zero_padding(body, True), (b"\x70\x47", None))

    def test_does_not_mark_double_literal_as_single_word(self):
        body = bytearray(bytes.fromhex("df ed 08 0b") + b"\x70\x47" * 16 + b"\0" * 4)
        self.assertEqual(delink.trim_zero_padding(body, True)[1], None)


if __name__ == "__main__":
    unittest.main()
