"""Guard the ARM-backed ship-material run in BuildResourceList."""

import re
import unittest
from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "src/engine/render/BuildResourceList.cpp").read_text()


class ResourceMaterialTableTest(unittest.TestCase):
    def test_table_slots(self):
        slots = [int(slot) for slot in re.findall(r"ADD_RES\(resources\[(\d+)\]", SOURCE)]
        self.assertEqual(len(slots), len(set(slots)))
        self.assertEqual(set(range(2488)) - set(slots),
                         {896, 897, 899, 900, 977, 997, 998, 1700, 1871, 1874, 1877, 1883})

    def test_ship_material_records(self):
        records = {
            int(slot): (int(resource_id), int(diffuse), int(normal))
            for slot, resource_id, diffuse, normal in re.findall(
                r"ADD_RES\(resources\[(\d+)\],\s*(\d+),\s*6,\s*"
                r"new AbyssEngine::ResourceMaterial\((\d+),\s*(\d+),\s*"
                r"static_cast<BlendMode>\(28\)\)\)", SOURCE
            )
        }
        for slot, suffix in zip(range(634, 675), [*range(13), *range(16, 44)]):
            with self.subTest(slot=slot):
                self.assertEqual(records[slot], (34200 + suffix, 34000 + suffix, 34100 + suffix))
        self.assertEqual(records[681], (34250, 34050, 34150))
        self.assertRegex(SOURCE, r"ADD_RES\(resources\[632\], 34604, 6, "
                                 r"new AbyssEngine::ResourceMaterial\(34504, BlendMode_2\)\)")


if __name__ == "__main__":
    unittest.main()
