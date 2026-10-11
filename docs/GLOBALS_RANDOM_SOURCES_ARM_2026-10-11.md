# Globals random station and drinks system (2026-10-11)

Android 2.0.16 ARM `Globals::getRandomStation` at `0xe4af0` constructs a
one-byte `FileRead`, draws `AERandom::nextInt(135)` from `Globals::rnd`,
loads that station, destroys the reader, and returns the station. The
recovered code instead dereferenced a null RNG placeholder. It now uses
the real `FileRead` class and `Globals::rnd`; the incomplete local
`FileRead` declaration was removed.

ARM `Globals::getRandomSystemForDrinks` at `0xe4b38` draws
`AERandom::nextInt(22)` from the same RNG and returns
`Galaxy::getSystem(Globals::galaxy, index)`. The recovered function had
the wrong `void` return type and wrote through null placeholders; both
behavior and C++ return type are now restored.

Focused ARM verification builds **206/206** translation units.
`getRandomStation` rises from **75.6%** source-shape / **72.7%** strict
to **100%/100% linked-exact**. `getRandomSystemForDrinks` rises from
**51.1%/8.7%** to **94.3%/45.7%** and is not exact. Native `gof2` and
36 verifier tests pass (one skipped). Full-corpus results are in
`PROJECT_STATUS.md`.

This restored two dependencies of `NewsTicker::replaceTokens`, not that
function itself at the time. Subsequent audits recovered random-name
loading and the token function; station/data-file behavior has still not
been tested inside a running game.
