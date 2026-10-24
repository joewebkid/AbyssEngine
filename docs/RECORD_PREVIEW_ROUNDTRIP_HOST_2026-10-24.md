# Record preview native round trip (2026-10-24)

Android ARM `recordStoreReadPreview` at `0xcbbf0` reads the preview fields
into `GameRecord` offsets `0x10`, `0x08`, `0x194`, `0x188`, `0x40`, `0x20`,
`0x11c` and `0x1a0`. The `GameRecord *` writer at `0xcfea4` writes the same
ordered fields: playing time, credits, station name, system name, campaign
mission, level, rank and ship ID. This is the separate slot-preview file;
fallback from a damaged primary save is not established by this gate.

`tools/run_record_preview_roundtrip_smoke.py` extracts the production
preview read and `GameRecord *` write bodies, file/string helpers and the
real `GameRecord` constructor/destructor. Windows host `-O0/-O2` runs them
through a controlled file boundary. The writer's station and ship getters
use synthetic data. The test independently constructs the expected
little-endian file bytes, including UTF-16 station and Cyrillic system names,
and checks the decoded native `GameRecord` fields. It also checks an absent
slot, a null input record and replacement of an existing preview. Both
optimization levels pass.

Native `gof2` builds (no-op), **208/208** ARM translation units compile and
**93** verifier tests pass (one existing skip). Focused preview-family
source-shape / strict fuzzy is **86.5% / 55.9%** over three functions; none
is exact. Production C++ did not change, so the last full corpus remains
**4,323/4,524** originals over **4,737** pairs, mean **94.24% / 83.60%**,
**2,569 linked-exact**, **1,082 byte-exact**, with zero skips and no
exact-set losses.

This proves a bounded 64-bit host round trip, not the live-status writer,
real filesystem failure handling, original save fixtures, full-record
compatibility or Android runtime behavior. Next: validate a real preview
fixture and the complete save/preview lifecycle before claiming live
save compatibility.
