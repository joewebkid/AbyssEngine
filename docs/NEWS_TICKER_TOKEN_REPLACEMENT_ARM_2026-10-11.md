# NewsTicker token replacement (2026-10-11)

Android 2.0.16 ARM `NewsTicker::replaceTokens` at `0x15c858` tests 21
marker literals in a fixed sequence. The recovered function previously
returned its input unchanged. It now restores the station/system/drink,
child/ship/platform, catastrophe/victims/laser, Vossk item/revenue,
profession/name/activity, race/name/crime, Nivelian planet/demonstrators,
poll, and casualties groups. Dependent markers are replaced only within
their original parent branch. The token-literal set was checked against
the original dump: **21/21** are present.

The original ARM instructions resolved ambiguities in the decompiler:

- all three `Globals::getRandomName(int, bool)` calls pass `true`;
- `#VICTIMS` uses `(100000 + nextInt(99000))` rounded down to 1000;
- `#DEMONSTRATORS` uses `(1000000 + nextInt(990000))` rounded down to 1000;
- Vossk revenue uses `getPlayingTime() / 1000000` plus 140 for the ship
  branch or 12 for the item branch, not the division remainder;
- the platform letters are A–F: ARM `.data.rel.ro` at `0x20e188` points
  to A–D in `.rodata`, with E/F selected in the following branch;
- `#CASUALTIES` uses the original `int + String` concatenation with 2
  and `nextInt(48)`.

Focused ARM verification builds **206/206** translation units. The
function rises from **0.3%/0.2%** to **91.8%/45.6%** source-shape/strict
fuzzy and is not linked- or byte-exact. Native `gof2` and 36 verifier
tests pass (one skipped). The full corpus compares **4,323/4,524**
original functions at **92.05%** source-shape and **80.12%** strict,
with **2,325 linked-exact** and **997 byte-exact**. The two new linked
matches are emitted `Array<Item *>` template operations, not the ticker
function itself. A by-symbol comparison found no lost exact pair, skipped
row or wrong-type symbol. The retained report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_ticker_tokens_20261011.json`.

No running-game news feed or locale-specific asset lookup was tested.
In particular, the original Vossk item branch assumes a nonempty
eligible-item list and the Nivelian branch assumes an available race-2
system; those data-dependent paths still need controlled runtime checks.
