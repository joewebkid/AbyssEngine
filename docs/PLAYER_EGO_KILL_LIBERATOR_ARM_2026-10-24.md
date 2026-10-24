# PlayerEgo Liberator shutdown (Android ARM, 2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerEgo::killLiberator` at `0xa1a18–0xa1aae`. REA/IDA pseudocode was
cross-checked against the raw Thumb instructions, especially the return
register and the `0x47435000` float literal.

The original returns integer **0** for an absent gun array, empty relevant
bank or secondary item other than `0xb3`; after entering the `0xb3` loop it
returns **1**, even if no gun in the bank has that item. The recovered
method had a `void` declaration. For each matching gun, original ARM
stores `-1` through the gun's lifetime pointer at `+0x3c`, assigns
`(50000.0f, 50000.0f, 50000.0f)` to the first position through its
array pointer at `+0x0c`, and clears the active byte at `+0x4c`. The
previous source instead copied three integer `-1` bit patterns as floats,
which is not the original off-screen position. The C++ now uses the typed
`Gun` fields at those ABI-locked offsets. The 14 observed `LevelScript`
callers ignore the return value, so no call-site control flow was changed.

The source-extracted O0/O2 host smoke covers matching and nonmatching guns,
all early returns, the positive return without a matching gun, lifetime,
position and active-byte effects. Native UCRT64 `gof2` builds; **93**
verifier tests pass with one existing skip. NDK r18b compiles all **208/208**
ARM translation units. Focused comparison is **97.8% source-shape** and
**63.5% strict fuzzy**, with 63 target and 63 generated instructions and
100% ordered control-flow shape. The method remains nonexact.

The full corpus compares **4,323/4,524** original functions across
**4,736** pairs at **94.09%** mean source-shape and **83.41%** strict fuzzy.
It reports **2,557 linked-exact** and **1,082 byte-exact** pairs, unchanged
in count from the preceding checkpoint, with 201 missing dependency/runtime
symbols, zero skipped and zero wrong-type symbols. Report:
`_work/kill_liberator_full_20261024.json` (SHA-256
`2b5eeea74660037a94fc9ea64ed59243de06aade87c3616acede2f6fd63264c9`).
The preceding full report's pair identities are no longer available locally,
so equal exact counts do not by themselves prove set-wise zero losses.

The smoke uses a small mock gun layout; it does not prove linked game-frame
behavior or pointer validity for every mission state. The next priority is
to check the LevelScript-triggered shutdown in a real ARM game frame after
the Pan stage-one gate reaches ship creation.
