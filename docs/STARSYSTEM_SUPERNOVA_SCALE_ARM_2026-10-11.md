# StarSystem supernova scale recovery (2026-10-11)

Three `StarSystem` scale paths used implausible floats unrelated to the
Android 2.0.16 ARM literal words. The original `libgof2hdaa.so` confirms:

| Function | ARM evidence | Recovered value |
| --- | --- | --- |
| `switchSunForSupernovaReversal` (`0x134e4c`) | `0x134e74..0x134e82` repeats literal `0x3e6a6000` in a stack `Vector` and calls `AEGeometry::setScaling(Vector const&)` | `0.2288818359375f` on each axis |
| `switchSunForSupernovaExpansion` (`0x134fc4`) | `0x134fd4..0x134fe2` repeats literal `0x3fafc800` in the same overload | `1.373291015625f` on each axis |
| `scaleSunDuringSupernovaIntro` (`0x134ffc`) | `0x135024..0x135042` multiplies integer delta by literal `0x3827c5ac` and adds the current X scale before `setScaling(float,float,float)` | `0.00004f` per delta unit |

The previous source values were `2.429073312463973e24f`,
`11332.0703125f`, and `-9.769497830779909e32f`, respectively. The two
constant-vector paths now call the original `Vector const&` overload. The
delta path retains its existing scalar overload and control flow. The new ARM
object contains little-endian words `00606a3e`, `00c8af3f`, and `acc52738`,
matching the three original float bit patterns.

Focused ARM validation builds **206/206** translation units. Both vector
paths rise to **100% source-shape and 100% strict fuzzy** (neither linked-
nor byte-exact); the delta path stays at **96.2% source-shape / 73.0% strict
fuzzy** despite the semantic literal correction. Native `gof2` and 36
verifier tests pass (one skipped). A full ARM corpus result is recorded in
`PROJECT_STATUS.md` and the retained verifier JSON at
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_supernova_20261011.json`:
**4,323/4,524** compared, **91.94%** mean source-shape (+0.03), **80.03%**
strict (+0.03), **2,319 linked-exact**, **997 byte-exact**, no lost exact
pair, no skipped or wrong-type symbols. Supernova gameplay on a device and
the larger `StarSystem` state machine remain untested. The subsequent
[intro transform audit](STARSYSTEM_SUPERNOVA_INTRO_ARM_2026-10-11.md)
removes the null call targets identified here; device gameplay remains the
next validation priority.
