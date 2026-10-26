# PaintCanvas font-query call recovery (2026-10-26)

The original Android ARMv7 `PaintCanvas` calls `String::operator const
unsigned short*` and the existing `ImageFontGetWidth` overloads,
`ImageFontGetHeight`, and `ImageFontCheckString`. The local IDA
extraction locates the callers at `0x7842c` (whole-string width),
`0x7c2fc` (range width), `0x7c33c` (height), and `0x78194`
(`CheckString`); the string conversion at `0x725cc` returns the
UTF-16 data pointer at offset `+4` on ARMv7. The existing `ImageFont`
functions and `String` conversion are implemented in the repository.

Four `PaintCanvas` methods now call those functions directly, replacing
external placeholder names. The old whole-string and `CheckString`
calls cast the text pointer through `unsigned int`, which truncates it
on AArch64; their signatures are now pointer-sized and typed. The
focused ARMv7 verifier compares **198** `PaintCanvas` symbols and
retains all **93 linked-exact / 23 byte-exact** matches. Each of the
four changed methods remains **100% source-shape, 100% strict fuzzy,
linked-exact**. The native and AArch64 static libraries build.
The full ARM corpus compares **4,323/4,524** original functions over
**4,745** pairs: **94.76% source-shape / 84.27% strict fuzzy**,
**2,606 linked-exact / 1,083 byte-exact**, zero skips and no exact-set
losses versus the previous revision.
Strict JNI-reachable AArch64 unresolved names fall **157 → 153**,
with four font placeholder names removed and none added.

The original `ImageFontCheckString` is itself a count loop without
glyph validation; connecting it does not prove broader text rendering
correctness. Live Android text layout, non-BMP characters and actual
font-resource loading remain unverified. The shared library still does
not link, and compatible audio plus further call closure remain open.
