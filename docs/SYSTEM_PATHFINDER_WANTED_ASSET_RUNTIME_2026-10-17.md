# Real pathfinder in wanted movement (2026-10-17)

The Android 2.0.16 ARM binary places
`SystemPathFinder::getSystemPath(Array<SolarSystem*>*,int,int)` at
`0x11b7a8` and `search(Node*,Node*)` at `0x11b968` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
The former constructs one graph node per system, adds visible routes,
searches from the current to the target system, and returns an `Array<int>`
whose first element is the starting system and whose later elements are
successive hops. `Status::moveWanted` uses element 1 as the next system's
warp-gate source. This matches the recovered source's path layout;
there is no production-source change in this package.

The asset-backed host fixture now links the **production**
`SystemPathFinder.cpp` and `Node.cpp` instead of substituting a path.
After starting a game from seven real local Android catalogs, it makes
systems visible and checks a route from system 20 to a catalog neighbor.
It also discovers a reachable target with a path of at least three
systems, checks the start/end IDs, and executes the production
`Status::moveWanted` cross-system branch. The wanted record moves to the
warp gate of the route's **next** system, not directly to the target.
Both `-O0` and `-O2` tests pass with **66/66** balanced file opens/closes.

The focused four-symbol ARM family report at
`A:/Galaxy on Fire new version/_work/system_pathfinder_focused_20261017.json`
is **97.0%** average source-shape and **81.0%** strict fuzzy, with one
linked-exact and one byte-exact (`contains`). In particular,
`getSystemPath` is **96.0% / 59.1%** source-shape/strict fuzzy and
nonexact; `search` is **99.0% / 86.6%**, nonexact. Native `gof2` and
40 verifier tests (one existing skip) pass. Because no production source
changed, the last full ARM corpus remains applicable: **208/208** TUs,
**4,323/4,524** original functions in **4,730** rows,
**93.78% / 82.95%** average source-shape/strict fuzzy, **2,527
linked-exact / 1,076 byte-exact**, 201 absent, zero wrong-type or
skipped units, and no exact-pair change.

Test boundaries: `Globals::getRandomStation` still makes a deterministic
catalog-backed choice, while the production pathfinder, saved station
loads, systems, visibility graph and wanted movement execute. Generator,
stochastic choice, repeated failed routes, ownership under sustained
path queries, `Level::init` and actual flight remain unverified. Next:
integrate the real Generator or enter a bounded `Level::init` stage with
explicit graphics/platform boundaries.
