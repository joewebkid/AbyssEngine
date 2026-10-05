# GOF1/GOF2 iOS contracts from the two-world mod

Date: 2026-10-03. Research handoff only: no Android C++ implementation,
source-shape improvement or native gameplay acceptance is claimed.

## Version and evidence boundary

These are exact **iOS ARM32/Thumb soft-float** contracts. Android ARMv7 remains
this project's primary implementation reference. Do not copy iOS addresses,
object offsets or resource IDs into Android without checking its binary.
Godot validation is a separate downstream gate.

| Source | Owned version | Executable SHA-256 |
|---|---|---|
| GOF1 | iOS 1.1.5 | `be45cd232b742ed1a14d99e00b07b5772147fe0b7b1ad6c21fe22c863d4e0cb5` |
| GOF2 HD | iOS 1.1.4 | `74ece5bedf561eed4e71b8d0b875e201c47858a2649a59d65af57390a0554958` |

Owned archives: `D:/FROM_PC/IOS/Galaxy_On_Fire_3D_1.1.5.ipa` and
`D:/FROM_PC/IOS/Galaxy_on_Fire_2__HD_1.1.4_ios_4.3.ipa`.
Original binaries/assets are not added to this repository.
Local decompiler references under `../../analysis/`:

- `gof1_engine_full_source.c`, SHA `718d8c8e5660d2daabfb179782427ceb68c6d31d6a025d83c0a6f275699ef50d`.
- `gof2_ios_ida_full_dump.c`, SHA `5de722e1b769bbc1792a65c6bfbf63b362eb7c139e35f11d6ab12862a7af8376`.

`ghidra_export.txt` supplies naming leads, but some `0x73xxx` anchors are not
callable entries in this owned iOS executable. Exact bytes, body and native
caller must agree before permitting a call.

## 1. World formats and the early field error

GOF1 `loadStation` at `0x24c64` assigns numeric CSV columns after the name:

| Column | Meaning | iOS object destination |
|---:|---|---|
| 1 | isPlanet | `+0x08` |
| 2 | imageIndex | `+0x0c` |
| 3 | population × 1000 | `+0x10` |
| 4 | race | `+0x14` |
| 5 | tech | `+0x30` |
| 6/7 | x/y | `+0x24/+0x28` |
| 8 | offersShop | `+0x2d` |

ImageIndex is not race. Eden Prime: image 4, population 3, Terran race 0, tech 7.
`Galaxy::getPosition` `0x19924..0x19a1c` proves 2×2 quadrants, 5×5 cells each,
step 100, quadrant size 500. Exact range SHA:
`5c8eed03c22c3e06efde466fc343010d4efad6e34906157210e8483c5f16b9de`;
float literals 100/500 at `0x19a14/0x19a18`.

GOF2 `FileRead::loadSystems` (`0x18fc0`) feeds `SolarSystem` ctor
`0x1af92c → 0x1af984`: name, eight big-endian words
(security, visible, race, x, y, z, gate, texture), four counted groups
**RGB (three words), stations, routes, forbidden**. Ambiguous reconstructed
local array names are not an authoritative file schema.

SolarSystem fields (decimal offsets): RGB +0/+4/+8, String +12, ID +20,
security +24, race +28, xyz +32/+36/+40, gate +44, texture +48,
stations +52, forbidden +56, routes +60, visible +64.
Station ctor `0x1ccb04`: ID +8, system +12, texture +20, tech +28.
Station file record: name then ID, system, tech, texture.

Append-only conversion preserves 34/135 GOF2 rows and adds 100/500 GOF1 at
IDs 34..133 / 135..634. Race fallback, existing station models and all-to-all
GOF1 routes are declared **adaptations**, not original visual/travel parity.

## 2. GameText loads a count, not all appended bytes

Confirmed missing contract: appending `.lang` strings is insufficient.
Every load path must pass the extended count.

| Count MOVW VA | Register | Call VA → callee |
|---|---|---|
| `0x1cf5a` | r1 | `0x1cf60 → 0x304770` (startup detection) |
| `0x15f576` | r2 | `0x15f580 → 0x304f94` (language switch) |
| `0x1a772a` | r2 | `0x1a772e → 0x304f94` (options reload) |

`GameText::setLanguage` is `0x304f94`; startup forwards count via r2.
`getText` at `0x33cccc` bounds-checks against this+24.
Original count 3385; Hawk-only 3387; GOF1 base 3387 / 675 strings, total 4062.
Original GOF2 strings remain unchanged. Owned GOF1 DE/ES/FR/GB/IT are exact;
missing languages explicitly fall back to GB; no owned GOF1 RU file exists.
Three SHA/old-byte gated patches execute actual Thumb MOVW in CPU tests.
This proves text reachability, **not a cure for Hawk's black startup**.

## 3. LevelScript, actor partition and soft-float ABI

- LevelScript wrapper `0x130a00`, body `0x130a1c`, size `0xe4`.
  r0=this, r1=Level*, r2=Hud*, r3=Radar*, stack[0]=TargetFollowCamera*.
  Fields camera +0x10, Level +0x14, Hud +0xcc, Radar +0xd0.
- MGame allocation `0x1621ea`, call `0x16220c`, insertion `0x16221a`
  after ctor and before camera reset / Level init.
  MGame Hud +0x6c, Level +0x70, script +0x74, Radar +0x78, camera +0xec.
- Level::createShip `0x125518`: r0=Level*, r1=race, r2=class, r3=type;
  stack waypoint pointer, hostile, group.
- Level actor array +0xf8; friend prefix count +0x160, hostile suffix count
  +0x16c. Preserve this partition.
- ArraySetLength<KIPlayer*> `0x13088c` **zeros every slot**, not just the
  appended tail. Save stock actor pointers before resizing.
- KIPlayer::setSpeed `0x1001e8` bytes `01 66 70 47`:
  `str r1,[r0,#0x60]; bx lr`. Float bits go in **r1**, not VFP.
  Vtable address point `0x445398`, slot +0x1c contains `0x1001e9`;
  native caller `0x13bb1c` passes `0x40000000` (2.0) via r1.
- Sleep primitive `0xffe88`. Route ctor `0x1aca0c → 0x1aca1c`, size 0x18;
  actor-bound ctor `0x1acbe0 → 0x1acbfc` (**not** `clone(Route*)`),
  getWaypoint `0x1aced0`, setRoute `0x1001f8`.
  Mission-0 ctor gets nine scalars, representing three Vector waypoints.

Content identity: Inflict 5, Hiro 2, Azov 23 agree with owned 36-byte ships.bin
records and `ship_005_terran`, `ship_002_pirates`, `ship_023_pirates` mesh files.
GOF1 type IDs are a different namespace. Reusing stock pirates [2,23,2], adding
two Hiro and an Inflict is a leak-free six-actor **adaptation**, not model parity.

## 4. Objectives, radio dependencies and progress ownership

Objective ctor `0x184a1c`: this,type,value,Level*; alternate `0x184a34` adds
a stack argument. Radio ctor `0x1a3d6c`: this,text,image,condition with value
on stack; selector-array overload `0x1a3da0 → 0x1a3dbc` adds count at stack+4.
Level fields: success +0x28, failure +0x2c, route +0x108, radio +0x114.

GOF1 mission 0: three route points, five enemies, one ally, no-enemies success,
17 radio events. Condition 13 remaps to GOF2 16; condition 7 needs a boolean
gate preserving triggered state and current-message reference. Event 16 depends
on 15; a timer is not equivalent. Mission 2 has selectors 0,1,2 in one event:
using the single-selector overload changes the dialogue order.

MGame::success `0x16d784/0x16d8bc` reaches Status::nextCampaignMission
`0x16e034`. GOF1 must own separate durable, slot-aware progress and must not
advance GOF2 campaign. The dispatcher and all 13 transitions are **not yet
implemented**. AsteroidField/Fog translation and complete retry/save behavior
remain open. The original mod bridge document contains the full radio tables.

## 5. ImageFactory namespaces and original Christine

Original Christine PNG is 63×63 (hash/path in the Godot note below).
Registration: kind 2 / texture 2870; kind 3 / Image2D 1420 and 5152.
Descriptor `(12,4,-1,-1,-1)`; row 12 at file `0x3ea8a0` is
`(5148,-1,-1,-1)`. **5152 is Image2D, not texture 2870**.
API3 additive resources cover ordinary/iPad/iPad-large variants without
rewriting original NPC image files.

User confirmed the portrait in the desktop Inflict prototype. The temporary
character-2 descriptor override is not a safe final global policy: isolate GOF1
mapping from GOF2 NPCs. No HD or ARM64 portrait acceptance is claimed.

## 6. Expansion/save lessons, not blanket Android patches

Audit constructor/load/reset/save/visited consumers together. Two persisted
Status arrays migrate 34→134 with zero tail; visited migrates 135→635.
Two StarMap arrays are runtime-only. Reject corrupt lengths before element
reads and keep native cleanup. Map filtering preserves global IDs/discovery.

An early touch-filter bug bypassed end-of-array termination for hidden IDs.
The fix distinguishes NULL/end/count or ID≥134 exit, valid-hidden increment,
valid-visible continuation. CPU tests cover all-hidden lists and 133→134 exit.
Do not use Mach-O load commands as a cave: the true aligned zero-run starts
at file `0x13e8`; `0x1000..0x13e5` is not free space in this owned build.
These are iOS mod contracts, not automatic Android serialization/size changes.

## Local evidence and acceptance boundary

- [Bridge and radio tables](C:/Users/Dima/Documents/SuperDuperHyperHle/docs/GOF1_GOF2_STORY_RUNTIME_BRIDGE_2026-10-02.md).
- [Byte/ABI probe](C:/Users/Dima/Documents/SuperDuperHyperHle/dev-scripts/gof2_story_bridge_probe.py); tests in `dev-scripts/tests/test_gof2_story_bridge_probe.py`.
- [World converter](C:/Users/Dima/Documents/SuperDuperHyperHle/patches/scripts/gof2_world_tables.py), [text loader](C:/Users/Dima/Documents/SuperDuperHyperHle/patches/scripts/gof2_text_bank.py), world-size/map-bank modules and adjacent tests.
- [Mod checkpoint journal](C:/Users/Dima/Documents/SuperDuperHyperHle/docs/GOF1_TWO_WORLDS_MOD_STATE.md).
- [Godot handoff](../../godot_frontier/docs/gof1_gof2_mod_research_handoff_20261003_ru.md).

Handoff gate rerun on 2026-10-03: 61 GOF2 patch tests and four story bridge
tests pass, no skips. All local Markdown links were checked. Private 0.0.1
data-stage boots to animated title/native dialog and
normal UITouch delivery; 0.0.2 map filter has CPU/byte/composition evidence only.
Hawk cold startup, travel, durable GOF1 progress and full campaign are not
accepted. This docs-only handoff changes neither native recovery metrics nor
Godot runtime; no Android/native build is implied.

## 7. Corrected Route constructor and position contracts

The earlier `Route::clone` label was wrong. Owned instructions prove the
five-argument actor-bound constructor at `0x1acbe0 → 0x1acbfc`:
`r0=this, r1=int-coordinate-array, r2=actor-array, r3=auxiliary-int-array,
stack[0]=scalar-count`. Body `0x1accae..0x1accb2` adopts r2 at `this+0x10`;
`Route` destructor frees that array. Do not call it with just a Route pointer,
and do not pass a borrowed array without separately proving lifetime.
The ordinary constructor remains `r0=this,r1=coords,r2=scalar-count`.
Hex-Rays omitted the meaningful arguments to `Waypoint` in its body;
`0x1acb34..0x1acb4e` explicitly loads three coordinate words and calls
`0x1dd3b0`. This is a concrete example of why named entry anchors alone cannot
prove complete ABI or behavior.

`Level::setRoute 0x12653c` destroys/deletes its old `+0x108` Route and adopts
r1. `KIPlayer::setRoute 0x1001f8` similarly replaces `+0x68` and writes a
non-null route to `+0xb0`. The mission-0 stage constructs two independent
ordinary Routes from the same coordinates, avoiding shared owned arrays.

`Level::getPlayer 0x12dc68` reads `+0xf0`. `Player::getPosition 0x18ae14`:
r0 is the hidden 12-byte Vector result pointer and r1 is Player*. It calls
Transform `0xdbc0`, which resolves the matrix and extracts translation through
`0x324890`: matrix offsets 12/28/44 (float elements 3/7/11).
`Player::setPosition 0x18af80`: r0=Player*, r1/r2/r3=float bits x/y/z;
it calls `0xdc74`. The underlying Player pointer is `KIPlayer+4`; its
Transform pointer is `Player+8`. These remain exact iOS contracts, not Android
offset changes or a Godot axis conversion.

The new callable [actor stage](C:/Users/Dima/Documents/SuperDuperHyperHle/patches/scripts/gof2_mission0_actors.py)
occupies file `0x1800..0x1a47` / VM `0x2800..0x2a47` (584 bytes). No native
GOF2 callsite is changed. The dispatcher must explicitly gate its call after
LevelScript construction. It rejects incompatible/repeated scenes before any
native mutation, keeps the three stock actors across ArraySetLength, creates
an Inflict and two Hiro, and restores partition 1/5. Deterministic enemy
formation is explicitly a mod adaptation, not original random spawn parity.

Four new tests cover owned native Route/Array/position bodies, transfer of
old route ownership, soft-float arguments, stack alignment, saved registers,
rejection without mutation, ASLR, composition with world/map/Christine/Hawk
hooks, and freshly compiled ASM equality with the byte template. Engine ship
creation, waypoint construction, teardown and host/math edges are mocked;
no rendered scene or full gameplay is accepted. Fresh total: **65 patch tests
and four bridge tests**, all pass without skips. [Updated probe JSON](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-story-bridge-20261003-actor-contracts.json)
records the corrected names and contracts. No native Android C++/metric,
Godot runtime, original IPA or ordinary save was changed by this handoff.

## 8. Radio ownership and append only iOS carrier

**Source-backed and CPU-confirmed for the exact owned iOS image**, not an
Android offset correction: native `Radio::setMessages 0x1a3584` stores its
Array pointer at Radio+0 and calls `0x1a3e50` on each message to set message+0
to that Radio. MGame instructions `0x162080..0x162094` store Radio at +0x7c,
read Level messages through `0x12e200` and bind Radio before LevelScript is
constructed. Replacing Level+0x114 without retaining/rebinding that array is
not a safe lifetime policy. Radio+4 is the current-message pointer.

`RadioMessage` destructor `0x1a3e30` deletes selector array +0x1c via ARM
`delete[] 0x39c0a4`, clears it, returns this; it does not delete this or free
the attached Objective +4. `Objective` wrapper/body `0x184a50→0x184a60` releases
owned nested resources but likewise leaves deleting this to the caller.
The caller's scalar delete is ARM `0x39c0b4`. These are concrete ownership
edges; do not infer Android structure sizes or ownership solely from them.

The new callable [event installer](C:/Users/Dima/Documents/SuperDuperHyperHle/patches/scripts/gof2_mission0_events.py)
and adjacent assembly are 624 bytes. After the guarded actor stage, it prepares
17 exact source messages and Objective `(0,0,Level*)`, preserves the shared
Array object, releases old ordinary messages/selectors and Objectives through
native functions, copies replacements, calls setMessages, and clears failure
and cached success. Current-message, alias, duplicate/null, capacity and repeat
guards fail before mutation. An isolated Christine descriptor is a required
caller input and is not yet integrated with a final GOF1-only NPC policy.

GOF1 original trigger 7 is now identified as latched Objective success at
MGame+0x54 (see section 9); the installer leaves it 7, not a timer. GOF2
`0x1a3e7c` uses its fifth argument in `0x14b2dc(a5)`, matching the recovered
Android LevelScript parameter, **not** GOF1's Radio boolean. A pointer-to-bool
cast would not recover that event. Native GOF1 switch/decompiler output for
`RadioMessage::triggered` is incomplete and cannot by itself name the boolean.

The code carrier appends RX `__MODTEXT` and RW `__MODDATA`, preserving old
Mach-O commands/segment order/indexes/file ranges/VM addresses. New commands
occupy file `0x13e8..0x1457`; the old 390-byte map filter moves to
`0x1460..0x15e5`. All 11 map CPU checks pass at the new location. Current code
VM `0x503000`, state VM `0x504000`; PC-relative data avoids new absolute-pointer
rebasing. Four initial state bytes are not durable campaign progress. This is
an iOS local-emulator mod technique, not an Android ELF/native recovery edit.

Fresh gate: **75 GOF2 patch tests + 4 bridge tests**, no skips; [probe](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-story-bridge-20261003-events-contracts.json)
validates 41 owned byte anchors. Constructor/destructor/array/binding bodies
are real; allocator/SjLj and full engine edges are mocked. Private components
package has no campaign callsite hook. Desktop loader recognized both segments
and produced an EAGL loading frame, not dialogue/combat/travel acceptance.
No Android C++ source, native build/byte-match metric or Godot code changed.

## 9. Owned GOF1 success dataflow and scoped iOS bridge

The missing caller semantics are now proven by instructions from the exact
owned GOF1 executable (`be45cd23…`): `0x44b38` calls Level::checkObjective
`0x296ca`; `0x44b40` stores the result byte at **MGame+0x54**. OnRender2D
`0x45ee8..0x45ef6` passes it through Radio `0x59498→0x5928a` to triggered
`0x596f8`; case 7 `0x59744` assigns that boolean, writes message+0x20 and
selects current message via `0x591f8`. Two CPU tests execute owned windows
with 0/1 results and the actual switch-table entry. Objective result,
SjLj imports and external __switch16 helper are mocked; UI stops before draw.
This resolves the dataflow without pretending the decompiler named the flag.

New [scoped bridge](C:/Users/Dima/Documents/SuperDuperHyperHle/patches/scripts/gof2_mission0_bridge.py)
and adjacent `.S`: 672 bytes, 32-byte volatile context, PC-relative state and
descriptor addresses. Exact GOF2 hooks are success `0x16d784`, triggered
`0x1a3e7c`, process `0x134098` and static portrait LDR `0x1a3758`. The first
three are confirmed LC_FUNCTION_STARTS; the last is a mid-function load.
Bind requires the installed six actors, partition 1/5, 17 messages, exact
event 15 and matching MGame/Level/Script/Radio/Objective ownership. Native
fallback replays the four stolen bytes; portrait fallback preserves NZCVQ,
registers and stack. Repeat bind fails; explicit unbind is required.

Bound success uses real native Objective and a latch. Source event 15 then
fires once; the native type-6 implementation handles dependency event 16.
Native process is bypassed only for the bound script; this does **not** bypass
the constructor's existing mutations. Descriptor `(12,4,-1,-1,-1)` replaces
selector 12 only for the bound Radio, leaving stock NPC descriptors intact.
Texture/Image2D registration remains a dispatcher/asset packaging obligation.

`--mission0-bridge` composition is tested but starts unbound; New Game and
scene lifecycle are unchanged. No new IPA/gameplay run was generated this
turn. Fresh gate: **83 patch + 4 extractor + 4 probe = 91 tests**, no skips.
[Updated probe](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-story-bridge-20261003-success-bridge.json)
validates 45 owned anchors. No complete scene, campaign transition, map travel
or durable reload is accepted. These iOS addresses do not establish Android
ELF ABI/structure offsets. Native Android C++, metrics and Godot code untouched.

## 10. Exact iOS first-scene lifecycle and tutorial constructor boundary

This supersedes section 9's unbound-only stage, not its provenance limits.
Private `0.0.5-first-scene` enables mission 0; **13-mission progression and
durable Continue are still unimplemented**. Android addresses/layout remain
independent and require their own ELF proof.

Owned iOS startup `0x1cfcc` calls MGame constructor wrapper `0x161224`;
`0x1cfd6` sets module id 2 and `0x1cfde` registers it via `0x33356c`.
New Game `0x160ed0..0x160ee2` selects id **2 = MGame**, not an intro module.
The intro is the native campaign-0 branch inside LevelScript.

New exact-revision [dispatcher](C:/Users/Dima/Documents/SuperDuperHyperHle/patches/scripts/gof2_mission0_dispatch.py)
and adjacent `.S`: **924 bytes**, 64-byte total volatile state (32 bridge +
32 activation/owners/phase/error), PC-relative addresses and aligned calls.

- Entry `0x160d50`: activate before native Status::resetGame. At `0x1d49b4`
  select Inflict record +20 only for that Status and phase; native fallback
  retains original Phantom record +40 and the stolen stack store.
- `0x160ed0`: retain native module 2. Before it, use FileRead station lookup
  `0x1c2f8`, verify Station::systemID `0x1ccf0c` =34 and id `0x1ccf1c` =135,
  then native Status::setStation `0x1ce088`. These are appended original
  Eden Prime/Astere, not the historical Mido reskin.
- `0x130bc6`: bypass only explicitly armed native campaign-0 tutorial and
  the same Status owner. At this point **r6 is Status\*\*, not Status\***.
  Before jumping `0x133290`, restore Level r8 to `sp+0x5c` AND r6 to `sp+0x64`.
  The common constructor immediately dereferences the latter. Omitting it
  is an actual uninitialized-local defect, not a harmless optimization.
- `0x16221a`: LevelScript already exists, MGame+0x74 is assigned and Radio
  owns the native array. Install actors, replace messages/objectives, bind,
  release the constructor's camera lock (`Camera+69`, setter `0x1da5ec`).
  Resume at `0x16221e`: preserve native resetCamera `0x14af94` and Level init.
- `0x167b50`: unbind BEFORE native Level/Script/Radio destructors. Clear
  activation rather than claiming restart/Continue works without persistence.
- Every installed hook has native fallback, replaying exact stolen bytes;
  mid-function hooks preserve NZCVQ, r0-r12/LR, d0-d7 and FPSCR. On partial
  installation failure, write stage 1..5 and execute UDF, never continue a
  damaged native scene. The emulator's undefined-instruction diagnostic
  behavior has not been end-to-end certified by a deliberate runtime failure.

Scoped portrait mapping remains `(12,4,-1,-1,-1)` only for the bound Radio.
The existing verified Texture/Image2D registration plus three original-pixel
AEI scale variants are now packaged. Stock NPC descriptor `file 0x44a504`
and old portrait resources remain unchanged. Original 63x63 pixels are not a
finished high-resolution adaptation. Original IPA and ordinary save namespaces
remain untouched; `gofw_` isolation is not durable campaign serialization.

Fresh gate: **89 patch + 4 extractor + 4 probe = 97 tests**, no skips; six
dispatcher tests include the owned constructor prefix, staged lifecycle,
ASLR/VFP/stack preservation, native fallback and all five failure stages.
Native Route/Array/Objective/Radio instructions execute. Hud rendering,
file loading/station offers, ship creation and full graphics are mocked.
The [probe](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-story-bridge-20261003-first-scene.json)
now validates **51** owned anchors. Desktop loader reached the title EAGL
frame (raw capture orientation is not final window orientation); no mission,
combat, docking, world-switch or reload acceptance is claimed.

Package SHA `c59d282d076f13d920d9857430766eef96ea8a8a2c509b08b228e550ba72dfb6`;
derived executable SHA `0e7b81af7065924eecf9f378a92bafa075bc2252dde1384a85b388a2346c9541`.
Only this technical note changed here: no Android C++ or byte-match metric edits.

## 11. Live KIPlayer layout correction (2026-10-03)

**Confirmed for owned iOS 1.1.4 only:** the first-scene GDB snapshot contains
three distinct native KIPlayers and installer failure stage 3. Our old fixtures
incorrectly treated KIPlayer+4 as a nested actor with another geometry slot.
The observed actor prefix is +4 combat stats, +8 own scene node; it does not
prove C++ inheritance. `0x18ae14` reads position and `0x18af80`
sets position through **this+8**, not through the Ship stats at this+4.
`0xffe88` deliberately uses +4 for Ship state, a different contract.

Actor installer validation now separately checks Ship and the actor's own node;
both ally/enemy positioning pass KIPlayer itself. Fixture Ship+8 stays zero
to reproduce the original real-game failure. 90 patch + 8 extractor/probe
tests pass; 0.0.6 live scene/combat acceptance is still pending.

Do not blindly copy these offsets into Android C++. Its current Player class
models combat state, while PlayerEgo/KIPlayer own geometry separately. Cross-check
class identity, inheritance and getters against the Android ARMv7 primary binary.
The current desktop UDF compatibility path loops up to 256 times: the previous
«UDF stops the partial scene» claim is not a robust primitive. It needs a
separate fail-stop correction/test, without global renderer/game exceptions.

[Original live scene trace](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-gdb-b/scene-trace.json),
[native-source fixture correction](C:/Users/Dima/Documents/SuperDuperHyperHle/patches/scripts/test_gof2_mission0_actors.py).

## 12. Rendered scene and platform-specific Radio caption contract

**Confirmed desktop iOS 1.1.4, private 0.0.6:** ordinary New Game/Easy reaches
native `0x16221e` after actors/events/bridge each return 1; six distinct actors
and live owner-bound context are captured. No actor/scene/save values were
injected by the observer. Inflict, original Christine pixels and original GOF1
line 199 are visible. Pause/resume, visible firing projectiles and steering
were exercised. This is not mission completion or full-campaign acceptance.

**Source-backed iOS caption:** `0x1a3bac` computes name ID as image+1586
(`ADDW r1,r4,#0x632`); Radio owner is in the caller's stack local at sp+0x20.
The Android Radio source instead uses +0x63d. Portrait image IDs and name IDs
must not be treated as one portable identity; r6 at this instruction is Layout,
not Radio. The previous private scene's correct portrait still read Security.

The new 0.0.7 hook checks the bound Radio, image12 and current owners, then
selects appended text4062, Christine Hammond. Other callers replay the exact
native addition. The original 675 GOF1 strings and original GOF2 strings retain
their indices; total text count is4063. Two-ASLR positive/fallback/stale-owner
CPU cases preserve flags, SP and all registers except the intended r1 output.
Rendered name acceptance is a separate pending check.

Follow-up, **confirmed rendered 0.0.7**: clean desktop New Game/Easy displays
Christine Hammond with the original blue-haired portrait and source line200
(the yellow waypoint-marking instruction). The five-stage
[live trace](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-caption-d/scene-trace.json)
again shows all three installers returning1, valid owners and native continuation.
This accepts the caption in this scene, not all dialogue order or mission completion.

The private dispatcher now uses BKPT #0xfe instead of ignored UDF, retains
failure stage in r1 and clears only context magic. A CPU check reaches the
exception, but full-emulator intentional-failure acceptance is still pending.
Do not transplant either ARM instruction into an Android C++ error API.

[Live scene lifecycle evidence](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-layout-c/scene-trace.json),
[owned 32-byte caption anchor](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-story-bridge-20261003-caption-contracts.json),
[CPU caption checks](C:/Users/Dima/Documents/SuperDuperHyperHle/patches/scripts/test_gof2_mission0_bridge.py).
No Android runtime source, byte-match metric or Godot code changed here.

## 13. Deliberate failure reaches native fail-stop, not UDF bypass

Separate private desktop `20261003-failstop-e`: the observer temporarily
replaced only the actor-installer entry in guest RAM with return0, explicitly
flagged as fault injection. Real dispatcher returned from actors with0 and
stopped at BKPT `0x5038e6`: phase0x83, error3, r1=0x83, context magic0. No
events/bind/native scene continuation followed. The original actor entry was
restored and the owned halted process was closed. Original IPA and ordinary
saves remain unchanged. This proves the debugger halt/failure propagation,
not the no-debugger user-facing error UI or cleanup of every failure stage.

[Fault trace](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-failstop-e/scene-trace.json).
The generic trace label unexpected_stop means a non-observer breakpoint; in
this explicitly injected case the helper asserted the expected failure PC/data.

Naming caution: current Android Player.h models combat state/transform, while
KIPlayer and PlayerEgo own geometry separately. The confirmed iOS wrapper's
direct +8 access is a compatible prefix, not evidence for a Player base class.
Do not infer C++ inheritance or rename recovered Android classes from this
iOS mod alone; verify its ELF body/layout first.

## 14. Native scene finalization and Radio scheduling (iOS evidence)

**Source-backed, owned iOS 1.1.4; Android addresses/layout not implied.**
Native load calls `connectPlayers 0x120b68`, then `PlayerEgo::setRoute 0x18a510`
with Level+0x108, before the post-LevelScript hook at0x16221a. Level.setRoute
0x12653c adopts the new route and destroys the old one, but does not change
PlayerEgo+0xfc. Level.init0x121370 initializes rendering/scene geometry, not
the player route or combat target graph. The prior label of0x12ee5c as a
partition-update hint must not be used as connectPlayers: it is periodic
respawn/update logic with a delta/time argument.

**Confirmed live0.0.7:** after normal New Game/Easy, Level has the three-point
route but PlayerEgo+0xfc remains0. New ally/new two pirates have empty targets;
the three stock pirates retain only the old player target. Player's49 targets
still include the old3 actors plus other native objects. Do not interpret
this as49 enemy ships. This is a late-scene-mutation/finalization defect.

Native `Ship::setEnemies 0x185794` replaces/copies its array and propagates it
to gun objects; NULL clears the old array and gun target references.
`Ship::addEnemies 0x1859e8` appends, so calling connectPlayers again without
clearing KI targets can duplicate old entries. Private actor stage0.0.8 now
assigns the player route, clears all6 KI target arrays with the native setter,
then calls native connectPlayers. CPU fixtures execute the real route setter
and target clearing/ownership bodies; the full connect graph remains an engine
edge in that fixture and must be accepted in the actual game separately.

**Source-backed position accessor:** actor+8 is a node; node+44 is a scene
pool and node+12 its transform index. DBC0 calls3181ec(pool,index), which uses
the transform array atpool+344, or fallbackpool+248; 324890 reads translation
words3/7/11. Treating node+44 directly as Matrix is wrong. Observer H's
player_position field is invalid and excluded; the pointer/count findings
above remain valid. The corrected observer has indexed/fallback tests.

**Confirmed Radio lifecycle:** native1a35cc sets a current message and derives
duration from wrapped text;1a3a10 draws it and1a3e6c marks finished/clears
current before polling the next. Condition6 checks message.triggered(+32),
not finished(+33), but the presentation queue serializes lines. Live G records
original196→197→198→199 with starts2018/7563/15118/25006ms and final finish34530ms.
MGame increments LevelScript+4/+8 before calling process134098: do not add a
second delta inside a replacement process hook.

[Radio trace](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-radio-g/scene-trace.json),
[late binding defect trace](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-route-h/scene-trace.json),
[57 matching owned anchor windows](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-story-bridge-20261003-route-sync.json).
No Android C++/Godot code or byte-match metric is changed by these notes.
Full first-mission completion and cold Continue remain unaccepted.

## 15. Explicit ally policy and passed-waypoint events

Live0.0.8 I confirms native reconnect: player route is non-null and combat
targets49→52 include all6 actors plus native other objects; all KI lists are
rebuilt. It also starts source206/type16 at1ms with active flags1,0,0,0,0,0.
The only active KI is the added ally. Native condition16 scans player targets
and excludes Ship.alwaysFriend through1856c8; actor race/model is not sufficient.

Source-backed setter1856b0 takes r0=Ship*,r1=boolean, returns the same pointer,
and writes bytes+237=argument,+92=0,+93=1,+224=0. Private0.0.9 calls it on the
ally's combat stats before route assignment and reconnect. CPU tests execute
the real setter; live Radio/flag acceptance is pending. Do not port these byte
offsets to Android without its separate ELF/layout verification.

Native Radio case0 is a **waypoint passed** predicate: if current index exceeds
the previous message observation, it compares that previous value with the
expected waypoint, updates its stored index and triggers. A valid route with
index0 is not itself a passed point. New200/201 must be verified by actual
navigation through their points; no fake timer substitutes this contract.

[Live I evidence](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-route-i/scene-trace.json),
[58 matching anchor windows](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-story-bridge-20261003-ally-friend.json).
No Android byte-match or Godot acceptance claim is made.

## 16. Live acceptance of explicit ally policy, private iOS0.0.9

**Confirmed in normal desktop New Game/Easy, not a CPU-only fixture.**
Run J reaches actors/events/bind return1 and native continuation phase4/error0.
Player route is non-null, player target count52 (not52 ships). The ally has
always_friend1; five hostiles have always_friend0, state5/sleep1/active0.
Observed original Radio sequence196→197→198→199 starts at2028/7580/15124/25036ms
and finishes the last line at34561ms. The erroneous206 at1ms from I is absent
through that observed interval. This accepts the policy setter and initial
queue; all17 events, combat, travel and cold Continue remain unaccepted.

Route index remains0 at those message boundaries. Corrected position readings
show continuous flight slightly to the side, missing waypoint0 without steering.
Do not reinterpret this as a route constructor failure or replace passed-point
events with elapsed-time events. Actual navigation is the next acceptance gate.

[Live J](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-ally-j/scene-trace.json).
Evidence belongs to the exact owned iOS revision. Android C++ source/layout,
ELF byte-match and Godot runtime are not modified or accepted by this result.

## 17. Route refresh and scoped orbit navigation

**Source-backed owned iOS 1.1.4, not Android address/layout acceptance.**
Radar draw `sub_19F430` obtains Level's current route through `0x12DC90`
and stores it at Radar+336 each draw. The matching reconstructed intent is in
`src/game/weapons/Radar.cpp`, not a one-time route cache. No additional Radar
route setter is justified by this experiment. PlayerEgo and Level route owners
still require the explicit synchronization described in section 14.

Native `Route.update` at `0x1ACFF4` uses strict per-axis differences inside
(-2000, 2000), provided the current point has no docking target. It marks that
point reached and advances the route index. This is not a Euclidean radius.
The current `src/game/world/Route.cpp` expresses the same behavior; that
cross-check does not substitute for separate Android ELF verification.

The orbit-menu input path has `CMP r0,#2` at `0x166AF2`, followed by
`BLT.W 0x166EB4` at `0x166AF4`, bytes `c0f2de81`. Thus the first GOF1 scene,
which currently uses native campaign index 0, inherits the GOF2 tutorial's
navigation lock. Private iOS package 0.0.10 adds a hook at this branch only:
an owner-valid bound GOF1 context continues at `0x166AF8`; all other contexts
replay the original signed branch. Earlier alien-orbit and later mission-48,
docking, landing, mining and mission-type restrictions remain unchanged.
The hook checks native MGame in r11, preserves r0-r12/LR/SP/NZCVQ and does not
modify VFP state. CPU tests cover slides 0/0x10000, indices -1/0/1/2, unbound,
wrong MGame and stale Level ownership. Assembly matches the embedded template.

The stock waypoint action is mask `0x02000000`; MGame obtains the current
route point and calls PlayerEgo's existing autopilot setter. Owned iOS setter
`0x18AE3C` stores target at +344, active at +340 and waypoint flag at +348
from target+110 bit0; it resets the engine input slot and sets throttle 1.0.
These are iOS field offsets only. No direct RAM autopilot/route-index injection
is used for normal gameplay acceptance.

Fresh local gate: 92 patch tests, 7 observer/story tests, 4 extractor tests and
11 ship/world probe tests = **114 passed, no skips**. Both owned archives were
explicitly supplied to the optional integration probes.
[59 matching iOS anchor windows](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-story-bridge-20261003-navigation.json),
[exact private package metadata](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-two-world-first-scene-0.0.10.stage.json).
Bridge size is 792 bytes; dispatcher now starts at 0x503588. Diagnostic tools
must read metadata for the exact package instead of using the previous address.
Malformed stage input is rejected before consuming the single GDB connection.

Live orbit-menu/autopilot and actual waypoint acceptance are still pending;
all-mission progression, two-bank travel and durable Continue remain incomplete.
No native Android C++ or Godot runtime is changed by this handoff.

### Independent Hud draw gate in private iOS 0.0.11

**Source-backed:** Hud draw also requires native campaign index >=2 before
drawing its orbit button. Owned `0xFBBEE` is CMP r0,#2, `0xFBBF0` BLT0xFBC3E,
`0xFBBF2` LDR r0,[r6]. Bytes at the 4-byte hook are `25db3068`.
r10 holds Hud and r6 the Status pointer slot; the draw later uses Hud+916/918
for the button coordinates. The previous input-only0.0.10 did not make this
button visible. Other radial buttons are not the orbit action.

Private0.0.11 adds an owner-valid HUD-specific draw override, without global
campaign/Status changes. On visible paths it replays the stolen LDR and
continues0xFBBF4; native hidden paths retain the original comparison flags and
leave r0 unchanged, continuing0xFBC3E. Separate CPU coverage checks both slides,
indices-1/0/1/2, unbound/wrong HUD/stale owner, all registers/SP/LR/NZCVQ/D8.
The mission-type183 and alien-orbit restrictions remain native.

Fresh current gate is93 patch +7 observer/story +4 extractor +11 root probes
=115 passed, no skips. Bridge864 bytes, dispatcher0x5035D0; diagnostics read
per-package metadata. The observer additionally reads native autopilot state
and HUD button position, without setting either.
[60/60 owned anchor matches](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-story-bridge-20261003-navigation-draw.json),
[0.0.11 metadata](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-two-world-first-scene-0.0.11.stage.json).
Live button/menu/autopilot acceptance remains pending in an isolated desktop
run; this is neither Android ELF acceptance nor Godot implementation.

### Live correction of the HUD owner field in private iOS 0.0.12

The0.0.11 draw gate did not become active: its owner comparison mistakenly
used MGame+0x78, which is Radar. Its recorded navigation_position=[0,0] read
Radar bytes and is excluded as HUD evidence. Native LevelScript constructor
call at0x1621FC..162200 proves r2=game+0x6C (Hud), r1=game+0x70 (Level),
r3=game+0x78 (Radar); exact bytes are `ea6e296fab6f`.
Private0.0.12 corrects the hook and observer to+0x6C. The CPU check now asserts
these owned callsite bytes and uses distinct Hud/Radar pointers rather than
letting a fixture alias conceal the mistake. Bridge size/dispatcher VM remain
864 bytes/0x5035D0. Live menu/navigation acceptance remains pending.
Do not transfer these iOS field offsets to the Android MGame layout.

### Bounded live P result, private iOS 0.0.12

Ordinary New Game / Easy in an isolated sandbox now visibly draws the orbit
navigation button left above the joystick. Correct Hud origin reads [17,165]
in client coordinates, excluding the window decoration. Clicking the visible
button did not open the orbit menu; ordinary Pause did work. Neither a global
touch-remap defect nor a working orbit action is proved. Trace native
FDB24 hit-test -> FDFCC retained action -> FE158 release -> MGame gates next;
do not compensate coordinates without observing this chain.

Native radio sources196..199 (appended text IDs3583..3586) start/finish in
order; original Christine portrait/caption are visible. Observed phase4/error0,
autopilot=[0,0,0], route index0. Waypoint passage/combat remain unaccepted.
[P snapshots](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-navigation-p/scene-trace.json).
Only this owned diagnostic process was paused then closed; evidence and its
isolated sandbox remain. No scene/actor/route data injection, Android ELF
acceptance, Godot implementation or full campaign completion is claimed.

## 18. Native cinematic input lock and live waypoint progression

**Source-backed, owned iOS 1.1.4 only:** `0x14B2D4` is the eight-byte getter
returning `LevelScript+0x1C & 1`. The early MGame gate at `0x16686E` branches
to the movie-skip path at `0x16688C` before orbit navigation and weapon input.
`0x14B0BC` does not clear this lock in the custom campaign-0 scene: its own
native skip conditions do not hold. Camera reset/unfreeze is a different
contract and does not clear the script flag.

**Confirmed live in private 0.0.12:** hit-test `0xFDB24`, result `0xFDFD8`,
release `0xFE158`, and the MGame release callsite `0x164B74` receive a normal
button click and action64. The navigation guard is not reached; script byte
0x1C remains1. This rules out coordinate adjustment as the fix for that click.
The custom LevelScript process skips the stock tutorial movie-completion
transition, leaving its constructor's movie lock behind.

Private0.0.13 adds `movs r0,#0; strb.w r0,[r8,#0x1c]` only after successful
actor/event installation, bridge binding and camera unfreeze. CPU coverage
executes the real native early input gate: movie1 blocks; installed scene
movie0 with living hull proceeds; dead hull still blocks. Neighbor byte0x1D,
native fallback and unrelated cinematics remain unchanged. Do not replace
the getter globally or copy these offsets into Android C++.

**Confirmed live on desktop:** ordinary New Game/Easy -> visible orbit button
-> native menu -> Waypoint activates native autopilot. Route index advances
0->1->2; Radio source200 starts at index1 near `[541,943,88036]`, source201
at index2 near `[9781,78011,80286]`. Observed autopilot is active with its
waypoint flag1 and native target pointers; script movie byte0. Original
Christine name/portrait render. No scene, position, route or autopilot data
was injected by the observer; temporary traps restore the native instruction.
Later in the same unmodified runtime, proximity to the final waypoint wakes
all five hostile KI from state5/active0 to state1/active1. The observer's
`sleep_byte` at KI+0xAD remains1, so do not mistake that byte for the current
sleep state without checking its native semantics. At
Radio212 finish the route is3, autopilot is[0,0,0], player position is near
[30257,69850,312985]. Ordinary fire-button input produces visible muzzle
flashes/projectiles. A fabricated wake timer is not required for this scene;
the exact native proximity threshold is not established by these snapshots.
Killing all enemies, mission completion, all13 missions, bank travel and cold
Continue are not accepted. Audio was intentionally null in this diagnostic run.

[0.0.12 input evidence](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-navigation-input-q/scene-trace.json),
[0.0.13 live progression](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-navigation-movie-r/scene-trace.json),
[63/63 owned native anchors](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-story-bridge-20261003-movie-input.json),
[exact package metadata](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-two-world-first-scene-0.0.13.stage.json).
Current local gate:96 patch +8 observer/story +4 extractor +11 root probes
=119 passed, no skips. Dispatcher is930 bytes at0x5035D0; bridge864 bytes.
No native Android or Godot runtime is changed by this technical note.

## 19. GOF1 mission completion lifecycle and return destination table

**Source-backed, owned GOF1 iOS1.1.5 only:** `MGame::successCheck0x449A0`
latches successful Objective completion, changes the victory camera and makes
the player invulnerable. It waits for `Radio::lastMessageShown0x591FC`, opens
LootWindow for non-instant missions and resets the 64-bit mission timer to
20000 ms. Finish requires timer >25000 ms and another lastMessageShown check.
The normal threshold difference is approximately five seconds; this is not
a measured LootWindow interaction duration. Literal values are read at
`0x44BE0=3000`, `0x44BF0=20000`, `0x44BF4=25000`.

Owned native call anchors:

| Callsite | Target | Meaning |
|---|---|---|
| 0x448C8 | 0x5E36C | Status.missionEnd, before campaign-mode branch |
| 0x448F4 | 0x5DFB0 | Status.campaignMode after missionEnd |
| 0x44910 | 0x19A1E | Galaxy.getStation, r1=quadrant0 |
| 0x44918 | 0x5E168 | Status.setStation |
| 0x44A3A, 0x44B18 | 0x591FC | Radio.lastMessageShown |
| 0x44B22 | 0x44884 | MGame.finishLevel |

GOF1 Radio getter bytes `007f7047` read byte+0x1C. GOF2 iOS getter
`0x1A35C0` instead reads+0x28 bit0. These are not interchangeable layouts.
RadioMessage.triggered is likewise not RadioMessage.finished; using an empty
current-message pointer alone risks settlement in a gap before closing lines.

**Confirmed data:** literal0x4499C resolves to0x15AD60, 104 bytes, SHA-256
`837f9167616ecd903217af0d88cdf826be16a6ba7cf89d098c3093e558f26947`.
The13 exact `(quadrant0 local system, station slot)` pairs are:

```text
(0,0), (0,0), (0,1), (0,2), (0,3), (0,3), (0,4),
(0,4), (1,0), (1,1), (1,1), (1,1), (1,1)
```

These map to source global station IDs
`0,0,1,2,3,3,4,4,5,6,6,6,6`: Eden Prime, Phoebos, Tortuga, Vega15,
Isinor12, Opos and Novaterra. The exact mapping is exported per mission in
the local source inventory. The private append-only GOF2 adapter adds135 to
source station IDs; this is our adapter's namespace, not original GOF1 IDs.

**Terminal control-flow caveat:** missionEnd increments completed missions
and sets campaign index to `Mission.level+1`; when >12 it clears campaignMode.
finishLevel tests campaignMode only afterwards. Thus table entry12 exists
but the terminal completion does not take its setStation branch. Do not
mistake raw table presence for an executed final-mission return.

finishLevel first obtains friendsLeft/asteroidsDestroyed and applies kills.
For campaign mission8 it sets reward to `base_reward*(friends_left-1)`.
Its non-instant destination application-module5 call is GOF1-specific;
there is no proof that GOF2 module5 is a compatible station/settlement path.
Both settlement ownership and reward timing need target-native call evidence
before invoking them from a mod. Android equivalents need an independent
ARMv7 layout/call audit; no iOS field offset is copied into C++ here.

### Private GOF2 readiness gate and evidence limits

Private0.0.14 bridge process-state+24 is0 (not won),1 (Objective latched),
or2 (closing dialogue finished). Only its bound mission0 context may use
the override. Stage2 requires current radio=null, native lastMessageShown,
event16 belonging to this Radio, appended text3595 (source208), trigger6,
predecessor15, triggered+20 and finished+21. Tutorial text212/event14 is
not accepted as the closing event. Native success return remains0; no
settlement, stock campaign progression, reward or save is written.

CPU coverage executes owned native RadioMessage.finish0x1A3E6C and
lastMessageShown0x1A35C0, checks early/not-triggered/not-finished/wrong-owner
and malformed event cases at both supported slides. Objective victory uses
a fixture surviving-hostile count0, so this is not combat acceptance.
Observer completion_snapshot reads native remaining-hostile/friend counters,
radio/current/final-event and readiness; it never injects progress and marks
settlement_or_save_accepted=false.

Bridge968 bytes at0x503270; dispatcher930 bytes at0x503638. Diagnostics must
use exact per-package metadata, not the previous dispatch address.
Package SHA-256 `5f77c156001ec98a9fc749591ac23099c2850619114bff57186ec9cddbf44f6e`;
derived executable SHA-256
`94a0741e63334001499bbc3138522ac14f6b3b94d4c59285f0d75d90fa5e7df9`.

[Original story inventory](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-story-source-inventory.json),
[extractor and call validation](C:/Users/Dima/Documents/SuperDuperHyperHle/dev-scripts/extract_gof1_story.py),
[exact0.0.14 metadata](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-two-world-first-scene-0.0.14.stage.json).
Fresh gate:97 patch +9 observer/story +5 extractor +11 ship/world
=122 passed, no skips. Extractor rejects changed calls/getter/quadrant and
out-of-domain destination system. Complete13-mission runtime, rewards,
bank switching/travel and cold Continue remain open.

The isolated0.0.14 desktop run displayed a black window before an observed
GOF1 scene; no completion snapshots were produced. Its log reports a live
presentRenderbuffer loop, not a proven root cause. The exact owned diagnostic
process was closed and evidence/sandbox retained. Earlier0.0.13 waypoint/fire
acceptance is not acceptance of0.0.14 settlement. No Android native or Godot
runtime changes, native rebuild or byte-match improvement are claimed.

## 20 Submitted frame and host presentation evidence

**Confirmed observation, private iOS mod 0.0.14 on desktop:** a captured
submitted renderbuffer is 480x320 and contains the GOF2 HD title and Tap to
Continue while the host window was observed black. The capture establishes
an application-generated title frame, not correct final presentation,
working menu input or mission execution. PNG is a format conversion of the
captured PPM, not a reconstructed reference image.

An explicit diagnostic direct-EAGL run still had a black host window.
Its log confirms that Core Animation composition is suppressed by that
override. GLES1-on-GL2 in the initial log belongs to the emulator's internal
UI context; the subsequent guest context successfully uses native GLES2.
Do not attribute a guest GLES2 presentation defect to the internal GLES1
fallback. The trace includes
0x502 errors and attribute/uniform queries against program 0xFFFFFFFF with
0x501 results. **Needs confirmation:** neither their relationship to final
presentation nor a causal regression in the story bridge is established.
Do not repair native campaign data or rewrite every shader on this evidence.
The per-launch override and GL trace are diagnostics, not accepted game fixes.

Useful recovery/testing boundary: distinguish the native game's submitted
framebuffer from the platform/compositor output. A framebuffer capture alone
cannot certify the latter. Keep the same frame, camera, viewport, context and
options when comparing the two. Raw ReadPixels row order must also be
accounted for before diagnosing a native orientation defect. No Android ARM
or recovered C++ renderer defect is inferred from this desktop emulator run.

[Submitted title frame](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-completion-present-v/submitted-frame.png),
SHA-256 `fa915e447eb7fa33486ba3864e0d13a24ebb30811821069cdb9665827b7d93b4`;
[matching diagnostic log](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-completion-present-v/stderr.log);
[earlier non-GDB submitted frame](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-completion-direct-t/submitted-frame.png).
Root cause of direct presentation remains unaccepted. This is a research
handoff only: no recovered native source edit or native build is claimed.

### Framebuffer comparison and working desktop composition

**Confirmed observation:** the opt-in read-FBO diagnostic reads nonzero
pixels from both the guest renderbuffer and host default framebuffer before
the swap while the visible window remains black. At frame120 the centers
are respectively RGBA[27,27,27,255] and[18,18,18,255]; read errors are0 and
guest framebuffer status is0x8CD5 (complete). At frame600 both centers are
[255,255,255,255]. This is a sampled pixel comparison, not equality of the
complete images. It narrows the desktop symptom to a later output stage but
does not prove a native game or driver defect.

The existing `--force-composition` route on the same private0.0.14 IPA and
cached emulator produces a visible title, functioning menu and the GOF1
start scene through ordinary New Game/Easy input. Normal navigation exposes
Waypoint; native route advancement and original Christine's portrait are
observed. Ordinary double-click on Fire enables the game's automatic fire.
No objective count, actor position, reward or save was injected. This is
bounded scene/input evidence, **not mission completion or durable Continue**.
Audio is intentionally routed to the per-launch null backend and was not
tested. No phone result is inferred.

[Read-FBO log](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-completion-readfbo-w/stderr.log),
[composition run log](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-completion-composition-x/stderr.log),
[read-only scene observations](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-20261003-completion-composition-x/scene-trace.json).
The latter is a live diagnostic record, not a finalized successful campaign.
The launcher has opt-in `ReadFboProbe` and `ForceComposition` parameters;
normal defaults and ordinary game settings are unchanged.

**Needs confirmation:** the current emulator GLES2 direct-present helper
restores the guest framebuffer before its caller swaps the host window,
whereas its GLES1 route swaps before restoring it. A controlled test of that
ordering is a possible next emulator investigation, not an iOS game patch.
SDL's documented framebuffer0 requirement explicitly mentions macOS;
it must not be treated as proof of this Windows cause.
[SDL_GL_SwapWindow](https://wiki.libsdl.org/SDL2/SDL_GL_SwapWindow).

### iOS 1.1.4 survivor accounting and gun initialization order

The exact owned iOS executable separates configured ship totals from cached
survivors. `Level+0x16C/+0x160` hold the configured hostile/friend counts;
`Level+0x118/+0x11C` hold the surviving counts consumed by mission checks.
The stock loader computes the latter from its original three actors before
the private GOF1 scene replaces the actor array. Changing only the configured
totals leaves the mission reading 3/0 instead of 5/1.

The owned iOS Thumb entries are `getEnemiesLeft 0x12E1F0`,
`getFriendsLeft 0x12E1F8`, hostile death accounting `0x12E158`, friend death
accounting `0x12E17C`, and `assignGuns 0x11FD24`. The enemy death body also
increments the global kill counter at `Status+0xAC`; its PC-relative global
pointer slot is `0x422044`. The exact-byte probe matches 68/68 owned iOS
anchors in the current inventory. A CPU fixture binds that global slot like
dyld and executes the original death/getter instructions at slides 0 and
0x10000: five hostile deaths reduce 5→0 and one friend death reduces 1→0.
The six-actor mod installs the initial 5/1 once before gameplay. This is
source-backed iOS behavior and CPU evidence, not an Android layout assertion
or a live mission completion.

`assignGuns` runs during native stock load before the mod adds three actors.
Its body first releases `Level::enemyGuns`, then allocates weapon objects and
passes them to the actors. The recovered Android `Player::addGun` replaces a
nested slot array; a second full `assignGuns` call therefore has an ownership
risk for weapons already issued. The exact iOS function prologue is verified;
the subsequent live slot probe below shows that no wholesale replay is needed
for this scene. Do not transfer the iOS field offsets to Android without its
own ARM layout check. No recovered C++ source was edited or built in this pass.

[Mod status and diagnostic boundary](C:/Users/Dima/Documents/SuperDuperHyperHle/docs/GOF1_TWO_WORLDS_MOD_STATE.md),
[owned iOS anchor report](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof2-story-bridge-20261003-survivor-accounting.json).

The following ordinary local iOS run resolved the suspected gun deficit for
this particular scene. Immediately after native scene continuation, the
read-only observer saw six actors and survivor counts 5/1. Every actor,
including the three new ones, had observed nested weapon-slot counts
`1,1,0`. Thus `createShip`/existing setup yields populated slots here despite
the earlier `assignGuns` timing; do not add a second wholesale `assignGuns`
call. The sample does not establish weapon identity, firing behavior, a won
mission or durable save. Subsequent radio snapshots still have success stage
zero and no accepted settlement/save. This is an iOS 1.1.4 observation, not
evidence that the recovered Android layout or implementation is identical.

[Read-only six-actor scene trace](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-survivors-weapons-z/scene-trace.json).

An independent New Game/Easy run reproduced the visible Keith/Christine
exchange and true Christine portrait. The observer again recorded six armed
actors and 5/1 survivors. In the bounded sample route index remained zero,
success stage zero and settlement/save false; that is a verification limit,
not yet a proven defect in waypoint progression. The isolated run was closed
after collecting evidence. [Repeat trace](C:/Users/Dima/Documents/SuperDuperHyperHle/artifacts/gof1-first-scene-survivors-weapons-aa/scene-trace.json).
