# SpaceLounge special dialogue sound selection

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::getSoundId(Agent *)` at `0x1714f8` in the REA/IDA
analysis database and original Thumb disassembly.

The original initializes a special-text flag to one, branches
directly to the random 781–782 sound selection for offer 1, and
clears that flag for every other offer before its 30% random
override (`0x171564`–`0x17159c`). Only with the flag set does it
compare the mission text against game texts 820, 824, 827 and 833;
a match selects sound 788 or 789 (`0x1715c8`–`0x17162e`).

The prior source re-enabled the special-text check after the 30%
override for non-1 offers. Those conversations could therefore
choose 788–789 instead of retaining the override's 781–782.
The flag now follows the original offer-1 condition.

The focused `getSoundId()` comparison rises **84.9% → 85.1%**
source-shape and **48.4% → 50.9%** strict fuzzy, with 225 rebuilt
instructions versus 235 original. ARMv7 compiles 209/209 units;
the focused unit retains **22 linked-exact / 7 byte-exact** matches
across 42 comparisons. Windows and ARM64 static builds pass.
Focused report:
`D:/gof2-arm64-crypto/space-sound-special-20261102-focused.json`.

The three direct ARM call sites for `onKeyPress(int)` at `0x16c8ae`,
`0x171aa0` and `0x172d40` discard its return register. The
decompiler infers an integer return, but these calls do not establish
the original C++ return type. Its current `void` declaration is
retained pending stronger evidence. Live sound behavior remains
unverified while device work is deferred.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs at **96.18%** mean source-shape and **85.94%** strict fuzzy.
The normalized comparison-pair, **2,741 linked-exact**, and **1,096
byte-exact** sets are unchanged from the preceding report. There are
zero skipped and 201 missing originals. Full report:
`D:/gof2-arm64-crypto/space-sound-special-20261102-full.json`.
