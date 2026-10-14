# PlayerEgo pitch/yaw ramp recovery (2026-10-14)

Android Thumb `right` `0xa08a2–0xa08f2`, `down`
`0xa0d58–0xa0da4`, and `up` `0xa0fc6–0xa1012` is the primary
evidence, cross-checked with the IDA methods and the options layout.
The formerly declared-only `PE_pitchRampDelta` and
`PE_yawRampDelta` paths now use the original settings record:
`Globals::options+0x11` selects the float at `+0x14` or `+0x18`.

With cursor steering active, yaw takes the lesser of
`rate * 25 / 6` and 280, while pitch uses `rate * frameTime / 12`.
Otherwise yaw divides `rate * frameTime` by
`(3.3 - sensitivity) * 20`. Pitch applies the same denominator,
but multiplies the `+0x18` sensitivity by 1.45 for down or 1.25
for up. The helpers are forced inline because the original has
these calculations inside each control method. ARM `0xa088c`
also stores the unadjusted handling value (negated) into `yawRamp`
on right-turn input; the previous source stored the load-adjusted
rate.

The production-helper host smoke checks both option branches,
both pitch directions, cursor mode and the yaw cap. Native `gof2`,
206/206 ARM translation units and 37 verifier tests (one skipped)
pass. Across `right`, `left`, `down` and `up`, focused average
source-shape/strict fuzzy changes **80.3%/58.9% → 88.0%/69.8%**;
none is exact.

Full report `_work/player_axis_ramp_full_20261014.json` compares
**4,323/4,524** original functions at **93.37%** average source-shape
and **82.26%** strict fuzzy, with **2,482 linked-exact**,
**1,065 byte-exact**, **123 extras**, **201 missing** and zero skipped.
The linked- and byte-exact pair sets have no losses or gains against
`_work/player_approach_asteroid_stage0_full_20261014.json`.

This does not prove live input feel or ARM32 runtime. The right-turn
`yawRamp` store is ARM-backed but not separately exercised by the
host helper smoke. Next priority: the declared-only turret placement
and material helpers in `checkForTurret`.
