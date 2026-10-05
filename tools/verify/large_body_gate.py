#!/usr/bin/env python3
"""Print the six large gameplay bodies from an ARM verify report."""

import argparse
import json
from pathlib import Path


BODIES = (
    ("HangarWindow::render", "_ZN12HangarWindow6renderEv"),
    ("HangarWindow::OnTouchEnd", "_ZN12HangarWindow10OnTouchEndEii"),
    ("Hud::draw", "_ZN3Hud4drawExxP9PlayerEgobjj"),
    ("Radar::draw", "_ZN5Radar4drawEP6PlayerP3Hudi"),
    ("MGame::OnTouchEnd", "_ZN5MGame10OnTouchEndEiiPv"),
    ("MGame::OnUpdate", "_ZN5MGame8OnUpdateEv"),
)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("report", type=Path, help="Full tools/verify/verify.py report.json")
    args = parser.parse_args()
    report = json.loads(args.report.read_text(encoding="utf-8"))
    functions = {row["symbol"]: row for row in report["functions"]}

    print("| Body | Source-shape | Strict | Linked-exact | Byte-exact | ARM instructions (original/local) |")
    print("| --- | ---: | ---: | :---: | :---: | ---: |")
    missing = []
    for name, symbol in BODIES:
        row = functions.get(symbol)
        if row is None:
            missing.append(symbol)
            continue
        linked = "yes" if row["linked_equal"] else "no"
        byte = "yes" if row["bytes_equal"] else "no"
        print(f"| `{name}` | {row['source_match']:.1f}% | {row['match']:.1f}% "
              f"| {linked} | {byte} | {row['n_target']}/{row['n_base']} |")
    if missing:
        parser.error("report does not contain: " + ", ".join(missing))


if __name__ == "__main__":
    main()
