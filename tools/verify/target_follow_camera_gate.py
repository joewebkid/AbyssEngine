#!/usr/bin/env python3
"""Check the fixed TFC body registry and whole-corpus exact-pair retention."""
import argparse
import hashlib
import json
from pathlib import Path


def read(path):
    return json.loads(Path(path).read_text())


def exact_pairs(report, field):
    return {(f['unit'], f['symbol']) for f in report['functions'] if f[field]}


def metrics(report, registry):
    rows = {f['symbol']: f for f in report['functions']}
    names = {name for body in registry['bodies'] for name in body['symbols']}
    assert set(rows) == names, 'TFC symbol denominator changed'
    assert len(names) == 42 and len(registry['bodies']) == 40
    # Aliases sharing one Android address must have identical target/body scores.
    for body in registry['bodies']:
        variants = [rows[name] for name in body['symbols']]
        assert len({(f['source_match'], f['n_target']) for f in variants}) == 1
    unique = [rows[body['symbols'][0]] for body in registry['bodies']]
    weights = [body['instructions'] for body in registry['bodies']]
    symbol_weights = registry['weights']
    return {
        'symbols': len(rows), 'unique_bodies': len(unique),
        'symbol_source_mean': sum(f['source_match'] for f in rows.values()) / len(rows),
        'symbol_instruction_weighted': sum(f['source_match'] * symbol_weights[n]
            for n, f in rows.items()) / sum(symbol_weights.values()),
        'unique_source_mean': sum(f['source_match'] for f in unique) / len(unique),
        'unique_instruction_weighted': sum(f['source_match'] * w for f, w in zip(unique, weights)) / sum(weights),
        'unique_fuzzy_mean': sum(f['match'] for f in unique) / len(unique),
        'instruction_weight_total': sum(weights),
        'linked_exact': report['linked_exact'], 'byte_exact': report['byte_exact'],
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('registry', 'baseline', 'final', 'full_baseline', 'full_final', 'output'):
        parser.add_argument('--' + name.replace('_', '-'), required=True)
    args = parser.parse_args()
    registry = read(args.registry)
    baseline, final = read(args.baseline), read(args.final)
    before, after = read(args.full_baseline), read(args.full_final)
    result = {'baseline': metrics(baseline, registry), 'final': metrics(final, registry)}
    result['full_corpus'] = {label: {key: report[key] for key in (
        'count', 'avg_match', 'avg_source_match', 'linked_exact', 'byte_exact',
        'skipped', 'original_functions', 'compared_unique', 'missing_wrong_type', 'missing_absent')}
        for label, report in (('baseline', before), ('final', after))}
    result['exact_pair_retention'] = {}
    for label, field in (('linked', 'linked_equal'), ('byte', 'bytes_equal')):
        old, new = exact_pairs(before, field), exact_pairs(after, field)
        lost = sorted(old - new)
        result['exact_pair_retention'][label] = {
            'baseline': len(old), 'final': len(new), 'lost': lost,
            'gained': sorted(new - old),
        }
        assert not lost, f'{label} exact-pair regressions: {lost}'
    assert after['skipped'] == 0 and after['missing_wrong_type'] == 0
    assert {f['symbol']: f['n_target'] for f in baseline['functions']} == {
        f['symbol']: f['n_target'] for f in final['functions']}, 'original target bodies changed'
    result['gate_passed'] = all(result['final'][key] >= 99.0 for key in (
        'symbol_source_mean', 'symbol_instruction_weighted',
        'unique_source_mean', 'unique_instruction_weighted'))
    assert result['gate_passed'], 'TFC 99% mean gate not met'
    result['input_sha256'] = {name: hashlib.sha256(Path(getattr(args, name)).read_bytes()).hexdigest()
        for name in ('registry', 'baseline', 'final', 'full_baseline', 'full_final')}
    Path(args.output).write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result['final'], indent=2))
    print('TFC 99% / zero exact losses / full ARM wrong-type gate: PASS')


if __name__ == '__main__':
    main()
