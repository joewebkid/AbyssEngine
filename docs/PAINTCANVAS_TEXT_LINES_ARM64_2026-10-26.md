# PaintCanvas text-line recovery for AArch64 (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The local IDA dump identifies `PaintCanvas::GetLine` at `0x7c354` and
`GetLineArray` at `0x7c488`. The latter appends a newline, calls `GetLine`
twice per segment (counting, then filling), advances by each segment's
length, allocates an output `String` for every segment, and trims spaces.

The recovered C++ used 16-byte raw buffers for temporary `String`
objects and passed the entire input from offset zero on every pass.
The raw buffers were too small for the 64-bit `String`, and repeated
substrings could return the first line more than once. Both functions
now use typed `String` objects, real `GetLine`/`GetTextWidth` calls,
and `ArraySetLength<String *>`. The second pass advances its offset
before trimming so it consumes exactly the same untrimmed spans as
the counting pass. Empty output after trimming is explicitly cleared
because the current `String::operator=` ignores a null data pointer.
An empty segment advances one character to keep the loop finite.

Before this change, the focused source-shape was **78.9%** for
`GetLineArray` and **81.1%** for `GetLine`; strict fuzzy was **26.2%**
and **43.3%**. An initial focused ARM verification of the typed rewrite
measured **89.9% / 97.1%** source-shape and **46.3% / 79.2%** strict
fuzzy. The final empty-string edge-case change is included in the
full-corpus run recorded below.

ARM64 and native static-library builds pass. The strict JNI-initialize
ARM64 link diagnostic falls from **51 to 39** unresolved names, removing
12 obsolete `PaintCanvas` text shims with no new names. The final
`GetLineArray` empty-string guard yields **84.8% source-shape / 46.4%
strict fuzzy**; `GetLine` yields **97.1% / 79.2%**. The full ARM corpus
compares **4,323/4,524** originals over **4,745** pairs, with **94.79%
source-shape / 84.31% strict fuzzy**, **2,607 linked-exact / 1,083
byte-exact**, and zero skips. Comparing the exact sets by unit and
symbol shows **zero lost** linked-exact or byte-exact functions. A
game `.so` and APK remain unbuilt.

The IDA decompilation does not precisely expose every index and trim
boundary in the original loops. Whitespace layout, wrapping and live
rendering have not been checked on Android. The accepted code preserves
the observed two-pass structure and a safe 64-bit object lifetime;
the raw-buffer and zero-offset forms were rejected as incorrect.
