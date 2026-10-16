# Particle emitter argument ABI (2026-10-16)

The Android ARM `IParticleSystem::emit(int)` call sites at `0x1825b0`
and `0x182638` pass `r2` from `s21`. That value starts at the particle
definition's `+0x14` lifetime field and may receive the `+0x18` random
delta. The later stack arguments for the initial and final sizes come
from `+0x1c` and `+0x20`, with separate random draws. The call sites
also pass UV values in the recovered `u0, v0, u1, v1` order.

The two Android ARM `IParticleSystem::emitManual` call sites at
`0x182a74` and `0x182aba` likewise pass the lifetime in `r2` and the
two sizes on the stack. The first call adds the optional random lifetime
delta; the second uses the selected lifetime unchanged. This confirms
the caller-side mapping after the sprite callee's `p2` size correction:
`p2` is lifetime, while `p9`/`p10` are the defined start/end sizes.
The original sprite callee's use of `p2` for its initial SetSize remains
counterintuitive but is directly evidenced by its ARM instructions; no
caller argument was renamed or changed to make the behavior look nicer.

The extracted production `emit` and `emitManual` bodies pass host
smoke tests using distinct lifetime/size values, randomized deltas and
UV permutations. Both fixtures now implement the corrected `void init`
virtual interface. `run_particle_emit_smoke.py` passes at `-O0/-O2`,
and `run_particle_control_smoke.py` passes at `-O0`. Native `gof2` and
40 verifier unit tests pass (one pre-existing skip). No production C++
changed in this package, so the previous 208/208 ARM build and full
corpus still apply: 4,323/4,524 compared, 201 missing, 123 extras,
93.68% average source-shape, 82.78% strict fuzzy, 2,515 linked-exact
and 1,075 byte-exact. In that corpus `emit` is 88.8%/19.9% and
`emitManual` is 99.6%/78.0% source-shape/strict fuzzy; neither is exact.

This checks the static Android call ABI and host behavior, not actual
ARM32 runtime, GPU output, or the reason the original sprite renderer
uses lifetime as its initial size.
