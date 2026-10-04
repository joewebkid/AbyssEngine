# Status agent-array pointer ABI (2026-10-04)

Scope: `Status::getAgents()` and its sole call in
`Generator::createAgents(Station*)`. Android 2.0.16
`libgof2hdaa.so:0x0ac7a6..0x0ac7aa` loads the pointer stored at
`Status+0x20` and returns it unchanged in `r0`. The recovered `Status`
layout already types that field as `Array<Agent *> *`, but its getter used
`int` and the caller converted that result through `long` before treating it
as a pointer. On the Windows UCRT64 host both `int` and `long` are 32-bit,
so a 64-bit address can be truncated before dereference.

The getter now returns `Array<Agent *> *` directly and the sole caller uses
that type directly. This removes the host pointer-width loss without
changing Android ARMv7's 32-bit register return, symbol name or layout.

Focused ARM comparison is unchanged: `Status::getAgents()` remains 100%
source-shape, strict fuzzy and byte-exact; `Generator::createAgents` remains
93.6% source-shape and 40.6% strict fuzzy, not exact. Native `gof2` and all
206 ARM translation units build. The complete corpus exact-set comparison is
unchanged at 4,322/4,524 unique originals over 4,675 rows: 202 absent,
zero wrong-signature, zero skipped, 90.09% mean source-shape, 77.22% strict,
2,164 linked-exact and 977 raw-byte-exact. Both exact `(unit, symbol)` sets
show zero losses and zero gains. Local focused reports are
`_work/blueprint-isunlocked/status_getagents_{before,after}.json` and
`generator_createagents_{before,after}.json`; the complete report is
`status_getagents_full.json` in the same ignored directory.

No integrated game runtime was exercised. Other pointer-to-`int`/`long`
paths exist elsewhere and are not corrected by this narrow change; they
should be audited separately with their own callers and ARM evidence.
