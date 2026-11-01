# SpaceLounge drawing and offer controls

Date: 2026-11-01. Reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::drawLounge()` at `0x170d38`.

The original hover path uses the projected agent rectangle, names known
or story agents directly, and otherwise uses text ID `406 + race`.
Mission labels use `354 + mission type`; offer labels use IDs **306**,
**305**, and **884** for offers 6, 2, and 7 (`0x170e82`–`0x171258`).
The previous source passed unbound globals to text and canvas calls,
used ID 304 for offer 7, and did not measure either string correctly.
The reconstructed path measures the name and optional offer label with
`Globals::font`, keeps the tooltip within the screen edge, and draws
the original white/blue/orange text and panel colors (`0x171270`–
`0x17141e`).

The non-hover path now draws the agent panel and character image through
`Globals::Canvas`, `Globals::layout`, and `Globals::imageFactory`
(`0x170d6c`–`0x170e04`). The original mode-2 controls show three
buttons for the offer bitmask `0x60c`, or four for eligible outside
missions of type other than 12; `singleOffer` and mode 3 use one
button (`0x170e12`–`0x17124e`). The previous source did not draw
the third button and omitted the outside-mission buttons. A null
mission guard is retained in the final offer-0 branch because the
available source does not establish that every offer-0 agent has a
mission; this is a defensive deviation from the unguarded ARM call.

Focused ARM `source-shape` improved from **19.3% to 91.1%** and strict
fuzzy from **6.6% to 27.0%**. The compiled body has 667 instructions
against 91 before and 727 in the original. It is not exact. Windows
and ARM64 static builds pass; ARMv7 compiles 209/209 units. The focused
`SpaceLounge` comparison has no linked-exact or byte-exact losses.

Live rendering and button behavior is unverified while device testing
is deferred. The constructor, touch-end, update, key handling and
`startChat()` paths still have incomplete branches or unbound globals.

The full ARM corpus covers **4,323/4,524** originals over **4,761**
comparison pairs at **96.08%** mean source-shape and **85.82%** strict
fuzzy. It has **2,733 linked-exact**, **1,091 byte-exact**, zero skipped,
and 201 missing originals. The normalized exact sets and comparison-pair
set are unchanged from the preceding corpus. Reports:
`D:/gof2-arm64-crypto/space-drawlounge-20261101-focused.json` and
`D:/gof2-arm64-crypto/space-drawlounge-20261101-full.json`.
