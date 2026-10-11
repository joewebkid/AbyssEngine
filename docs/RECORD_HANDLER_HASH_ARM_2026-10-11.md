# RecordHandler hash inputs — Android ARM evidence (2026-10-11)

The recovered `addHash`, `checkHash`, and `addHashToOptions` used three
zero-filled 25-byte salts and dereferenced three null key-pointer aliases.
They could neither produce the original save/options signatures nor safely
execute. The original Android functions at `0xcdea0`, `0xcd7b0`, and
`0xcdafc` instead feed SHA-256 with file bytes, a common 25-byte salt,
and 16 bytes from the JNI-owned `g_android_origami_super_club` pointer.
Each ARM function loads lengths 25 and 16 at its two final SHA updates.

The salt is the 25 bytes at ELF `0x1fb520`, ending in NUL. A shared
constant now matches those exact bytes, and all three paths pass the
JNI-owned pointer directly. This changes no file layout or digest order.
The generated ARM object's `.rodata` matches the original bytes; its
relocations contain three salt references and three hash-path references
to the JNI global (plus two references in the JNI setters). A compile-time
assert checks the salt length.

Native `gof2` and 206/206 ARM translation units build. All 36 verifier
tests pass (one skipped). Focused source-shape rises from 81.0% to
82.3%; strict fuzzy rises from 42.5% to 50.0%. None of the three
functions is exact. The SHA input sequence is confirmed statically,
but a real Android save/load round-trip was not run. In particular,
the original code expects the JNI key to have been installed and to
contain at least 16 readable bytes; this source does not invent a key
or define what native-only gameplay should do when it is absent.

The complete ARM corpus compares 4,323/4,524 original functions at
92.05% average source-shape and 80.13% average strict fuzzy, with
2,325 linked-exact and 997 byte-exact. There are no skipped units or
wrong-type symbols; pairwise comparison with the preceding full report
found no lost exact matches.
