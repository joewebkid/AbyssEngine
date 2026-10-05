import struct
import unittest

from delink import elf_function_sizes, function_end


class DelinkSizeTests(unittest.TestCase):
    def test_dynsym_function_size_bounds_next_symbol_gap(self):
        elf = bytearray(0x180)
        elf[:5] = b"\x7fELF\x01"
        struct.pack_into("<I", elf, 0x20, 0x40)  # section headers
        struct.pack_into("<HH", elf, 0x2E, 40, 2)
        struct.pack_into("<I", elf, 0x68 + 4, 11)  # DYNSYM
        struct.pack_into("<II", elf, 0x68 + 16, 0x100, 64)
        struct.pack_into("<I", elf, 0x68 + 36, 16)
        struct.pack_into("<IIIBBH", elf, 0x110, 0, 0x1001, 12, 2, 0, 1)
        struct.pack_into("<IIIBBH", elf, 0x120, 0, 0x1001, 0, 2, 0, 1)
        struct.pack_into("<IIIBBH", elf, 0x130, 0, 0x1000, 108, 0, 0, 1)

        sizes = elf_function_sizes(elf)
        self.assertEqual(sizes, {0x1000: 12})
        self.assertEqual(function_end(0x1000, 0x106C, sizes), 0x100C)
        self.assertEqual(function_end(0x2000, 0x206C, sizes), 0x206C)


if __name__ == "__main__":
    unittest.main()
