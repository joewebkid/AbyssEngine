# PlayerEgo turret construction and stream return (2026-10-14)

Primary evidence is Android Thumb `PlayerEgo::checkForTurret` at
`0x9b340–0x9b778`, both `addGun` wrappers at `0x9b778–0x9b7a4`,
and `dockToStream` at `0xa1732–0xa1772` in the 2.0.16
`libgof2hdaa.so`. IDA lines 81855–82099 and 86891–86914
independently expose the construction and stream-return flow.

`checkForTurret` now queries gun sort 2 and item attribute 17,
constructs the confirmed mesh table, and translates the yaw geometry
by the item-specific offset. Temporary mesh nodes attach by their
transform handles; the retained extra node uses its own handle for
visibility. The root transform receives the original 1,000,000
bounding radius, the yaw transform receives animation state 2, and
both base and yaw rotate by 3.1416 except for indices 198–200.
Cloned cloak materials go to `PlayerEgo+0x38c/+0x390`.

The prior source wrote these objects to misleading aliases at
`+0xf0/+0x34/+0x38/+0x3c`. ARM instead stores the base, yaw, root,
and retained extra at `+0xdc/+0x28/+0x2c/+0x30`. ARM32 assertions
now lock these addresses, including the previously misplaced base
pointer. The true `+0xdc` turret base exposed the adjacent
`dockToStream` error: original stream-return coordinates are read
from `+0xe0/+0xe4/+0xe8`, followed by `setAutoPilot(nullptr)`;
the former source used `+0xdc/+0xe0/+0xe4` and a geometry-visibility
call. Both `addGun` overloads now call `checkForTurret` after adding
the gun, as the original tail branches do.

The extracted production-method host smoke exercises both `addGun`
overloads, no available gun, automatic turret indices 181/182,
compound indices 200/224, child transform ownership, animation and
visibility, cloak material outputs, repeated calls, and the stream
return position/autopilot tail. Native `gof2`, 206/206 ARM translation
units, and 37 verifier tests (one skipped) pass.

Focused source-shape/strict fuzzy changes:

| Symbol | Before | After |
| --- | ---: | ---: |
| `checkForTurret` | 86.5% / 42.3% | 95.2% / 57.5% |
| `dockToStream` | 81.4% / 58.3% | 100.0% / 95.2% |
| `addGun(Gun*, int)` | 90.7% / 82.4% | 100.0% / 100.0% |
| `addGun(Array<Gun*>*, int)` | 90.7% / 82.4% | 100.0% / 100.0% |

Full report `_work/player_check_turret_full_20261014.json` compares
**4,323/4,524** original functions, with **201 missing**, zero
skipped and **123 extras**. Average source-shape changes
**93.37% → 93.38%**, strict fuzzy **82.26% → 82.28%**. The
**2,482 linked-exact** and **1,065 byte-exact** pair sets have zero
losses and zero gains against
`_work/player_axis_ramp_full_20261014.json`. None of the four
focused functions is linked- or byte-exact.

This does not prove live ARM32 gameplay or the lifetime of the
unidentified `+0xd8` word. Unsupported item-index behavior is
preserved from ARM but not exercised by the host smoke. Next priority:
audit turret camera/render and cloak-material consumers against the
now-confirmed geometry offsets, without assuming that the remaining
legacy aliases describe distinct objects.
