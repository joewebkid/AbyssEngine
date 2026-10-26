# Menu touch dispatch and ARM64 closure (2026-10-26)

Reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`MenuTouchWindow::OnTouchBegin` starts at `0x139bd4` and `OnTouchMove`
at `0x13a33c` in the Ghidra listing. Their direct calls pass both
incoming coordinates to `TouchButton`, `TouchSlider`, `ChoiceWindow`,
`Layout`, `ScrollTouchWindow`, and `MissionsWindow`. The project had
14 undefined touch-dispatch wrappers; most accepted only the first
coordinate. The calls now reach the existing typed methods with both
coordinates. The row-selection division calls the same arithmetic
operation directly instead of an undefined `_mtw_idiv` wrapper.

Native and AArch64 static `gof2` builds pass. ARMv7 focused verify
compiles 209/209 units and compares 48 symbols with zero skips.
`OnTouchBegin` source-shape changes **94.0% to 94.7%**, strict fuzzy
**30.5% to 28.9%**; `OnTouchMove` changes **86.1% to 87.7%** and
**22.5% to 25.7%**. Both remain nonexact. The unit retains all **21
linked-exact / 6 byte-exact** matches. The lower Begin fuzzy score
does not override the confirmed two-coordinate call contract.

Strict AArch64 diagnostics show JNI-reachable unresolved names falling
from **194 to 179**, and whole-archive unresolved names from **434 to
419**, with no new names. The remaining menu route still has audio,
application-data, rendering and steering helpers; AArch64 shared
linking fails. Menu touch behavior has not been exercised on a device.

The full ARMv7 report compares **4,323/4,524** original functions over
**4,745** pairs, **94.76% source-shape / 84.26% strict fuzzy**,
**2,606 linked-exact / 1,083 byte-exact**, zero skips. Both exact
symbol sets are identical to the preceding full report with zero
losses. The local report is at
`D:\gof2-arm64-crypto\mtw_touch_full_20261026.json` outside Git.

Next priority: source-backed `ParticleSystemMesh` matrix and mesh
operations, then the remaining PaintCanvas helpers and audio backend.
