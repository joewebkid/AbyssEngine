"""Run production TouchButton/Layout UI bodies with fixture services."""

import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    source = (ROOT / "src/game/ui/TouchButton.cpp").read_text()
    extracted = '''#include "game/ui/TouchButton.h"
#include "game/ui/Layout.h"
#include "game/core/Globals.h"
#include "engine/audio/FModSound.h"
#include "game/mission/Status.h"
#include "game/ship/Ship.h"
'''
    extracted += function(source, "void TouchButton::setVisible(")
    extracted += function(source, "int TouchButton::getWidth(")
    for signature in ("bool TouchButton::touchedInside(",
                      "bool TouchButton::OnTouchBegin(",
                      "unsigned int TouchButton::OnTouchMove(",
                      "unsigned int TouchButton::OnTouchEnd("):
        extracted += function(source, signature)
    mask_start = source.index("// Android 2.0.16 TouchButton::draw literal")
    draw_start = source.index("void TouchButton::draw(", mask_start)
    extracted += source[mask_start:draw_start]
    extracted += function(source, "void TouchButton::draw(")
    layout = (ROOT / "src/game/ui/Layout.cpp").read_text()
    extracted += function(layout, "void Layout::setDrawColor(")
    extracted += function(layout, "void Layout::drawBGPattern(")
    extracted += function(layout, "void Layout::drawEmptyFooter(")
    extracted += function(layout, "uint8_t Layout::drawFade(")
    for signature in ("int Layout::getFooterTransitionWidth(",
                      "int Layout::getHelpButtonOffset(",
                      "void Layout::drawBG()",
                      "void Layout::drawMask()",
                      "void Layout::drawMask(int"):
        extracted += function(layout, signature)
    for signature in ("void Layout::drawFooterNoBackButton(",
                      "void Layout::drawFooter()",
                      "void Layout::drawFooterStation(",
                      "void Layout::drawFooter(bool"):
        extracted += function(layout, signature)
    extracted += function(layout, "void Layout::drawBox(int style, int x, int y, int w, int h, String text, unsigned char flags)")
    extracted += function(layout, "void Layout::drawBox(int style, int x, int y, int w, int h, String text)")
    extracted += function(layout, "void Layout::drawBGBorder(unsigned corner, unsigned edge, int x, int y, int w, int h, int inset, int pad)")

    with tempfile.TemporaryDirectory(prefix="touchbutton-smoke-", dir=ROOT / "_work") as temp:
        generated = Path(temp) / "production_touch.cpp"
        generated.write_text(extracted)
        exe = Path(temp) / "touchbutton_touch_smoke.exe"
        command = [args.cxx, "-std=gnu++14", "-O0", "-fpermissive",
                   "-Wno-int-to-pointer-cast", "-Wno-int-to-void-pointer-cast",
                   "-Isrc", "-Ithird_party/fmod/inc",
                   "-Ithird_party/gl", "-Ithird_party/jni",
                   "tools/touchbutton_touch_smoke.cpp", str(generated), "-o", str(exe)]
        build = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
        if build.returncode:
            print(build.stdout[-4000:])
            print(build.stderr[-4000:])
            build.check_returncode()
        subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
