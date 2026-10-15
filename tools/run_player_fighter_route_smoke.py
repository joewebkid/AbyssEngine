"""Compile the production PlayerFighter route setup against recording dependencies."""

from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    compiler = shutil.which("g++")
    if compiler is None:
        raise SystemExit("g++ is required")
    source = (ROOT / "src/game/ship/PlayerFighter.cpp").read_text()
    begin = source.index("AERandom *rng = static_cast<AERandom *>(Globals::rnd);")
    end = source.index("self->field_0x130 = -1;", begin)
    route_setup = source[begin:end]
    effects = "\n".join(line for line in source.splitlines() if
                        "self->workingPosition." in line or
                        "self->route->setLoop(" in line or
                        "stationRouteAliens->setLoop(" in line)
    fixture = (ROOT / "tools/player_fighter_route_smoke.cpp").read_text()
    fixture = fixture.replace("// PRODUCTION_ROUTE_SETUP", route_setup)
    fixture = fixture.replace("// PRODUCTION_ROUTE_EFFECTS", effects)
    with tempfile.TemporaryDirectory(prefix="fighter-route-", dir=ROOT / "_work") as temp:
        subject = Path(temp) / "subject.cpp"
        subject.write_text(fixture)
        exe = Path(temp) / "smoke.exe"
        subprocess.run([compiler, "-std=gnu++14", "-O0", str(subject), "-o", str(exe)],
                       cwd=ROOT, check=True)
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
