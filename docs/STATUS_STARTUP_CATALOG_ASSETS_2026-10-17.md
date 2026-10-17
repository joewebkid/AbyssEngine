# Status startup catalog assets (2026-10-17)

`Status` startup calls `loadAgents()` and `loadWanted()` after the
item/ship catalog setup. The host file-stack test now compiles the
unchanged production bodies of those two FileRead loaders and the
Agent/Wanted constructors, destructors and image-part setters. It
uses the real Android 2.0.16 `agents.bin` and `wanted.bin` through
AEFile and the native read-only asset bridge; no assets are committed.

At host `-O0` and `-O2`, all **27 agents** load with sequential IDs.
Every agent's station belongs to the agent's declared system in the
separately loaded station and system catalogs. Agent 0's name,
station, blueprint offer and image parts, and agent 26's name and
location, agree with the local independent `agents.json` decoding.
All **25 wanted entries** load with sequential IDs, and every ship
and loot-item ID is within the loaded ship/item catalogs. The first
entry's name, ship, loot and reward agree with the independent local
`wanted.json` decoding. That JSON contains only the first wanted
entry, so the remaining 24 entries have **structural checks only**;
their complete semantics are not independently verified.

Before mounting the asset directory both loaders return null. Across
the seven mounted binary resources and 34 per-system station loads,
**44** streams open and **44** close. The native `gof2` target and
40 verifier tests (one pre-existing skip) pass. Android ARM REA/IDA
confirms the loaders' resource paths, 27/25 array lengths and
five-byte optional image-part reads at `loadAgents` circa `0x1202d4`
and `loadWanted` circa `0x1204ac` in the Android binary with SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

No production C++ changed, so the most recent full ARM corpus remains
4,323/4,524 originals, 93.76% average source-shape, 82.91% strict
fuzzy, 2,521 linked-exact and 1,076 byte-exact. In that report,
`loadAgents` is 95.6%/48.0% and `loadWanted` 96.4%/63.2%
source-shape/strict fuzzy; neither is exact. These are inherited
figures, not gains from the host test.

This does not execute `Status::activate`, save loading, missions or
station-to-flight gameplay. The next integration package should
exercise an actual `Status` startup path with explicit unresolved
service boundaries, then connect that state to a flight-scene entry.
