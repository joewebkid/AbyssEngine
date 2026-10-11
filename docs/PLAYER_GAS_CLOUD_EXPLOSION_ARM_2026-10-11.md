# PlayerGasCloud explosion — Android ARM (2026-10-11)

The Android constructor at `0x1763cc` and render at `0x176e18` obtain
their canvas from `Globals::Canvas`. The explosion at `0x176660`
also uses that canvas, `Globals::items`, and `Globals::rnd`, rather than
translation-unit-local null pointer aliases. The ARM call at `0x176698`
deactivates the player before marking the cloud exploded. Its item
lookup calls `getAttribute(56)`; all five RNG calls are bounded
`nextInt`, with bounds `10000` (three times), `200`, and `14000`.

The literal pool at `0x176940–0x17694c` supplies `130.0`, `100.0`,
`5000.0`, and `200.0`. They control shard count, attribute scaling,
position spread, and lifetime. The recovered source had zero values
in each place, including division by zero. These exact constants and
dependencies are now in the source; the existing branch and vector
construction were preserved.

Native `gof2` and 206/206 ARM translation units build. All 36 verifier
tests pass (one skipped). Source-shape of `explode` improves from
49.4% to 95.8%, strict fuzzy from 28.0% to 59.2%. The two constructor
aliases improve from 91.3% to 93.5% source-shape each; render from
90.9% to 93.6%. Across these four comparisons, source-shape rises
from 80.7% to 94.1% and strict fuzzy from 60.25% to 70.9%.

Full ARM corpus: 4,323/4,524 original functions compared, 92.13%
average source-shape, 80.37% strict fuzzy, 2,343 linked-exact and
1,008 byte-exact, zero skipped units and zero wrong-type symbols.
No previously exact pair regressed. Five `ArrayAdd` template symbols
newly emitted by this unit account for the linked-exact increase;
none of the old comparisons newly became exact. Running-game effects
remain untested.

`PlayerGasCloud::update` is still incomplete: null canvas, sound,
item-table, and campaign aliases; zero movement and fade parameters;
and unverified cargo handling. It must be audited separately before
claiming gas-cloud gameplay is restored.
