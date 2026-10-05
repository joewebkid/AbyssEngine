# Missing Symbols and ABI Audit

Date: 2026-09-27. Scope: Android ARM verifier corpus and the seven `Array`
constructor/destructor aliases previously reported as wrong-type. This is not
a whole-engine ABI certification.

## Verifier Correction

`objdump` prints one label at an address, but C1/C2 and D1/D2 aliases may
share that address inside distinct `.text.*` COMDAT sections. The previous
parser accepted only `.text` and keyed aliases by address alone. It therefore
missed seven symbols that `nm` already found in
`base/engine/core/ArrayInstantiations.o`. `asmdiff.py` now keys aliases by
`(section, address)` and accepts `.text.*`; a synthetic two-section regression
test guards against accidentally merging unrelated zero-address functions.

The seven newly recognized names are:

| Original symbol | Local result |
| --- | --- |
| `_ZN5ArrayIPN11AbyssEngine7KeyCodeEEC2Ev` | 100% source-shape |
| `_ZN5ArrayIPN11AbyssEngine7KeyCodeEED2Ev` | linked-exact |
| `_ZN5ArrayIPS_IP9ImagePartEEC2Ev` | 100% source-shape |
| `_ZN5ArrayIP9BluePrintEC2Ev` | 100% source-shape |
| `_ZN5ArrayIP9BluePrintED2Ev` | linked-exact |
| `_ZN5ArrayIP14PendingProductED2Ev` | linked-exact |
| `_ZN5ArrayIbED2Ev` | 89.2% source-shape / 94.7% strict |

The final `Array<bool>` destructor still has a local null check before
`operator delete[]` that is absent in the target. Its match is not byte-exact;
the verifier correction alone does not prove ownership equivalence. Changing
the generic destructor would affect many call sites, so that difference needs
a separately measured ARM and runtime pass.

## Full Corpus

Saved final report: `_work/full_arm_array_alias_abi_postbuild_20260927.json`.
`--fail-on-wrong-type` exited 0 after all 204 ARM translation units rebuilt
successfully. The focused Array report is
`_work/array_alias_section_fix_20260927.json` (14 comparisons, 99.2% mean
source-shape). The new layout assertions below do not emit code.

| Metric | Earlier saved report | Current report |
| --- | ---: | ---: |
| Comparisons | 4,613 | 4,625 |
| Unique original functions compared | 4,274 / 4,524 | 4,281 / 4,524 |
| Missing exact-name symbols | 250 | 243 |
| Reported wrong-type | 7 | 0 |
| Linked-exact | 2,040 | 2,060 |
| Raw-byte-exact | 932 | 937 |
| Average source-shape | 88.65% | 89.44% |

The previous report is `_work/full_arm_hangar_render_amount_gate_20260927.json`.
These are dated snapshots, not a controlled claim that the verifier edit alone
caused every aggregate score change. In particular, source-shape and linked
counts include alias accounting.

## What Is Still Missing

The pre-ParticleSettings-pass `missing.txt` had 243 original names without an
exact-name local counterpart.
A name-prefix audit groups them as follows; provenance still needs checking
against the binary/link map when a specific library implementation matters.

| Group | Count | Interpretation |
| --- | ---: | --- |
| C++ standard library / ABI runtime | 98 | Not recovered game code |
| libzip | 74 | Third-party archive implementation |
| zlib | 47 | Third-party compression implementation |
| OpenSSL/crypto | 22 | Third-party crypto and CPU probes |
| `_armv7_tick` | 1 | Platform/crypto helper; owner not proven |
| `ParticleSettings::{unnamed type#1}::operator=` | 1 | Engine-owned ABI/lifetime gap at this snapshot; restored below |

The Android IDA body at `0xa06ce` first assigns an `AbyssEngine::String`,
then copies the following `0x94` bytes. This body is now implemented for the
32-bit build under the original anonymous-type ABI name, while the 64-bit
host retains its opaque-name adapter. Focused ARM verification reports the
operator linked-exact (100% source-shape), alongside linked-exact C1/C2 and
D2 for the 48-string slot array. The native smoke covers scalar table behavior;
it cannot exercise 32-bit String ownership. Do not infer complete particle
renderer parity from this isolated result.

After rebuilding all 204 ARM translation units, the full verifier report
`_work/full_arm_particle_string_abi_20260927.json` exits successfully with
zero wrong-type symbols: 4,626 comparisons, 4,282/4,524 original functions
compared, 242 exact-name symbols missing, 2,064 linked-exact functions, and
89.49% average source-shape (previously 4,625, 4,281/4,524, 243, 2,060,
and 89.44%). The missing-symbol remainder is external libraries/platform
code plus ABI/library variants; this count alone is not a game-code coverage
percentage.

The current-tree recheck is
`_work/full_arm_missing_symbols_abi_current_20260927.json`: 4,626 comparisons,
4,282/4,524 original functions compared, 242 missing exact names, zero
wrong-type, 2,064 linked-exact, 937 raw-byte-exact, and 89.5% rounded mean
source-shape. A case-sensitive prefix classification of the current
`missing.txt` accounts for every name: 98 C++ ABI/standard library, 74 libzip,
47 zlib, and 23 OpenSSL/crypto/ARM CPU probes. This groups symbols by name;
it does not prove that substitute libraries will behave identically at runtime.
`_armv7_tick` belongs in the crypto/probe group by its position at `0x18ff08`
in the Android symbol table, immediately between `_armv7_neon_probe` and the
other ARMv8 probes, following `OPENSSL_cleanse`/`CRYPTO_memcmp`. Its exact
object-file owner is not available from that ordering alone. The focused
post-rebuild Array report is `_work/array_alias_abi_current_20260927.json`:
14 comparisons, 99.2% mean source-shape.

## ABI Checkpoints

- `Array<T>` ARM layout is now compile-locked at size `0x0c`, with
  `size/data/capacity` at `0x00/0x04/0x08` in `ArrayInstantiations.cpp`.
- `AbyssEngine::String` is compile-locked at size `0x0c`, with `data` at
  `+0x04` and `length` at `+0x08`, on ARM32 in `AEString.h`. Android String's
  default constructor at `0x72398` initializes the vptr at `+0x00` and these
  two fields in order; the Android `Hud` constructor's twenty String slots
  advance by twelve bytes. The assertions emit no code and do not verify
  hidden return-slot placement or String ownership.
- `AEMath::Vector` has size and component-offset assertions in `Vector.h`;
  `ParticleSettings` has stride/field assertions in `ParticleSettings.h`;
  `HangarWindow` has size and selected field assertions in `HangarWindow.h`.
- `ApplicationManager.cpp` checks offsets of the named module-vtable slots.
  This proves the local table's layout, not that every live object's vptr or
  every virtual target has been validated at runtime.
- Hangar `initialize` has focused evidence for `getPosition` hidden-return
  lifetimes, and several recovered String paths have ARM source-shape checks.
  This does not establish all String/Vector return-slot placement or ownership
  across the engine.
- `Array<T>` uses allocation, growth and release paths that need an independent
  ownership audit before treating native execution as safe. Green static-library
  builds and high source-shape scores are not runtime proof.

Next ABI work: trace construction, assignment, growth and destruction of
`Array<bool>` and the `ParticleSettings` String slots on a real 32-bit runtime.
The `Array<bool>` destructor still differs by one null check, while other
specializations are already linked-exact; changing the shared destructor for
one score would be unsafe. Audit `operator new[]`/`realloc`/`operator delete[]`
pairing and test the actual call sites before changing ownership. For other
classes, add assertions and call-site checks only when an Android/iOS/PC body
supplies actual offsets or return-slot evidence; do not pad types or invent
vtable entries for a metric. The updated header compiled across all 204 ARM
translation units; the UCRT64 `gof2` static library also linked successfully.

## Full-Corpus Scope Recheck

The subsequent complete `--fail-on-wrong-type` run is saved as
`_work/full_arm_scope_abi_20260927.json`: 4,626 comparisons, 4,282/4,524
unique originals compared, 242 exact-name misses, zero wrong-type, 2,064
linked-exact and 937 byte-exact. The seven `Array` aliases remain recognized;
the focused 14-symbol report is `_work/array_abi_scope_check_20260927.json`.

`tools/scope_filter.py --report _work/full_arm_scope_abi_20260927.json`
classifies all 242 misses as external glue: 98 C++ library/ABI runtime
(including six global new/delete operators), 74 libzip, 47 zlib and 23
crypto/CPU-probe names. It now reads the local Android symbol table rather
than a developer-specific path. Scoped reports and stale `missing.txt` or
`extra.txt` files are rejected, because they cannot establish whole-corpus
absence. The in-scope counts are **0 absent, 0 wrong-type, 44 extra**.

Those 44 extras are definitions *we* emit that the original symbol table
does not list, not missing game implementations. Many are local adaptation
helpers; four Hangar lambdas are also present. The higher-risk remainder
includes generated `AbstractGun` and `IApplicationModule` destructor variants
and `AbyssEngine::String` copy construction. The absence of their exact names
does not by itself prove a vtable or ownership error, but neither does the
green link prove safety. Audit actual vtable entries and call sites before
removing or suppressing any of them. `Array<bool>::~Array()` remains
89.2% source-shape / 94.7% strict with one extra null test before delete;
no container lifetime code was changed in this bookkeeping pass.
