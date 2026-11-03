# ModStation release globals (Android ARM)

The original Android `libgof2hdaa.so` (2.0.16, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`)
places `ModStation::OnRelease` at `0xd9050–0xd9226`. At `0xd9062–0xd9070`
it reads the global sound pointer, conditionally calls
`FModSound::disableReverb` and `stopAllSoundFXEvents`, and then disables
fog. At `0xd9190–0xd91a8`, after releasing UI resources and loading the
font, it conditionally calls `Layout::reload`, `ImageFactory::reload`,
and `Layout::initTip`. At `0xd920e–0xd9220` it conditionally calls
`FModSound::freeAllEvents`.

The recovered source used three null-initialized static pointer-to-pointer
placeholders for those globals. Replace them with `Globals::sound`,
`Globals::layout`, and `Globals::imageFactory`, preserving the original
call order and conditions. This restores reachable cleanup and resource
reloads; the previous sound placeholder could dereference null before
reaching button cleanup.

Verification:

- Windows native `gof2` and ARM64 static `gof2` builds pass.
- Focused Android ARM build: 209/209 translation units. `OnRelease`
  source-shape **83.5% → 92.4%**, strict fuzzy **59.9% → 74.9%**,
  generated instructions **138 → 168** versus original **166**.
- Full Android ARM corpus: **4,323/4,524** originals, **4,767** pairs,
  mean source-shape **96.45% → 96.46%**, strict fuzzy **86.32%**,
  **2,759 linked-exact / 1,098 byte-exact**, **201** missing and
  zero skipped. No pair or exact-set losses.

This is static assembly and build evidence. Live station release and
sound behavior remain unverified. The next priority is restoring a
bounded unverified station interaction path while preserving the exact
sets. Redmi testing remains stopped.
