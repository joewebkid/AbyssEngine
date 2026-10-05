#!/usr/bin/env python3
import contextlib
import io
import tempfile
import unittest
from pathlib import Path
from unittest import mock

import verify


class VerifyScopeTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        for unit in ("game/weapons/Radar", "game/menu/MGame"):
            path = self.root / "base" / (unit + ".o")
            path.parent.mkdir(parents=True, exist_ok=True)
            path.touch()

    def test_scoped_discovery_does_not_walk_corpus(self):
        with mock.patch.object(verify.os, "walk", side_effect=AssertionError("corpus walk")):
            objects = verify.find_base_objects(str(self.root), unit="game/weapons/Radar")
        self.assertEqual([unit for unit, _ in objects], ["game/weapons/Radar"])
        self.assertEqual(Path(objects[0][1]), self.root / "base/game/weapons/Radar.o")

    def test_missing_unit_does_not_fall_back_to_full_scan(self):
        self.assertEqual(verify.find_base_objects(str(self.root), unit="missing"), [])

    def test_full_discovery_is_unchanged(self):
        self.assertEqual([unit for unit, _ in verify.find_base_objects(str(self.root))],
                         [verify.os.path.join("game", "menu", "MGame"),
                          verify.os.path.join("game", "weapons", "Radar")])

    def test_scoped_collect_compares_only_selected_object(self):
        with mock.patch.object(verify, "_diff_unit", return_value=([], None, {"_ZN5Radar10hasScannerEv"})) as diff:
            rows, skips, symbols = verify.collect(str(self.root), only="^_ZN5Radar", unit="game/weapons/Radar")
        self.assertEqual((rows, skips), ([], []))
        self.assertEqual(symbols, {"_ZN5Radar10hasScannerEv"})
        self.assertEqual(diff.call_count, 1)
        self.assertEqual(diff.call_args.args[0], "game/weapons/Radar")

    def test_partial_report_cannot_run_full_signature_gate(self):
        argv = ["verify.py", "--no-build", "--unit", "game/weapons/Radar", "--fail-on-wrong-type"]
        with mock.patch.object(verify.sys, "argv", argv), contextlib.redirect_stderr(io.StringIO()):
            with self.assertRaises(SystemExit) as error:
                verify.main()
        self.assertEqual(error.exception.code, 2)

    def test_scoped_report_without_regex_suppresses_global_coverage(self):
        argv = ["verify.py", "--no-build", "--build-dir", str(self.root), "--unit", "game/weapons/Radar"]
        with mock.patch.object(verify.sys, "argv", argv), mock.patch.object(verify, "collect", return_value=([], [], set())) as collect:
            with contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(verify.main(), 1)
        collect.assert_called_once_with(str(self.root), only=".*", unit="game/weapons/Radar")


if __name__ == "__main__":
    unittest.main()
