# String float family (2026-10-19)

Starting commit: published main `04ee4257ae63d7e325084181c237061349d942c3`.
Branch: `codex/sol-string-float-20261019`, in the existing isolated
`_work/sol-portrait-runtime-20261019` checkout. The previous branch and commit
are retained. No dirty root checkout, shared status, StarSystem, FileRead,
PaintCanvas/TextureCreate or Astra gate is edited.

## Fixed baseline, recorded before implementation

Unit for both logical functions: `game/core/String`.

| Original symbol | ARM address / ELF size | Source-shape | Strict fuzzy | Linked / byte exact |
| --- | --- | ---: | ---: | --- |
| `_ZN11AbyssEngine6String3SetEf` | `0x72cb0 / 0x1cc` | 96.6% | 65.6% | no / no |
| `_ZN11AbyssEngine6StringC2Ef` | `0x729bc / 0x24` | 97.3% | 96.6% | no / no |
| Two-logical-function mean | | **96.95%** | **81.10%** | **0 / 0** |

Aliases: the original exported dynsym has C2 (weak), but **no C1 float
constructor symbol**. The baseline local ARM object defines C1 and C2 at the
same address `0x1014`, size `0x24`. C1 has no separate original comparison:
its source-shape/strict/exact metrics are N/A, not an extra zero or an extra
copy of C2 in the family average. Both aliases remain explicitly audited.
Baseline Set(float) local address/size: `0xd20 / 0x1c0`.

Source: original Android 2.0.16 ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Original binaries and assets are read in place, never copied.

## Confirmed ARM contract and changes

Read AGENTS.md, README/CONTRIBUTING/current status, relevant validation guidance,
AEString.h, String.cpp, the FloatStringImpl.inc dependency, actual float helper
callers and existing String/ABI/ResourceTexture fixtures. Ponytail is applied to
keep changes within the selected family. No existing checkout graph is available.

Set(float)'s softfp member input is in r1. ARM `0x72cd2` moves it to r0 for the
free helper, `0x72cd4` sets precision 10, r2/r3 point to exponent/sign outputs,
and the fifth argument is **1**, stored at [sp,#0] by `0x72cce–0x72cd0` before
the call at `0x72cd6`. Existing source passed 0. Independently, the helper's
`0x72c00–0x72c0c` adds exponent to the digit limit only for mode 0. Thus mode 1
selects ten significant digits; mode 0 selects a fixed fractional count.

Set now passes mode 1 and treats exponent/sign as the helper's output parameters,
matching the original uninitialized output slots. The unchanged helper writes
both on its supported finite paths. Formatting still builds an owned temporary
String: prefix `0.` and -exponent zeros when exponent <= 0, append digits, insert
`.` at a positive exponent, and prepend `-` for any nonzero sign flag. Original
PC-relative literal references are checked for all four strings. The raw digit
array is deleted at `0x72d9a` **before** the final Set(result.data) at `0x72da2`;
the result String stays alive through that copy and is destroyed at `0x72da8`.

The float constructor at `0x729cc` initializes only vptr and null data, then
calls Set(float) at `0x729d2`. It does not store length before Set. Its source
now initializes data and calls Set directly; the redundant length initialization
is removed. Set's final wide-string copy initializes the length before return.
No header, String layout, signature, precision, rounding helper implementation,
or allocation/deallocation API is changed. ABI offsets are ARM32 evidence, not
assumptions about the host executable's pointer width.

The one specifically authorized correction in
`STRING_FLOAT_HELPER_ARM_2026-10-15.md` removes its erroneous mode-0 attribution
to Set. Its separate direct mode-0 helper smoke remains a historical direct
helper test, not evidence for Set's fifth argument.

## Fixed-family result and aliases

| Original comparison | After source-shape | After strict fuzzy | Linked / byte exact |
| --- | ---: | ---: | --- |
| Set(float) | **96.7%** | **68.3%** | no / no |
| String(float), C2 | **100%** | **100%** | yes / no |
| Two-logical-function mean | **98.35%** | **84.15%** | **1 / 0** |

The >=98% target is met by the **unweighted fixed-family mean**. Set(float)
itself remains below 98%; the tiny constructor does not prove the formatter
complete. Set has 178 base versus 182 target instructions, unchanged counts
from baseline; C2 has 14 versus 14 after (previously 15 versus 14). No compiler
flags, verifier normalization, symbol selection or semantic behavior was changed
to force the remaining source shape. Numerical readiness is reported separately.

After-build local aliases C1/C2 both occur at `0x1020`, size `0x24`; local
Set is `0xd2c / 0x1c0`. C1 remains an extra local alias with no original C1
dynsym comparison, and is still counted once with C2 as one logical constructor.
Original C2 is weak whereas local C1/C2 are strong, as before this patch; export
binding parity of a complete shared game library is not established here.
The entire String TU comparison changes only the two selected original rows,
gains C2 linked-exact, and loses no prior linked- or byte-exact pairs.

## Executable tests and functional limits

`run_string_float_smoke.py` compiles the actual String.cpp TU at `-O0/-O2 -g0`.
The concatenation function is extracted unchanged from AbyssEngine.cpp; the
unchanged production FloatStringImpl.inc body executes for the real finite
cases. The only unexecuted service traps are GameText::getLanguage and print.
No GPU or game service is replaced by a successful mock.

Eight real-helper inputs exercise both Set on an existing allocation and the
float constructor: +/-zero, +/-1.5, 0.25, 0.025, 10 and 1,000,000. Examples of
the corrected mode-1 output are `1.500000000`, `-1.500000000`,
`0.02500000000`, and `1000000.000`; zero retains `0.0000000000`.
These are checks of the supported production route, not a universal decimal
rounding oracle for the unchanged helper.

Eight controlled-helper cases exercise the actual String formatting bodies
with exponent 0/-3/1/10, nonzero sign flags other than 1, and bit-preserving
forwarding of denorm_min, the largest float below 2^31, infinity, -infinity,
quiet NaN and FLT_MAX. The helper boundary always checks precision 10, mode 1,
distinct output pointers and exact input bits. Its controlled digit results
are explicit test inputs, **not recovered NaN/infinity/out-of-range numerical
conversion outputs**. The unchanged production helper converts float to int;
nonfinite/out-of-int-range casts do not supply a portable C++ numerical contract.
Actual numerical handling of those inputs, all rounding edges, and ARM runtime
floating-point environment/denormal behavior remain unverified.

The test's array allocator observes every formatting allocation and release.
Each call checks that the helper-owned digits are freed once before the old
destination data is released, only the final destination data remains live
after Set/constructor, and no arrays remain live after destruction. Both
functions execute for all sixteen cases at both optimization levels. The
published baseline source fails the same gate on `mode == 1`, as an expected
negative control.

`test_string_float_family_arm.py` independently checks original symbols/alias
absence, constructor stores, softfp argument forwarding, stack mode 1, literal
strings and deletion/final-copy/destruction order. The full verifier suite runs
66 tests with one existing skip and no failures. The previous actual
String/ResourceTexture TU gate stays green at `-O0/-O2`, without a GetAEChar shim.

The explicitly authorized follow-up updates exactly four expectations in
`tools/abi_runtime_smoke.cpp`: positive/negative 1.5, small 0.025 and appended
1.5 now use the real mode-1 production outputs above. The direct mode-0 helper
check remains unchanged and valid. The legacy gof2_abi_runtime_smoke is also
rebuilt and executed; its result is recorded with the build gates below.

## Build and corpus gate

Existing native and ARM directories are reused. Native gof2 passes with
`-O0 -g0`; matching ARM uses unchanged canonical NDK r18b `-Oz` flags.
Both ARM builds report 208/208 units with zero failures. No original binary,
OBB or asset is copied and no other agent's data is deleted.
The existing native build enables GOF2_BUILD_ABI_RUNTIME_SMOKE and builds both
gof2 and gof2_abi_runtime_smoke; executing the latter reports
`GOF2 native ABI/lifetime smoke: OK` at the native build's `-O0 -g0` settings.

Both full runs compare **4,731 (unit, symbol) pairs**, covering **4,323/4,524**
original functions. Both have zero skipped units, zero wrong-type signatures,
201 absent originals and 124 extra symbols. Rounded corpus means remain
**93.91% source-shape / 83.19% strict fuzzy**; the focused mean's improvement
is not misreported as a large whole-corpus gain.

Actual `(unit, symbol)` exact-set comparison gains exactly constructor C2:
**2,547 -> 2,548 linked-exact**, with **zero losses**. Byte-exact remains
**1,079**, with zero gains or losses. Comparing every report row changes only
Set(float) and constructor C2. SHA-256 comparison of all 208 object pairs finds
only game/core/String.o changed; all other 207 are identical. No C1 counterpart
is invented in the original comparison universe.

Generated local reports, excluded from the commit:

- `_work/string-float-before-full.json`, SHA-256
  `9a506a8757b3c2e96b7339ba746ee98a12f583ba34fc6f6baf91786b2fc3db38`;
- `_work/string-float-after-full.json`, SHA-256
  `104de11a8292db30b6b111f64d8cd353c77930a831024e2d175dd1a7b6699506`;
- `_work/string-float-before-focused.json`, `_work/string-float-after-focused.json`;
- `_work/string-float-before-whole-string.json`, `_work/string-float-after-whole-string.json`;
- `_work/string-float-exact-pairs.json` and `_work/string-float-objects.json`;
- `_work/string-float-host.log`, `_work/string-float-negative.log`,
  `_work/string-float-tests.log`, `_work/string-float-abi.log` and
  `_work/string-float-texture-regression.log`.

Both complete ARM runs use the original external SO/symbol work directory,
the configured external NDK r18b, independent reused base/target directories,
and the unchanged verifier with `--no-build --fail-on-wrong-type`.

```powershell
python tools/run_string_float_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
python tools/run_resource_texture_string_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
python -m unittest discover -s tools/verify -p 'test_*.py'
```

All work remains on the isolated branch. Full-game linking, a visible native
frame, and comprehensive float digit-generation equivalence are not established
by a two-function source-shape mean or these bounded executable tests.
