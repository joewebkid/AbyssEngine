# Campaign step 43 radio continuation (Android ARMv7)

Source: `libgof2hdaa.so` from Android 2.0.16, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`.
The evidence below is original ARM disassembly and dynamic ELF relocations;
live gameplay behavior remains unverified.

## Recovered sequence

`ModStation::OnTouchEnd(int,int,void*)`, `0xd797a–0xd7b4e`, checks
campaign step `0x2b`, removes the dialogue mission at station `+0x9c`,
sets `Mission::empty` (GOT `0x2102bc`) as the status mission, clears the
dialogue flag at `+0x69`, stops the current music event, and plays event
`0x90`. It allocates `Radio` (`0x48` bytes) at station `+0x54` and an
`Array<RadioMessage*>` (`0x0c` bytes) of length four at `+0x58`.
The four `RadioMessage(int,int,int,int)` calls at
`0xd79e4–0xd7a5a` use `(text, image, condition, value)` of
`(0x817,1,5,4000)`, `(0x818,1,6,0)`, `(0x819,1,6,1)`, and
`(0x81a,1,6,2)`. The array is passed to `Radio::setMessages`.

The same branch creates image `0x1b5a` into station `+0x5c`, then
sets the vector at `+0x104` to `(50, imageHeight + h + h/2 + 10, 0)`.
The `ScrollTouchBox` allocation is `0x40` bytes at station `+0x98`;
its width derives from `Globals::w - 100`, and its height is `5000`.
It receives text `0x30` and centered alignment. Station words `+0xb4`
and `+0xb8` become `1000001` and `0`; flag byte `+0x60` becomes `1`.
The `Globals::Canvas`, `Globals::h`, `Globals::w`, and
`Globals::gameText` GOT relocations are at `0x2101f4`, `0x210280`,
`0x210278`, and `0x2102a0`.

`Radio::Radio()` at `0x154f7c–0x154fc8` gets its box width and Y
coordinate from `Globals::layout` (GOT `0x210224`) and centers the box
using `Globals::w` (GOT `0x210278`). Its original allocation and
`lastMessageShown()` byte load at `0x15505a` place the latter flag at
offset `0x2c`, the sound ID at `0x30`, and the box Y at `0x40`. The
recovered class layout now asserts these offsets on ARM32. The eight
bytes at `Radio+0x20` have no recovered semantic name.

The branch previously used unbound shims, allocated `0x40` for a
`0x48`-byte `Radio` and `0x80` for a `0x40`-byte scroll box, stored the
box in the vector field at `+0x104`, and truncated the radio pointer
on a 64-bit host. The source now uses the native classes and typed
vector field. An unbound status holder that was read before the branch
was replaced with a handle derived from `Globals::status`; later
branches still retain provisional 32-bit handle logic.

## Verification

- Windows native `gof2` and ARM64 static `gof2` builds pass.
- ARMv7 verifier builds 209/209 translation units, with zero errors.
- Focused `ModStation::OnTouchEnd(int,int,void*)`: source-shape
  `76.9% -> 83.1%`; strict fuzzy `13.0% -> 22.8%`; generated
  instructions `1220 -> 1368` versus 1925 in the original.
- Focused `Radio::Radio()` C1/C2: source-shape `59.3% -> 98.0%`;
  strict fuzzy `0.0% -> 66.7%`. `Radio::lastMessageShown()` becomes
  linked-exact and byte-exact.
- Full ARM corpus: 4,323/4,524 original functions compared in 4,769
  pairs, 201 missing and zero skipped. Mean source-shape `96.49%`,
  strict fuzzy `86.39%`; 2,763 linked-exact and 1,099 byte-exact.
  Compared with the preceding 4,767-pair corpus, no prior pairs or
  exact matches were lost; two pairs, two linked-exact and one
  byte-exact match were gained.

Reports: `_work/status_remove_mission_sentinel_full_20261103.json`,
`_work/modstation_radio_step43_final_focused_20261103.json`,
`_work/radio_ctor_step43_focused_20261103.json`, and
`_work/modstation_radio_step43_final_full_20261103.json`.

## Limits and next priority

The radio scene has not been exercised in live gameplay. Other
`ModStation::OnTouchEnd` campaign branches and its update/render paths
still contain provisional shims; `Radio::update` and `Radio::draw`
also need runtime and ARM review before a complete scene claim. The
next bounded package is the station radio update/render path and
step-43 exit behavior, without Redmi testing.
