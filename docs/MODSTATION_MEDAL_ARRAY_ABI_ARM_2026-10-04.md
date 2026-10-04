# ModStation medal-array type and receiver ABI (2026-10-04)

This follows the medal reward binding pass on local branch
`codex/blueprint-isunlocked-arm-20261002`. Scope is the source-level
`ModStation::medalArray` type and the campaign-mission call to
`ModStation::addAchievement`. It does not establish integrated game runtime.

## Evidence and change

- Android 2.0.16 `libgof2hdaa.so`, `ModStation::checkMedals` at
  `0x0d8b1c..0x0d8cf0`, stores the result of a 12-byte allocation and
  `Array<int *>::Array()` at `ModStation+0xbc`, then passes it to
  `ArraySetLength<int *>` and accesses its element buffer at `Array+4`.
  The source field was `int *` and was read/written through
  `Array<int *> **` casts. It is now `Array<int *> *` directly. A 32-bit
  `offsetof` assertion locks the field at `+0xbc`; this does not change its
  pointer width or ARM class layout.
- At `0x0d8bc8`, the original calls `addAchievement(0, 1)` with `r0`
  still equal to the campaign-mission result `1`. The observed callee at
  `0x0dbff8..0x0dc078` does not read `this`. The old C++ source created an
  invalid `ModStation *` from integer `1`; it now calls through the real
  station object. The rebuilt ARM call sequence remains unchanged at this
  site, and `addAchievement` remains linked-exact. This removes the
  source-level invalid receiver without claiming that the original intended
  a valid object pointer in `r0`.

## Verification

| Check | Before | After |
| --- | ---: | ---: |
| `checkMedals` source-shape / strict | 94.5% / 52.3% | 94.5% / 52.3% |
| `addAchievement` source-shape / strict | 100% / 100% | 100% / 100% |
| Full compared originals | 4,322/4,524 | 4,322/4,524 |
| Full comparison rows | 4,675 | 4,675 |
| Full mean source-shape / strict | 90.09% / 77.24% | 90.09% / 77.24% |
| Linked-exact / byte-exact rows | 2,165 / 977 | 2,165 / 977 |

The UCRT64 `gof2` static library and all 206 ARM translation units build.
The full `--fail-on-wrong-type` report has 202 absent originals, zero
wrong-signature and zero skipped. Exact `(unit, symbol)` set comparison
against the previous full report finds zero linked- or byte-exact losses and
zero gains. Focused before/after reports are
`_work/blueprint-isunlocked/modstation_medal_abi_{before,after}.json`;
full before/after reports are `modstation_medal_full.json` and
`modstation_medal_abi_full.json` in the same ignored directory.

The lifetime and cleanup of `medalArray` are not established by this type
correction. No medal/credit notification path was exercised in a running
game. A separate ownership and integrated-runtime audit remains necessary.
