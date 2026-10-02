"""Compile actual focused Player bodies; external game/audio systems are test doubles.

Usage on Windows: python tools/test_player_recovery.py
Optional --so PATH also checks the two Android 2.0.16 sound tables byte for byte.
"""
import argparse
import os
from pathlib import Path
import re
import struct
import subprocess

root = Path(__file__).resolve().parents[1]
source = (root / 'src/game/ship/Player.cpp').read_text()
names = ['Player', '~Player', 'updateDamageRate', 'update', 'setHitVector', 'StopEngineSound',
         'reset', 'resetDamageDoneByPlayer', 'damageGamma', 'setEnemies', 'setEnemy',
         'addEnemy', 'gunAvailable', 'playShootSound']
parts = [source[:source.index('void Player::pitchAllPrimaryGuns')]]
for name in names:
    match = re.search(r'^.*Player::' + re.escape(name) + r'\([^\n]*\) \{', source, re.M)
    assert match, name
    depth = 1
    end = match.end()
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    parts.append(source[match.start():end])
parts.append('int Player::velocity = 100;')
work = root / '_work/player-smoke'
work.mkdir(parents=True, exist_ok=True)
subject = work / 'subject.cpp'
subject.write_text('\n\n'.join(parts))
compiler = Path('C:/msys64/ucrt64/bin/g++.exe')
env = dict(os.environ)
env['PATH'] = str(compiler.parent) + os.pathsep + env['PATH']
command = [str(compiler), '-std=gnu++14', '-O1', '-fpermissive',
           *['-I' + str(root / p) for p in ['src', 'third_party/fmod/inc', 'third_party/gl',
                                          'third_party/jni', 'third_party/libzip', 'third_party/crypto']],
           str(root / 'tools/player_update_smoke.cpp'), str(subject),
           str(root / 'src/engine/math/AEMath.cpp'), '-o', str(work / 'smoke.exe')]
subprocess.run(command, check=True, env=env, cwd=root)
subprocess.run([str(work / 'smoke.exe')], check=True, env=env)
parser = argparse.ArgumentParser()
parser.add_argument('--so', type=Path)
args = parser.parse_args()
if args.so:
    data = args.so.read_bytes()
    for name, address, count in [('kItemSoundIds', 0x1fbb50, 233), ('kGroupSoundIds', 0x1fbf00, 11)]:
        body = re.search(name + r'\[\] = \{(.*?)\};', source, re.S).group(1)
        values = tuple(map(int, re.findall(r'-?\d+', body)))
        assert values == struct.unpack_from('<' + str(count) + 'i', data, address), name
    print('Player Android sound-table check: OK')
