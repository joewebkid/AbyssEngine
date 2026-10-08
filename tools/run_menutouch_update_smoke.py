"""Exercise the unchanged production update body with host boundary doubles.

As with run_particle_control_smoke.py, extract the body to avoid unrelated
renderer/shim dependencies in the host PE linker. Constructors are fixture
boundaries: this does not exercise menu resource ownership or ARM32 runtime.
"""
import argparse
from pathlib import Path
import subprocess

from run_particle_control_smoke import function

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--cxx', required=True)
    parser.add_argument('--out', required=True)
    args = parser.parse_args()
    out = Path(args.out).resolve()
    out.mkdir(parents=True, exist_ok=True)
    text = (ROOT / 'src/game/ui/MenuTouchWindow.cpp').read_text()
    source = '#include "game/ui/MenuTouchWindow.h"\n#include "game/core/Globals.h"\n'
    source += '#include "game/ui/MissionsWindow.h"\n#include "game/ui/ChoiceWindow.h"\n'
    source += '#include "game/mission/Status.h"\n#include "game/mission/RecordHandler.h"\n'
    source += '#include "engine/core/GameText.h"\n#include "engine/render/Engine.h"\n'
    start = text.index('struct MtwAppData {')
    end = text.index('\n};', start) + 3
    source += text[start:end] + '\n'
    math = (ROOT / 'src/engine/math/AEMath.cpp').read_text()
    source += 'namespace AbyssEngine { namespace AEMath {\n'
    source += function(math, 'Vector &Vector::operator=(')
    source += '}}\n'
    for signature in ('static inline uint8_t &_mtw_option_byte(',
                      'static inline void _mtw_TouchButton_getPosition(',
                      'static inline void _mtw_TouchButton_setYPosition(',
                      'static inline int _mtw_TouchButton_isVisible(',
                      'void MenuTouchWindow::update('):
        source += function(text, signature)
    generated = out / 'production_update.cpp'
    generated.write_text(source)
    exe = out / 'menutouch_update_smoke.exe'
    subprocess.run([args.cxx, '-std=c++14', '-O0', '-Isrc',
                    '-Ithird_party/fmod/inc', '-Ithird_party/gl', '-Ithird_party/jni',
                    'tools/menutouch_update_smoke.cpp', str(generated), '-o', str(exe)],
                   cwd=ROOT, check=True)
    subprocess.run([str(exe)], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
