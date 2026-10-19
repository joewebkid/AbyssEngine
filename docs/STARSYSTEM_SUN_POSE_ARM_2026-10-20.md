# First sun pose and VectorNormalize ABI — 2026-10-20

## Primary evidence and correction

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `AERandom::nextInt(4096)` in `StarSystem::StarSystem(int)` at
`0x134526–0x134534`, subtracts 2048, and converts that pitch and
`sunSlot * 0xaaa` to floats. At `0x134550–0x134574` each value is multiplied
in sequence by the literals `0x37800000` (`2^-16`) and `0x3e6a6000`
(`0.2288818359375f`) before `AEGeometry::setRotation(Vector const&)`.
The original then passes the literal `0xc69c4000` (`-20000.0f`) to
`AEGeometry::moveForward(float)` at `0x13457c–0x134584`; that call is shared
by sun and non-sun orbit bodies. The former C++ omitted the second RNG draw,
used a different yaw factor and passed `0x447a0000` as a numeric integer,
which converted to a huge float instead of the original distance.

The real `AEGeometry::moveForward` source was calling a declared-but-absent
`VectorNormalize(Vector *, const Vector *)` symbol. ARM `moveForward` calls
`VectorNormalize(Vector const&)` at `0xa49de`, using the hidden return
pointer convention. Two calls in `PlayerFighter::initPush` and one each in
`PlayerGasCloud::explode`/`update` target the same original symbol. The
production callers now use the existing return-by-value method; the false
pointer declaration is removed. A focused test asserts all five original
call sites.

## Executed boundary

The continuous original-asset Level/GLES gate passes at `-O0/-O2` on Windows
SwiftShader and a real Android ARMv7/Adreno device. It starts from the Pan
station fixture, executes the real second draw (958, giving pitch -1090),
rotates the first sun, moves it forward by -20000, stores its position and
publishes the negated light direction. It checks the post-draw seed, exact
rotation fields, bounded world position and copied position/direction. The
same path retains 18 meshes, 17 transforms, 44 collision volumes and 26/26
closed file opens. Native `gof2` and a fresh ABI/lifetime smoke build in a
local C: directory; 208/208 ARM units compile and 86 verifier tests pass with
one pre-existing skip. Focused constructor aliases change
**76.3%/14.3% → 78.1%/13.3%** source-shape/strict fuzzy. `AEGeometry::moveForward`
remains **89.9%/53.6%**; the three affected ship functions retain their
previous local scores. All six are nonexact. The strict decrease for the
constructor is recorded, not optimized away.
The ignored focused unit reports are
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/sun-pose-{star,geom,fighter,gas}-unit.json`;
the before values come from the preceding `sun-scale-full.json` report.

The complete ARM corpus still compares **4,323/4,524** original functions in
**4,735 pairs**, at **93.92%** mean source-shape and **83.19%** strict fuzzy.
It retains **2,551 linked-exact** and **1,079 byte-exact** pairs. Independent
`(unit, symbol)` set comparisons against `sun-scale-full.json` find zero
lost or gained exact pairs of either kind; skipped units and wrong-type
signatures remain zero. The ignored full report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/sun-pose-full.json`
(SHA-256 `6304b675f7dadf1db4903d0041a5c17ba25efa30f4334ba994d06bb8d8864355`).

## Limits

The first non-sun planet and its slot-rejection loop have not executed; the
old non-sun rotation expression is still present and is not asserted as
original behavior. The changed `PlayerFighter` and `PlayerGasCloud` call
sites are ARM-targeted and compile, but their runtime paths were not executed
by this Level gate or the generic ABI smoke. The supernova asset path and
station-texture override also remain unexecuted. The gate stops before the
next planet is built;
complete `StarSystem` lifetime, Level stage 0 and a playable native frame
remain open.
