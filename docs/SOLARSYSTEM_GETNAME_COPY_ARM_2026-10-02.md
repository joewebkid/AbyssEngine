# SolarSystem::getName string-copy ABI — Android ARM, 2026-10-02

Android HD `libgof2hdaa.so` at `0x1556f0` returns the name using
`AbyssEngine::String(const String&, bool)` with `reverse=false`. The
extracted `SolarSystem.c` shows this constructor call. The former C++
`return this->name` chose the implicit one-argument copy constructor;
the focused ARM diff showed the missing `movs r2, #0` argument. The
accepted one-line change explicitly selects the observed overload.
Both local constructors copy the same text when `reverse` is false;
no string implementation or caller changed.

Focused source-shape improved **93.0% → 100%** and strict fuzzy
**90.9% → 100%**; this function became linked-exact but not byte-exact.
The native UCRT64 `gof2` target rebuilt, and **204/204 ARM translation
units** compiled. The full corpus compared **4,620 rows**, covering
**4,274/4,524** original symbols with **0 skipped**. Linked-exact rose
**2,087 → 2,088**, byte-exact remained **960**, and a complete
`(unit, mangled symbol)` comparison found **zero lost exact pairs**.
The only gained pair is `SolarSystem::getName()`. Missing symbols remain
250 (7 wrong-signature, 243 absent). Local reports:
`_work/blueprint-isunlocked/after_solarsystem_getname.json`,
`_work/blueprint-isunlocked/solarsystem_destructor_full.json`, and
`_work/blueprint-isunlocked/solarsystem_getname_full.json`; generated
reports are not committed.

Native gameplay execution was not tested. The `SolarSystem` constructor
remains substantially non-exact and needs a separate ownership and
parameter-layout review against Android and available PC/iOS sources.
