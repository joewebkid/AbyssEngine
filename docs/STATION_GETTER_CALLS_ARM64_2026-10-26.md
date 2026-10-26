# Typed station and system calls in the AArch64 link path (2026-10-26)

Primary evidence: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The local IDA decompilation confirms direct calls to `Status::getStation`,
`Station::getIndex`, `Station::getSystem`, `Status::getSystem`, and
`SolarSystem::getRace` in `ModStation::ModStation` (`0xd52d0`),
`ModStation::resetIdleCamForHangar` (`0xd8cf0`), `GameRecord::load`
(`0x1559f0`), `CutScene::process` (`0x98e8c`),
`PlayerEgo::isDockedToMiningPlant` (`0xa1634`),
and the affected `MGame` routes. The named methods already exist in this tree.

Five source units now call those typed methods in place of eight unresolved
global helpers. `ModStation::ModStation` previously called a receiverless
`SolarSystem_getRace_msc()` after discarding `Status_getSystem_msc()`'s
result; it now reads the race from the current `SolarSystem`, as the ARM
reference does. The repeated station fetch in the second branch is retained
because the original performs it. No synthetic null guard or altered station
choice was added.

The five focused ARM verifier units compile. Their previously exact symbols
retain all linked-exact and byte-exact matches. Both constructor aliases
in `ModStation` improve from **86.1% to 86.8% source-shape** and from
**38.1% to 39.8% strict fuzzy**. The other affected functions retain their
previous reported percentages; their relocation targets change to the
existing typed functions without changing the focused instruction metrics.
The full ARM corpus compares **4,323/4,524** original functions over
**4,745** symbol/unit pairs, with **94.79% source-shape / 84.29% strict
fuzzy**, **2,607 linked-exact / 1,083 byte-exact**, and zero skips.
The linked-exact and byte-exact symbol sets have zero gains and losses
against the preceding full report.

Native `gof2` and AArch64 `gof2` static-library builds pass. The strict
JNI-initialize AArch64 link diagnostic drops from **115 to 107** unique
unresolved names; the broader JNI-onload diagnostic drops from **333 to
325**, with no new names in either. This is still not a game `.so` or APK.
Of the 107 remaining strict names, 42 are FMOD/FModSound calls and 30 are
PaintCanvas helper calls; the remaining names span UI, math, constructors,
JNI, and gameplay bridges. These category counts describe the link error
inventory, not an estimate of playable-game completeness.

Live Android execution of the changed station paths remains unverified.
The next source-backed link packages should address coherent PaintCanvas,
UI, or FMOD caller families. Audio on arm64 needs a separate backend and
sound-bank compatibility work: the FMOD Ex API used by this game has no
Android arm64 release, and current FMOD does not directly accept its old
Designer event content. See the
[FMOD maintainer's arm64 statement](https://qa.fmod.com/t/where-can-i-get-a-fmodex-pre-built-library-to-use-on-build/14634?page=5)
and [FMOD's bank compatibility note](https://www.fmod.com/docs/2.04/api/glossary.html).
