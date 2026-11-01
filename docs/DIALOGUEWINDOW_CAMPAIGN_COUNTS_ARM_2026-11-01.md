# DialogueWindow campaign dialogue counts (Android ARM)

## Evidence and change

The original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
uses the 162-entry table at `0x2037b0` for briefing dialogue and the
162-entry table at `0x203a38` for success dialogue. REA/IDA confirmed
these addresses independently in `DialogueWindow::length()` at
`0x168044`, `hasBriefingDialogue()` at `0x1671b4`,
`hasSuccessDialogue()` at `0x1671d4`, and `init()` at `0x166c58`.
Both tables lie in the first ELF load segment, where virtual address
equals file offset. Their 32-bit little-endian values were extracted
from that segment and the source arrays were checked element by
element against all 324 original values.

The source previously had four separate zero-filled placeholders for
these two tables. It now has one exact constant array per dialogue
kind, shared by all four functions. `init()` accumulates briefing
counts into briefing offsets and success counts into success offsets,
matching the original code.

## Verification

- Source table self-check: 162/162 values matched at each address;
  briefing sum 120, success sum 2,056.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, average source-shape
  86.7% -> 90.3%, strict fuzzy 72.9% -> 78.9%, linked-exact 11 -> 13,
  byte-exact 4. Both `has*Dialogue` functions became linked-exact;
  `length()` reached 100% source-shape and strict fuzzy.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped, 201 missing. Average source-shape
  95.95% -> 95.97%, strict fuzzy 85.73% -> 85.77%, linked-exact
  2,722 -> 2,724, byte-exact 1,091. Exact-set comparison found zero
  losses and only the two `has*Dialogue` gains.

No phone runtime check was performed. Mission text and portrait tables,
and the remaining `loadContent()` branches, still need separate
source-backed recovery.
