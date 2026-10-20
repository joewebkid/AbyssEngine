# PlayerFighter cloak effect and exhaust recovery (Android ARM, 2026-10-20)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
`PlayerFighter::setExhaustVisible` is at `0xdcd24–0xdcd58`; `handleCloaking`
starts at `0xdcdbc`. The independently recovered IDA listing agrees with the
ARM child-transform selection, material clone/change calls, 2,000 ms fade
branches, and bounded `AERandom::nextInt(100)` calls.

The prior C++ dereferenced a null placeholder for the exhaust transform and
used an invalid 32-bit pointer round trip. Cloaking divided by zero, called
unresolved `PF_cloakApply`/`PF_cloakStart`/`PF_cloakStop` shims, and used an
unbounded random result in place of the original 100-way draws. The restored
path calls `Globals::Canvas` for transform visibility, material cloning and
shader animation, restores the saved material mode at expiry, and calls the
real `PlayerFighter::cloak(-1, false)` for both random activations.

`tools/run_player_fighter_cloak_effect_smoke.py` compiles the production
methods with recording services at `-O0` and `-O2`. It checks gating,
main/subgeometry transform selection, absent transform handling, material
clone/change order, fade-in and fade-out values, exhaust visibility, mode
restoration, first-frame skip and both bounded random thresholds. The existing
reset/cloak smoke, native UCRT64 `gof2` build, 208 ARM translation units, and
93 verifier tests (one existing skip) also pass. Focused source-shape/strict
fuzzy changes are `handleCloaking` **87.5%/50.1% → 90.6%/50.6%** and
`setExhaustVisible` **93.6%/81.8% → 94.5%/86.4%**; neither is exact.

The full NDK r18b corpus compares **4,323/4,524** original functions across
**4,735** pairs, with **94.02%/83.36%** mean source-shape/strict fuzzy,
**2,557 linked-exact** and **1,082 byte-exact**. There are no skipped or
wrong-type symbols. The only source-shape changes are the two methods above;
identity-by-identity comparison finds zero linked- or byte-exact losses or
gains. Full report SHA-256:
`813606d91ee9a9bad5481d783f2338052fea498a96d3e412c23526c20a874b3f`.

This is a source-backed control and call-path recovery, not a proof of the
visual result in a complete game frame. The downstream Canvas shader-effect
implementation, actual mesh/material ownership and the unresolved
`PF_update_body` remain separate verification priorities.
