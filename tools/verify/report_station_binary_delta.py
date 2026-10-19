"""Compare full reports by normalized (unit, symbol), not aggregate exact counts."""
import argparse
import hashlib
import json
from pathlib import Path


def pairs(report):
    return {(r["unit"].replace("\\", "/"), r["symbol"]): r for r in report["functions"]}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("before", type=Path)
    parser.add_argument("after", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    reports = [json.loads(p.read_text()) for p in (args.before, args.after)]
    indexed = list(map(pairs, reports))
    for report, data in zip(reports, indexed):
        assert len(data) == len(report["functions"]), "duplicate unit/symbol pair"
    result = {"reports": [{"path": str(p), "sha256": hashlib.sha256(p.read_bytes()).hexdigest()}
                          for p in (args.before, args.after)]}
    for name in ("bytes_equal", "linked_equal"):
        old, new = [{key for key, row in data.items() if row[name]} for data in indexed]
        result[name] = {"before": len(old), "after": len(new), "lost": sorted(old - new), "gained": sorted(new - old)}
    result["pairs_lost"] = sorted(indexed[0].keys() - indexed[1].keys())
    result["pairs_gained"] = sorted(indexed[1].keys() - indexed[0].keys())
    result["extra_symbols_lost"] = sorted(set(reports[0]["extra"]) - set(reports[1]["extra"]))
    result["extra_symbols_gained"] = sorted(set(reports[1]["extra"]) - set(reports[0]["extra"]))
    family = json.loads(Path(__file__).with_name("station_binary_family.json").read_text())
    result["family"] = family
    result["family_rows"] = []
    for pair in family["canonical_pairs"] + family["alias_pairs"]:
        key = tuple(pair)
        result["family_rows"].append({"pair": pair, "before": indexed[0][key], "after": indexed[1][key]})
    for side, data in zip(("before", "after"), indexed):
        canonical = [data[tuple(pair)] for pair in family["canonical_pairs"]]
        result["family_" + side] = {"source_shape": round(sum(r["source_match"] for r in canonical) / len(canonical), 4),
                                  "strict_fuzzy": round(sum(r["match"] for r in canonical) / len(canonical), 4)}
    result["family_target_met"] = result["family_after"]["source_shape"] >= family["target_source_shape"]
    args.output.write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps({k: v for k, v in result.items() if k not in ("family_rows", "family")}, indent=2))
    assert not result["pairs_lost"]
    assert not result["bytes_equal"]["lost"] and not result["linked_equal"]["lost"]
    assert reports[1]["missing_wrong_type"] == 0 and reports[1]["skipped"] == 0


if __name__ == "__main__":
    main()
