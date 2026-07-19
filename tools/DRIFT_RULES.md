# Drift fix rules — make every field_0xNN land at its named offset 0xNN

A `field_0xNN` (or byte_0xNN/flag_0xNN/image_0xNN/pad_0xNN) name encodes the field's TRUE offset in
the original binary (Ghidra-derived). If our struct puts it elsewhere, our layout differs from the
original and accesses hit the wrong byte. Fix the layout so each named field sits at its name.

## Method
1. Get TRUE offsets with the template trick (works for standard-layout classes):
   `template<int N> struct S; S<(int)__builtin_offsetof(Class, field_0xNN)> a;` then
   `clang++ -std=c++14 -Isrc -Ithird_party/fmod/inc -Ithird_party/gl -Ithird_party/jni
    -Ithird_party/libzip -Ithird_party/crypto -m32 -include cstdint -fsyntax-only x.cpp 2>&1`
   The `S<NNN>` in the error is the actual decimal offset.
2. Walk the struct top-down. For each named field, compare actual vs named:
   - actual < named  -> insert `uint8_t _pad_0xXX[named-actual];` immediately BEFORE it (missing bytes
     upstream). Often one pad fixes a whole run of trailing fields (they're all short by the same N).
   - actual > named  -> a PRIOR field is over-sized. Infer the correct size from the gap to the NEXT
     named field (consecutive named offsets give field sizes), and shrink that field's type
     (e.g. int->uint8_t) OR split it. Do NOT just delete bytes blindly.
   - For matrix-embedding structs: our AEMath::Matrix is 0x3c but the original often reserves 0x40 for
     a matrix region; add 4 bytes padding AFTER the matrix member (local fix; never change Matrix.h).
3. PRESERVE total size where a trailing/region size is known; do not shift fields that are already
   correct. Verify with the template trick that EVERY named field now matches AND that fields you
   didn't intend to move didn't move.
4. Keep code compiling: if you rename/resize a field that is USED, update use sites (verify receiver
   types; field_0xNN names are reused across classes). Prefer inserting padding (no use-site changes)
   over retyping where possible.
5. Add a guarded static_assert block after the class:
   `#if __SIZEOF_POINTER__ == 4` ... `static_assert(__builtin_offsetof(Class, field_0xNN)==0xNN,"");`
   for every former-drift field plus the next field, then `#endif`.

## Verify (self-check before reporting)
- Re-run the template trick: all named fields match their names.
- `python3 tools/drift_scan.py 2>/dev/null | grep YourClass` -> no rows.
- Do NOT run the full build (orchestrator does that + parity + ASM no-degradation centrally).
Report per field: actual->named and how you fixed it (pad N bytes / retyped X).
