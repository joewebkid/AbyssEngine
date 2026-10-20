# Globals ship-group resource tables and Pan loader — 2026-10-20

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
contains `Globals::getShipGroup(int,int,bool)` at Thumb `0xe4e14–0xe5554`.
Its PC-relative loads address five contiguous `.rodata` tables:

| Table | Address | Entries | Use |
| --- | --- | ---: | --- |
| base resource | `0x1fe3a0` | 64 × 16-bit | `e510a` |
| secondary mesh | `0x1fe420` | 64 × 16-bit | `e5122` |
| additive mesh | `0x1fe4a0` | 64 × 16-bit | `e518e` |
| LOD meshes | `0x1fe520` | 192 × 32-bit | `e5256` |
| LOD child meshes | `0x1fe820` | 192 × 32-bit | `e5486` |

The previous C++ arrays had one zero element each, so normal ship indexes
read past their bounds. The complete five arrays are now recovered. A verifier
test checks all **576 values** against the original ELF bytes, including
`0xffff` sentinels and the first ship's IDs: base **17000**, additive
**18700**, engine glow **17900**, LOD **17100/17101**. These IDs resolve to
five original Midorian AEM files in the registered resource catalog.

The same original body passes the additive ID in `r2` and `false` in `r3`
at `0xe51a6–0xe51ae`. It passes `17900 + kind` on the enabled flag path
(`0xe51d6`, `0xe5228`) or `18000 + kind` on the other path (`0xe5224`,
`0xe5228`), also with `false` at `0xe522e`. The old C++ passed mesh ID zero
in all three places and converted the intended ID to a boolean flag. The
production calls now match those IDs and flag. A focused extracted-branch
test checks kinds 0, 4, 38, 39, 41 and 63 at `-O0/-O2`, including the
special no-engine-mesh cases 39 and 41.

The continuous Pan original-asset gate now calls the **production**
`getShipGroup(0,0,true)` after the completed `PlayerEgo` constructor and
loads five ship AEM meshes on Windows SwiftShader and Android ARMv7 at
`-O0/-O2`. It checks **73 meshes**, **77 transforms** and **37/37** closed
original-file operations. Native `gof2`, **208/208** ARM translation units
and **93** verifier tests pass (one existing skip). Its platform canvas,
material creation and nonselected branches
still use bounded test services. It does not execute `PlayerEgo::setShip`,
material-state propagation or the rest of `Level::init`; those remain the
next integration step.

Focused `getShipGroup` source-shape/strict fuzzy moves from
**82.6%/18.7%** to **83.0%/7.7%**; generated/original instruction counts
move from **548/694** to **559/694**. This is a source-backed behavior fix,
not an exact-match gain; the lower strict fuzzy score is recorded rather
than changing correct IDs for the metric. The `Globals` unit retains its
**17 linked-exact** and **1 byte-exact** function sets.

The final full ARM corpus compares **4,323/4,524** original functions
(**4,735** pairs; **201** absent, **0** skipped), with **93.95%** mean
source-shape and **83.24%** mean strict fuzzy. It retains **2,551
linked-exact** and **1,079 byte-exact** functions, with no losses in either
exact-symbol set against the preceding `PlayerEgo` baseline. The report is
`ship-group-loader-full.json` (SHA-256
`8b8bfabfc34b38878fa4f5c877c1e698c3340a82a8082febe5e5dab752f8b9e1`).
