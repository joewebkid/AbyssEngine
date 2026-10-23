# Level wingman death removal (Android ARM, 2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`Level::wingmanDied(String const&)` at `0xc4484–0xc44ea` and
`ArrayRemove<String*>` at `0xc44ec–0xc452e`. The disassembly loads
`Globals::status`, calls `Status::getWingmen`, clears the list through
`Status::setWingmen(nullptr)` when it contains fewer than two entries, and
otherwise compares each stored String directly with the argument. The
first matching pointer is passed with the list to `ArrayRemove<String*>`.
That helper removes every occurrence of the pointer and shrinks both the
size and capacity, including a `realloc` of the backing data.

The previous source copied the String before comparison and used a local
removal helper that only changed `size_`. The replacement calls the existing
`ArrayRemove<String*>`, restores the native global, and removes the unused
helper. A production-method smoke exercises null, one-entry, no-match,
match, and duplicate-pointer lists at `-O0/-O2`; it also checks that no
String copy is made and capacity follows the removal. The existing Level
player-allocation smoke passes. Native UCRT64 `gof2`, all 208 NDK r18b ARM
translation units, and 93 verifier tests pass (one existing skip).

Focused `wingmanDied` source-shape/strict fuzzy changes **48.9%/28.6% →
96.9%/51.3%**, with target/base instruction counts **39/39** after the
patch; the method is not linked- or byte-exact. The newly emitted
`ArrayRemove<String*>` comparison is **100%** source-shape and **96.6%**
strict fuzzy, also nonexact. The full ARM corpus compares **4,323/4,524**
original functions across **4,736** pairs (one more pair than before), at
rounded **94.07%/83.39%** mean source-shape/strict fuzzy. It retains
**2,557 linked-exact** and **1,082 byte-exact** pairs, with zero exact-pair
losses, skipped or wrong-type symbols. Only `wingmanDied` changed score
among existing pairs. Full report:
`wingman-died-after-full.json` (SHA-256
`2e570208cdb9eb9c8732ffb60392858dda2c95f8fd3c669e9d0e307482d50a3e`).

The smoke does not establish a complete death/game-frame path or prove
`Status::setWingmen` ownership under real assets. Those remain separately
unverified. The next candidate is the low-scoring `KIPlayer::captureCrate`,
but it needs its own source and caller audit before changing behavior.
