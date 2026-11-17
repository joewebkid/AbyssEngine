# `Globals::~Globals()` recovery (Android ARM32)

Date: 2026-11-17

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Globals::~Globals()` (`_ZN7GlobalsD1Ev`) starts at `0xe3444`.

## Original-binary evidence

The original destructor (`0xe3444`–`0xe35da`, 166 Thumb-2 instructions)
calls `RecordHandler::saveOptions()` when `Globals::recordHandler` is
non-null, destroys and deletes `Globals::galaxy`, `Globals::status`,
`Globals::gameText`, `Globals::rnd`, `Globals::layout`,
`Globals::generator`, `Globals::recordHandler`,
`Globals::instantActionPlayerName` (virtual deleting dtor at `vtable[1]`),
`Globals::sound`, `Globals::items` (`ArrayReleaseClasses` + delete),
`Globals::ships` (`ArrayReleaseClasses` + delete), `Globals::galaxy`
(second check), `Globals::achievements`, `Globals::status` (second
check), `Globals::imageFactory`, `this->soundResources` (`ArrayRelease`
+ delete), and clears `Globals::globals = nullptr`.

The prior source bound all 14 slot pointers (`gG_recordHandler`..`gG_tail`)
to `nullptr` and called eight undefined `*_dtor` stand-ins, causing
Clang `-Oz` to fold the entire destructor to a single trap instruction
(`0.1%` source-shape, `0.0%` strict fuzzy).

## Recovery and checks

`src/game/core/Globals.cpp` now operates directly on the real `Globals`
static members, uses typed `delete` expressions and `ArrayRelease`, and
removes the eight `*_dtor` declarations.

- Windows native `gof2`: pass.
- ARMv7 NDK r18b: 211/211 translation units compile.
- ARM64 static `gof2`: pass. Strict whole-archive ARM64 shared link:
  pass, producing an ELF64/AArch64 shared object.
- Full ARM corpus: **4,323/4,524** original functions in **4,776**
  pairs (from **4,772**), mean source-shape **96.92%** (from **96.88%**),
  strict fuzzy **87.02%** (from **86.98%**), **2,806 linked-exact**
  (from **2,802**, +4 in `Globals`), **1,116 byte-exact**, 201 missing,
  zero skipped or wrong-type, and 151 extra symbols. Both exact
  membership sets have zero losses.
- Focused `Globals::~Globals()` (`D1Ev`/`D2Ev`): source-shape **0.1% ->
  97.8%**, strict fuzzy **0.0% -> 54.8%**, control-flow/call-count/size
  match **100.0%** (**166/166** instructions).
