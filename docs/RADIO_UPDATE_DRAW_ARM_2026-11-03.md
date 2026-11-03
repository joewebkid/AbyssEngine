# Radio update and draw recovery (Android ARMv7)

Source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The REA IDA worker was unreachable during this review; direct ARM
disassembly, ELF relocations, section data, and focused comparison
provided the evidence below.

## Recovered data and calls

`Radio::update(long,PlayerEgo*,LevelScript*)` at
`0x155060–0x155378` indexes a 63-entry pointer table in
`.data.rel.ro` at `0x20df04`. Entries 1–62 point to five-word
character descriptions in `.data` at `0x215f24–0x2163fc`.
Entry 0 points to zero-initialized `.bss` at `0x218388`. The recovered
source contains all 63 rows; a local check compared every initialized
word with the original ELF bytes. Runtime writes to entry 0 are
unverified.

The update path obtains wanted portraits from
`Globals::status->wanted`, generated characters through
`Globals::imageFactory`, dialogue strings through
`Globals::gameText`, and line wrapping and sound selection through
`Globals::globals`. The font selection uses `Globals::font` normally
and `Globals::fontAlien` for image IDs `0x38` and `0x13`. Original
GOT relocations identify `Globals::status` at `0x21015c`,
`imageFactory` at `0x21021c`, `gameText` at `0x2102a0`,
`font` at `0x21029c`, `fontAlien` at `0x210470`, `layout` at
`0x210224`, and `globals` at `0x210218`.

`Radio::draw(long long,PlayerEgo*,LevelScript*)` at
`0x155378–0x1555d0` uses the real sound, canvas, layout, wanted,
text, image factory, and globals objects. Its `Layout::drawBox`
call uses the original six-argument overload. The `Radio+0x34`
font field is a 32-bit font handle, now represented as an unsigned
integer rather than a `String*`. The class's ARM32 size and other
known offsets remain checked by static assertions.

## Verification

- The original table check matched 63/63 rows, including the initial
  zero state of the `.bss` entry.
- Windows native and ARM64 static `gof2` builds pass; ARMv7 compiles
  209/209 translation units with zero errors.
- Focused `Radio::update` source-shape rises `50.3% -> 91.0%` and
  strict fuzzy `24.7% -> 38.7%`; generated instructions rise
  `101 -> 316` against 288 original.
- Focused `Radio::draw` source-shape rises `23.6% -> 91.7%` and
  strict fuzzy `17.6% -> 60.2%`; generated instructions rise
  `33 -> 208` against 217 original.
- Full ARM corpus compares 4,323/4,524 originals in 4,769 pairs,
  with 201 missing and zero skipped. Mean source-shape is 96.52%,
  strict fuzzy 86.40%, 2,763 linked-exact and 1,099 byte-exact.
  Compared with the previous full report, no compared pair or exact
  match was lost.

Reports: `_work/modstation_radio_step43_final_full_20261103.json`,
`_work/radio_runtime_focused_20261103.json`, and
`_work/radio_runtime_full_20261103.json`.

## Limits and next priority

The radio update/draw paths are still short of exact match, and their
behavior has not been exercised in live gameplay. The station's radio
update route still uses provisional handles and the wrong timer field;
its render route still has an unbound draw shim. Next recover the
station timer, scroll positioning, rendering, and exit behavior from
`ModStation::OnUpdate`/`OnRender2D` ARM, without Redmi testing.
