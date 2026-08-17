# TouchButton Array Release ARM ABI

Date: 2026-08-17

## Scope

This package restores the out-of-line Android ARMv7 specialization of
`ArrayReleaseClasses<TouchButton *>`. The ownership loop was already correct,
but exposing its generic definition at each call site allowed Clang to inline
it into owners such as `Hud::~Hud`.

The Android binary instead exports and calls:

`_Z19ArrayReleaseClassesIP11TouchButtonEvR5ArrayIT_E`

at `0x000d923c`.

## Source Shape

`TouchButton.h` now declares the explicit specialization before UI owners can
implicitly instantiate the generic template. `ArrayInstantiations.cpp` owns
the single specialization body. The loop keeps the already verified native
behavior: delete every non-null entry, clear each slot, release the storage,
then clear the data pointer.

ARM symbol tables confirm the intended linkage:

| Object | Symbol type |
| --- | --- |
| `engine/core/ArrayInstantiations.o` | `T` (strong definition) |
| `game/ui/Hud.o` | `U` (out-of-line reference) |

The specialization itself is `100.0%` linked-exact with `29/29` normalized
instructions. Raw bytes differ only because the unlinked objects contain
different relocation values.

## Hud Result

Restoring the call boundary changes `Hud::~Hud` from `72.7%` with an inlined
`80/52` instruction shape to `97.5%` linked-exact with matching `80/80`
instruction counts. The remaining difference is limited to two unwind/LSDA
tail instructions, so byte-match is not claimed.

Across all 48 verified `Hud` functions:

- average similarity rises from `66.9%` to `67.9%`;
- linked-exact functions rise from 18 to 20;
- raw-byte-exact functions remain 15;
- `Hud::closeHudMenu` remains `93.8%`.

No extra `ArrayRemoveAll` call was reintroduced to manipulate similarity. The
destructor retains the ownership sequence recovered from Android.

## Validation

- UCRT64 native build links `libgof2.a` successfully.
- ARM corpus builds `201/204` translation units.
- The same three unrelated `SolarSystem *` versus integer system-index errors
  remain.
- The specialization is `100.0%` linked-exact.
- `Hud::~Hud` D1 and D2 are both `97.5%` linked-exact.

