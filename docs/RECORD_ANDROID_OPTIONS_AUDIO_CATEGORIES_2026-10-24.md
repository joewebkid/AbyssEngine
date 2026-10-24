# Android options audio categories (2026-10-24)

Primary reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `RecordHandler::loadOptions` at `0xcd704..0xcd73c`, ARM passes category
indices **1, 2, 3** to both `FModSound::enableCategory` and
`FModSound::setVolume`. The recovered code passed **0, 1, 2**, routing
the loaded SFX, music and ambient settings to the wrong categories.
`Globals::loadSettings` already uses 1, 2, 3 for the same calls. The
six `loadOptions` arguments now follow the original ARM sequence.

The original game on the connected Android device wrote a 126-byte
`gof2_save_options`. Its structure agrees with the recovered writer's
fixed-size path: 94 bytes of field data, including a zero-length name
encoded by a four-byte length, followed by a 32-byte hash. The local
copy is ignored under `_work/bins/` and is not committed. This size
check does not validate the device-dependent hash or prove that the
recovered game can load its own settings end to end.

The original ARM `loadOptions` also calls `AEFile::Close` at `0xcd6b6`
before a final one-byte `AEFile::Read` at `0xcd746`. The recovered code
has the same order. It is a likely original-game defect; moving the
read would improve apparent safety but would not reproduce this ARM
behavior, so this change leaves it alone and records the limitation.

Validation: native `gof2` build; NDK r18b ARM build **208/208** units;
verifier suite **93** tests with one skip. Focused `loadOptions` shape
remains **18.1%** source-shape and **9.9%** strict fuzzy, with no exact
match; these semantic argument changes do not improve its low-level
shape score. The recovered `loadOptions` still uses file-local global
pointers initialized to null and not wired to the real settings or
sound objects; its 41-instruction build is far shorter than the
301-instruction original. Thus this fixes a confirmed argument error
in the source but does **not** make the recovered loader runnable.
Wiring these state references from ARM evidence is the next priority.
The full corpus compares **4,323/4,524** originals over **4,737**
pairs, with **94.31%/83.73%** mean source-shape/strict fuzzy,
**2,578 linked-exact** and **1,082 byte-exact**. Exact-pair comparison
against the pre-edit corpus finds zero losses or gains and zero skips.
