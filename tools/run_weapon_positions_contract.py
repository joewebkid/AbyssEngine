"""Check the production loader against every original record without copying assets."""
import argparse
import hashlib
from pathlib import Path
import shutil
import struct
import subprocess
import tempfile

from run_particle_control_smoke import function

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--asset', type=Path, required=True)
    parser.add_argument('--cxx', default=shutil.which('g++'))
    args = parser.parse_args()
    if not args.cxx:
        parser.error('specify --cxx')
    if shutil.disk_usage(ROOT).free < 64 * 1024 * 1024:
        parser.error('bounded host gate requires 64 MiB free working margin; no cleanup attempted')
    data = args.asset.read_bytes()
    assert len(data) == 7484 and hashlib.sha256(data).hexdigest() == (
        'eebc7b6361bfdb19248f3f40071f915dad0cca919b292ad015d8d890f5570f82')
    cursor = 0
    records, entries, widths = [], [], []
    while cursor < len(data):
        ship, count = struct.unpack_from('<hh', data, cursor)
        cursor += 4
        widths += [2, 2]
        begin = len(entries)
        assert count >= 0
        for _ in range(count):
            kind, x, y, z = struct.unpack_from('<hhhh', data, cursor)
            cursor += 8
            widths += [2] * 4
            extra = (0, 0, 0)
            assert 0 <= kind < 4
            if kind == 3:
                extra = struct.unpack_from('<III', data, cursor)
                cursor += 12
                widths += [4] * 3
            entries.append((kind, x, y, z, *extra))
        records.append((ship, begin, count, cursor, len(widths)))
    assert cursor == len(data) and len(records) == 59 and len(entries) == 552
    assert len({r[0] for r in records}) == 59 and sum(e[0] == 3 for e in entries) == 236
    assert set(range(64)) - {r[0] for r in records} == {13, 14, 15, 50, 53}
    oracle = 'static const Entry expectedEntries[] = {\n' + ''.join(
        '{' + ','.join(str(v) if i < 4 else f'0x{v:08x}u' for i, v in enumerate(e)) + '},\n'
        for e in entries) + '};\n'
    oracle += 'static const Record expectedRecords[] = {\n' + ''.join(
        '{' + ','.join(map(str, r)) + '},\n' for r in records) + '};\n'
    oracle += 'static const unsigned char expectedReadSizes[] = {' + ','.join(map(str, widths)) + '};\n'

    reader = (ROOT / 'src/engine/file/FileRead.cpp').read_text()
    ae = (ROOT / 'src/engine/file/AEFile.cpp').read_text()
    bodies = '\n'.join(function(reader, signature) for signature in (
        'FileRead::FileRead()', 'FileRead::~FileRead()',
        'Array<Array<Vector *> *> *FileRead::loadWeaponPositions('))
    bodies += function(ae, 'uint32_t AEFile::Read(int16_t &value, uint32_t handle)')
    bodies += function(ae, 'uint32_t AEFile::Read(float &value, uint32_t handle)')
    level = (ROOT / 'src/game/world/Level.cpp').read_text()
    create = function(level, 'void Level::createPlayer()')
    cleanup = function(create, 'if (wpos != nullptr)')
    gun = (ROOT / 'src/game/weapons/Gun.cpp').read_text()
    consumers = 'namespace Consumers {\nstatic const float kZOffset = 100.0f;\n'
    assert 'static const float kZOffset = 100.0f;' in gun
    consumers += function(gun, 'void Gun::setOffset(Vector *v)')
    consumers += function((ROOT / 'src/game/ship/PlayerEgo.cpp').read_text(),
                          'void PlayerEgo::setTurretPosition(Vector v)') + '\n}\n'
    consumers += 'namespace AbyssEngine { namespace AEMath {\n' + function(
        (ROOT / 'src/engine/math/Vector.cpp').read_text(), 'Vector &Vector::operator=(const Vector &rhs)') + '\n}}\n'
    with tempfile.TemporaryDirectory(prefix='weapon-contract-', dir=ROOT / '_work') as temp:
        out = Path(temp)
        (out / 'weapon_contract_oracle.inc').write_text(oracle)
        (out / 'weapon_contract_body.inc').write_text(bodies)
        (out / 'weapon_contract_consumers.inc').write_text(consumers)
        (out / 'weapon_contract_cleanup.inc').write_text(cleanup)
        for opt in ('-O0', '-O2'):
            exe = out / f'weapon_contract_{opt[1:]}.exe'
            subprocess.run([args.cxx, '-std=gnu++17', opt, '-g0', '-Isrc', '-I', str(out),
                            'tools/weapon_positions_contract.cpp', 'src/game/core/String.cpp',
                            '-o', str(exe)], cwd=ROOT, check=True)
            subprocess.run([str(exe), str(args.asset.resolve())], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
