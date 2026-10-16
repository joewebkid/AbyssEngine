# ObjectGun destructor ownership (Android ARM)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ObjectGun::~ObjectGun()` aliases `D1`/`D2` begin at `0x15f904`.

The ARM destructor deletes owned `AEGeometry*` at object offset
`0x18` (`0x15f910-0x15f91c`), calls
`ArrayReleaseClasses<Explosion*>` on the array at `0x2c`
(`0x15f920-0x15f928`), then directly calls the array destructor and
`operator delete` (`0x15f92c-0x15f934`). It finally deletes the
byte array at `0x30` and nulls the stored pointers. There is no
`ArrayRemoveAll` between releasing classes and deleting the array.

The prior C++ did call `ArrayRemoveAll` there. In this codebase,
`ArrayReleaseClasses` already deletes the elements and buffer and
sets `data_` to null; `ArrayRemoveAll` then reallocates a new
one-element buffer just before `delete` frees it. Removing that
extra allocation restores the ARM call sequence and avoids a
potential allocation failure during destruction.

Focused `D1`/`D2` source-shape/strict fuzzy improves from
**89.2%/95.1%** to **100%/100%** for both aliases; both are now
linked-exact, not byte-exact. Native `gof2`, 207/207 ARM translation
units, the ObjectGun host fixture, and 40 verifier tests (one existing
skip) pass. The source-backed change is confined to `ObjectGun`;
similar release/remove patterns elsewhere need independent audits.

The neighboring `ObjectGun::setScaling(int,int,int)` at `0x16035a`
was also checked: ARM converts only `x` and `y`, writes offsets
`0x3c` and `0x40`, and ignores `z`. Its existing C++ is byte-exact
and was not changed.

The full ARM corpus compares **4,323/4,524** original functions,
with **201 missing**, **124 extras**, zero skipped or wrong
signatures, **93.67%/82.77%** average source-shape/strict fuzzy,
and **2,513 linked-exact / 1,074 byte-exact**. Exact
`(unit,symbol)` sets against the preceding report show two linked
gains (`D1`/`D2`) and zero linked/byte losses. Reports:
`A:/Galaxy on Fire new version/_work/objectgun_setenemies_full_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_dtor_before_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_dtor_focused_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_setscaling_audit_20261016.json`,
and `A:/Galaxy on Fire new version/_work/objectgun_dtor_full_20261016.json`.

Live ARM32 teardown, allocator fault paths, and object ownership
beyond these call sites remain unverified.
