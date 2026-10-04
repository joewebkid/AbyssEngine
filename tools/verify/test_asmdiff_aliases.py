"""Regression checks for section-local objdump function aliases.

Run with python tools/verify/test_asmdiff_aliases.py [ArrayInstantiations.o].
"""
import json
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

import asmdiff


class AliasTests(unittest.TestCase):
    def test_sections_with_equal_addresses_keep_distinct_bodies(self):
        output = """SYMBOL TABLE:
00000000 g     F .text 00000002 plain
00000000 g     F .text 00000002 plain_alias
00000000 w     F .text.first 00000002 first
00000000 w     F .text.first 00000002 first_alias
00000000 w     F .text.second 00000002 second
00000000 w     F .text.second 00000002 second_alias
00000000 g     O .data 00000002 data
00000000 w     F .text.absent 00000002 absent
Disassembly of section .text:
00000000 <plain>:
   0:\t4770\tbx lr
Disassembly of section .text.first:
00000000 <first>:
   0:\t2001\tmovs r0, #1
Disassembly of section .text.second:
00000000 <second>:
   0:\t2002\tmovs r0, #2
Disassembly of section .data:
   0:\tffff\t.raw
"""
        with patch.object(asmdiff.subprocess, "check_output", return_value=output):
            functions = asmdiff.disassemble("fixture.o")
        for name, raw in (("plain", "4770"), ("first", "2001"), ("second", "2002")):
            self.assertIs(functions[name], functions[name + "_alias"])
            self.assertEqual(functions[name]["bytes"], raw)
        self.assertNotIn("data", functions)
        self.assertNotIn("absent", functions)

    @unittest.skipUnless(len(sys.argv) > 1, "pass a real ArrayInstantiations.o")
    def test_seven_real_array_abi_aliases(self):
        functions = asmdiff.disassemble(sys.argv[1])
        root = Path(__file__).resolve().parents[2]
        audit = json.loads((root / "docs/validation/gun_abi_alias_audit_2026-10-02.json").read_text())
        self.assertEqual(len(audit["seven_cases"]), 7)
        for case in audit["seven_cases"]:
            with self.subTest(symbol=case["symbol"]):
                self.assertIs(functions[case["symbol"]], functions[case["same_body_as"]])


if __name__ == "__main__":
    unittest.main(argv=[sys.argv[0]])
