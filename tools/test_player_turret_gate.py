"""Fixed Android PlayerTurret body/alias and exact-preservation regression gate.

Run with BASELINE FINAL [--full-baseline JSON --full-final JSON]. No verifier
normalization or compiler settings are changed by this standalone gate.
"""
import argparse
import copy
import json
import sys
from pathlib import Path
from unittest import mock

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "docs/validation/player_turret_manifest_2026-10-02.json"


def check_comdat_aliases():
    sys.path.insert(0, str(ROOT / "tools/verify"))
    import asmdiff
    output = """SYMBOL TABLE:
00000000  w    F .text._ZN1AC2Ev 00000002 _ZN1AC1Ev
00000000  w    F .text._ZN1AC2Ev 00000002 _ZN1AC2Ev
00000000  w    F .text._ZN1BD2Ev 00000002 _ZN1BD1Ev
00000000  w    F .text._ZN1BD2Ev 00000002 _ZN1BD2Ev
Disassembly of section .text._ZN1AC2Ev:
00000000 <_ZN1AC1Ev>:
   0:\t70 47\t\tbx\tlr
Disassembly of section .text._ZN1BD2Ev:
00000000 <_ZN1BD1Ev>:
   0:\t00 bf\t\tnop
"""
    with mock.patch.object(asmdiff.subprocess, "check_output", return_value=output):
        functions = asmdiff.disassemble("unused.o")
    assert functions["_ZN1AC1Ev"] is functions["_ZN1AC2Ev"]
    assert functions["_ZN1BD1Ev"] is functions["_ZN1BD2Ev"]
    assert functions["_ZN1AC2Ev"] is not functions["_ZN1BD2Ev"]


def read(path):
    return json.loads(Path(path).read_text())


def exact_pairs(report, field):
    return {(r["unit"], r["symbol"]) for r in report["functions"] if r[field]}


def metrics(report, manifest):
    rows = {r["symbol"]: r for r in report["functions"]}
    expected = {s for body in manifest for s in body["symbols"]}
    assert len(rows) == len(report["functions"]) == 21
    assert set(rows) == expected, "Fixed bodies/aliases omitted or added"
    assert len(manifest) == len({m["android_address"] for m in manifest}) == 19
    assert report["skipped"] == 0
    representatives = []
    for body in manifest:
        representative = rows[body["symbols"][0]]
        assert representative["n_target"] == body["n_target"]
        for alias in body["symbols"]:
            for key in ("n_target", "n_base", "source_match", "match", "linked_equal", "bytes_equal"):
                assert rows[alias][key] == representative[key], (alias, key)
        representatives.append(representative)
    result = {}
    for field in ("source_match", "match"):
        result[field] = sum(r[field] for r in representatives) / len(representatives)
        for weight in ("n_target", "target_instruction_count"):
            result[field + "_" + weight] = sum(
                r[field] * body[weight] for r, body in zip(representatives, manifest)
            ) / sum(body[weight] for body in manifest)
    return result


def gate(baseline, final, manifest):
    before, after = metrics(baseline, manifest), metrics(final, manifest)
    for field in ("source_match", "source_match_n_target", "source_match_target_instruction_count"):
        assert after[field] >= 99.0, (field, after[field])
    for field in ("linked_equal", "bytes_equal"):
        assert exact_pairs(baseline, field) <= exact_pairs(final, field), field
    return before, after


def self_check(baseline, final, manifest):
    # A high alias-count mean must not hide omitted bodies or a low weighted mean.
    omitted = copy.deepcopy(final)
    omitted["functions"] = [r for r in omitted["functions"] if "6updateEi" not in r["symbol"]]
    weighted = copy.deepcopy(final)
    for row in weighted["functions"]:
        if "6updateEi" in row["symbol"]:
            row["source_match"] = 97.0
    for bad in (omitted, weighted):
        try:
            gate(baseline, bad, manifest)
        except AssertionError:
            continue
        raise AssertionError("Gate accepted an invalid family report")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("baseline")
    parser.add_argument("final")
    parser.add_argument("--full-baseline")
    parser.add_argument("--full-final")
    args = parser.parse_args()
    baseline, final, manifest = read(args.baseline), read(args.final), read(MANIFEST)
    check_comdat_aliases()
    self_check(baseline, final, manifest)
    before, after = gate(baseline, final, manifest)
    if args.full_baseline or args.full_final:
        assert args.full_baseline and args.full_final
        full_before, full_after = read(args.full_baseline), read(args.full_final)
        for report in (full_before, full_after):
            assert not report.get("scoped_unit")
            assert report["skipped"] == report["missing_wrong_type"] == 0
        assert full_before["count"] == full_after["count"]
        for field in ("linked_equal", "bytes_equal"):
            assert exact_pairs(full_before, field) <= exact_pairs(full_after, field), field
    print(json.dumps({"baseline": before, "final": after}, indent=2))
    print("PlayerTurret fixed 19-body / weighted / exact-preservation gate: OK")
