# SolarSystem constructor name lifetime — Android ARM, 2026-10-02

## Evidence and accepted form

Android HD `libgof2hdaa.so` at `0x1555dc` default-constructs the member
name, stores `systemId`, constructs a temporary copy of the incoming
name through `AbyssEngine::String(const String&, false)`, assigns that
temporary to the member, and destroys the temporary. Both the IDA
extraction in `gof2_recovered_sdk_first_pass/extracted/world_level_missions/SolarSystem.c`
and the independent Ghidra listing in
`analysis/gof2_gidra_engine_full_source.c` show this order. The prior C++
assigned the parameter directly before storing `systemId`. The accepted
change uses `String(displayName, false)` and moves the independent
`systemId` assignment before it. The remaining scalar, RGB, and array
pointer assignments were not changed. No separate iOS/PC constructor
source was established for this step.

## Verification and limits

For the C1 constructor, focused ARM source-shape improved **55.2% →
99.1%** and strict fuzzy **25.7% → 96.4%**. The intermediate copy-only
form was 98.6% / 73.9%; restoring the field order gave the final result.
C1 remains non-exact: the residual disassembly difference is grouping of
independent scalar stores (69 target instructions versus 70 locally).
Generated focused reports are
`_work/blueprint-isunlocked/after_solarsystem_ctor_copy.json` and
`_work/blueprint-isunlocked/after_solarsystem_ctor_order.json`.

The C2 comparison row remains **16.6% source-shape / 10.1% strict**.
`llvm-nm -S` reports C2 in the local object at the same address as C1
with size **0**, whereas the target C1/C2 symbols share a 188-byte body.
The verifier presents only 10 local instructions for C2, so this row
cannot establish a separate C2 body regression. The constructor is not
claimed exact, and the alias/reporting issue needs a separate verifier
review before interpreting C2 as source quality.

The native UCRT64 `gof2` target rebuilt, and **204/204 ARM translation
units** compiled. Full corpus comparison covered **4,620 rows** and
**4,274/4,524** original symbols with **0 skipped**. Linked-exact stayed
**2,088**, byte-exact **960**; complete `(unit, mangled symbol)` set
comparisons found **zero lost exact pairs**. Mean source-shape rose
**89.02% → 89.03%** (rounded), and mean strict fuzzy **76.00% → 76.02%**.
The 250 missing symbols remain 7 wrong-signature and 243 absent. Local
full reports are `_work/blueprint-isunlocked/solarsystem_getname_full.json`
and `_work/blueprint-isunlocked/solarsystem_ctor_full.json`; generated
reports are not committed.

Native gameplay execution, exception-path behavior, and allocator
instrumentation were not tested. A next behavior-focused target is
`SolarSystem::hasPirateBase()` or `hasHiddenBlueprint()`, both currently
well below 100% source-shape; the C2 verifier alias issue should be
addressed separately from game-source reconstruction.
