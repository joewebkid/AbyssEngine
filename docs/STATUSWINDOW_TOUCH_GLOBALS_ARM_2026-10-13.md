# StatusWindow touch globals — Android ARM audit (2026-10-13)

Scope: `StatusWindow::OnTouchMove(int,int)` and `OnTouchBegin(int,int)`.
The Android 2.0.16 IDA listing in
`A:/Galaxy on Fire new version/analysis/gof2_libgof2hdaa_full_ida.c`
at `0x15a0bc` identifies `Globals::layout`, `Globals::h`,
`Globals::mouse_wheel`, and `Globals::iPad` at the affected reads.
The previous local source used six separate static null pointer holders.
They had no assignments or other callers; the four values they represented
are now read from their real global symbols. The `Layout` field offsets and
touch-button dispatch are unchanged.

| ARM function | Before source-shape | After source-shape | Before strict fuzzy | After strict fuzzy |
| --- | ---: | ---: | ---: | ---: |
| `OnTouchMove` | 0.2% | 99.0% | 0.0% | 85.2% |
| `OnTouchBegin` | 91.2% | 98.5% | 48.4% | 80.3% |
| `OnTouchEnd` (control) | 82.7% | 82.7% | 39.7% | 39.7% |

Focused verification compiled 206/206 ARM translation units without errors.
Native `gof2` builds. The verifier's 37 tests pass (one skipped). Full ARM
corpus: 4,720 comparison rows; 4,323/4,524 original functions covered;
201 missing; 2,388 linked-exact and 1,008 byte-exact, with no losses among
the previously exact `(unit,symbol)` pairs. Corpus average source-shape
92.68% → 92.70%, strict fuzzy 81.04% → 81.07%.

The remaining 1.0%/1.5% source-shape gaps were not tuned with extra state or
semantic changes. This audit establishes symbol and shape correspondence,
not successful interactive input on a device; no ARM32 runtime or real touch
session was exercised.
