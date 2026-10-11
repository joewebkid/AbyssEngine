# NewsTicker global wiring and lifetime (2026-10-11)

The recovered `NewsTicker` had null static stand-ins for canvas, screen
dimensions, layout, font, random generator, status, and game text. The
original Android 2.0.16 ARM/Hex-Rays functions instead use the established
`Globals` members: constructor `0x15c468` reads `rnd`, `status`, `gameText`,
`Canvas`, and `font`; `draw` at `0x15e174` uses `Canvas`, `h`, `layout+0x10`,
and `font`; `getHeight` at `0x15e2b4` and `OnTouchBegin` at `0x15e2d8` use
`h` and `layout+0x10`; `OnTouchMove` at `0x15e328` uses `w`. Those paths now
use the typed globals directly. The constructor separator is the original
`"    +++    "`, not an empty string.

The member `String` is already default-constructed by C++ before the
constructor body, so the manual pre-assignment buffer deletion was removed.
Likewise, C++ automatically destroys that member after the `NewsTicker`
destructor body; the former explicit call destroyed it twice. The original
destructor at `0x15e0d2` has one `String` destruction.

Focused ARM verification builds **206/206** translation units. Across the
11 `NewsTicker` report rows, mean source-shape rises **63.9% to 88.0%** and
strict fuzzy **50.7% to 71.3%**. `getHeight` and both destructor aliases are
now linked-exact; none is byte-exact. Native `gof2` and 36 verifier tests
pass (one skipped). The full ARM corpus compares **4,323/4,524** originals
at **92.01%** mean source-shape (+0.06) and **80.09%** strict (+0.05), with
**2,322 linked-exact** (+3) and **997 byte-exact** (unchanged). A by-symbol
comparison found no lost exact pair, no skipped rows, and no wrong-type
symbols. The report is retained at
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_news_ticker_20261011.json`.

This is not a complete ticker restoration. `replaceTokens` remains a stub,
and the recovered `ModStation::OnInitialize` does not yet create the ticker
seen in original ARM at `0xd5708`; that initialization, asset decoding, and
in-game rendering remain untested. Next priority: recover the station's
ticker construction/ownership path, then the token substitutions, using the
original ARM behavior rather than invented fallbacks.
