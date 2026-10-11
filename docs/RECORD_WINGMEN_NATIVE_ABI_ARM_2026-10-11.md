# Record wingmen pointer ABI — Android ARM (2026-10-11)

Android `GameRecord::load` transfers slot `0x14c` to the `Status`
wingmen slot at `0x24` and slot `0x158` to the five-integer stats
buffer at `Status + 0x28`. `Status::getWingmen` returns the array
pointer; `Status::setWingmen` duplicates its strings. The former
native C++ narrowed those addresses to `int32_t`, while `Level`
decoded the wingmen array through 32-bit pointer arithmetic.

`GameRecord` now uses typed ARM pointer fields at `0x14c` and `0x158`
and aligned native sidecars. `Status` preserves its 32-bit ARM layout
while holding full-width native pointers separately. Its writer,
reset path, setter, and `Level` consumers now use `Array<String *> *`
and `int *` directly. The `Status + 0x28` byte overlay remains in the
native raw layout because other code reads its flag byte.

`Globals::init` previously allocated a fixed `0x1f0` bytes for
`Status` and called a declared-only `Status_ctor`. It now allocates
the greater of `0x1f0` and `sizeof(Status)` and invokes the real
constructor. The ARM allocation width remains `0x1f0`; the native
object has enough room for its pointer members.

Native `gof2`, all 206 ARM translation units, and 36 verifier tests
pass (one skipped). Focused `Status::getWingmen` remains byte-exact.
`Status::setWingmen` source-shape moves from 93.3% to 91.2%, and
`Level::createWingmen` from 79.7% to 78.7%; neither was exact.
The full corpus has 4,713 comparisons of 4,323/4,524 original
functions, 92.24% average source-shape, 80.43% strict fuzzy,
2,353 linked-exact, and 1,008 byte-exact. There are zero skipped
units, zero wrong-type symbols, and no loss of any previously exact
pair. `Status::getWingmen` remains byte-exact.

The final game executable and a live save/reload round trip are
untested. Other `GameRecord` slots from `0x160` onward still hold
raw pointer-like values, and the existing `Status::setWingmen`
ownership pattern was not changed beyond removing pointer narrowing.
