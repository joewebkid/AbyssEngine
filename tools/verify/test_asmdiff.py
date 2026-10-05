#!/usr/bin/env python3
import unittest
from unittest import mock

import asmdiff


def entry(address, width, mnemonic, operands="", token=None):
    return {
        "address": address,
        "width": width,
        "mnemonic": mnemonic,
        "operands": operands,
        "token": token if token is not None else asmdiff.normalize(mnemonic, operands),
    }


class AsmDiffNormalizationTests(unittest.TestCase):
    def test_comdat_aliases_use_section_and_address(self):
        output = """SYMBOL TABLE:
00000000  w    F .text._ZN1AC2Ev\t00000002 _ZN1AC1Ev
00000000  w    F .text._ZN1AC2Ev\t00000002 _ZN1AC2Ev
00000000  w    F .text._ZN1BD2Ev\t00000002 _ZN1BD1Ev
00000000  w    F .text._ZN1BD2Ev\t00000002 _ZN1BD2Ev

Disassembly of section .text._ZN1AC2Ev:

00000000 <_ZN1AC1Ev>:
   0:\t70 47\t\tbx\tlr

Disassembly of section .text._ZN1BD2Ev:

00000000 <_ZN1BD1Ev>:
   0:\t00 bf\t\tnop
"""
        with mock.patch.object(asmdiff.subprocess, "check_output", return_value=output):
            functions = asmdiff.disassemble("unused.o")
        self.assertIs(functions["_ZN1AC2Ev"], functions["_ZN1AC1Ev"])
        self.assertIs(functions["_ZN1BD2Ev"], functions["_ZN1BD1Ev"])
        self.assertIsNot(functions["_ZN1AC2Ev"], functions["_ZN1BD2Ev"])

    def test_conditional_direct_calls_are_calls(self):
        self.assertEqual(asmdiff.normalize("blxmi", "0x68364"), "call <t>")
        self.assertEqual(asmdiff.normalize("blne.w", "0x1234"), "call <t>")

    def test_tbh_payload_uses_bounds_check(self):
        insns = [
            "cmp r2, #2",
            "tbh [pc, r2, lsl #1]",
            ".short #x",
            ".word #x",
            "movs r0, #0",
        ]
        normalized, widths = asmdiff._normalize_jump_tables(insns, [2, 4, 2, 4, 2])
        self.assertEqual(normalized, [
            "cmp r2, #2",
            "tbh [pc, r2, lsl #1]",
            "jump-table tbh 3",
            "movs r0, #0",
        ])
        self.assertEqual(widths, [2, 4, 6, 2])

    def test_jump_table_with_unproven_boundary_is_untouched(self):
        insns = ["cmp r1, #2", "tbb [pc, r1]", ".short #x"]
        self.assertEqual(
            asmdiff._normalize_jump_tables(insns, [2, 4, 2]),
            (insns, [2, 4, 2]),
        )

    def test_pc_relative_reference_recovers_undefined_literal(self):
        entries = [
            entry(0, 4, "vldr", "s0, [pc, #12]"),
            entry(4, 2, "movs", "r0, #0"),
            entry(6, 2, "bx", "lr"),
            entry(8, 4, ".word", "0x0", ".word #x"),
            entry(12, 4, ".word", "0x0", ".word #x"),
            entry(16, 4, ".raw", "", ".raw "),
        ]
        insns, widths = asmdiff._normalize_literal_pools(entries)
        self.assertEqual(insns[-1], ".word #x")
        self.assertEqual(widths[-1], 4)

    def test_only_pool_alignment_nop_is_removed(self):
        insns, widths = asmdiff._normalize_pool_alignment(
            ["nop", "movs r0, #0", "nop", ".word #x"], [2, 2, 2, 4])
        self.assertEqual(insns, ["nop", "movs r0, #0", ".word #x"])
        self.assertEqual(widths, [2, 2, 4])

    def test_source_shape_ignores_register_and_stack_slot_allocation(self):
        target = [
            "ldr r0, [r10, #452]",
            "str r0, [sp, #24]",
            "add r1, sp, #32",
            "sub sp, #96",
            "add r4, r4, r5",
            "bne <t>",
        ]
        base = [
            "ldr r0, [r5, #452]",
            "str r0, [sp, #80]",
            "add r6, sp, #88",
            "sub sp, #144",
            "add r8, r8, r6",
            "bne <t>",
        ]
        metrics = asmdiff.source_shape_metrics(target, base)
        self.assertEqual(metrics["source_match"], 100.0)
        self.assertEqual(metrics["instruction_inventory_match"], 100.0)

    def test_source_shape_ignores_vfp_register_allocation(self):
        target = [
            "vsub.f32 s4, s0, s18",
            "vmul.f32 s10, s8, s2",
            "vst1.64 {d16-d17}, [r2]",
        ]
        base = [
            "vsub.f32 s2, s0, s18",
            "vmul.f32 s4, s8, s2",
            "vst1.64 {d4-d5}, [r8]",
        ]
        metrics = asmdiff.source_shape_metrics(target, base)
        self.assertEqual(metrics["source_match"], 100.0)
        self.assertEqual(metrics["instruction_inventory_match"], 100.0)

    def test_source_shape_ignores_thumb_encoding_width(self):
        target = ["ldr.w r0, [r5, #452]", "mov.n r1, r0", "add.w r2, r3, #4"]
        base = ["ldr r4, [r8, #452]", "mov r6, r4", "add r7, r9, #4"]
        metrics = asmdiff.source_shape_metrics(target, base)
        self.assertEqual(metrics["source_match"], 100.0)
        self.assertEqual(metrics["instruction_inventory_match"], 100.0)
        self.assertEqual(metrics["opcode_inventory_match"], 100.0)

    def test_source_shape_equates_thumb_zero_branch_encodings(self):
        target = ["ldr r0, [r4, #12]", "cbz <t>", "movs r0, #1"]
        base = ["ldr r5, [r8, #12]", "cmp r5, #0", "beq <t>", "movs r5, #1"]
        metrics = asmdiff.source_shape_metrics(target, base)
        self.assertEqual(metrics["source_match"], 100.0)
        self.assertEqual(metrics["control_flow_match"], 100.0)

        changed = asmdiff.source_shape_metrics(target, [
            "ldr r5, [r8, #12]", "cmp r5, #0", "bne <t>", "movs r5, #1"
        ])
        self.assertLess(changed["source_match"], 100.0)

    def test_source_shape_equates_dualword_memory_encodings(self):
        target = ["ldrd r2, r1, [r0, #308]", "strd r4, r5, [r6, #20]"]
        base = [
            "ldr.w r8, [r10, #308]", "ldr r9, [r10, #312]",
            "str r1, [r3, #20]", "str.w r2, [r3, #24]",
        ]
        metrics = asmdiff.source_shape_metrics(target, base)
        self.assertEqual(metrics["source_match"], 100.0)
        self.assertEqual(metrics["instruction_inventory_match"], 100.0)
        self.assertEqual(metrics["opcode_inventory_match"], 100.0)

    def test_source_shape_ignores_spills_and_vector_store_element_width(self):
        target = [
            "push {r4, r5, r7, lr}",
            "vpush {d3-d8}",
            "vst1.64 {d16-d17}, [r2]",
            "vpop {d3-d8}",
            "pop {r4, r5, r7, pc}",
        ]
        base = [
            "push {r4, r5, r6, r7, lr}",
            "vpush {d2-d10}",
            "vst1.32 {d4-d5}, [r8]",
            "vpop {d2-d10}",
            "pop {r4, r5, r6, r7, pc}",
        ]
        metrics = asmdiff.source_shape_metrics(target, base)
        self.assertEqual(metrics["source_match"], 100.0)
        self.assertEqual(metrics["instruction_inventory_match"], 100.0)
        self.assertEqual(metrics["opcode_inventory_match"], 100.0)

    def test_source_shape_ignores_frame_pointer_slots_and_epilogues(self):
        target = ["add.w r0, fp, #-80", "addeq sp, #204", "ldr r1, [fp, #-44]"]
        base = ["add.w r5, fp, #-120", "addeq sp, #172", "ldr r8, [fp, #-76]"]
        metrics = asmdiff.source_shape_metrics(target, base)
        self.assertEqual(metrics["source_match"], 100.0)
        self.assertEqual(metrics["instruction_inventory_match"], 100.0)

    def test_source_shape_keeps_field_offsets_and_control_flow(self):
        target = ["ldr r0, [r5, #452]", "beq <t>", "call <t>", "b <t>"]
        base = ["ldr r0, [r10, #456]", "bne <t>", "b <t>", "call <t>"]
        metrics = asmdiff.source_shape_metrics(target, base)
        self.assertLess(metrics["instruction_inventory_match"], 100.0)
        self.assertLess(metrics["control_flow_match"], 100.0)
        self.assertLess(metrics["source_match"], 100.0)

    def test_source_shape_tolerates_basic_block_placement(self):
        target = ["call <t>", "beq <t>", "call <t>", "bne <t>"]
        base = ["call <t>", "bne <t>", "call <t>", "beq <t>"]
        metrics = asmdiff.source_shape_metrics(target, base)
        self.assertLess(metrics["ordered_control_flow_match"], 100.0)
        self.assertEqual(metrics["control_inventory_match"], 100.0)
        self.assertEqual(metrics["control_flow_match"], 100.0)
        self.assertEqual(metrics["call_count_match"], 100.0)

    def test_source_shape_counts_missing_calls(self):
        target = ["call <t>", "call <t>", "bne <t>"]
        base = ["call <t>", "bne <t>"]
        metrics = asmdiff.source_shape_metrics(target, base)
        self.assertEqual(metrics["call_count_match"], 50.0)
        self.assertLess(metrics["source_match"], 100.0)


if __name__ == "__main__":
    unittest.main()
