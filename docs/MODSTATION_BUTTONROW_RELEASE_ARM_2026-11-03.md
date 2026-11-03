# Station button-row release

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`.
The `ModStation::OnRelease` block at `0xd9086–0xd90a4` was checked
against the original NDK ARM objdump.

The original tests the row pointer at `ModStation+0x8c`. When it is
non-null, it calls the exported `ArrayReleaseClasses<TouchButton*>`
specialization, then the `Array<TouchButton*>` destructor and
`operator delete`, and clears the field. The previous C++ used three
unbound integer/pointer wrappers for these operations. The code now
calls the specialization on the actual array and uses `delete row` for
the destructor and allocation. The specialization deletes button
objects, frees the backing array, and nulls its data pointer before
`delete row` runs.

Focused ARM verification compiles **209/209** translation units.
For `OnRelease`, source-shape changes **84.2% → 83.5%** and strict
fuzzy **77.0% → 59.9%**; the compiler output remains 138 instructions
against 166 original instructions. This local metric decrease is
recorded because the real release path removes opaque calls. The
`ModStation` focused report gains the linked-exact
`Array<TouchButton*>::~Array()` symbol: **37** comparisons,
**10 linked-exact / 3 byte-exact**. Windows native and ARM64 static
builds pass.

The full ARM corpus compares **4,323/4,524** original functions in
**4,765** pairs, with **96.45% mean source-shape / 86.32% strict
fuzzy**, **2,758 linked-exact / 1,098 byte-exact**, 201 missing and
zero skipped. No previous pair or exact match was lost. The one new
pair and linked-exact match are the array destructor.

The original `OnInitialize` creates this row at `0xd5b68–0xd5b80`
and fills five buttons afterward. Its current C++ reconstruction
still delegates that larger setup to an unbound placeholder. This is
the next priority. Live station cleanup and re-entry have not been
verified; Redmi testing remains stopped by user request.
