# PlayerEgo collision branches (Android ARM, 2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerEgo::calcCollision(Array<KIPlayer*>*)` at `0x9f0cc–0x9f472`.
The ELF disassembly was cross-checked with the live REA/IDA function
dossier. The old C++ body dispatched into `PE_cc_wormhole`,
`PE_cc_obstacle`, `PE_cc_destructible`, `hitCamera_`, and `Vec_length`;
these had declarations but no implementations in the project. A native
static-library build alone did not prove this method could link into an
executable path.

The recovered method directly handles all three collision branches:

- Wormhole type `0x4262`: exclude shrinking holes and active mining,
  compute the distance using the scratch vectors at `+0xec/+0xe0`, stop
  sound event 34 at 40,000 or farther, otherwise play/update its 3D
  position, pull the geometry by `(40000 - int(distance)) >> 8` along the
  normalized direction, shake the camera, set the close-entry flag below
  1,000, and propagate the Canvas local-matrix product to `Player`.
- Non-stealing obstacle: require visibility, project the current geometry
  position onto the surface, set the position and propagate the matrix
  when nonzero, set the first-candidate station flag from KIPlayer byte
  `+0x71`, and call `hitCamera` even when projection is zero.
- Destructible object: skip dying/dead or the active docking target,
  obtain the collision vector through the native virtual slot, apply
  hit vector and bomb force `0.2` while boosting or `0.1` otherwise,
  damage it by 9,999 and the Ego by 20, optionally increase Player
  `flShake` for volatile goods, then shake the camera.

The method reads a fresh geometry position for each virtual projection,
and checks the low byte of `dockingState`, as the ARM instructions do.
The pre-collision station alarm also uses KIPlayer `+0x71`, not the
unrelated `+0x69` flag. The production-method smoke covers null and
docked guards, station alarm, invisible and visible obstacles, current
position re-read, destructible hit/docking/boost branches, and near/far
wormhole sound and pull at `-O0/-O2`. Native UCRT64 `gof2`, all 208
NDK r18b ARM units and 93 verifier tests pass (one existing skip).
Focused source-shape/strict fuzzy changes **47.8%/23.7% → 96.6%/61.9%**;
the method has **347/344** target/base instructions and remains nonexact.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,736** pairs. Mean source-shape/strict fuzzy changes from
**94.08%/83.40% → 94.09%/83.41%**. It retains **2,557 linked-exact**
and **1,082 byte-exact** pairs, with zero exact-pair losses, skipped or
wrong-type symbols; 201 original functions remain missing. Only
`calcCollision` changed score among existing pairs. Full report:
`ego-collision-final-full.json` (SHA-256
`ca2e41762900dbbccbbc489055919a5d353ba3d920003c0c6b2793ff7ac761ff`).

Six of 29 older PlayerEgo smoke scripts fail outside the changed method:
three have incomplete fixture types and three fail existing assertions in
other method paths; 23 pass. The new collision smoke passes.
The six failures are not counted as a green full-class smoke gate. A
complete game frame with real collision volumes, FMOD and Canvas output
is still unverified. Next priority is an end-to-end collision call-path
gate on original assets/ARM before asserting playable flight behavior.
