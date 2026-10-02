import unittest
from unittest.mock import patch
import asmdiff


class NeonLiteralPoolTests(unittest.TestCase):
    def entries(self, middle=None, displacement=16):
        instructions = [(0, 4, 'addw', f'r0, pc, #{displacement}'),
                        (4, 4, 'vmov.i32', 'q8, #0')]
        if middle:
            instructions[1] = (4, 4, *middle)
        instructions += [(8, 4, 'vld1.64', '{d18-d19}, [r0]'),
                         (12, 4, 'bx', 'lr'), (16, 4, 'nop', '')]
        instructions += [(address, 2, 'movs', 'r0, r0') for address in range(20, 36, 2)]
        return [dict(address=a, width=w, mnemonic=m, operands=o,
                     token=asmdiff.normalize(m, o)) for a, w, m, o in instructions]

    def test_proven_16_byte_pool(self):
        insns, widths = asmdiff._normalize_literal_pools(self.entries())
        self.assertEqual(insns[-4:], ['.word #x'] * 4)
        self.assertEqual(widths[-4:], [4] * 4)

    def test_overwritten_address_not_data(self):
        insns, _ = asmdiff._normalize_literal_pools(self.entries(('mov', 'r0, r1')))
        self.assertNotIn('.word #x', insns)

    def test_branch_breaks_proof(self):
        insns, _ = asmdiff._normalize_literal_pools(self.entries(('bne', '0x10')))
        self.assertNotIn('.word #x', insns)

    def test_outside_function_not_data(self):
        insns, _ = asmdiff._normalize_literal_pools(self.entries(displacement=100))
        self.assertNotIn('.word #x', insns)

    def test_incoming_branch_prevents_data_classification(self):
        entries = self.entries()
        entries[3].update(mnemonic='b', operands='14', token='b <t>')
        insns, _ = asmdiff._normalize_literal_pools(entries)
        self.assertNotIn('.word #x', insns)

    def test_pop_clobbers_address_register(self):
        insns, _ = asmdiff._normalize_literal_pools(self.entries(('pop', '{r0}')))
        self.assertNotIn('.word #x', insns)

    def test_pc_address_offset_is_location(self):
        self.assertEqual(asmdiff.normalize('addw', 'r0, pc, #16'),
                         asmdiff.normalize('addw', 'r0, pc, #24'))
        self.assertNotEqual(asmdiff.normalize('addw', 'r0, r1, #16'),
                            asmdiff.normalize('addw', 'r0, r1, #24'))

    def test_comdat_aliases_do_not_cross_sections(self):
        dump = '''00000000  w    F .text.first 00000002 ctor1
00000000  w    F .text.first 00000002 ctor2
00000000  w    F .text.second 00000002 dtor1
00000000  w    F .text.second 00000002 dtor2
Disassembly of section .text.first:
00000000 <ctor1>:
   0:\t4770      \tbx\tlr
Disassembly of section .text.second:
00000000 <dtor1>:
   0:\t2000      \tmovs\tr0, #0
'''
        with patch('asmdiff.subprocess.check_output', return_value=dump):
            functions = asmdiff.disassemble('test.o')
        self.assertIs(functions['ctor1'], functions['ctor2'])
        self.assertIs(functions['dtor1'], functions['dtor2'])
        self.assertIsNot(functions['ctor1'], functions['dtor1'])


if __name__ == '__main__':
    unittest.main()
