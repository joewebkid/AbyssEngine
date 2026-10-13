# Layout window-dimension reset — Android ARM audit (2026-10-13)

Scope: `Layout::resetWindowDimensions()`
(`_ZN6Layout21resetWindowDimensionsEv`) in the Android 2.0.16 ELF
at `0xd3048`. Primary evidence is its Thumb disassembly; the IDA
export at `analysis/gof2_libgof2hdaa_full_ida.c:129671-129710`
confirms the field writes and calls. No iOS/PC implementation was
used.

Android resets the window to the screen dimensions, positions the
help, back and secondary buttons, then calls `getPosition()` twice
to publish the back button's X and Y as integers. It reads the
layout's button inset and the window/footer fields independently
for both footer-button calls. The local implementation reused
calculated coordinates across those calls and accessed the returned
`Vector` through `operator[]` and assignment, adding calls absent
from ARM. It now uses direct `.x`/`.y` fields and evaluates each
button position as the original does. The production-method UI
smoke checks window dimensions, all three buttons and published
coordinates.

Focused source-shape rises from 77.8% to 99.8%, strict fuzzy from
57.3% to 89.4%. The function is still neither linked- nor
byte-exact: remaining differences are in the initial load/register
sequence, with equal instruction counts. Native `gof2`, all 206
ARM units and 37 verifier tests pass (one skipped). The original
calls `setPosition()` before its null check of `backButton`; this
behavior remains, rather than adding an unevidenced guard. Real
assets, visual output and ARM32 runtime remain untested.

Full corpus: 4,718 comparisons, 4,323/4,524 covered, 201 missing,
92.94% average source-shape, 81.36% strict fuzzy, 2,401 linked-exact
and 1,008 byte-exact. Against the preceding full run, complete
linked-exact, byte-exact and extra-symbol sets are unchanged.
Next priority: audit `Layout::drawBox(int,int,int,int,int,String)`
(79.2% source-shape).
