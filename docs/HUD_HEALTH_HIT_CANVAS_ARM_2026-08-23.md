# Hud Health, Hit And Canvas ARM Pass

Date: 2026-08-23

## Scope

This package audits one connected `Hud::draw` spine from the shield, armor and
gamma bars through rocket-control secondary/steering rendering, four
directional hit pulses and the normal steering color reset. The primary
Android evidence is `Hud::draw` at `0x163b90`, especially the recovered body
from the health setup through the common normal-flight continuation. The IDA
body was cross-checked against the independent Android Ghidra output and the
typed `Hud`, `PlayerEgo`, `Radar`, `Status` and `Station` layouts.

## Restored Source Shape

- The health/hit spine now reloads `Globals::Canvas` at the native draw and
  metric call sites instead of carrying the outer cached Canvas through this
  complete cluster.
- The gamma predicate follows the native `Globals::status -> getStation() ->
  Station::getIndex() -> getCurrentCampaignMission()` order.
- Two non-native null guards around `Status` and `Station` were removed. The
  original frame assumes the live gameplay status and station contract before
  querying gamma damage.
- Shield, armor, armor-regeneration and gamma geometry retain the confirmed
  `(rate * 0.01f) * width` and `gammaHP / 100.0f * width` recurrences.
- Rocket-control and normal steering preserve their distinct color exits:
  rocket control restores the entry color before returning, while normal
  flight resets to opaque white before the cargo/mission continuation.
- All four 300 ms hit pulses retain the native masks, Radar-centred offsets,
  image metric queries and left/right/top/bottom draw transforms.

No image IDs, layout coordinates or gameplay predicates were inferred in this
pass.

## Rejected Broad Canvas Pass

Reloading `Globals::Canvas` at every remaining call in the complete
`Hud::draw` body was source-plausible but too broad for the current local
lifetime graph. It reduced the focused score from `51.4%` to `39.9%`. That
experiment was fully removed; only the independently positive health/hit
cluster is retained.

This confirms that Canvas ownership must be recovered one connected native
cluster at a time. It must not be converted mechanically across the whole
function.

## Validation

- Focused `Hud::draw`: `50.0% -> 51.4%`.
- Target/base instruction count: `3223/3025` (previous base: `3047`).
- Stack reservation remains `224` bytes and the VFP save set remains
  `d8-d11`.
- All 48 `Hud` functions: `91.0%` average, 27 linked-exact and 19
  raw-byte-exact.
- UCRT64 `libgof2.a`: green.
- No synthetic padding, fake stack variables, volatile register forcing,
  stack-canary imitation or inline assembly was added.

## Remaining Boundary

`Hud::draw` is source-backed across its major behavior paths but is not
byte-matched. The largest remaining differences are the prologue/iPad call
frame, health coordinate-address lifetime order, cargo String cleanup graph,
camera banner ownership and the dock/jump/cloak progress-label return slots.
The next pass should take one of those regions from its full predicate through
its draw calls and final color/String cleanup.
