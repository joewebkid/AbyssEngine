# Main-menu GOF2 start dispatch (2026-11-05)

The original Android ARMv7 `MenuTouchWindow::startGOF2` begins at
`0x12cf84` in `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
Its Thumb instructions and ELF relocations establish the following
sequence:

1. `Globals::status` (`GOT 0x21015c`) calls `Status::resetGame`.
2. `this+0x1a4` is stored at `Globals::options+0x2c`.
3. `Globals::sound` (`GOT 0x210168`) supplies its first word, the
   current music event, to `FModSound::stop(int)`.
4. The same sound object calls `FModSound::play(0x8f, nullptr,
   nullptr, 0.0f)`.
5. `Globals::appManager` (`GOT 0x20fd50`) calls
   `ApplicationManager::SetCurrentApplicationModule(2)`. The terminal
   ARM/Thumb veneer at `0x19b8f8` resolves to PLT `0x61d64`, whose
   GOT slot `0x211864` has that method's relocation.

The ARM32 reconstruction had called an undefined stop helper, treated
its result as the playback pitch, and invoked a function pointer from
the application manager object's first word. The source now calls the
identified methods directly on both architectures. `Globals::init`
constructs and assigns the sound object before menu startup. The
original path has no null-sound branch.

Focused ARMv7 verification compiles **209/209** translation units and
raises source-shape **84.8% → 97.3%**, strict fuzzy **60.9% → 77.3%**.
The method remains non-exact. Windows native and ARM64 static builds
pass. This revision has not been exercised on a device; successful
source and link checks do not establish a working game transition.

Strict ARM64 shared linking passes with zero undefined symbols and
53 JNI exports. Full ARMv7 verification compares **4,323/4,524**
original functions in **4,770** pairs: mean source-shape **96.79%**
(previously 96.78%), strict fuzzy **86.78%**, **2,782 linked-exact**,
**1,108 byte-exact**, 201 missing, zero skipped and zero wrong-type
matches. No comparison pair or exact-match member was lost.
