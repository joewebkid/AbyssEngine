# Pan first-friend loot dependency (2026-10-24)

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
exports `Generator::getLootList(int,int)` at `0x96504`. The recovered
`PlayerFighter` constructor calls it with `(-1,-1)` on the ordinary-friend
path. Its prior focused ARM score was **99.2% source-shape / 95.6% strict
fuzzy**, not exact; this checkpoint tests behavior rather than changing code
shape.

`tools/run_level_stage1_first_friend_loot_pan_gate.py` extends the Pan
Player/Route/Explosion gate. It source-extracts production
`FileRead::loadItemsBinary`, `Item::init`, `Generator::getLootList`, their
direct getters, and the five-entry ARM-backed loot type table. The original
Android `items.bin` (27,628 bytes, SHA-256
`55b953629d994d553604418988d43d2b1935f93061da7040a464acad57677e95`)
is streamed through the existing read-only ADB-reverse file boundary. The
reader loads 233 catalog entries in order. The inherited Pan fixture has a
minimal `Item` constructor, so this gate explicitly runs the production
`Item::init` on each loaded entry; it does **not** revalidate the full Item
constructor or file stack, which have separate host coverage.

After the earlier Pan checks, 20 deterministic local RNG seeds exercise
both null and populated `getLootList(-1,-1)` returns on the Android device.
Populated outputs contain one or two item/quantity pairs with catalog-valid
indices and quantities 1..9. The fixture checks balanced asset opens/closes,
releases the loaded catalog and loot arrays, and restores the Pan RNG pointer.
NDK r18b ELF32 ARM executables passed at `-O0` and `-O2` on Android 16:
SHA-256 `8098aed21b965d6b4e2a7396724f6914dbdd51447f93f00072b2b02f69616767`
and `ab984aea82b4571c63a866d7b665552a14125c3f204de8687e9104bdea628fb9`.
Native `gof2` remains green. The temporary executables were removed from
the device after the pass.

The seeded calls are a bounded dependency test, **not** the exact RNG stream
inside a linked `PlayerFighter` constructor. The jump-drive energy-cell
override, full Fighter construction, ownership/destruction, enemy-array
insertion, and rendering remain unverified here. Production C++ did not
change: the last full corpus remains **4,323/4,524** originals across
**4,736** pairs, mean **94.11%/83.43%** source-shape/strict fuzzy,
**2,557 linked-exact** and **1,082 byte-exact**, with zero skipped or
wrong-type symbols. Next, link and run the actual Fighter constructor in
the Pan asset scene, then check array insertion and ownership.
