"""Guard the ARM-backed ship-material run in BuildResourceList."""

import re
import unittest
from pathlib import Path


SOURCE = (Path(__file__).resolve().parents[2] / "src/engine/render/BuildResourceList.cpp").read_text()


class ResourceMaterialTableTest(unittest.TestCase):
    def test_table_slots(self):
        slots = [int(slot) for slot in re.findall(r"ADD_RES\(resources\[(\d+)\]", SOURCE)]
        self.assertEqual(len(slots), len(set(slots)))
        self.assertEqual(set(slots), set(range(2488)))
        self.assertIn("Resource *resources[2488];", SOURCE)

    def test_final_arm_records(self):
        self.assertRegex(SOURCE, r"ADD_RES\(resources\[310\],\s*33010, 2,\s*"
                                 r"new AbyssEngine::ResourceTexture\(\s*\"[^\"]*stations_terran_diffuse\.aei\", 0\.0f\)\)")
        self.assertRegex(SOURCE, r"ADD_RES\(resources\[977\], 34817, 6, "
                                 r"new AbyssEngine::ResourceMaterial\(34816, BlendMode_2\)\)")
        for slot, resource_id, diffuse, normal in (
            (997, 34810, 24204, 24205),
            (998, 34811, 24206, 24207),
        ):
            with self.subTest(slot=slot):
                self.assertRegex(SOURCE, rf"ADD_RES\(resources\[{slot}\], {resource_id}, 6, "
                                         rf"new AbyssEngine::ResourceMaterial\({diffuse}, {normal}, "
                                         r"static_cast<BlendMode>\(28\)\)\)")
        self.assertRegex(SOURCE, r"ADD_RES\(resources\[1700\],\s*6801, 4,\s*"
                                 r"new AbyssEngine::ResourceMesh\(\s*\"[^\"]*projectile_025_anim_add\.aem\", "
                                 r"24201, false\)\)")

    def test_visitor_glow_resources(self):
        for slot, resource_id, name in (
            (457, 33400, "terran"), (458, 33401, "vossk"),
            (459, 33402, "nivelian"), (460, 33403, "midorian"),
        ):
            with self.subTest(slot=slot):
                self.assertRegex(SOURCE, rf"ADD_RES\(resources\[{slot}\],\s*{resource_id}, 2,\s*"
                                         rf"new AbyssEngine::ResourceTexture\(\s*\"[^\"]*bar_visitor_glow_{name}\.aei\"")
        self.assertRegex(SOURCE, r"ADD_RES\(resources\[461\],\s*33404, 2,\s*"
                                 r"new AbyssEngine::ResourceTexture\(\"[^\"]*bar_visitor_shadow\.aei\"")

        for slot, texture_id, blend in (
            (896, 33400, "BlendMode_2"), (897, 33401, "BlendMode_2"),
            (898, 33402, "BlendMode_2"), (899, 33403, "BlendMode_2"),
            (900, 33404, "BlendMode_1"),
        ):
            with self.subTest(slot=slot):
                self.assertRegex(SOURCE, rf"ADD_RES\(resources\[{slot}\],\s*{34800 + slot - 896}, 6,\s*"
                                         rf"new AbyssEngine::ResourceMaterial\({texture_id}, {blend}\)\)")

    def test_relocated_map_meshes_and_plane_ids(self):
        for slot, resource_id, filename, material_id in (
            (1869, 18081, "plane.aem", 27281),
            (1871, 18083, "plane.aem", 27284),
            (1874, 18086, "plane.aem", 27287),
            (1876, 18088, "plane.aem", 27286),
            (1877, 18180, "map_planet_000.aem", 20027),
            (1883, 18186, "map_planet_006.aem", 20027),
        ):
            with self.subTest(slot=slot):
                self.assertRegex(SOURCE, rf"ADD_RES\(resources\[{slot}\],\s*{resource_id}, 4,\s*"
                                         rf"new AbyssEngine::ResourceMesh\(\s*\"[^\"]*{re.escape(filename)}\", "
                                         rf"{material_id}, false\)\)")

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
