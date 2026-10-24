# Android options language handoff (2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

`RecordHandler::loadOptions` at `0xcd450` loads its state through
original ELF GOT relocations for `Globals::options` (`0x21016c`),
`status` (`0x21015c`), `instantActionPlayerName` (`0x2103b0`),
`lastRecordWritten` (`0x2103b4`), `first_start_ever` (`0x2103b8`),
`startLiteVersionWithMoreCredits` (`0x2103bc`), `gameText`
(`0x2102a0`), `globals` (`0x210218`) and `sound` (`0x210168`).
The recovered reader had never-assigned local null pointers for all
nine. It now uses the named `Globals` fields, including a pointer-width
safe player-name replacement. Original ARM also passes sound category
IDs 1, 2, 3, corrected in the preceding change.
Both options readers now use the original typed `AEFile::Read(short&,fd)`
call; removing the generic-size helper avoids one extra local symbol.

`GameText::setLanguage(short,int)` at `0x7f2c0` treats **r1/short as
language**: it compares r1 with `GameText::currentLang` and switches on
it. It treats **r2/int as string count**: it stores r2 at `this+28`,
allocates r2 table entries, and passes r2 to `ReadLangFile`. At
`0x7f570..0x7f576`, it passes the handle returned by `AEFile::OpenRead`,
not zero. The recovered C++ had reversed the two parameter meanings,
used separate private language storage, and passed a zero handle.
Those are corrected. `getLanguage` now reads the original
`GameText::currentLang` symbol (GOT `0x20fef4`); it is linked-exact in
focused verification. The original `.data` at `0x2150c4` initializes
that halfword to `0xffff`, so the first English-language request is
not skipped as an already-active language. The recovered initializer
now uses the same bytes.
The built ARM `GameText.o` `.data` section was independently checked
to contain that `ffff` halfword.

The Android startup caller already passed `(countryCode, 3401)`.
`MenuTouchWindow::OnTouchEnd` at `0x12ac66..0x12ac70` also passes
`(language, 3401)` in ARM; the recovered menu call is corrected to
that order. `loadOptions` at `0xcd6d0..0xcd6de` passes the loaded
language and 3401 to the same two-argument function. It also calls
`Globals::loadFont` with the loaded language. The original's data
indirection around `Globals::globals` differs from the recovered
declaration; the source uses the existing initialized instance, so
this call remains ABI/behavior unverified rather than exact.

The ARM `.rodata` at `0x1ca4cd..0x1ca53e` supplies the language files:
`gb`, `de`, `fr`, `it`, `es`, `ru`, `pl`, `pt`, `cz`, `ar`, `gb`, `gb`,
`ptl`, `mex`, `ko`, `ja` (`.lang`), with `gb.lang` as default and
missing-file fallback. The original APK independently contains 11
matching `assets/*.lang` entries. Both `assets/gb.lang` and the live
settings language's `assets/ru.lang` parse as exactly **3401**
big-endian-length-prefixed strings with no trailing bytes, matching
the ARM caller's count. The OBB also has language
files, but its `gb.lang` is an older **3361**-row variant. The
recovered Android `FileInterfaceAndroid::OpenRead` probes the APK's
`assets/` path before the OBB's content path, matching this version
selection. No asset bytes are committed.

Focused ARM source-shape/strict fuzzy: `loadOptions` **18.1%/9.9% →
95.5%/84.9%**, still nonexact; `GameText::setLanguage(short,int)`
**43.1%/18.1% → 51.0%/28.6%**, still nonexact; `getLanguage` becomes
linked-exact. Native `gof2`, 208/208 ARM units, and the 93-test
verifier suite (one skip) pass. Full-corpus results are in
`PROJECT_STATUS.md`: **4,323/4,524** originals over **4,737** pairs,
**94.34%/83.78%** mean source-shape/strict fuzzy, **2,579
linked-exact** and **1,082 byte-exact**. Exact-pair comparison against
the pre-edit corpus found no losses, and `getLanguage` added the sole
linked-exact gain. There were zero skipped units and 123 extra symbols,
unchanged from baseline.

No recovered end-to-end Android settings/language run has passed.
The original options hash depends on JNI-provided data, and
`GameText::setLanguage` retains substantial shape and semantic gaps.
The original also reads one options byte after closing its handle;
that original behavior is preserved, not claimed safe. Next priority:
host-test the language table loading against an authorized original
asset fixture, then inspect the remaining `GameText` reader gaps.
