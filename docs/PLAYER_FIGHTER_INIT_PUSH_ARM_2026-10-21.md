# PlayerFighter initPush recovery (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::initPush(const Vector&, int)` at `0xdfc88–0xdfdc8`.
The ARM call sequence obtains the fighter position twice. It computes
`target - position` for distance, clamps `distance / radius` at one, and
stores `int((1 - ratio) * 5000)` into the timer and duration at
`+0x104/+0x108`. It normalizes `position - target` into the direction at
`+0x10c`, calls `Globals::rnd->nextInt(200)` three times, subtracts 100
from each result, normalizes that vector and multiplies it by the literal
`0.2f` before storing the spin at `+0x118`. The independent recovered
listing agrees with the ARM instruction and call order. The float literals
come from the original ARM pool (`5000.0f` at `0xdfdb4`, `0.2f` at
`0xdfdb8`).

The previous C++ dereferenced a null stack-guard placeholder, used a
zero strength and null RNG function, truncated an RNG pointer to 32 bits,
and normalized a zero direction. These placeholders made push initialization
unusable on the native host. The method now uses the existing vector
operators and `AERandom::nextInt`; no ABI layout or caller signatures were
changed. Its two known call sites are the weapon impact in `Gun.cpp` and a
scripted event in `LevelScript.cpp`.

The production-method smoke (`tools/run_player_fighter_init_push_smoke.py`)
passes at `-O0` and `-O2`. It checks two distance cases, both timer fields,
outward direction, spin magnitude, three bounded RNG calls and the two
position reads. Native UCRT64 `gof2` builds; all 208 NDK r18b ARM
translation units compile; 93 verifier tests pass (one existing skip).
Focused source-shape/strict fuzzy improves from **70.4%/35.1%** to
**96.6%/74.7%**. `initPush` remains nonexact (111 original versus 114
recompiled instructions).

The full NDK r18b corpus compares **4,323/4,524** original functions across
**4,735** pairs, with **94.03%/83.37%** mean source-shape/strict fuzzy,
**2,557 linked-exact** and **1,082 byte-exact**. There are zero skipped or
wrong-type symbols. Comparison by `(unit, symbol)` against the preceding
`PlayerFighter::push` full report finds no exact-pair losses, and the only
source-shape change is `initPush`. The full report is
`fighter-init-push-full-current.json` (SHA-256
`b80754f56b5a30cb917e90a5edfe349f28a22f9c24e05021acaaa41088d7b336`).

The smoke uses mock math and RNG rather than a complete game frame. The
effect of zero or negative radius and the visible ship response in a running
level are not independently verified. The `PF_update_body` call still
interrupts the continuous Fighter update path; reconstructing it remains
the next priority before claiming end-to-end push behavior.
