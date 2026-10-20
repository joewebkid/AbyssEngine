# PlayerEgo ordinary ship binding — 2026-10-20

The Android 2.0.16 ARM `PlayerEgo::PlayerEgo(Player*)` constructor is at
Thumb `0x998cc` (symbol size 2,008 bytes). It forms `r4+0x1b0` at
`0x9993e` and writes 16 zero bytes there with `vst1.64 {d8-d9}, [fp]`
at `0x99a16`. The verified ARM32 layout maps those four words to
`cloak`, `tractorBeam`, `repairBeams`, and `asteroidTarget`. C++ already
initialized `cloak` but omitted the other three, leaving the gear-free
`setShip` path with indeterminate pointers. It now initializes all four.

The new `tools/run_player_ego_set_ship_pan_gate.py` extends the existing
Pan original-asset gate after the completed `PlayerEgo` constructor. It
extracts the full production `PlayerEgo::setShip`, checks those fields
before the call, then binds ship kind 0/race 0. At `-O0/-O2`, Windows
SwiftShader and Android ARMv7 complete this path: the player hull owns
the ship group transform, five original Midorian AEM meshes load in
catalog order, and all **37/37** original-file operations close.

The gate uses a bounded material service that gives the first ship mesh
material mode 7; this is a test value, not a claim about the original
material's mode. Tractor/repair-beam, emergency, cloak, supernova and
other ship-kind branches remain untested in this continuous asset path.
The method is called after the `Level::createPlayer` constructor boundary,
not yet as an uninterrupted call inside `Level::init`; that stage still
has not returned. Next: connect the production `Level::createPlayer`
continuation through `setShip`, then test equipment/material branches.

Focused constructor source-shape/strict fuzzy changes from
**83.6%/51.7%** to **83.9%/53.2%**. It remains non-exact. Native `gof2`
build, **208/208** ARM translation units and **93** verifier tests (one
existing skip) pass.

The final full ARM corpus compares **4,323/4,524** original functions
in **4,735** pairs, with **201** absent and **0** skipped units. Its mean
source-shape/strict fuzzy remains **93.95%/83.24%**. The **2,551
linked-exact** and **1,079 byte-exact** `(unit, symbol)` sets have zero
losses or gains against the preceding ship-group baseline. The report is
`set-ship-init-full.json` (SHA-256
`fcd8ec2b7f2e3ff9c694dcc94e95bc1ee314160cda570539f29d666a8430bd8c`).
