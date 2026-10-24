# PlayerStaticFar camera-distance update (2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`PlayerStaticFar::update(int)` at `0x11c2e0` returns when geometry is null,
reads the camera through `Globals::Canvas` (GOT `0x2101f4`), and constructs
the object position from signed integer fields at `0x124/0x128/0x12c`.
The literal pool at `0x11c444–0x11c450` supplies an integer near-distance
threshold of **295001**, a float far-position radius of **295000.0f**,
and float scale factors **4096.0f** and **1/4096**. The near branch calls
the three-float `AEGeometry::setScaling(1,1,1)` and `setPosition` with
the integer-derived coordinates. The far branch normalizes the object-
camera vector, scales it to 295000, adds the camera position, passes the
resulting vector to `setPosition`, and calls three-float `setScaling`
with the quantized uniform scale.

The previous reconstruction used five file-static variables that had no
assignments: a null canvas pointer and four zero-valued distance/scale
parameters. On a non-null geometry it would dereference that null canvas;
even if assigned externally, the zero parameters would not reproduce
the original distance behavior. Its near branch also used the unrelated
float `posX/Y/Z` fields and wrapper overloads. The recovered body uses
the evidenced global, constants, integer fields and call ABIs.

Focused NDK r18b ARM `-Oz` source-shape/strict fuzzy moves from
**89.6%/76.5% to 98.7%/80.0%**; the function remains nonexact. A
source-extracted host gate at `-O0/-O2` checks the null-geometry return,
near integer-coordinate path, far radius, vector position and quantized
uniform scaling. It is not a live Android camera/geometry test.

Native `gof2` and all 208 ARM translation units build; 93 verifier tests
pass (one skip). The full ARM corpus compares **4,323/4,524** original
functions across **4,736** pairs, with mean **94.17% source-shape /
83.50% strict fuzzy**, **2,562 linked-exact**, **1,082 byte-exact**,
**2,728 100%-fuzzy**, **201 missing**, and no skipped functions.
Pairwise comparison with the preceding collision-bounds baseline finds
no linked- or byte-exact losses or gains. ADB currently lists no device;
the first-friend Pan ship-insertion gate remains the next device-runtime
priority. The next source candidate is `PlayerAsteroid::update`, pending
an ARM behavior review rather than a metrics-only edit.
