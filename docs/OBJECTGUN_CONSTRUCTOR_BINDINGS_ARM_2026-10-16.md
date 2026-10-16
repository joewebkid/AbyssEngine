# ObjectGun constructor bindings (Android ARM)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The C1 constructor begins at `0x15f700` (C2 aliases it).

- At `0x15f738-0x15f770`, the GOT slot `0x2101f4` has
  `R_ARM_GLOB_DAT _ZN7Globals6CanvasE`. Both canvas reads call the
  typed `PaintCanvas::TransformCreate(unsigned&)` and
  `TransformAddMesh(unsigned, unsigned short, bool)` PLT entries.
- At `0x15f79c-0x15f7a6`, GOT slot `0x21032c` has
  `R_ARM_GLOB_DAT _ZN7Globals4iPadE`; the constructor reads its byte
  for weapon scaling.
- The two PC-relative table loads at `0x15f85e` and `0x15f880`
  both resolve to `.rodata 0x203398`. The first takes the low
  16 bits of entry `itemIndex` for `AEGeometry`; the second
  compares the same signed 32-bit entry to `-1` when deciding
  whether a player gun has geometry. `ObjectGun::update` at
  `0x15f9f0` also resolves to this table. It has 232 entries
  through `0x203737`; the following bytes are RTTI strings.
  Its 928 bytes hash to SHA-256
  `a04e5be2b034cf0bf36b64d32f26c73a3ecad7c54e487a27581c076306526b2c`.

The previous C++ constructor dereferenced null canvas/iPad
placeholders and used two separate zero-filled 256-entry arrays.
The recovered source uses `Globals::Canvas`, `Globals::iPad`,
the typed canvas methods, and one signed ID table. All 232 source
entries were checked against the original bytes. The existing
`update` table access now uses that same verified table; no
index bounds behavior was invented.

Native `gof2` and **207/207** ARM translation units build.
The existing extracted `replaceGun` fixture passes at host
`-O0` and `-O2`; verifier tests: **40**, with one existing skip.
The two constructor symbols improve from **80.2%/41.1%** to
**87.4%/65.5%** source-shape/strict fuzzy; neither is linked- or
byte-exact. The full corpus compares **4,323/4,524** original
functions, with **201 missing**, **124 extras**, zero skipped,
**93.66%/82.75%** average source-shape/strict fuzzy and **2,510
linked-exact / 1,074 byte-exact**. Comparing exact
`(unit,symbol)` sets with the prior full report shows zero gains
and zero losses. Reports are
`A:/Galaxy on Fire new version/_work/objectgun_replace_full_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_ctor_before_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_ctor_after_focused_20261016.json`,
and `A:/Galaxy on Fire new version/_work/objectgun_ctor_full_20261016.json`.

This is a bounded binding/table recovery, not proof that the full
constructor or game runtime works. The constructor has additional
source-shape differences. `ObjectGun::update` and `render` still
hold null canvas and other function-pointer placeholders; no live
ARM32 rendering test was run.
