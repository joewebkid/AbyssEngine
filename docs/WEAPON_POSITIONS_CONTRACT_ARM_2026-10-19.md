# Weapon positions: bounded resource and ownership contract (2026-10-19)

Base: published main `f7c4798d8b3fd4e3e5786d3354a4423440d15b57`.
Branch: `codex/sol-weapon-positions-20261019`, in the existing isolated checkout
`_work/sol-portrait-runtime-20261019`. Earlier branches/commits are preserved.
No String, StarSystem, GLES/Astra gate or shared status/validation files change.

The family is fixed **before implementation** to one logical original method:
`engine/file/FileRead`, `_ZN8FileRead19loadWeaponPositionsEi`, Android ARM
`0x1212e4`, size `0x1f4`. Array helpers, constructors and consumers are checked
dependencies, not extra entries used to raise the mean.

Fresh baseline: **97.7% source-shape / 53.1% strict fuzzy**, 176 target versus
178 base instructions, neither linked-exact nor byte-exact. Original SO SHA-256:
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Original weapons_hd.bin: 7,484 bytes, SHA-256
`eebc7b6361bfdb19248f3f40071f915dad0cca919b292ad015d8d890f5570f82`.

## Primary contract and narrow correction

Inspected AGENTS.md, entry/validation documentation, FileRead.cpp/h,
AEFile::Read overloads and dispatch, AENormalFile, Array.h, Vector.h,
Level::createPlayer, CutScene::checkForTurret and existing asset/lifetime checks.
The authoritative evidence is the original Android ARM and original asset;
iOS/PC inputs were not needed. No wider graph or dependency installation occurs.

ARM `0x121306–0x12131e` probes the temporary path String, destroys it before
OpenRead, and returns null on FileExist == 0. With a file present, it allocates
an outer four-slot Array, clears all slots and reads signed 16-bit ship/count
headers, then raw 16-bit type/x/y/z in file order. Type 3 additionally reads
three raw floats **before** the matching-ship check. The first Vector is
`(x,z,-y)` after signed extension; a type-3 second Vector is `(extraX,extraZ,extraY)`.
Rows remain in original order within each slot; type-3 positions and scales
are consecutive pairs. Read return values are not checked by the original loader.

The confirmed change is read-slot lifetime. Native float destinations are the
same stack locations at +28/+24/+20 for every record/entry; only the +20 slot
has a one-time initial zero at `0x121368`, before the loop. There is no
per-entry reset. The original Read(float&) wrapper at `0x70c3a` forwards a
4-byte destination without initializing it. Existing C++ instead reset all
three extras inside each entry. Moving those locals outside both loops restores
retention of already-read values when a subsequent read returns zero without
touching the destination, including after a skipped ship record.

The existing C++ initial zeros for extraX/extraY are retained. They are an
existing portable-model default, **not recovered original initial values**:
the corresponding native stack slots have no initialization before their first
read. This patch does not introduce uninitialized C++ values to improve matching.
Other pre-existing read-local defaults are not changed. Normal complete-asset
reads overwrite these values and all original asset outputs remain unchanged.

## All-record production-body host gate

`run_weapon_positions_contract.py` independently parses the 7,484-byte original
asset as little-endian records and checks its SHA, size, unique IDs, counts,
field widths and final offset. Its decoded expectation header is temporary;
the original file is read in place. The executable uses the unchanged extracted
production loader body, real FileRead header/constructor/destructor, real
Array/Vector definitions, actual String.cpp TU and production scalar Read wrappers.
FileExist/OpenRead/raw byte service/Close are explicit bounded host services.
Unexecuted String language/float/print routes abort. No GPU result is supplied.

Both `-O0/-O2 -g0` runs check all **59 IDs, 552 entries and 236 extra vectors**,
every scalar-read width/offset through each requested record's prefix, one
open/close per successful request, all four slots, absent-slot nulls and exact
float bits/coordinate order. The outer Array has four slots; populated inner
arrays are nonempty and type-3 lists have even size. Missing-file return makes
no open/read and still destroys the temporary path before any OpenRead.

Returned data is consumed after destroying FileRead. The actual createPlayer
cleanup fragment releases all **788 Vector objects** once, as well as every
outer/inner Array object and its final data buffer, without a double delete.
Production Gun::setOffset and PlayerEgo::setTurretPosition bodies execute on
bounded CPU receiver layouts; copied coordinate values remain valid after
cleanup (including Gun's source +100 Z adjustment). This validates value-copy
ownership, not the full Gun/PlayerEgo ABI, equipment selection, geometry or game
execution. The complete Level::createPlayer is not executed.

A separate in-memory fault case first reads a complete type-3 record for an
unrequested ship, then reads a matching type-3 record whose three extra-float
reads return zero and leave output unchanged. The returned extra vector retains
the previous values in native order, the file closes, and the two returned
vectors/arrays are released. These injected zero-byte results are a **host file
boundary**, not a claim that corrupt files are valid game data or that every
platform's partial-byte behavior is tested. The exact published baseline body
passes all 59 normal records and then fails this same retained-value assertion,
providing a negative control for the behavioral change.

The optional read-only ARM test checks the original method size, temporary
String destruction, four slots, null return, raw short/float GOT relocations,
stable float read destinations, coordinate stores and signed count load.
The verifier suite runs **72 tests, one existing skip, no failures**; the
updated focused ARM evidence checks also pass separately. Native gof2 and
both matching builds pass **208/208** units with unchanged compiler flags.

## Measured result and remaining caller boundary

After the confirmed correction the fixed one-function family is **97.5%
source-shape / 49.9% strict fuzzy**, still neither linked-exact nor byte-exact.
This is a recorded decline from 97.7%/53.1%, despite the verified read-state
correction. Base instruction count becomes 177 (previously 178), target 176.
The requested 98% mean is **not reached**. Array helpers/consumers are not added
to disguise that result, and no further percentage-only changes are made.

The shipped asset lacks IDs **13,14,15,50,53**. The native/recovered loader
stops only on a matching ID and has no EOF guard; this task does not invent a
missing-ID fallback or run an unbounded absent-ID call. Current createPlayer
and CutScene consumers dereference the result before their late cleanup null
checks. Therefore a null return for a missing whole file is not proof of a safe
station-to-flight path. Future integration still needs a present asset, valid
catalog ID, compatible equipment/slot indices and a checked caller policy.
OpenRead failure, initially failed X/Y float reads, arbitrary corrupt counts
or types, partial-byte writes and a complete native createPlayer/game run remain
unverified. No guard, asset, platform file implementation or caller is changed.

## Disk and full-corpus gate

Existing native and both ARM build/output directories are reused. Free space
was rechecked as other tasks consumed disk: approximately 0.75 GB remained
before the after ARM run and approximately 0.74 GB during final host/evidence
checks; external activity later reduced that to 326 MB, then 295 MB at completion.
No further compilation was launched during that final pressure. The bounded
host runner requires 64 MiB free working margin and aborts
without cleanup below that. No new complete build duplicate, binary/OBB copy,
asset overlay or foreign-data deletion is used; scratch output stays small.

The entire FileRead-TU comparison changes only the selected method and loses
no prior linked-exact or byte-exact pairs. Of 208 before/after object hashes,
only engine/file/FileRead.o changes; the other 207 are identical.
Both complete reports contain **4,731 (unit, symbol) pairs** and cover
**4,323/4,524** original functions. They retain **2,548 linked-exact** and
**1,079 byte-exact**, with zero skipped units, zero wrong-type signatures,
201 absent originals and 124 extra symbols. Rounded whole-corpus means remain
**93.91% source-shape / 83.19% strict fuzzy**. The fresh before report's hash
exactly reproduces the published main report.

Comparing the actual `(unit, symbol)` linked- and byte-exact sets gives **zero
losses and zero gains** in both categories. Comparing every report row finds
only `_ZN8FileRead19loadWeaponPositionsEi` changed; the focused decline is not
hidden behind the rounded corpus mean. The unchanged exact sets establish a
comparison regression gate, not successful native createPlayer execution.

Generated local reports and outputs are not committed:

- `_work/weapon-positions-before-full.json`, SHA-256
  `ec8ea688f4a0dbd3603d001baf415a54aabeac0bdd744e9a78a5ff1a0f4b8b33`;
- `_work/weapon-positions-after-full.json`, SHA-256
  `34a89436badacc783ca1b9cfd4473344621eb77b306828e68d2577f7a778f31c`;
- `_work/weapon-positions-exact-pairs.json`, complete set/row audit;
- `_work/weapon-positions-before-focused.json`, `_work/weapon-positions-after-focused.json`;
- `_work/weapon-positions-before-unit.json`, `_work/weapon-positions-after-unit.json`;
- `_work/weapon-positions-objects.json`, all 208 object hashes;
- `_work/weapon-positions-contract.log`, `_work/weapon-positions-negative.log`,
  `_work/weapon-positions-tests.log` and `_work/weapon-positions-arm-evidence.log`.

Full ARM runs use the reused `_work/portrait-arm` and `_work/portrait-arm-after`
directories and the unchanged verifier with `--no-build --fail-on-wrong-type`;
each has separate base and target objects, so the before/after work never
overwrites the other run's artifacts. No new full build duplicate was created.

```powershell
python tools/run_weapon_positions_contract.py --cxx C:/msys64/ucrt64/bin/g++.exe --asset 'A:/Galaxy on Fire new version/analysis/gof2_raw_bin/weapons_hd.bin'
python -m unittest discover -s tools/verify -p 'test_*.py'
```

ARM tools use the existing external NDK r18b and GOF2_VERIFY_WORK original
binary/symbol directory. Production-body host execution and source-shape
percentages do not establish native ARM runtime or completion of Level stage 0.
