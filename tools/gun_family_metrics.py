"""Gun-only metrics against the immutable original-body manifest.

Usage: python tools/gun_family_metrics.py FAMILY.json [BASE_CORPUS.json FINAL_CORPUS.json]
Does not build, normalize instructions or modify the common verifier.
"""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "docs/validation/gun_fixed_bodies_2026-10-02.json"


def metrics(rows):
    weight = sum(row["n_target"] for row in rows)
    return {
        "bodies": len(rows), "original_instructions": weight,
        "source_unweighted": sum(row["source_match"] for row in rows) / len(rows),
        "source_weighted": sum(row["source_match"] * row["n_target"] for row in rows) / weight,
        "strict_unweighted": sum(row["match"] for row in rows) / len(rows),
        "strict_weighted": sum(row["match"] * row["n_target"] for row in rows) / weight,
        "linked_exact": sum(row["linked_equal"] for row in rows),
        "byte_exact": sum(row["bytes_equal"] for row in rows),
    }


def exact_losses(before, after):
    def pairs(report, field):
        return {(row["unit"].replace("\\", "/"), row["symbol"])
                for row in report["functions"] if row[field]}
    return {field: sorted(pairs(before, field) - pairs(after, field))
            for field in ("linked_equal", "bytes_equal")}


def main():
    manifest = json.loads(MANIFEST.read_text())
    family = json.loads(Path(sys.argv[1]).read_text())
    current = {row["symbol"]: row for row in family["functions"]}
    expected = {row["symbol"]: row for row in manifest["bodies"]}
    assert len(expected) == 26
    assert set(current) == set(expected) | set(manifest["aliases"]), "Gun family scope changed"
    selected = [current[symbol] for symbol in expected]
    for row in selected:
        assert row["n_target"] == expected[row["symbol"]]["n_target"], "Original body weight changed"
    result = {
        "base": manifest["base"], "weight": manifest["weight"],
        "baseline": metrics(list(expected.values())), "current": metrics(selected),
        "symbol_count": len(current), "aliases": manifest["aliases"],
        "family_exact_losses": exact_losses({"functions": manifest["bodies"]}, family),
    }
    result["target_met"] = (result["current"]["source_unweighted"] >= 99.0 and
                            result["current"]["source_weighted"] >= 99.0)
    if len(sys.argv) == 4:
        before = json.loads(Path(sys.argv[2]).read_text())
        after = json.loads(Path(sys.argv[3]).read_text())
        assert "scoped_unit" not in before and "scoped_unit" not in after
        result["corpus_exact_losses"] = exact_losses(before, after)
    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
