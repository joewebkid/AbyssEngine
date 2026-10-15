# String numeric constructors (Android ARM, 2026-10-15)

## Instruction evidence and change

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `String(char)` at `0x6e484`, `String(int)` at `0x737b4`, and
`String(long long)` at `0x72904` (C2 constructor symbols). REA/IDA Thumb
instructions show each constructor writing the vtable and a null data
pointer with `STRD`, then calling `String::Set(long long)`. None writes
`length` before that call. The `int` constructor uses `ASRS #31` for the
high word; the `char` constructor passes a zero high word.

The recovered constructors previously initialized `length` redundantly.
They now initialize only `data` before `Set`; both its zero and nonzero
paths assign `length` before use. The `char` constructor also converts
through `unsigned char`, preserving the Android unsigned-byte behavior on
native hosts where plain `char` is signed. `String(long long)` calls `Set`
directly instead of an inlined forwarding shim. The native ABI/lifetime
smoke checks `0xff → "255"`, `INT_MIN`, `INT64_MIN`, `INT64_MAX`, and zero.

## Focused and native checks

Focused source-shape/strict fuzzy changes for the C2 symbols are:

| Constructor | Before | After | Exact result |
| --- | --- | --- | --- |
| `String(char)` | 97.6% / 97.0% | 100% / 100% | linked-exact, not byte-exact |
| `String(int)` | 97.6% / 97.0% | 100% / 100% | linked-exact, not byte-exact |
| `String(long long)` | 97.3% / 96.6% | 100% / 100% | linked-exact, not byte-exact |

Native `gof2`, the ABI/lifetime smoke, standalone String reverse check,
**206/206** ARM translation units and **37** verifier tests (one skipped)
pass. Focused reports are in the shared workspace `_work/` directory as
`string_numeric_ctors_{before,after}_20261015.json`.

## Adjacent 64-bit append audit

Android `String::operator+=(long long const&)` (`0x7289c–0x728fa`)
constructs a temporary `String`, checks its length, grows the receiver with
`realloc`, copies the temporary digits, destroys the temporary, and returns
the receiver. The assembly does not explicitly write a terminator after
the copied digits. This does not establish the resulting byte's runtime
value; it depends on the allocator state and has not been observed live.

The recovered receiver is owned through `new[]`/`delete[]`, so passing it
to `realloc` would violate its allocator contract. Two safe direct-append
experiments were rejected: loops changed focused source-shape/strict fuzzy
from **81.2%/61.1% to 60.4%/40.0%**, and `memcpy` to **59.2%/31.2%**.
The production implementation was restored unchanged. Additional native
smokes check append to an empty receiver, zero as a no-op, and an explicit
terminator after `INT64_MIN`. These validate the safe port; they are not a
claim that the original always terminates the appended string.

## Full-corpus gate and limits

The fresh shared-workspace report `_work/string_numeric_ctors_full_20261015.json`
compares **4,323/4,524** unique original functions across 4,720
comparisons, with **201 missing**, **123 extras**, zero skipped and zero
wrong-type. Average source-shape/strict fuzzy remains **93.59%/82.63%**
at two-decimal resolution. Against `_work/string_append_int_full_20261015.json`,
the linked-exact `(unit, symbol)` set gains only the three C2 constructors
and loses none: **2,500 → 2,503**. The byte-exact set remains **1,074**
with zero losses.

Live Android ARM32 constructor behavior, exception paths and
allocator-dependent append bytes remain unverified. A controlled ARMv7
observation of numeric append boundaries is the next behavioral priority;
iOS results must be labeled as a different version rather than Android
2.0.16 proof.
