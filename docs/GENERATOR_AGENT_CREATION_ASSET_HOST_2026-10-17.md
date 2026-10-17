# Generator agent creation host gate (2026-10-17)

The real-asset host fixture now extracts and executes the production
`Generator::createAgent` body, plus production `Globals::getRandomName`,
`FileRead::loadNamesBinary` and `ImageFactory::createChar(bool, int)`.
Sixty-four agents are generated at catalog station 41 with real
`AERandom`, item catalog, station and system state. Both host `-O0`
and `-O2` runs check station/system IDs, valid race and offer values,
portrait rows and per-part choice bounds. The deterministic run also
visits item-sale and wingman offers, checking sale data and costs.
The earlier 68/68 file open/close balance remains green.

The host-only `ImageFactory` constructor intentionally omits rendering
setup; the tested portrait-description method is the unchanged
production body and uses no instance fields. Two resource modes pass:
the primary loose ARM catalog lacks `names_*.bin` and exercises the
missing-name path; an optional overlay adds 13 name binaries from the
project's extracted Java reference, exercising nonempty name loading
and its RNG path. Those supplementary binaries have not yet been
proven byte-identical to Android 2.0.16, so exact name content remains
cross-source evidence rather than an ARM claim. Production
`Generator::createAgents` is still substituted in the station
transition, so agent counts, retained story agents and mission offers
are also **not** verified by this package. No production source changed.

Native `gof2` builds; the verifier unit suite runs 40 tests with one
existing skip. Because only host tests changed, the last full ARM
report (`A:/Galaxy on Fire new version/_work/generator_market_full_20261017.json`)
still applies: 208/208 translation units, 4,323/4,524 original
functions, 93.78% mean source-shape, 82.95% strict fuzzy, 2,527
linked-exact and 1,076 byte-exact, with no exact-pair losses.

Next: establish provenance for the name binaries, then connect
`createAgents` and mission creation in a bounded station transition
with explicit ownership checks. Only after that enter `Level::init`
and a controlled flight scene.
