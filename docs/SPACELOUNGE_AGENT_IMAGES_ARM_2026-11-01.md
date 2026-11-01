# SpaceLounge agent images and cleanup

Date: 2026-11-01. Reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The relevant original functions are `SpaceLounge::init()` at `0x169f80`
and `SpaceLounge::~SpaceLounge()` at `0x16a89a`.

The original `init()` allocates one character-image array per station
agent and two zero vectors per agent when the silhouette and rectangle
arrays are absent (`0x169fcc`–`0x16a07a`). It also loads default agent
image parts from a five-word zero-initialized BSS descriptor at
`0x2183d4` (`0x16a082`–`0x16a098`). The reconstruction now performs
these steps using the existing typed arrays and `ImageFactory::loadChar`.
It omits a transient allocation that the original immediately overwrites
before use.

The original destructor releases owned strings, buttons, image parts,
per-agent image arrays, and vectors before deleting their containers
(`0x16a8c8`–`0x16a976`). The previous source cleared the arrays without
deleting their elements and returned early whenever default agent image
parts existed. The corrected source releases the contents and completes
destruction of the remaining objects.

Focused ARM comparison: `init()` **60.1% → 70.4% source-shape** and
**24.6% → 32.6% strict fuzzy**; both destructor variants **84.9% → 90.3%
source-shape** and **22.3% → 27.0% strict fuzzy**. Windows and ARM64 static
builds pass; ARMv7 compiles 209/209 units. The focused unit has no
linked-exact or byte-exact losses. None of the three changed functions is
exact.

The image and rectangle arrays are only initialized here; later geometry
and selection depend on `updateScreenPositions()`, which still has unbound
pointers. The iPad layout, scene lighting, post effects, mission-specific
auto-chat, and `startChat()` remain separate work. Live behavior is not
verified while device testing is deferred.

The full ARM corpus covers **4,323/4,524** original functions over **4,761**
comparison pairs at **96.05%** mean source-shape and **85.82%** strict fuzzy.
It has **2,733 linked-exact**, **1,091 byte-exact**, zero skipped, and 201
missing originals. Six newly instantiated array helpers gained linked-exact;
no prior linked-exact or byte-exact symbols were lost. The old non-exact
`ArrayRemoveAll<String *>` comparison is absent because cleanup now uses
the original `ArrayReleaseClasses<String *>` form. Reports:
`D:/gof2-arm64-crypto/space-agents-20261101-focused.json` and
`D:/gof2-arm64-crypto/space-agents-20261101-full.json`.
