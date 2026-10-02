# ARM verifier: section-qualified ctor/dtor aliases — 2026-10-03

The seven symbols previously listed in `missing_wrong_type.txt` were not
wrong C++ signatures. All were `Array<T>` C2/D2 constructor/destructor
variants: the local ARM object symbol table contains each exact mangled
name, paired at the same section address with its C1/D1 sibling. GNU
`objdump` labels only one sibling when disassembling. These weak template
functions are in individual `.text.<mangled>` sections, not plain `.text`.

The verifier's alias pass formerly accepted only `.text` symbols and
keyed labels by address alone. Since every COMDAT text section begins at
address zero, that also risks associating a symbol with a body from a
different section. `asmdiff.disassemble()` now keys a disassembly label
by `(section, address)` and accepts both `.text` and `.text.*` function
symbols. It does not synthesize aliases that are absent from the object
symbol table. A direct check on `ArrayInstantiations.o` confirmed the
C1/C2 and D1/D2 variants point to the same parsed body, while two
different sections at address zero remain distinct. `py_compile` passed.

The full ARM corpus was rerun with `--fail-on-wrong-type`; it exited
successfully with **0 wrong-signature** and **0 skipped** units. Against
the previous report, comparison rows rose **4,620 → 4,632** and covered
original symbols **4,274 → 4,281 / 4,524**; the 243 genuinely absent
symbols are unchanged. Linked-exact rows rose **2,092 → 2,113**,
byte-exact **960 → 965**, with **zero lost exact pairs**. Of the 21 new
linked-exact rows, 15 were existing rows now attributed to the correct
section body, and 6 were among 12 newly compared alias rows. The five
new byte-exact rows were already compared previously. Mean source-shape
rose **89.05% → 89.85%** and strict fuzzy **76.06% → 76.84%** solely from
measurement correction, not new game-source recovery. Local reports:
`_work/blueprint-isunlocked/solarsystem_runtime_roots_full.json` and
`_work/blueprint-isunlocked/alias_sections_full.json`.

The native `gof2` build stayed green and **204/204 ARM translation units**
compiled. No game source or original binary changed. Some newly visible
`Array<T>` constructors and the `Array<bool>` destructor remain
non-exact; their code quality must be assessed separately. The next
priority for actual coverage is a source-backed subset of the 243
genuinely absent symbols, not artificial C2/D2 wrappers.
