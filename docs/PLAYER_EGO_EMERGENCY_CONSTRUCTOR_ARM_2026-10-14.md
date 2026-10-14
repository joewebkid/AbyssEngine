# PlayerEgo emergency-state construction (2026-10-14)

Android Thumb `PlayerEgo::PlayerEgo(Player*)` in the 2.0.16
`libgof2hdaa.so` stores zero at `+0x314/+0x318/+0x31c`
(`0x99946–0x9994c`) and `+0x30c/+0x310/+0xac`
(`0x99a82–0x99a86`). Android IDA lines 80122–80124 and
80215–80217 independently show those vector, timer, duration and
geometry-pointer initializations. The C++ constructor omitted all six
stores. They now precede `setShip`, which may create the geometry for
emergency equipment at Android Thumb `0x9a3f6–0x9a448`.

The extracted production-constructor fragment smoke starts from
nonzero field values and confirms all six become zero. Native `gof2`,
206/206 ARM translation units, the docking/emergency, cloak and
setShip smokes, and 37 verifier tests (one skipped) pass. Focused
source-shape for both constructor aliases rises **44.8% → 45.1%**;
strict fuzzy rises **28.2% → 28.3%**. Neither alias is exact.
Full report `_work/player_emergency_ctor_full_20261014.json`
compares **4,323/4,524** original functions with **201 missing**,
**123 extras** and zero skipped. Corpus averages remain **93.39%**
source-shape and **82.28%** strict fuzzy. The **2,482 linked-exact**
and **1,065 byte-exact** pair sets lose and gain zero members versus
`_work/player_emergency_full_20261014.json`.

The original `PlayerEgo` destructor at `0x9a0a4` does not read or
release `+0xac`; Android IDA lines 80554–80684 and its ARM body agree.
The only confirmed deletion is the active effect's expiry path in
`update` at `0x9e716–0x9e748`. The current C++ call site constructs
a new ego before `setShip`; whether another lifecycle can discard an
unused effect or transfer its ownership is not established. No
unsupported destructor cleanup was added.

This smoke exercises the extracted constructor fragment, not the full
constructor or live ARM32 runtime. Further constructor recovery and
the `tryToStartEmergencySystem` transition are the next priorities.
