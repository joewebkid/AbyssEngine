# ASM Validation: original `.so` ↔ our build

> Read [PROJECT_STATUS.md](PROJECT_STATUS.md) before interpreting a saved
> metric. `report.json` is a dated snapshot; a fresh local verifier run is the
> authoritative result for the revision and environment being inspected.

Continuously check whether our recovered C++ still assembles to the **same
instructions** as the original Android binary, function by function, and tune
compiler flags toward byte-exactness — without disturbing normal macOS
development.

## TL;DR

```bash
# one-time on macOS/Linux with OrbStack: provision the ubuntu machine
bash tools/verify/setup.sh

# Windows/MSYS fallback: use local Android NDK r18b and populate _work from the
# original Android .so.
export GOF2_VERIFY_LOCAL_NDK=1
python3 tools/verify/prepare_work_from_so.py \
  --so ../references/Apk/lib/armeabi-v7a/libgof2hdaa.so \
  --work _work

# configure the matching build and run the report
cmake --preset match
cmake --build cmake-build-match --target verify

# inspect one function (side-by-side disassembly diff)
FN=_Z8ArrayAddIP14AEPakFileEntryEvT_R5ArrayIS2_E cmake --build cmake-build-match --target verify-fn

# or directly:
python3 tools/verify/verify.py --build-dir cmake-build-match/verify --no-build \
  --unit game/weapons/Radar --only '^_ZN5Radar'
python3 tools/verify/verify.py --build-dir cmake-build-match/verify --no-build --show _ZN5Radar10hasScannerEv
# On Windows/MSYS, add the known translation unit to avoid scanning every object:
python3 tools/verify/verify.py --build-dir cmake-build-match/verify --no-build \
  --unit game/menu/MGame --show _ZN5MGame10OnTouchEndEiiPv
# Focused Radar inspection also avoids a whole-corpus scan:
python3 tools/verify/verify.py --build-dir cmake-build-match/verify --no-build \
  --unit game/weapons/Radar --show _ZN5Radar4drawEP6PlayerP3Hudi
```

Normal development is unchanged: `cmake --preset debug` still uses local Apple
clang and never touches OrbStack/local NDK verify tooling.

## Radar Snapshot (2026-09-15)

The unchanged 19-symbol family averages **99.0% source-shape**, with 13
linked-exact and eight raw-byte-exact symbols. Large draw itself measures
**90.8% source-shape / 41.7% strict fuzzy**, not byte-exact. Constructor and
destructor aliases and short accessors affect the unweighted mean; never
describe it as 99% exact draw or game recovery. See
[the evidence and remaining work](RADAR_DRAW_SOURCE_SHAPE_2026-09-15.md) and
[the per-function snapshot](validation/radar_family_2026-09-15.json).

`--unit` now selects a single object for family reports too, before any corpus
traversal or comparison. Add `--no-build` to reuse current objects. Such reports
contain `scoped_unit`, not global coverage claims, and reject
`--fail-on-wrong-type`: that gate still requires the full unfiltered corpus.
Run `python3 -m unittest discover -s tools/verify -p 'test_*.py'` for the scoped
runner and ASM normalization regressions. Never run simultaneous reports or
`--show` operations that rewrite the same target object.

## ParticleSettingsRef Native Smoke

The focused ABI/runtime smoke target does not link the whole recovered host
library: unrelated render shim symbols still prevent that library from forming
a standalone executable. It compiles only the two audited table translation
units and exercises their actual initialization/scaling/interpolation path.

```powershell
C:\msys64\ucrt64\bin\cmake.exe -S . -B cmake-build-ucrt -G Ninja `
  -DGOF2_BUILD_PARTICLE_SETTINGS_SMOKE=ON
C:\msys64\ucrt64\bin\cmake.exe --build cmake-build-ucrt `
  --target gof2_particle_settings_smoke
.\cmake-build-ucrt\gof2_particle_settings_smoke.exe
```

Expected output: `ParticleSettingsRef ABI/runtime smoke: OK`.

Windows note: the wrapper names are still `orbcc`, `orbnm`, `orbas`, and
`orbobjdump`, but they now also support a local NDK r18b fallback. Set
`GOF2_VERIFY_LOCAL_NDK=1` and keep the NDK at `.cache/ndk/android-ndk-r18b`, or
point `GOF2_NDK_ROOT` / `NDK` at another NDK r18b checkout.

## Why this exists / how it works

The original `libgof2hdaa.so` is ARMv7-A Thumb-2 + VFP/NEON, built with **NDK
r18b clang 7.0.2** for `armeabi-v7a`. We validate against it like this:

1. **Compile** each `src/*.cpp` with the *matching* toolchain. That toolchain only
   runs on Linux, so it lives in the **OrbStack `ubuntu`** machine and is invoked
   through `tools/verify/orbcc` (OrbStack mirrors the macOS filesystem 1:1, so
   objects land back under the repo). The per-TU build is **resilient** — a TU that
   doesn't compile yet is skipped and reported, never aborting the run.
2. **Delink** the matching originals out of the stripped, fully-linked `.so` into a
   relocatable ARM `.o` carrying the real mangled symbol names (`delink.py`).
3. **Diff**: disassemble both sides with `arm-linux-gnueabihf-objdump`, truncate to
   each symbol's real size, normalize away post-link absolute addresses, and
   compute strict fuzzy, source-shape, linked-exact and byte-exact evidence
   (`asmdiff.py`).
4. **Auto-discovery**: any mangled symbol present in *both* our object and the
   `.so` symbol table is compared — coverage grows automatically as more TUs
   compile. No function list to maintain.

> **Why not [objdiff](https://github.com/encounter/objdiff)?** Its 32-bit ARM
> disassembler caps at ARMv6K (GBA/DS/3DS). This game is ARMv7-A Thumb-2 with
> VFP/NEON, which objdiff rejects outright, so we disassemble with GNU objdump
> (full ARMv7 support) and do our own normalize + match.

## Reading the report

```
 match   source  link  unit                               symbol
  100.0%  100.0%    ==  engine/math/AEMath                 _ZN11AbyssEngine6AEMath...mlERKNS0_6VectorES3_
   67.5%   96.4%        engine/math/AEMath                 _ZN11AbyssEngine6AEMath6MatrixmLERKS1_
...
N comparisons   avg fuzzy 67.5%   avg source 96.4%   100%-fuzzy 30   byte-exact 21
coverage: compared 2311/4524 original functions   missing 2213 (-> cmake-build-match/verify/missing.txt)
report -> cmake-build-match/verify/report.json
```

- **`L` (linked-exact)** is the real matching signal: the machine code is byte-identical to
  the original **after linking** — i.e. identical except inside relocation-covered fields (call
  targets, GOT/literal-pool addresses), which our unlinked `.o` leaves as placeholders while the
  delinked target carries the linker-resolved value. This is what to chase. (`asmdiff.linked_equal`
  masks the relocation byte-ranges, read from our object's `-r` table, before comparing.)
- **`==` (byte-exact)** is the rarer strict subset: raw bytes identical with *no* relocations at
  all — only leaf functions that never call out or load a global can reach it. A function can be a
  perfect match (`L`) yet not `==` purely because it makes a call; don't chase `==`, chase `L`.
- **`match`** is a fuzzy instruction-level score (0–100). Useful for "how close" and
  for comparing flag settings. It can read below 100% even for a byte-exact function
  when the original contains a literal pool (clang marks pools as data on our side;
  the delinked original has no such marker so objdump decodes the pool as code) — so
  trust `bytes ==` for "matched", `match` for "getting warmer".
- **`source` (`source_match`)** is an allocation-insensitive source-shape score,
  not a byte-match claim. It is the equal-weight mean of five separately recorded
  signals: duplicate-preserving instruction-shape inventory, opcode inventory,
  control-flow shape, call-count agreement, and instruction-count agreement. Control
  flow accepts either the ordered branch sequence or its duplicate-preserving inventory
  so compiler-only basic-block placement is not treated as missing behavior. It ignores
  which general-purpose register or local stack slot Clang selected and tolerates
  non-control instruction scheduling. It still penalizes changed calls, branch kinds,
  operation counts, constants, argument order and object-field offsets.
  Use it to audit recovered behavior across compiler allocation differences; use `L`
  or `==` for exact machine-code claims. Per-function components are written as
  `instruction_inventory_match`, `opcode_inventory_match`, `control_flow_match`,
  `ordered_control_flow_match`, `control_inventory_match`, `call_count_match`, and
  `size_match` in `report.json`.
- **`coverage` / `missing`** is the progress metric: how many of the original's
  `.text` functions we compared at all. *Missing* = original functions with no
  counterpart in our build — either not decompiled/compiled yet, or whose signature
  mangles differently (the `Array`-vs-`std::vector` gap accounts for a few hundred).
  The full list is written to `cmake-build-match/verify/missing.txt`.
- **`wrong type` (`missing_wrong_type`)** splits that *missing* pile into the two cases that
  look identical to an exact-name match but need very different fixes. We demangle every missing
  original and every symbol our build defines, strip the parameter list to a *qualified name*
  (`AEFile::Open`), and group by it. A missing original whose qualified name we **do** define is
  reported as *implemented under a different signature* — the body exists, only a parameter or
  qualifier type is off (`String` by-value vs `String&`, a global enum that should be nested,
  `void*` vs `char const*`), so the mangled name differs and the exact-name match skipped it.
  These are the cheap wins: retype our params to match the original and the function starts
  comparing (and usually matching). The rest are *genuinely absent* (never written). Both sides'
  full signatures, grouped by name, are written to
  `cmake-build-match/verify/missing_wrong_type.txt`; `report.json` carries the counts
  (`missing_wrong_type`, `missing_absent`) and the structured `wrong_type` list. Demangling uses
  `c++filt -n` (Itanium-aware on macOS — no OrbStack needed for this step).
- `report.json` has the full per-function data, `avg_match`, `avg_source_match`,
  the source-shape component scores, plus the coverage counts
  (`count`, `compared_unique`, `original_functions`, `missing`, `missing_wrong_type`,
  `missing_absent`) and the structured `wrong_type` pairings for scripting/CI.

As of 2026-09-23, `source_match` combines five equally weighted evidence
signals: allocation-normalized instruction inventory, opcode inventory,
control-flow shape, call-count agreement, and instruction-count agreement.
The report keeps both ordered control flow and duplicate-preserving control
inventory; `control_flow_match` uses the stronger of the two so compiler-only
basic-block placement does not dominate the source score. This affects only
the evidence score. Strict fuzzy, linked-exact, and byte-exact comparisons are
unchanged and remain the authority for binary matching.

## Tuning compiler flags

The canonical flag set is `tools/verify/match_flags.sh` (the *one* source of truth,
read by both `build_objs.sh` and the report driver). The biggest knob is the
optimization level:

```bash
# A/B test opt levels — re-run picks up changed flags and rebuilds:
cmake --preset match -DGOF2_MATCH_OPT=-O2
cmake --build cmake-build-match --target verify
```

`GOF2_MATCH_OPT` flows through to the build via the `match` preset. Other flags
(fpu, stack protector, API level, exceptions/rtti) live in `match_flags.sh`; edit
there and re-run `verify`. Known facts already baked in: `-mfpu=neon` (the original
uses NEON), `-D__ANDROID_API__=21` (needed for libc++ `<cmath>` to compile),
`-mthumb`, `-frtti`, `-fstack-protector-strong`, **`-DGOF2_MATCH=1`** (selects the real `Array<T>` over the dev-build
`std::vector` alias, see common.h).

> **`-Oz` vs `-O2` — settled: `-Oz`.** A clean A/B on equal coverage (2326 functions compared in
> both) gave `-Oz` 468 byte-exact / 66.1% avg vs `-O2` 444 / 54.9%; 24 functions match only at
> `-Oz` and none only at `-O2`. The original was built `-Oz`. (`match_flags.sh` defaults to it.)

> **Stack protector level - settled: `-fstack-protector-strong`.** On the same
> 4436-function corpus it raised average fuzzy similarity from `70.1%` to
> `71.64%` and linked-exact functions from 1848 to 1871, without losing a
> linked- or raw-byte-exact function. It also reproduces the exact 224-byte
> `Hud::draw` frame and canary path. See
> [HUD_BOOST_STACK_PROTECTOR_ARM_2026-08-19.md](HUD_BOOST_STACK_PROTECTOR_ARM_2026-08-19.md).

## Files

| Path | Role |
|------|------|
| `tools/verify/setup.sh` | one-time OrbStack provisioning (NDK r18b + binutils) |
| `tools/verify/prepare_work_from_so.py` | create `_work/bins` + `_work/symbols` from original `libgof2hdaa.so` |
| `tools/verify/match_flags.sh` | canonical matching compiler flags (`GOF2_MATCH_OPT`) |
| `tools/verify/build_objs.sh` | resilient per-TU ARM compile → `verify/base/*.o` |
| `tools/verify/delink.py` | extract original functions from the `.so` → `verify/target/*.o` |
| `tools/verify/asmdiff.py` | objdump-based normalize + per-symbol match |
| `tools/verify/verify.py` | orchestrator: table + `report.json`; `--show` one function |
| `tools/verify/orb{cc,as,nm,objdump}` | run NDK clang / ARM binutils in OrbStack or local NDK fallback |
| `cmake/orbstack-ndk-arm.toolchain.cmake` | the `match` preset's toolchain |

Inputs (read-only): `_work/bins/android_2.0.16_libgof2hdaa.so`,
`_work/symbols/android_2.0.16.symbols.tsv`, `_work/symbols/android_thumb_map.tsv`.
If these files are absent but the original APK `.so` is available, regenerate
them with `tools/verify/prepare_work_from_so.py`.

## Known coverage gaps

Auto-discovery matches functions by **exact mangled name**, so anything whose signature mangles
differently than the original is invisible (not wrong — just not compared):

- **`Array<T>` vs `std::vector` — RESOLVED.** The match build (`-DGOF2_MATCH`) now uses a faithful
  hand-written `Array<T>` (global template, layout `{size@0,data@4,capacity@8}`, realloc-based
  growth — bodies transcribed from the Android binary; see common.h). This mangles as `5ArrayI...`
  like the original (so the previously-invisible container functions now compare) **and** matches
  the original's element-access / iteration codegen. Switching from the `std::vector` alias moved
  the report from ~724 → 910 linked-exact and ~2327 → 2474 compared. The macOS dev build keeps the
  `std::vector` alias (the `#else` branch in common.h) for natural 64-bit development.
- TUs that don't compile under the ARM toolchain yet are skipped (see the build summary); their
  functions simply aren't compared until they build. In the 2026-09-24 verification run, the NDK r18b
  build compiles all 204 source-mapped translation units with zero failures. This confirms the
  earlier `Player`, `NewsTicker`, and `StarMap` `SolarSystem*`/`int` conflicts are resolved. Count
  source-mapped objects, not orphan `.o` files left in the base directory.

The same verification run's full-corpus report compared 4,274/4,524 original functions
across 4,613 symbols, skipped no units, and preserved all 2,037 linked-exact and
932 raw-byte-exact functions from its baseline. The focused five-symbol
`TextureCreate*` report is saved at
`_work/texture-create-arm-scalar-trial.json`; it measures 97.54% unweighted
and 93.07%
instruction-weighted source-shape. The 99% target was not reached; see
[`AEI_TEXTURE_CREATE_SOURCE_SHAPE_ARM_2026-09-24.md`](AEI_TEXTURE_CREATE_SOURCE_SHAPE_ARM_2026-09-24.md)
for per-function metrics and rejected trials.

## Direct CLI (without CMake)

```bash
python3 tools/verify/verify.py                 # build + diff everything
python3 tools/verify/verify.py --no-build      # reuse existing base objects
python3 tools/verify/verify.py --only AEMath   # filter by symbol regex
python3 tools/verify/verify.py --show <mangled-symbol>
```

Both the build and the diff fan out across `GOF2_VERIFY_JOBS` workers (default 8). The per-object
delink/objdump calls have a 90s timeout (`asmdiff.DISASM_TIMEOUT`): if an OrbStack/local tool call
wedges, that one unit is skipped with a warning instead of hanging the whole run. If you kill an
OrbStack run mid-diff, check for an orphaned `arm-linux-gnueabihf-objdump` under `orb` and `kill` it.
For `--show`/`verify-fn` and scoped family reports, run one operation at a time per translation
unit; parallel operations for the same `.o` can race while rewriting `verify/target/.../*.o`.

## Selective fionera transfer snapshot (2026-09-20)

After rebuilding current ARM objects with NDK r18b, focused reports measured:

- `PlayerTurret`: 21 symbols, 95.5% source-shape, 12 linked-exact, four byte-exact.
- `TargetFollowCamera`: 42 symbols, 92.5% source-shape, 27 linked-exact, 19 byte-exact.
- `Gun`: 28 symbols, 72.5% source-shape, 14 linked-exact, 13 byte-exact.
- `CameraC2`: 100% strict/source and linked-exact.
- `ListItemWindow::update`: strict/source `71.7/93.6% -> 84.8/96.9%`.

These are scoped family results, not a replacement whole-corpus snapshot. ARM
layout constants independently confirm `PlayerTurret=0x168`, `Camera=0x5c`,
and the `TargetFollowCamera` tail offsets `0x120/0x130/0x138/0x13c`.
