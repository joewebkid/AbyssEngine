# Union audit: collapse "two identical fields, different names" — keep genuine type-puns

Working dir: /Users/fionera/Downloads/GalaxyOnFire2/gof2-decomp. The codebase matches the original
libgof2hdaa.so at the symbol level — symbol parity MUST be preserved (field names are not symbols, so
renaming fields is parity-safe, but the whole tree must still COMPILE and offsets/static_asserts must
not change).

For EACH `union { ... }` in your assigned file, classify and act:

## COLLAPSE (the members are the SAME field with different names)
Criteria — ALL members are the same underlying value, not a reinterpretation:
- same type (e.g. all `int`, all `uint8_t`, all `Foo*`), OR same-size integers differing only in
  signedness (`int32_t`/`uint32_t`) used for one value;
- typically one member is a decompiler placeholder `field_0xNN` and the other is descriptive, OR both
  are descriptive but mean literally the same thing.

Action:
1. Keep ONE name — prefer the descriptive one over `field_0xNN`; if both descriptive and identical in
   meaning, keep the clearer / more-used one.
2. Delete the `union { ... }` wrapper, leaving the single kept member (same type, same offset).
3. Rename EVERY use of the dropped name(s) across all of `src/` to the kept name.
   **CRITICAL**: `field_0xNN` names are reused across unrelated classes. Before rewriting a
   `x->field_0xNN` / `x.field_0xNN` hit, verify the receiver `x` is actually THIS class (check the
   declared type / the cast). Do NOT touch same-named fields on other classes.
4. If a kept member's signedness differs from a dropped one, add a cast at the few sites that need it
   rather than keeping the union.

## KEEP (genuine type-pun — leave the union, ensure a same-line `// lint: union_decl <reason>` waiver)
The members reinterpret the SAME bytes as DIFFERENT types/representations:
- `int` vs `float` (bit reinterpret);
- a wide field (`int`/`uint16_t`) overlaid with a `struct { uint8_t ...; }` of named bytes;
- an array (`float[15]`) overlaid with a typed struct/`Matrix`;
- a pointer overlaid with an integer handle or with flag bytes;
- members of DIFFERENT sizes (`uint16_t` vs `uint8_t`).
These model the binary's real memory layout and cannot be unified by renaming. The waiver goes on the
`union {` opening line (the linter waives a criterion named as a word in a `// lint:` comment within
+-2 lines).

Do NOT run the build (the orchestrator rebuilds + checks symbol parity centrally). Report, per union:
collapsed (kept name + dropped names + #cross-file renames) or kept-as-type-pun (one-line reason).

## Follow-up: is each union a DECOMPILER ARTIFACT or a GENUINE binary overlay?

DECOMPILER ARTIFACT (FIX — collapse to the one real field, rewrite the few divergent uses):
- A real COMPOSITE type (Vector / Matrix / a named struct) overlaid with scalar/component views that
  exist ONLY to bulk-init/zero it — e.g. `union { Vector v; struct { int vX, vY, vZ; }; }` where vX/
  vY/vZ are only ever set to 0. The int-bit zero equals the float 0.0f, so the component struct is a
  decompiler view for "zero the Vector". FIX: drop the scalar view, keep the composite, rewrite the
  init to the composite (`v = Vector();` / `v.x = v.y = v.z = 0;` / memset).
- A named struct (the real, full-width type) shadowed by a redundant scalar sub-view of its first
  member (the Blk16 case) — keep the struct, rewrite the scalar use as `.member`.

GENUINE OVERLAY (KEEP, with an accurate same-line `// lint: union_decl` reason):
- int<->float where BOTH the integer value AND the float value are genuinely computed/used as their
  types (not merely zeroed): empPoints/empPointsF, rank/rankBits, touchX/touchXf, fixed-point, etc.
- a flag WORD (`int`/`uint16`) that is cleared/compared as a word AND whose individual named flag
  BYTES are set/read — both access widths are real (faithful overlay).
- a pointer slot reused as an int handle or flag bytes in a different object state.
- an equal-size raw-array vs typed-class view where BOTH are load-bearing and the typed side is
  layout-sensitive (Matrix) — collapsing is high-risk; keep.

When in doubt, KEEP and say why. Verify the receiver type before any cross-file rename (field_0xNN /
component names are reused across classes). Do NOT run the build — the orchestrator rebuilds + checks
symbol parity centrally. Report per union: artifact (what you did) or genuine (one-line reason).

## FINAL DIRECTIVE: ELIMINATE EVERY UNION (the original source has none)

Ghidra invents a `union` whenever one offset is accessed as >1 type; the original C++ used a single
field + an explicit cast at the specific sites. Remove ALL `union`s. Result must have ZERO `union`
keywords, identical byte layout, identical behavior, and still compile + hold symbol parity.

Per pattern:
1. `union { Wide w; struct { sub-fields... }; };`  (a wide view over named sub-fields)
   -> promote the sub-fields to plain members at the same offsets (drop the union+struct wrappers).
      Rewrite every `w` access as `reinterpret_cast<Wide &>(firstSubField)`.
      e.g. `{int field_0x5c; struct{uint8_t pauseOpen,cutsceneActive,jumpActive,_pad;};}`
        -> `uint8_t pauseOpen, cutsceneActive, jumpActive, _pad_0x5f;`
           and `x->field_0x5c`  ->  `reinterpret_cast<int &>(x->pauseOpen)`.
2. `union { A a; B b; };`  (same offset, different types; A = the semantic value-type)
   -> keep `A a;`, rewrite each `b` use as `reinterpret_cast<B &>(a)`.
      float/int: keep the float, `rankBits` -> `reinterpret_cast<int32_t &>(rank)`.
      pointer/int-handle: keep the pointer, `field_14c` -> `reinterpret_cast<int32_t &>(voidStation)`.
3. `union { A a; A b; };`  (same type, two names) -> keep one, RENAME the other's uses (no cast).
      Verify the receiver is THIS class first (field_0xNN / flag names are reused across classes).
4. `union FlagWord { uint32_t word; uint16_t halfword; uint16_t halfwords[2]; uint8_t bytes[4]; };`
   -> delete the FlagWord type. Each `FlagWord` member becomes the underlying unioned slot (the
      pointer/int it overlaid). Rewrite:
        x.word         -> reinterpret_cast<uint32_t &>(slot)
        x.halfword     -> reinterpret_cast<uint16_t &>(slot)
        x.halfwords[n] -> reinterpret_cast<uint16_t *>(&slot)[n]
        x.bytes[n]     -> reinterpret_cast<uint8_t  *>(&slot)[n]
5. arrays `union { T arr[N]; named...; }` (Matrix m[15]/named, Globals arrays):
   keep the array as canonical, rewrite named-element uses as `arr[index]` (cross-file rename, verify
   receiver). Matrix is layout-critical: keep `float m[15]`, map m11_rightY->m[1], m12_upY->m[2], etc.

HARD CONSTRAINTS:
- Identical byte layout: keep every named sub-field at its existing offset; do NOT add/remove bytes.
  Add a `static_assert(__builtin_offsetof(Class, firstKeptMember) == 0xNN, "")` (guard with
  `#if __SIZEOF_POINTER__ == 4`) for each former-union site AND the field immediately after it.
- Identical behavior: reinterpret_cast preserves bits; an int-write of a value equals the union write.
- Verify receiver type on every cross-file rename/rewrite.
- Do NOT run the build (orchestrator rebuilds + checks parity + offsets centrally).
Report per union: how you eliminated it (kept field + rewritten sites).
