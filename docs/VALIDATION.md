# ASM Validation: original `.so` ↔ our build

> Read [PROJECT_STATUS.md](PROJECT_STATUS.md) before interpreting a saved
> metric. `report.json` is a dated snapshot; a fresh local verifier run is the
> authoritative result for the revision and environment being inspected.

## Layout number formatting (2026-10-13)

The Android-backed `Layout::formatNumber(int)` reaches 98.9%
source-shape and 80.1% strict fuzzy; it remains non-exact. The
production-method host smoke covers grouped and negative values,
language-specific separators and `INT_MIN`. Native `gof2`, all 206
ARM units and 37 verifier tests pass (one skipped). The `INT_MIN`
output is a host observation, not an ARM32 runtime test. See
[the evidence](LAYOUT_FORMAT_NUMBER_ARM_2026-10-13.md).

## Layout drawBox default-flags wrapper (2026-10-13)

The Android-backed six-argument `Layout::drawBox` wrapper reaches
100.0% source-shape and strict fuzzy, adding a linked-exact (not
byte-exact) result. Native `gof2`, all 206 ARM units, the
production-method UI smoke and 37 verifier tests pass (one skipped).
The smoke cannot observe flag bit 0, which the current renderer does
not test; the focused ARM comparison verifies the call shape. See
[the evidence](LAYOUT_DRAWBOX_WRAPPER_ARM_2026-10-13.md).

## Layout window-dimension reset (2026-10-13)

The Android-backed `Layout::resetWindowDimensions()` reaches 99.8%
source-shape and 89.4% strict fuzzy; it remains non-exact. The
production-method UI smoke checks button positions and published
coordinates. Native `gof2`, all 206 ARM units and 37 verifier tests
pass (one skipped). See
[the evidence](LAYOUT_RESET_WINDOW_DIMENSIONS_ARM_2026-10-13.md).

## Layout one-argument window wrapper (2026-10-13)

The Android-backed `Layout::drawWindow(String)` forwards
`drawBG=false`, correcting a behavior difference. Focused
source-shape and strict fuzzy reach 100.0%, adding a linked-exact
(not byte-exact) result. The production-method UI smoke checks the
background-free frame, and native `gof2`, all 206 ARM units and 37
verifier tests pass (one skipped). See
[the evidence](LAYOUT_ONE_ARG_WINDOW_ARM_2026-10-13.md).

## Layout full-screen window wrapper (2026-10-13)

The Android-backed `Layout::drawWindow(String,bool)` reaches 100.0%
source-shape and 100.0% strict fuzzy, adding a linked-exact (not
byte-exact) result. The production-method UI smoke checks both
background paths and the original double top-inset adjustment. Native
`gof2`, all 206 ARM units and 37 verifier tests pass (one skipped).
This is not an ARM32 runtime or visual rendering test. See
[the evidence](LAYOUT_WINDOW_WRAPPER_ARM_2026-10-13.md).

## Layout header (2026-10-13)

Android-backed `Layout::drawHeader(String,bool)` reaches 99.8%
source-shape and 97.7% strict fuzzy, but remains non-exact. The
expanded production-method UI smoke checks image IDs, cap placement,
title text and help-button branches. Native `gof2`, all 206 ARM units
and 37 verifier tests pass (one skipped). See
[the evidence](LAYOUT_DRAW_HEADER_ARM_2026-10-13.md).

## Layout framed window (2026-10-13)

The Android-backed six-argument `Layout::drawWindow` reaches 100.0%
source-shape and strict fuzzy, but remains non-exact. The expanded
TouchButton/Layout smoke checks background on/off, outline and image
placement, and color restoration. Native `gof2`, all 206 ARM units
and 37 verifier tests pass (one skipped). The original title
condition remains unusual; see [the audit](LAYOUT_DRAW_WINDOW_ARM_2026-10-13.md).

## Layout empty footer (2026-10-13)

Android-backed `Layout::drawEmptyFooter(bool)` reaches 100.0%
source-shape and strict fuzzy, with a linked-exact (not byte-exact)
result. The TouchButton/Layout smoke exercises both `showBack` paths,
and native `gof2`, all 206 ARM units and 37 verifier tests pass (one
skipped). This is not an ARM32 visual rendering test. See
[the evidence](LAYOUT_EMPTY_FOOTER_ARM_2026-10-13.md).

## Layout tip initialization (2026-10-13)

The Android-backed `Layout::initTip()` recovery reaches 100.0%
source-shape and 97.0% strict fuzzy but remains non-exact. Its
production-method host smoke checks two calls, the text ID, font,
wrap width and old-array release. Native `gof2`, all 206 ARM units and
37 verifier tests pass (one skipped). See
[the evidence](LAYOUT_INIT_TIP_ARM_2026-10-13.md).

## Layout tip (2026-10-13)

The Android-backed `Layout::drawTip()` recovery is 100.0% source-shape
and 100.0% strict fuzzy, with a linked-exact (not byte-exact) result.
All 206 ARM units, native `gof2`, the TouchButton/Layout host smoke and
37 verifier tests pass (one skipped). This verifies the recovered ARM
form, not rendering on an ARM device. See [the evidence](LAYOUT_TIP_ARM_2026-10-13.md).

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

## Focused ABI/Lifetime Smoke

```powershell
C:\msys64\ucrt64\bin\cmake.exe -S . -B cmake-build-ucrt -G Ninja `
  -DGOF2_BUILD_ABI_RUNTIME_SMOKE=ON
C:\msys64\ucrt64\bin\cmake.exe --build cmake-build-ucrt `
  --target gof2_abi_runtime_smoke
.\cmake-build-ucrt\gof2_abi_runtime_smoke.exe
```

Expected output: `GOF2 native ABI/lifetime smoke: OK`. This exercises real
String/Array paths and probe-module vtable dispatch on the host. It is not
an ARM32 runtime pass; see [the ABI gate](ABI_RUNTIME_GATE_2026-09-27.md)
for the original vtable relocations and remaining ownership risks.

## ImageFactory Portrait Host Smoke

For the Android-backed portrait table and dispatch path:

```powershell
python tools/run_image_factory_portrait_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `ImageFactory portrait load/draw smoke: OK`. The runner
executes extracted production factory/part bodies with fixture canvas,
random, sprite, and reload services; it removes its temporary executable.
It does not validate original assets or OpenGL. See
[the portrait audit](IMAGE_FACTORY_PORTRAITS_ARM_2026-10-13.md).

For the adjacent production Sprite/reload path:

```powershell
python tools/run_sprite_reload_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `Sprite reload/draw host smoke: OK`. It distinguishes
the original `Globals::Canvas` from `PaintCanvas::gCanvas`, checks
asymmetric scaling and frame-array destruction calls, and removes its temporary
executable. It does not render real assets; see
[the Sprite audit](SPRITE_CANVAS_SCALE_ARM_2026-10-13.md).

## TouchButton/Layout Host Smoke

```powershell
python tools/run_touchbutton_touch_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `TouchButton/Layout host smoke: OK`. The runner extracts the
unchanged production touch, draw, pattern, fade, footer, box, border, metric, and mask
method bodies, executes them with fixture canvas/sound/status services,
and removes its temporary executable.
This checks selected geometry and call behavior, not a full game UI or ARM
runtime. See [the UI host-smoke audit](TOUCHBUTTON_UI_HOST_SMOKE_ARM_2026-10-12.md).

```powershell
python tools/run_layout_scroll_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `Layout scrollbar host smoke: OK`. This extracts the
production scrollbar method and checks track/cap geometry, colors and
both thumb branches with fixture canvas services. It does not render
original textures or run on ARM32. See
[the scrollbar audit](LAYOUT_SCROLLBAR_ARM_2026-10-13.md).

```powershell
python tools/run_player_ego_throttle_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `PlayerEgo throttle host smoke: OK`. This extracts the
production throttle method and checks its color, region/text calls, and
inactive branch with fixture services; it does not run original assets or
the full game. See [the region-call audit](REGION_CALL_THROTTLE_ARM_2026-10-12.md).

```powershell
python tools/run_status_standing_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow standing host smoke: OK`. This extracts
only the production standing-bar block, checking geometry and region/image
calls with fixtures; it does not execute the whole window. See
[the standing-bar audit](STATUS_STANDING_REGION_ARM_2026-10-12.md).

```powershell
python tools/run_status_draw_prefix_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow draw-prefix host smoke: OK`. This extracts
the production draw setup and checks the shared canvas/layout, scroll-bar,
and iPad width behavior with fixtures; it does not render the full window.
See [the draw-globals audit](STATUSWINDOW_DRAW_GLOBALS_ARM_2026-10-13.md).

```powershell
python tools/run_status_draw_info_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow info-card host smoke: OK`. This extracts the
production credits, level, time and ship-text block and checks strings and
positions with fixtures; it does not load original assets or draw the
statistics table. See [the info-card audit](STATUSWINDOW_DRAW_INFO_CARD_ARM_2026-10-13.md).

```powershell
python tools/run_status_draw_stats_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow stats-table host smoke: OK`. This extracts
the production standing-section boxes and statistics table, checking the
two-column labels, values, positions and language-9 separator with
fixtures; it does not load original assets or render a complete UI frame. See
[the statistics-table audit](STATUSWINDOW_DRAW_STATS_TABLE_ARM_2026-10-13.md).

```powershell
python tools/run_status_draw_medals_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow medal-grid host smoke: OK`. This extracts
the production medal grid and detail pane, checking iPad/phone placement,
visibility, header and detail-text coordinates with fixture services. It
does not execute the full window. See
[the medal-grid audit](STATUSWINDOW_DRAW_MEDALS_ARM_2026-10-13.md).

```powershell
python tools/run_status_ctor_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow constructor host smoke: OK`. This runs
the production constructor with fixture services for phone and iPad tabs,
medals, images and dimensions; it does not load original assets or run
on ARM32. See [the constructor audit](STATUSWINDOW_CTOR_GLOBALS_ARM_2026-10-13.md).

```powershell
python tools/run_status_touch_end_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow touch-end host smoke: OK`. This runs the
production touch begin, move and end methods with fixture services for
scroll, tabs, medals and help; it does not use the original assets or
full `Array` ABI. See [the touch-end audit](STATUSWINDOW_TOUCH_END_ARM_2026-10-13.md)
and [the touch-globals follow-up](STATUSWINDOW_TOUCH_GLOBALS_ARM_2026-10-13.md).

```powershell
python tools/run_status_dtor_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow destructor host smoke: OK`. This runs
the production destructor with tracked fixture objects and arrays; it
does not test the original ARM32 allocator. See
[the destructor audit](STATUSWINDOW_DTOR_ARM_2026-10-13.md).

```powershell
python tools/run_status_medal_hint_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow medal-hint host smoke: OK`. This extracts the
production hint method and checks branch-specific separators and text IDs
with fixtures; it does not use original text assets or execute touch input.
See [the medal-hint audit](STATUSWINDOW_MEDAL_HINT_ARM_2026-10-13.md).

```powershell
python tools/run_status_reinit_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `StatusWindow reInit host smoke: OK`. This extracts the
production method and checks standing queries, rank-image selection and
size reads with fixture services. It does not load original image assets
or execute an ARM32 UI. See [the reInit audit](STATUSWINDOW_REINIT_ARM_2026-10-13.md).

```powershell
python tools/run_paintcanvas_region_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `PaintCanvas region host smoke: OK`. This extracts the
production ten-argument region method, checking its crop geometry,
rotation/pivot matrix, and draw-call order with fixture services. It
does not execute OpenGL or the full game. See
[the transform audit](PAINTCANVAS_REGION_TRANSFORM_ARM_2026-10-12.md).

```powershell
python tools/run_native_math_link_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `Matrix constructor/product host smoke: OK`. The runner
compiles the production math units, checks the native default matrix,
matrix product, and pre-existing assignment/operators, and removes its
temporary executable. This does not establish ARM runtime or the full
matrix owner layout. See [the matrix audit](MATRIX_CTOR_PRODUCT_ARM_2026-10-12.md).

```powershell
python tools/run_material_draw_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `MaterialDraw host smoke: OK`. The runner executes the
production batch-draw method with fixture math/render services and
removes its temporary executable. It checks routing, colors, ambient
restoration and queue reset, not real OpenGL or ARM runtime. See
[the batch-draw audit](MATERIAL_DRAW_MATRIX_LIGHT_ARM_2026-10-13.md).

```powershell
python tools/run_sprite_system_draw_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `SpriteSystemDraw host smoke: OK`. The runner checks
the production sprite quad and matrix-queue path, including the real
cached-array template's first capacity growth, with fixture math/render
services. It removes its temporary executable. It is not a full GL,
sanitizer, or ARM32 runtime test. See
[the sprite queue audit](SPRITE_SYSTEM_DRAW_MATRIX_QUEUE_ARM_2026-10-13.md).

```powershell
python tools/run_material_lifecycle_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `Material lifecycle host smoke: OK`. This compiles
the real `Material` and math units, checks the default ambient sentinel,
constructor fields, and two rounds of Array reuse, then removes its
temporary executable. It does not execute a full GL frame or ARM32
runtime. See [the ambient audit](MATERIAL_DEFAULT_AMBIENT_ARM_2026-10-13.md).

```powershell
python tools/run_mesh_clone_material_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `MeshCloneMaterial host smoke: OK`. This compiles
the production clone method against a fixture canvas and real
`Material`/`Mesh` objects, then removes the temporary executable. It
checks two clones and the invalid-index path, not a full render frame
or ARM32 runtime. See [the clone audit](MESH_CLONE_MATERIAL_ARM_2026-10-13.md).

```powershell
python tools/run_material_create_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Expected output: `MaterialCreate host smoke: OK`. This extracts both
production overloads with real `Material` and Array code, fixtures
resource/texture services, and removes the temporary executable. It
does not run the real texture loader or ARM32 game. See
[the material-create audit](MATERIAL_CREATE_ARM_2026-10-13.md).

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
  mangles differently. The original also contains statically linked C++ runtime,
  zip, compression and crypto functions; their missing names are not necessarily
  missing game code.
  The full list is written to `cmake-build-match/verify/missing.txt`.
- **`wrong type` (`missing_wrong_type`)** splits that *missing* pile into the two cases that
  look identical to an exact-name match but need very different fixes. We demangle every missing
  original and every symbol our build defines, strip the parameter list to a *qualified name*
  (`AEFile::Open`), and group by it. A missing original whose qualified name we **do** define is
  reported as *implemented under a different signature* — the body exists, only a parameter or
  qualifier type is off (`String` by-value vs `String&`, a global enum that should be nested,
  `void*` vs `char const*`), so the mangled name differs and the exact-name match skipped it.
  Inspect these before retyping: constructor/destructor C1/C2 and D1/D2 aliases
  can live in separate `.text.*` COMDAT sections. The verifier keys aliases by
  section and address. The rest are absent from the current object set, not
  necessarily absent game implementations. Both sides'
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

Latest TargetFollowCamera override (2026-10-02): the fixed 40 unique bodies
meet **99.965% unweighted / 99.269% instruction-weighted** source-shape;
42 symbols including aliases meet 99.967% / 99.340%. Focused exact counts
are 32 linked / 23 byte. Both bases were remeasured with tested NEON literal
and COMDAT alias corrections; the original 27/19 and every full-corpus exact
pair are preserved. Full ARM wrong-type, 204 TUs, native build and focused host
smoke pass. This does not claim an ARM32 runtime. See
[the complete gate report](TARGETFOLLOWCAMERA_ARM_SOURCE_SHAPE_2026-10-02.md).

After rebuilding current ARM objects with NDK r18b, focused reports measured:

- `PlayerTurret`: 21 symbols, 95.5% source-shape, 12 linked-exact, four byte-exact.
- `TargetFollowCamera`: 42 symbols, 92.5% source-shape, 27 linked-exact, 19 byte-exact.
- `Gun`: 28 symbols, 72.5% source-shape, 14 linked-exact, 13 byte-exact.
- `CameraC2`: 100% strict/source and linked-exact.
- `ListItemWindow::update`: strict/source `71.7/93.6% -> 84.8/96.9%`.

These are scoped family results, not a replacement whole-corpus snapshot. ARM
layout constants independently confirm `PlayerTurret=0x168`, `Camera=0x5c`,
and the `TargetFollowCamera` tail offsets `0x120/0x130/0x138/0x13c`.
