import tempfile
import unittest
import json
from pathlib import Path
from unittest import mock

import scope_filter


class ScopeFilterTests(unittest.TestCase):
    def test_global_allocation_operators_are_runtime_glue(self):
        for symbol in ("_Znwj", "_Znaj", "_ZdlPv", "_ZdaPv", "_ZdlPvj", "_ZdaPvj"):
            with self.subTest(symbol=symbol):
                self.assertEqual(scope_filter.classify(symbol, symbol), "glue")
        self.assertEqual(scope_filter.classify("_ZN5ArrayIbED2Ev", "Array<bool>::~Array()"),
                         "in_scope")

    def test_ctor_aliases_use_the_local_symbol_table(self):
        self.assertTrue(Path(scope_filter.SYMS_TSV).is_relative_to(Path(scope_filter.REPO)))
        with tempfile.TemporaryDirectory() as directory:
            symbols = Path(directory) / "symbols.tsv"
            symbols.write_text("00000000\t_ZN5ArrayIbED2Ev\n", encoding="ascii")
            with mock.patch.object(scope_filter, "SYMS_TSV", str(symbols)):
                names = scope_filter._binary_names()
            self.assertTrue(scope_filter.is_benign_alias("_ZN5ArrayIbED1Ev", names))
            self.assertFalse(scope_filter.is_benign_alias("_ZN5ArrayIbEC1Ev", names))

    def test_scoped_report_cannot_claim_zero_missing_game_functions(self):
        with tempfile.TemporaryDirectory() as directory:
            (Path(directory) / "report.json").write_text(json.dumps({"count": 14}), encoding="ascii")
            with mock.patch.object(scope_filter, "VDIR", directory), \
                    mock.patch.object(scope_filter.sys, "argv", ["scope_filter.py", "--report",
                                                                 str(Path(directory) / "report.json")]):
                with self.assertRaisesRegex(SystemExit, "full verify report"):
                    scope_filter.main()

    def test_stale_gap_files_cannot_claim_completion(self):
        with tempfile.TemporaryDirectory() as directory:
            report = Path(directory) / "report.json"
            report.write_text(json.dumps({"original_functions": 10, "missing": 1,
                                          "extra_symbols": 0}), encoding="ascii")
            (Path(directory) / "missing.txt").write_text("", encoding="ascii")
            with mock.patch.object(scope_filter, "VDIR", directory), \
                    mock.patch.object(scope_filter.sys, "argv", ["scope_filter.py", "--report",
                                                                 str(report)]):
                with self.assertRaisesRegex(SystemExit, "does not match"):
                    scope_filter.main()


if __name__ == "__main__":
    unittest.main()
