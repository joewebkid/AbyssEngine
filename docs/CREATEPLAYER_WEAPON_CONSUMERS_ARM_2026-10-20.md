# createPlayer immediate weapon consumers — 2026-10-20

Base: published `origin/main` at `95a89df553481a689bcfa27c3b064d74f6e1181b`,
confirmed with `git ls-remote origin refs/heads/main` before the change.

## Fixed family and measurements

Declared before implementation: exactly these three original methods, with
equal weight. Array helpers and the checked addGun dependencies are excluded
from the mean. The target was at least 98% source-shape; **it is not reached**.

| Method | Original Android address/size | Source before → after | Strict fuzzy before → after | Linked/raw exact after |
| --- | --- | --- | --- | --- |
| `Level::createPlayer()` | `0xaf1bc / 0x53c` | 86.5 → 86.0 | 37.8 → 34.5 | no / no |
| `Gun::setOffset(Vector*)` | `0x1522e4 / 0x44` | 100 → 100 | 100 → 100 | no / no |
| `PlayerEgo::setTurretPosition(Vector)` | `0x9b2bc / 0x30` | 100 → 100 | 100 → 100 | yes / no |
| Fixed arithmetic mean | three methods | **95.50 → 95.33** | **79.27 → 78.17** | **1/3 linked; 0/3 raw** |

These are fresh focused measurements using the original verifier and unchanged
flags. Gun's 100% normalized comparison is **not** a linked-exact result.
The bounded, source-confirmed correction lowers the shape scores. No members
were added to the family, and no allocation/getter rewrites were made to raise
them. This is the measured result of this patch, not a theoretical matching ceiling.
Remaining whole-method differences include cached Ship access versus repeated
Status::getShip calls, stack versus heap counters and the setup rotation noted
below. Restoring incidental calls/allocations solely for the mean is not part
of this consumer correction.

Original Android SO SHA-256:
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Original `weapons_hd.bin`: 7,484 bytes, SHA-256
`eebc7b6361bfdb19248f3f40071f915dad0cca919b292ad015d8d890f5570f82`.
Both inputs were read in place without making binary/asset copies.

## Contract proven by Android ARM

Inspected entry/validation documentation, Level.cpp, FileRead.cpp/h,
Gun.cpp/h, PlayerEgo.cpp/h, Player::addGun, Array.h, Vector.h/cpp,
AEFile scalar wrappers and the existing loader/ownership gates. The primary
evidence is original Android ARM; no formula is inferred from another platform.

* `0xaf360–0xaf36c`: load weapon positions, destroy FileRead, delete FileRead;
  returned arrays remain in use afterward. Slots 0–3 are addressed as four
  separate nullable lists. The whole result is dereferenced before any null check.
* `0xaf372–0xaf3e8`: slot 3 is an alternating position/scaling list. Vector
  index k advances by two; geometry index is k/2. Position uses vector k,
  scaling uses k+1. One geometry is appended per pair, in file order.
* `0xaf5a4–0xaf624`: bucket index is the decremented per-type remaining count,
  so equipment is published in reverse order within each type. For type 0/1,
  `Ship::getSlotPos(Item*)` selects that type's mount vector; equipment order
  is not the mount index. Gun stores item index, sort and magnitude first.
* **`0xaf5c8–0xaf5ec`: type 2 always loads `wpos[2]->data()[0]`.** There is
  no getSlotPos call on this path. The previous C++ applied getSlotPos to the
  turret too. Level.cpp now uses vector 0, the only production correction.
* `0x1522ea–0x15230e`: Gun copies into its offset vector at `this+0x7c`, adding
  the literal float 100 to Z. `0x9b2c2–0x9b2d6`: PlayerEgo copies the by-value
  turret vector into `this+0x224`. Neither setter retains a loader Vector pointer.
* `0xaf634–0xaf67c`: release each nonnull inner vector list, delete the inner
  Array, clear its outer pointer, then release/delete the outer Array.
  `0xaf688–0xaf698` publishes gun buckets only after this cleanup.
* `0xaf6a6–0xaf6a8` deletes the original heap slot-counter array. There is no
  gun-bucket deletion on the normal tail. Production Player::addGun creates
  another Array and copies Gun pointers; the staging arrays remain allocated
  in the original. This patch does not turn this into a leak fix. Current C++
  uses stack counters instead of the original heap counters; that difference
  is retained and contributes to source-shape differences.
  The original Player addGun normal path at `0xa3910–0xa3976` independently
  confirms new Array construction and pointer copying without freeing input
  arrays. Its delete at `0xa397c` is constructor exception cleanup. The
  PlayerEgo wrapper calls it at `0x9b796`; neither helper is added to the family.

The four-slot loader's independent all-record proof and its coordinate remap
remain documented in `WEAPON_POSITIONS_CONTRACT_ARM_2026-10-19.md`.

## Executed host checks and limits

`tools/run_createplayer_weapon_contract.py` independently decodes the original
asset into temporary expectations, checking its SHA/size, 59 unique ship IDs,
552 entries and 236 type-3 extras. It compiles the **entire unchanged extracted
production createPlayer body**, the production loader and AEFile scalar-read
bodies, real String.cpp, Array/Vector definitions and Vector assignment body.
It also executes the actual Gun/PlayerEgo setter bodies, PlayerEgo::addGun
wrapper and Player::addGun array-copy body. Constructors, Status/Ship/Item
services and AEGeometry receivers are explicit CPU boundaries; unreachable
audio/language/float-formatting routes abort. There is no graphics service.

At both `-O0/-O2 -g0`, all 59 records execute in both equipment orders:
**118 cases, 1,576 released loader vectors, 472 geometry pairs and 30 turret
cases**. Tests cover null/nonweapon equipment entries, absent slot lists,
per-type reverse bucket filling, independent primary/secondary mount indices,
createGun inputs and metadata, +100 Z, first-vector turret selection, geometry
position/scaling order and exact float bits after loader cleanup. The allocator
boundary observes real Array reallocations, every live loader allocation and
every final release; all loader storage is gone before gun publication.

The negative control extracts createPlayer from the fixed base commit, leaving
all other bodies and services identical. At **both O0 and O2** it exits 31 on
`turret unexpectedly requested getSlotPos`. The gate enforces the original
no-selector-call contract; the original asset has one type-2 mount per turret
record, so this does not demonstrate an original-asset mount displacement.

The existing all-record loader gate also passes at O0/O2, including controlled
short-read retention, missing-file return, all read widths/offsets and 788
vector releases. Four new read-only ARM tests check the original call sites,
slot routing, pair indexing, cleanup/publication order and setter destinations.
The full verifier unit suite passes 78 tests with one expected skip; the final
family-address assertions also pass in the four-test focused rerun. The one
skip requires an explicit ArrayInstantiations.o argument; a separate two-test
run with the actual after-build object passes, checking all seven Array ABI
alias pairs without a skip.

Native `gof2` static-library build succeeds; both canonical ARM builds compile
**208/208** translation units. Only `game/world/Level.o` changes between them.
Fresh complete before/after reports both contain **4,731 pairs**, **4,323
unique compared symbols / 4,524 originals**, **2,548 linked-exact** and **1,079
byte-exact** pairs, source mean **93.91%**, strict fuzzy mean **83.19%**,
**zero skipped / zero wrong-type**, **201 missing-absent / 124 extra symbols**.
The rounded corpus means stay the same. Both report commands exit zero.

Exact pairs were compared as complete sets of `(normalized unit, symbol)`,
separately for linked_equal and bytes_equal: **zero lost and zero gained** in
both sets. All pair keys are unchanged, and the only changed comparison row
is `game/world/Level, _ZN5Level12createPlayerEv`. SHA-256 of fresh full JSONs:

* `before-full.json`: `f5a8968437ed5c84c4b5ae65065cf68b494cc737c88365f12d0fac9607e966db`
* `after-full.json`: `22b301f13374edb91b5f0f45e477449f4758c97f0e640ea5521fe3a569bf3d12`

`exact-pairs.json` records the set delta and report hashes; `object-hashes.json`
records all 208 object hashes on both sides. Focused before/after JSONs and
host/native/unit/alias logs are in the output directory below.

This is **not** full Level stage-0, executable/runtime, GPU, platform-port or
native ABI validation of the host receiver stubs. Original missing-file/null
dereference behavior and malformed/absent-ID hangs are unchanged. Equipment
services in the host gate are bounded inputs, not a recovered Ship layout.
The original temporary gun-array leak remains. The separate observed rotation
difference (`{0,yaw,0}` in ARM versus current `{0,0,yaw}`) is outside this weapon
consumer correction and is left for a focused follow-up.

## Output placement

Fresh reports/logs and host compiler temporaries are local:
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/sol/`.
Existing `_work/portrait-arm`, `_work/portrait-arm-after` and
`_work/portrait-native` outputs were reused. Nothing was written to G:.
The bounded production change did not modify FileRead, StarSystem,
GLES/PaintCanvas or Godot code. Shared status was updated at integration.

Next bounded task: validate createPlayer's startup rotation/counter allocation
against Android before deciding whether to change setup behavior; keep the
full-stage-0 and native executable/runtime gates separate.
