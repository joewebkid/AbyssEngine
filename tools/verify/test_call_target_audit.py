import unittest
from unittest import mock

import asmdiff
import call_target_audit as audit


class CallTargetAuditTests(unittest.TestCase):
    def test_relocation_record_keeps_type_and_target_without_changing_mask(self):
        output = """SYMBOL TABLE:
00000000 g     F .text\t00000004 caller

Disassembly of section .text:

00000000 <caller>:
   0:\tf7ff fffe \tbl\t0 <callee>
\t\t\t0: R_ARM_THM_CALL\tcallee
"""
        with mock.patch.object(asmdiff.subprocess, "check_output", return_value=output):
            body = asmdiff.disassemble("fixture.o")["caller"]
        self.assertEqual(body["reloc"], set(range(4)))
        self.assertEqual(body["relocation_records"], [
            {"offset": 0, "type": "R_ARM_THM_CALL", "symbol": "callee"}])

    def test_wrong_callee_is_detected_even_when_masked_bytes_match(self):
        original = audit.original_direct_calls("""Disassembly of section .text:
00000100 <caller@@Base>:
 100:\tf7ff fffe \tblx\t200 <wanted@plt>
""")
        base = {"bytes": "f7fffffe", "reloc": set(range(4)),
                "relocation_records": [{"offset": 0, "type": "R_ARM_THM_CALL",
                                        "symbol": "wrong"}]}
        target = {"bytes": "f7ecabcd"}
        self.assertTrue(asmdiff.linked_equal(target, base))
        rows = audit.audit_function("test/unit", "caller", base, target, original)
        self.assertEqual(rows[0]["status"], "mismatch_candidate")
        self.assertEqual(rows[0]["original_target"], "wanted@plt")

    def test_matching_and_unresolved_calls_remain_separate(self):
        original = audit.original_direct_calls("""Disassembly of section .text:
00000100 <caller@@Base>:
 100:\tf7ff fffe \tblx\t200 <wanted@plt>
""")
        base = {"bytes": "f7fffffe", "reloc": set(range(4)),
                "relocation_records": [{"offset": 0, "type": "R_ARM_THM_CALL",
                                        "symbol": "wanted"}]}
        target = {"bytes": "f7ecabcd"}
        self.assertEqual(audit.audit_function("u", "caller", base, target, original)[0]["status"],
                         "match")
        self.assertEqual(audit.audit_function("u", "absent", base, target, original)[0]["status"],
                         "unresolved")


if __name__ == "__main__":
    unittest.main()
