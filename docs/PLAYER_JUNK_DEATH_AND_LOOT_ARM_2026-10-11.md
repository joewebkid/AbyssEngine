# PlayerJunk death and loot — Android ARM (2026-10-11)

The original Android `PlayerJunk::reset` at `0x15e77c` calls
`KIPlayer::reset`, stores state zero at `0x88`, then tail-calls
`KIPlayer::setActive(true)`. The recovered source had instead called
`setVisible(true)`. Both the old and corrected forms score
linked-exact in the focused verifier, so that score alone did not
expose the wrong call target; the ARM call target is the evidence.

The original `update` at `0x15e798` obtains sound and RNG from their
shared global pointers. On death it plays sound `0x16`, drops loot
when `nextInt(100) <= 9`, adds item `99` and quantity
`nextInt(10) + 1` to the cargo array, creates crate type `3`, and
sets `hasCargo` again after crate creation. Without a drop it
deactivates the player and clears a radar reference to this object.
The death particle effect passes `-1.0f`, confirmed by the literal
`00 00 80 bf` at `0x15e8bc`. The source had used null sound/RNG
aliases and `0.0f`; these have been corrected without changing the
branching structure.

Native `gof2` and all 206 ARM translation units build. The verifier's
36 tests pass (one skipped). Focused `update` source-shape/strict fuzzy
improves from 88.1%/70.4% to 93.6%/72.9%. The full 12-comparison
family averages 99.5% source-shape and 94.3% strict fuzzy, with
10 linked-exact and 2 byte-exact. Full ARM corpus: 4,323/4,524
original functions compared, 92.11% average source-shape, 80.34%
strict fuzzy, 2,338 linked-exact, 1,008 byte-exact, zero skipped
units and zero wrong-type symbols. Every previously exact pair remains
exact. These checks do not exercise a running game's loot, audio, or
particle behavior.

The next bounded priority is `PlayerGasCloud`, which still contains
null aliases and zero-valued parameters. Each candidate should be
confirmed from native evidence before replacement.
