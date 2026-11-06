# PlayerEgo shooting paths (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
This pass covers `PlayerEgo::shoot(int, int)` at `0x9b198`.

When the turret is active, original ARM gets the matrices from
`geometry` and `turretGeometry`, calls `AEMath::operator*(Matrix const&,
Matrix const&)` at `0x9b1d2`, then calls
`Player::shoot(int, long long, bool, Matrix)` at `0x9b23c` with slot 2,
the sign-extended `weapon` argument, false, and the product matrix.
The reconstruction used an undefined `Mat_mul` helper, copied 15
floats out of a 12-float local array, and wrote `mat.m[15]` even
though `Matrix::m` has only 15 elements. It also passed `weapon < 0`
as the boolean flag. The code now uses the original matrix operation
and call arguments on both 32-bit and 64-bit builds.

Outside the turret path, ARM at `0x9b250–0x9b270` calls
`Player::shoot(int, int, long long, bool)` with slot 1, the current
secondary weapon index, the sign-extended `weapon` argument, and
false. If that returns false, it writes `-1` to the secondary weapon
index at offset `+0x10c`. The reconstruction used the index as the
64-bit argument, passed the wrong boolean, and ignored failure.
The ordinary path at `0x9b272–0x9b27e` calls the distinct
`Player::shoot(int, long long, bool)` overload with `type`,
sign-extended `weapon`, and false. The reconstruction had called the
four-argument overload. Both paths now match the original call
targets and arguments.

Focused ARMv7 verification compiles **211/211** translation units.
The target rises **84.1% -> 100.0%** source-shape and **25.1% ->
99.0%** strict fuzzy, with 97 reconstructed instructions versus 97
original. It remains non-exact. ARM object relocations name the same
matrix operator and three `Player::shoot` overloads as the original
call sites. Windows native and ARM64 static builds pass, as does
strict ARM64 shared linking. The raw 211-object ARM32 unmatched
inventory falls **192 -> 191** with no new name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: mean source-shape stays **96.83%**, strict fuzzy
rises **86.91% -> 86.93%**, and **2,798 linked-exact** plus
**1,113 byte-exact** are retained with unchanged membership sets.
There are 201 missing, zero skipped, zero wrong-type, and 152 extra
symbols.

Live shooting and turret placement, as well as full ARM32 shared
linking, remain unverified. Next priority: inspect the nearby
`PlayerEgo::stopShooting` ARM32 call placeholders and test weapon
behavior on a device when available.
