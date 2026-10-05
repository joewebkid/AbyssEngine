"""Gate the unchanged 2026-09-24 StarMap body corpus and exact pairs.

Pass focused baseline/final JSON followed by full baseline/final JSON.
Default exit status requires both means >=99%; --allow-partial only relaxes
that threshold, never the corpus or exact-pair checks.
"""
import argparse
import json
from pathlib import Path


BODIES = {
    "_ZN7StarMap16drawOnScreenInfoEib": 1207,
    "_ZN7StarMap10OnTouchEndEii": 1037,
    "_ZN7StarMap6updateEi": 971,
    "_ZN7StarMap4drawEv": 916,
    "_ZN7StarMap4initEbP7Missionbi": 750,
    "_ZN7StarMap14initStarSystemEv": 710,
    "_ZN7StarMapC1EbP7Missionbi": 333,
    "_ZN7StarMap12OnTouchBeginEii": 275,
    "_ZN7StarMap11OnTouchMoveEii": 265,
    "_ZN7StarMap7drawKeyEv": 246,
    "_ZN7StarMap6departEb": 205,
    "_ZN7StarMapD1Ev": 69,
    "_ZN7StarMap6renderEv": 47,
    "_ZN7StarMap10initLightsEv": 21,
    "_ZN7StarMap24askForJumpIntoAlienWorldEv": 21,
    "_ZN7StarMap8setStartEii": 17,
    "_ZN7StarMap14isInPlanetModeEv": 5,
    "_ZN7StarMap14setJumpMapModeEbb": 3,
    "_ZN7StarMap14missionChangedEv": 2,
    "_ZN7StarMap8renderBGEv": 1,
}
ALIASES = {
    "_ZN7StarMapC2EbP7Missionbi": "_ZN7StarMapC1EbP7Missionbi",
    "_ZN7StarMapD2Ev": "_ZN7StarMapD1Ev",
}


def focused(report):
    assert report["skipped"] == 0
    rows = {r["symbol"]: r for r in report["functions"]}
    assert len(report["functions"]) == len(rows) == 22
    assert set(rows) == set(BODIES) | set(ALIASES)
    for symbol, count in BODIES.items():
        assert rows[symbol]["n_target"] == count, (symbol, "changed original weight")
    for alias, body in ALIASES.items():
        for key in ("n_target", "n_base", "source_match", "match", "linked_equal", "bytes_equal"):
            assert rows[alias][key] == rows[body][key], (alias, key)
    weight = sum(BODIES.values())
    assert weight == 7101
    summary = {}
    for key in ("source_match", "match"):
        summary[key] = {
            "unweighted": sum(rows[s][key] for s in BODIES) / len(BODIES),
            "weighted": sum(rows[s][key] * BODIES[s] for s in BODIES) / weight,
        }
    summary["linked_exact"] = sum(rows[s]["linked_equal"] for s in BODIES)
    summary["byte_exact"] = sum(rows[s]["bytes_equal"] for s in BODIES)
    return summary


def exact_pairs(report, key):
    assert report["skipped"] == 0
    return {(r["unit"].replace("\\", "/"), r["symbol"])
            for r in report["functions"] if r[key]}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("baseline_focused", type=Path)
    parser.add_argument("final_focused", type=Path)
    parser.add_argument("baseline_corpus", type=Path)
    parser.add_argument("final_corpus", type=Path)
    parser.add_argument("--allow-partial", action="store_true")
    args = parser.parse_args()
    reports = [json.loads(p.read_text(encoding="utf-8")) for p in
               (args.baseline_focused, args.final_focused, args.baseline_corpus, args.final_corpus)]
    baseline, final = map(focused, reports[:2])
    differences = {}
    for key in ("linked_equal", "bytes_equal"):
        before, after = (exact_pairs(r, key) for r in reports[2:])
        differences[key] = {"baseline": len(before), "final": len(after),
                            "lost": sorted(before - after), "gained": sorted(after - before)}
    target_met = all(value >= 99.0 for value in final["source_match"].values())
    result = {"body_count": 20, "original_instructions": 7101, "aliases": ALIASES,
              "baseline": baseline, "final": final, "exact_pairs": differences,
              "target_met": target_met}
    print(json.dumps(result, indent=2))
    assert not any(d["lost"] for d in differences.values()), "Exact pair regression"
    if not args.allow_partial:
        assert target_met, "StarMap >=99.0% target NOT reached"


if __name__ == "__main__":
    main()
