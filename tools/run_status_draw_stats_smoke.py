"""Exercise the production StatusWindow standing labels and statistics table."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/game/ui/StatusWindow.cpp").read_text()
    header_start = source.index("        String standingHeader = ")
    header_end = source.index("        int standingRowY = ", header_start)
    rows_start = source.index("        int standingLabelY = ", header_end)
    rows_end = source.index("        drewStats = land;", rows_start)
    fixture = (ROOT / "tools/status_draw_stats_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_STATS_HEADER", source[header_start:header_end])
    fixture = fixture.replace("// PRODUCTION_STATS_ROWS", source[rows_start:rows_end])

    with tempfile.TemporaryDirectory(prefix="status-draw-stats-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "status_draw_stats_smoke.cpp"
        generated.write_text(fixture)
        exe = Path(temp) / "status_draw_stats_smoke.exe"
        subprocess.run([args.cxx, "-std=gnu++14", "-O0", str(generated), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
