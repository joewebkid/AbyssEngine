# SpaceLounge light and iPad placement

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::init()` at `0x169f80`.

After setting the initial camera, the original initializes the cut
scene's star-system light when a cut scene exists and sets the
engine's specular light color to `(0.5, 0.5, 0.5)` for light `0x4000`
(`0x16a4e0`–`0x16a512`). The recovered source now makes both calls.

The iPad path (`0x16a524`–`0x16a770`) positions the lounge panel
beside the selected agent's screen rectangle, clamps it to screen
and layout margins, recreates the scroll window, and repositions the
three lower buttons. The icon and button paddings differ for HD,
large, and other iPad layouts. The 24 bytes at `SpaceLounge + 0x8c`
and `+0x98` are copied as two 12-byte vectors by the original
`startChat()` code (`0x16ad02`–`0x16ad48`); the six former integer
fields are now typed as `focusRectLeft` and `focusRectRight` with
offset assertions. Their constructor initialization keeps the
previous instruction shape.

Focused `init()` source-shape rose **72.6% → 89.2%** and strict
fuzzy **33.0% → 38.8%**. The ARM build has 683 instructions versus
749 in the original. The constructor remains **94.1% source-shape /
53.6% strict fuzzy**. Neither function is exact. The focused unit
retains 22 linked-exact and 7 byte-exact matches over 42 comparisons.
Windows and ARM64 static builds pass; ARMv7 compiles 209/209 units.
Focused report:
`D:/gof2-arm64-crypto/space-init-light-ipad-20261102-focused-final.json`.

The iPad positions and light state have not been checked in a running
game. `startChat()` remains incomplete; its mission-24 auto-chat path
in `init()` is still not restored. Next priority is that dependency
or another missing runtime path.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs. Mean source-shape rose **96.15% → 96.16%** and
strict fuzzy **85.93% → 85.94%**. It retains **2,741 linked-exact**
and **1,096 byte-exact** matches, zero skipped, and 201 missing
originals. The normalized comparison-pair, linked-exact, and
byte-exact sets are unchanged from the previous full report.
Full report:
`D:/gof2-arm64-crypto/space-init-light-ipad-20261102-full.json`.
