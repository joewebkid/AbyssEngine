# Ticker resource loader (2026-10-11)

Android 2.0.16 ARM `FileRead::loadTicker` at `0x121144` probes and opens
`data/bin/ticker.bin`, then reads 59 fixed records. Each record consists
of seven 32-bit words: a switched active flag, four raw race flag words,
and two switched level bounds. It maps the race words to four booleans
and constructs `NewsItem(id, active, flags, 4, min, max)`.

The recovered loader had the wrong path (`ticker_binary.bin`) and used
switched reads for the four raw flag words. Both are corrected without
changing `NewsItem` ownership or the original 59-item count. The
reference `A:/Galaxy on Fire new version/analysis/gof2_raw_bin/ticker.bin`
is **1,652 bytes = 59 × 28**;
all 59 records passed a read-only check for Boolean words and ordered
level bounds.

Native `gof2`, **206/206** ARM translation units, and 36 verifier tests
pass (one skipped). Focused source-shape/strict fuzzy remain
**81.6%/52.1%**; the loader is not exact. The full corpus compares
**4,323/4,524** originals at **92.05%** source-shape and **80.12%**
strict, with **2,325 linked-exact** and **997 byte-exact**. A by-symbol
comparison found no exact-pair loss, skipped row or wrong-type symbol.
The report is retained at
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_ticker_loader_20261011.json`.

The analyzed binary is not installed into a runtime `data/bin` directory
by this change. A running-game asset read and the news feed's behavior
across locales and campaign levels remain unverified. Next priority:
controlled asset-backed ticker entry/render checks, then remaining
loader and constructor shape differences.
