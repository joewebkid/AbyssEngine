# MiningGame outcomes and teardown audit (2026-10-14)

## Evidence

Android 2.0.16 `MiningGame::update(int)` at ELF `0x11ec00` and
`MiningGame::~MiningGame()` at `0x11eb74` are recorded in the shared
IDA export `analysis/gof2_libgof2hdaa_full_ida.c` (lines
156966–157145 and 156881–156895). The outside-layer branch caps the
loss timer at 2500, clears ore and `Status+0x124`, sets the loss byte
and returns zero. A completed layer increments `Status+0x124` if the
medal is not owned, sends a HUD event at 10%-step thresholds of at
least 30%, and sets only the byte at `Status+0x128` on reaching the
goal. The destructor destroys/deallocates only the drill `Sprite`
and nulls its pointer; it does not free the three `MarqueeImage`
objects created by the constructor. The current source matches those
observed branches. Ownership of the marquees elsewhere is not proven.

The production-method host smoke now checks the 2500/2501 loss
boundary, ore/status reset, 30% and 100% HUD events, and preservation
of the upper bytes in `Status::field_128`. The constructor smoke runs
the production destructor and confirms two drill sprites are
destroyed while no marquee destructor is called. These are isolated
host fixtures; they do not prove a full interactive mining session,
ARM32 runtime behavior or leak ownership.

No production source changed in this audit. Focused ARM comparison
remains 95.1% source-shape / 82.1% strict fuzzy for `update` (357
generated versus 376 target instructions, not exact); the destructor
aliases remain 100% source-shape / 100% strict fuzzy and linked-exact,
but not byte-exact. Native `gof2`, all 206 ARM units, the three mining
smokes and 37 verifier tests pass (one skipped). Full corpus:
4,323/4,524 original functions covered, 93.08% average source-shape,
81.50% strict fuzzy, 2,403 linked-exact and 1,008 byte-exact pairs;
the exact and 123-symbol extra sets are unchanged from the preceding
checkpoint.

The next blocking gap is `PlayerEgo::stopMining` at Android ELF
`0xa17b4` (IDA lines 86967–87049). The current implementation only
deletes `MiningGame` and clears its pointer; the ARM body also grants
ore/core cargo, updates collection/status flags, restores ship state,
redocks and resumes/stops audio. Therefore a full native mining path
would currently be behaviorally incomplete even though `MiningGame`
itself passes these fixtures. Next: recover that entire handoff as a
separate bounded package, then exercise the complete path and audit
the lifetime of the marquees. The existing focused tests cannot
establish either behavior.
