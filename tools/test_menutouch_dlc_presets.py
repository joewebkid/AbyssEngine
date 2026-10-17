"""Guard the Android 2.0.16 DLC preset indices and call order.

These assertions cover the recovered source, not a playable runtime. The
independent ARM source-shape verifier remains the code-generation gate.
"""
from pathlib import Path
import re
import unittest


SOURCE = (Path(__file__).resolve().parents[1] / "src/game/ui/MenuTouchWindow.cpp").read_text()


def body(name):
    start = SOURCE.index(f"void MenuTouchWindow::{name}() {{")
    opening = SOURCE.index("{", start)
    depth = 1
    for pos in range(opening + 1, len(SOURCE)):
        depth += (SOURCE[pos] == "{") - (SOURCE[pos] == "}")
        if depth == 0:
            return SOURCE[opening + 1:pos]
    raise AssertionError(f"unterminated {name}")


class DlcPresetTest(unittest.TestCase):
    def check_preset(self, name, ship, race, station, missions, equipment, cargo=()):
        source = body(name)
        self.assertIn(f"i < {missions}", source)
        self.assertIn("nextCampaignMission(false)", source)
        self.assertIn(f"Globals::ships)->data()[{ship}]->makeShip(-1)", source)
        self.assertIn(f"getShip()->setRace({race})", source)
        self.assertIn(f"getStation({station})", source)
        self.assertIn("SetCurrentApplicationModule(5)", source)
        self.assertNotIn("ShipDefTable", source)
        self.assertNotIn("_mtw_", source)
        found = re.findall(
            r"Globals::items\)->data\(\)\[(\d+)\]->makeItem\((\d*)\), (\d+)\)",
            source,
        )
        self.assertEqual([(int(i), int(q) if q else 1, int(s)) for i, q, s in found], equipment)
        found_cargo = re.findall(
            r"addCargo\(\(\(Array<Item \*> \*\) Globals::items\)->data\(\)\[(\d+)\]->makeItem\((\d+)\)\)",
            source,
        )
        self.assertEqual([(int(i), int(q)) for i, q in found_cargo], list(cargo))

    def test_valkyrie(self):
        self.check_preset("startValkyrie", 5, 0, 91, 45, [
            (2, 1, 0), (5, 1, 1), (36, 10, 0), (81, 1, 0),
            (51, 1, 1), (86, 10, 2), (85, 1, 3),
        ])

    def test_supernova(self):
        self.check_preset("startSupernova", 30, 3, 70, 84, [
            (176, 1, 0), (20, 1, 1), (36, 20, 0), (44, 20, 1),
            (81, 1, 0), (51, 1, 1), (68, 1, 2), (86, 1, 3),
            (85, 1, 4), (56, 1, 5),
        ], [(122, 8)])


if __name__ == "__main__":
    unittest.main()
