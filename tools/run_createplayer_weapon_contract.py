"""Execute full production createPlayer with original weapon data and CPU services."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import struct
import subprocess
import tempfile

from run_particle_control_smoke import function

ROOT = Path(__file__).resolve().parents[1]
BASE = '95a89df553481a689bcfa27c3b064d74f6e1181b'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--asset', type=Path, required=True)
    parser.add_argument('--out-dir', type=Path, required=True)
    parser.add_argument('--cxx', default=shutil.which('g++'))
    parser.add_argument('--git', default=shutil.which('git'))
    args = parser.parse_args()
    if not args.cxx or not args.git:
        parser.error('specify --cxx and --git')
    args.out_dir.mkdir(parents=True, exist_ok=True)
    if shutil.disk_usage(args.out_dir).free < 64 * 1024 * 1024:
        parser.error('host gate requires 64 MiB free; no cleanup attempted')
    data = args.asset.read_bytes()
    assert len(data) == 7484 and hashlib.sha256(data).hexdigest() == (
        'eebc7b6361bfdb19248f3f40071f915dad0cca919b292ad015d8d890f5570f82')
    cursor, entries, records = 0, [], []
    while cursor < len(data):
        ship, count = struct.unpack_from('<hh', data, cursor)
        cursor += 4
        begin = len(entries)
        assert count >= 0
        for _ in range(count):
            kind, x, y, z = struct.unpack_from('<hhhh', data, cursor)
            cursor += 8
            extra = (0, 0, 0)
            assert 0 <= kind < 4
            if kind == 3:
                extra = struct.unpack_from('<III', data, cursor)
                cursor += 12
            entries.append((kind, x, y, z, *extra))
        records.append((ship, begin, count, cursor))
    assert cursor == len(data) and len(records) == 59 and len(entries) == 552
    assert len({r[0] for r in records}) == 59
    assert sum(e[0] == 3 for e in entries) == 236
    oracle = 'static const Entry expectedEntries[] = {\n' + ''.join(
        '{' + ','.join(str(v) if i < 4 else f'0x{v:08x}u' for i, v in enumerate(e)) + '},\n'
        for e in entries) + '};\n'
    oracle += 'static const Record expectedRecords[] = {\n' + ''.join(
        '{' + ','.join(map(str, r)) + '},\n' for r in records) + '};\n'
    reader = (ROOT / 'src/engine/file/FileRead.cpp').read_text()
    ae = (ROOT / 'src/engine/file/AEFile.cpp').read_text()
    loader = '\n'.join(function(reader, sig) for sig in (
        'FileRead::FileRead()', 'FileRead::~FileRead()',
        'Array<Array<Vector *> *> *FileRead::loadWeaponPositions('))
    loader += function(ae, 'uint32_t AEFile::Read(int16_t &value, uint32_t handle)')
    loader += function(ae, 'uint32_t AEFile::Read(float &value, uint32_t handle)')
    vector = 'namespace AbyssEngine { namespace AEMath {\n' + function(
        (ROOT / 'src/engine/math/Vector.cpp').read_text(),
        'Vector &Vector::operator=(const Vector &rhs)') + '\n}}\n'
    gun = (ROOT / 'src/game/weapons/Gun.cpp').read_text()
    assert 'static const float kZOffset = 100.0f;' in gun
    consumers = 'static const float kZOffset = 100.0f;\n' + function(gun, 'void Gun::setOffset(Vector *v)')
    consumers += function((ROOT / 'src/game/ship/PlayerEgo.cpp').read_text(),
                          'void PlayerEgo::setTurretPosition(Vector v)')
    consumers += function((ROOT / 'src/game/ship/PlayerEgo.cpp').read_text(),
                          'void PlayerEgo::addGun(Array<Gun *> *arr, int x)')
    consumers += function((ROOT / 'src/game/ship/Player.cpp').read_text(),
                          'void Player::addGun(Array<Gun *> *gunsIn, int slot)')
    level = (ROOT / 'src/game/world/Level.cpp').read_text()
    current = function(level, 'void Level::createPlayer()')
    factor_bits = function(level, 'static inline float bitsToFloat(')
    old = subprocess.check_output([args.git, 'show', BASE + ':src/game/world/Level.cpp'],
                                  cwd=ROOT, text=True)
    baseline = function(old, 'void Level::createPlayer()')
    results = []
    with tempfile.TemporaryDirectory(prefix='createplayer-', dir=args.out_dir) as temp:
        out = Path(temp)
        (out / 'createplayer_oracle.inc').write_text(oracle)
        (out / 'createplayer_loader.inc').write_text(loader + vector)
        for label, body in [('current', current), ('negative-baseline', baseline)]:
            (out / 'createplayer_body.inc').write_text(consumers + factor_bits + body)
            for opt in ('-O0', '-O2'):
                exe = out / f'{label}-{opt[1:]}.exe'
                defines = ['-DCONTRACT_LEGACY_FLOAT_GETTERS'] if label == 'negative-baseline' else []
                subprocess.run([args.cxx, '-std=gnu++17', opt, '-g0', *defines, '-Isrc', '-I', str(out),
                                'tools/createplayer_weapon_contract.cpp', 'src/game/core/String.cpp',
                                '-o', str(exe)], cwd=ROOT, check=True)
                run = subprocess.run([str(exe), str(args.asset.resolve())], cwd=ROOT,
                                     text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                (args.out_dir / f'{label}-{opt[1:]}.log').write_text(run.stdout)
                print(label, opt, 'exit', run.returncode, run.stdout.strip(), flush=True)
                if label == 'current':
                    assert run.returncode == 0
                else:
                    assert run.returncode != 0 and 'turret unexpectedly requested getSlotPos' in run.stdout
                results.append(dict(label=label, optimization=opt, exit=run.returncode))
    (args.out_dir / 'host-results.json').write_text(json.dumps(results, indent=2))


if __name__ == '__main__':
    main()
