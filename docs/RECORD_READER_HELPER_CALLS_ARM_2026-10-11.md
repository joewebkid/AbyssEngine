# Record reader helper calls — Android ARM (2026-10-11)

In Android `RecordHandler::recordStoreRead` at `0xcbd34`, a saved
station index is passed to `Galaxy::getStation`, standing state is
constructed with `Standing::Standing` and assigned with
`Standing::setStandings`, and product/wingman names are copied with
`String(const String&, false)`. The native source instead declared
four helper functions with no definitions. These call sites now use
the existing class methods directly. The temporary product-name
copy no longer requires an unowned heap object.

The native `RecordHandler.cpp` object no longer lists the four helper
symbols as unresolved; `libgof2.a` defines the replacement methods.
Native `gof2` and all 206 ARM translation units build; 36 verifier
tests pass (one skipped). Focused `recordStoreRead` source-shape
improves from 91.6% to 91.8%, strict fuzzy from 16.9% to 23.3%.
Full ARM corpus: 4,323/4,524 original functions compared, 92.18%
average source-shape, 80.38% strict fuzzy, 2,350 linked-exact,
1,008 byte-exact, zero skipped units and zero wrong-type symbols.
Every previously exact pair remains exact.

The `gof2` target is a static library, not a final game executable.
Its `RecordHandler` object still has declared-only `AEFile_Read*` and
`AEFile_Write*` references, so full linking and running-game
save/reload are not yet proven. Those wrappers are the next priority.
