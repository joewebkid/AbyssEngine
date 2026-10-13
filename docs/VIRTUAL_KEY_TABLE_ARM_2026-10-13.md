# Virtual-key table — Android ARM audit (2026-10-13)

Scope: the Android 2.0.16 `keys` data symbol and the functions in
`engine/input/VirtualInput`. The input dispatch audit is in
[VIRTUAL_INPUT_KEY_DISPATCH_ARM_2026-10-13.md](VIRTUAL_INPUT_KEY_DISPATCH_ARM_2026-10-13.md).
No iOS or PC implementation was used for this package.

The original ARM ELF contains `keys` at virtual address `0x214010` in
`.data`, size 4,224 bytes: 48 records of 88 bytes each. Its entries include
key names and codes, overlay handles, coordinate pointers, and predicate
function pointers. The prior local table was zero-filled and could not
provide those bindings. The recovered C++ table now exports the original
`keys` symbol and is used by the existing key dispatch. The X/Y globals
referenced by its touch-coordinate fields were changed from `float` to
`int`, matching the integer pixel reads in the Android input path; their
independent Z fields were not changed.

Validation used the local original ELF and the freshly built ARM
`VirtualInput.o`. A temporary checker compared all 4,224 pre-relocation
initializer bytes and all 78 relocation triples (table-relative offset,
ARM relocation type, target symbol); both comparisons were exact. ARM
layout assertions check the 88-byte stride and the coordinate and
predicate field offsets. Focused verification built 206/206 translation
units without error and improved the 20-function family from 73.87% to
74.59% average source-shape and from 56.02% to 59.81% strict fuzzy.
Linked-exact grew from 3 to 8; byte-exact stayed at 2. The new linked-exact
rows are `GetKeyState(char*)`, `GetKeyState(int)`, `ArrowKeyPressed`,
`keyEventPressed`, and `keyEventReleased`. Native `gof2` and the verifier's
37 tests pass (one skipped). The predicate pointer and `IsSubMenuActive`
now both return `bool`, avoiding an incompatible function-pointer call on
native builds; this change preserved the table comparison and focused ARM
metrics.

Full ARM corpus: 4,720 comparisons, 4,323/4,524 original functions
covered, 201 missing. Average source-shape remains 92.79% at report
precision; strict fuzzy rises 81.17% to 81.18%. Linked-exact rises
2,389 to 2,394 and byte-exact remains 1,008. Comparing every
`(unit,symbol)` exact set shows the five gains above and **no losses**.

This proves the initialized table's ARM representation and preserves its
pointer targets, but does not prove interactive input on a device or exercise
the complete production key press/release path. `setValuesForGamepad` still
writes floats separate from those read by `simulateTouch`; that relationship
remains the next input-level investigation. Do not claim end-to-end native
controls from the table match alone.
