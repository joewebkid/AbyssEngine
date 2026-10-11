# PlayerCreature layout, stats, and update wiring — Android ARM (2026-10-11)

The recovered class had two duplicate derived fields, `renderGeometry`
and `creatureType`, that the original code accesses as inherited
`KIPlayer::crateGeometry` (offset `0x78`) and `KIPlayer::type` (offset
`0xac`). Their presence shifted every actual derived field by eight
bytes. They were removed; ARM-only compile-time assertions now pin
`lastElapsed` at `0x124`, byte flags `raging`/`hooked`/`caught` at
`0x128`/`0x129`/`0x12a`, `rageScale` at `0x12c`, `maxEndurance` at
`0x134`, `rageMatrix` at `0x144`, and the class size at `0x180`.
The original's 16-bit writes set or clear the first two flags together;
individual update writes preserve the neighboring flag.

Android `PlayerCreature::getWeight`, `rage`, and the constructor all
index the same four-integer table at ELF `0x201dd0`: `{1, 1, 1, 0}`.
The three zero-filled 16-element arrays were replaced with one
matching constant table. Its generated ARM bytes and three relocations
were checked against the original. The constructor now passes `kind`,
group `-1`, and inactive initialization to `KIPlayer`, as in Android;
`reset` reactivates the player. Rendering uses the inherited geometry
and `KIPlayer::render` instead of an undefined `PlayerJunk_render` call.

The update path now uses `Globals::rnd` and `Globals::sound` rather
than null aliases, and the death effect calls the existing
`ParticleSystemManager::emitManual` overload with original lifetime
`-1.0f`, replacing an undefined wrapper call. Native `gof2` and all
206 ARM units build. The 36 verifier tests pass (one skipped).
The 20-function focused family rises from 85.5% to 96.1% mean
source-shape and from 56.7% to 90.6% strict fuzzy. Its exact counts
rise from five to thirteen linked-exact and from two to eight byte-exact.

Runtime creature spawning, rage, audio, and death effects remain
unverified in a running game. The native `KIPlayer` constructor still
ignores its `active` argument, a separate base-class limitation;
this change restores the call's source arguments and the explicit
reactivation in `reset` without altering that broader constructor.

The complete ARM corpus compares 4,323/4,524 original functions at
92.09% mean source-shape and 80.27% mean strict fuzzy. Exact totals
rise to 2,333 linked-exact and 1,003 byte-exact (eight and six new
pairs). A pairwise comparison with the previous full report found no
lost exact pair, skipped unit, or wrong-type symbol.
