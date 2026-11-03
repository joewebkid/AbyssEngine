# ScrollTouchBox global bindings and text layout

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `ScrollTouchBox` constructor at `0x135598` reads
`Globals::font`. Its `setText(String)` at `0x13570c` uses the same
font ID. The `setText(String,int)` body at `0x135600` calls through
`Globals::globals`, reads `Globals::layout` offsets `0x4` and `0x48`,
and appends a newline string when the text must be laid out again.
The `draw()` body at `0x135778` reads `Globals::Canvas`,
`Globals::retinaDisplay`, `Globals::layout` offset `0x4`, and
`Engine::enableReverseFlag`; the final store sets that flag to one.

The reconstruction used private null-initialized pointer slots for
each of these symbols. The constructor, both text setters, and draw
now use the exported game globals. The wrap branch appends the
original newline instead of an empty string. Font IDs stored in the
pointer-shaped member are converted through `uintptr_t`, preserving
the 32-bit ARM value and avoiding narrowing an address through
`unsigned long` on Windows.

Focused ARM verification compiled **209/209** translation units.
`draw()` rises from **15.8% to 88.8% source-shape** and from **7.1%
to 37.2% strict fuzzy**, with 142 generated instructions against 159
original. `setText(String,int)` rises from **49.5% to 96.9%
source-shape** and **23.5% to 83.2% strict fuzzy**, with 101 generated
instructions against 101 original. Both constructor variants reach
**100% / 100%** and become linked-exact. The `ArrayAdd<String*>`
instantiation becomes present and linked-exact. The focused unit has
22 comparisons, **11 linked-exact** and **5 byte-exact**; neither
existing exact set loses a member.

The production constructor, layout, and draw bodies pass a host smoke
at `-O0` and `-O2`. It covers empty lines, wrapping with newline,
centering, right alignment, Retina clipping, and restoration of the
reverse-text flag. Windows native and ARM64 static builds pass.

The full ARM corpus compares **4,323/4,524** original functions in
**4,764** pairs at **96.41% mean source-shape / 86.27% strict fuzzy**,
with **2,757 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. Against the preceding full report, one `ArrayAdd<String*>`
pair and three linked-exact matches were gained. No pair or exact-set
member was lost.

Live GPU output and resource-dependent font metrics are unverified.
`draw()` and the text setters are not exact; other `ScrollTouchBox`
methods still have source-shape differences. No device run was made.
Redmi testing remains stopped by user request. The next bounded
priority is `ModStation::OnTouchMove`, whose current body has only
10 generated instructions against 139 original ARM instructions.
