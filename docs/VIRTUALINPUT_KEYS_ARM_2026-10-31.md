# Virtual-input key-state table (2026-10-31)

Primary evidence is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The exported `Globals::keys` object at `0x217d00` is **1,020 bytes**
in the ELF symbol table. The original `simulateTouch` at `0x6ab02–0x6ab08`
loads a 32-bit word at offset `0xBC` from that object. When the word
is nonzero, it toggles `Engine::EnablePostEffect`. The previous
source instead read offset `0xBC` from an `ApplicationManager`
instance, a different object. The recovered source now reads the
table word. Its C++ declaration is a 255-element `unsigned int`
array, preserving the observed byte size and word access.

The original `keyPressed` at `0x6a394–0x6a3ae` reads a second 32-bit
word at offset `0xA8` from the same table. In the completed key
combination path, when neither Accelerate nor Decelerate is pressed,
a nonzero word clears both virtual-input speed flags. The previous
source checked whether the manager's engine pointer was null, which
is unrelated to this ARM condition. The recovered source reads the
observed table word. The purpose and writers of these table entries
remain unverified; the offsets are retained without invented field
names.

Focused `keyPressed` improves **95.6% → 96.5% source-shape** and
**77.1% → 87.5% strict fuzzy**. `simulateTouch` retains its
**84.4% / 42.2%** focused scores because this correction changes a
relocation target while preserving the local instruction form. The
20-symbol input unit improves **85.5% → 85.6% source-shape** and
**74.3% → 74.8% strict fuzzy**, keeping nine linked-exact and two
byte-exact matches. Native Windows and ARM64 static builds pass;
ARMv7 compiles 209/209 translation units. Live key-state behavior
remains unverified while device testing is deferred.

The full ARM corpus compares **4,323/4,524** originals over **4,749**
pairs: **95.12% mean source-shape**, **84.82% mean strict fuzzy**,
**2,677 linked-exact**, **1,091 byte-exact**, zero skipped and 201
missing. Both rounded means and exact counts are unchanged from the
preceding revision. Complete `(unit, symbol)` exact-set comparison
finds no gains or losses. Reports are stored outside Git under
`D:\gof2-arm64-crypto`.
