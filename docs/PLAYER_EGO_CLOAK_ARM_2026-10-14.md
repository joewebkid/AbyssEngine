# PlayerEgo cloak activation (2026-10-14)

Primary evidence is Android Thumb
`PlayerEgo::toggleCloaking` at `0x9a708–0x9a9b0` in the 2.0.16
`libgof2hdaa.so`, cross-checked against IDA lines 81068–81226.
The method's two branches were already present, but several
arguments and one global source were wrong.

The entry path now reads cloak attribute **38**, compares it with
cargo item 122, removes exactly that many units, and passes the
amount to HUD event 30 before event 28. The activation path now
plays sound 30 through `Globals::sound`, rather than dereferencing
the uninitialized `g_PE_tc_sound` pointer. It uses `Globals::Canvas`,
sets the hull and optional turret materials to mode 14, and sends
shader animation channels **1 then 2** for each mesh. The native
material resource selection for turret equipment 224, 198 and 199
remains; its output slot is initialized as in ARM.

The extracted production-method smoke exercises insufficient and
sufficient cargo, exact cargo removal, the HUD event arguments,
charge gating, hull-only activation, and turret material/shader
updates including resource 199. Native `gof2`, 206/206 ARM
translation units, and 37 verifier tests (one skipped) pass.

Focused `toggleCloaking` source-shape rises **92.5% → 94.2%**,
while strict fuzzy falls **56.5% → 23.0%**. Instruction inventory
improves **85.3% → 87.5%**, call-count match improves
**96.7% → 100%**, and generated/target instruction counts are
**237/241**. This is a behavior-backed correction, not an exact
match or a claim that every code-order difference is resolved.

Full report `_work/player_cloak_full_20261014.json` compares
**4,323/4,524** original functions, with **201 missing**, zero
skipped and **123 extras**. Average source-shape remains **93.39%**;
strict fuzzy changes **82.29% → 82.28%**. The **2,482
linked-exact** and **1,065 byte-exact** pair sets lose and gain
zero members against `_work/player_render_full_20261014.json`.

Live ARM32 cloak behavior, sound playback and shader appearance
remain unverified. Next priority: audit `setShip` cloak-material
creation and ownership, then return to residual `toggleCloaking`
instruction ordering only where ARM evidence supports a semantic
or ABI correction.
