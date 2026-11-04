# Startup frame gaps checked with REA (2026-11-04)

Reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA opened this exact ELF/ARM file in the local IDA provider. Its
`ndk23_newrender` at `0x9412c` calls
`ApplicationManager::OnUpdate(long long)` at `0x827f0` once per render
callback (`0x941d4`). The original `OnUpdate` state-5 path at
`0x829cc–0x82a7a` starts post-effect rendering when active, selects
the refraction buffer, calls module `OnRender3D` and `OnRender2D`,
clears engine/canvas draw counters between them, stops the post-effect
buffer, and calls module `OnUpdate` unless the resume callback consumes
the frame. Its common tail calls `Engine::SwapBuffer` at `0x83732`.
REA evidence: `ev_838431a8f818f3d7237cb346e46fc5487516ece7d4f1a167bcedcbaa4f7a4e13`
and the function dossier for `0x827f0`.

The reconstructed state-5 path previously called the three module
methods, but did not start or stop the post-effect buffer, select the
refraction buffer, reset all four engine counters, or call
`Engine::SwapBuffer`. These calls and stores are now present in the
ARM-observed order. The existing frame-time update is unchanged.
The original also has state 6 (`0x8311c`): call
`PaintCanvas::WarmUpTexture`, then move to state 5 if it succeeds.
REA confirms that `WarmUpTexture` itself simply returns true, and
the current source is linked-exact and byte-exact. State 7
(`0x83180–0x83332`) incrementally loads one pending texture and up
to ten mesh VBOs per frame before returning to state 5. The
reconstructed `OnUpdate` has no state-6 or state-7 branch, so that
dispatch behavior remains absent. Current source references do not
establish that either state is entered on the tested title launch;
the installed diagnostic APK and captured log do not prove it.
The performance overlay also remains to be reconciled separately.

The original state-0 path at `0x8285e–0x8286c` first decrements a
positive namespace `SwapCounter`, clears the buffer, swaps, and
returns, delaying module initialization. The state-1 release path at
`0x828a4–0x828be` reloads that counter from
`Engine::SwapCounter`. When state 0 finishes initialization and
enters state 5, it also resets `loadMesh` and `loadTexture` at
`0x83780–0x83782`. These startup transition effects were absent and
are now restored. Focused `OnUpdate` verification rises from 12.3%
to 14.4% source-shape and from 6.3% to 8.5% strict fuzzy.

The verifier's **201 missing original symbols are all library/ABI
symbols**, not named game or AbyssEngine methods: 74 libzip, 47 zlib,
21 crypto, 55 C++/ABI, and four other ABI/compiler symbols. The names
are in `cmake-build-match/verify/missing.txt`. The separately linked
ARM64 test library already contains `zip_open`, `zip_name_locate`, and
`SHA256` (observed with `llvm-nm`). Thus the missing-symbol count is
not a count of missing game functions and does not by itself diagnose
the black screen. The per-function source-shape comparison identifies
incomplete game paths more directly.

Before this change, `OnUpdate` generated 129 instructions against
1,682 original instructions, with **8.6%** source-shape and **3.8%**
strict fuzzy. Focused verification after the change compiles
**209/209** ARMv7 translation units and reaches **12.3%** source-shape
and **6.3%** strict fuzzy; generated instructions rise from 129 to 167
against 1,682 original instructions. The function remains nonexact.
Windows native and ARM64 static `gof2` builds pass. The full corpus
compares **4,323/4,524** original functions across **4,769** pairs,
with mean **96.75%** source-shape, **86.74%** strict fuzzy,
**2,779 linked-exact** and **1,108 byte-exact**, 201 missing and zero
skipped units. Compared with the preceding steps 21–23 report, the
comparison-pair set and both exact sets have zero additions and zero
losses. Full report: `_work/appmgr_frame_full_20261104.json`.

The Redmi capture on 2026-11-03 shows the test process running behind
the notification shade with the screen off. It contains no verified
visible game frame, and the test app was force-stopped after capture.
On 2026-11-04 Poco X3 connected. REA pulled the installed test package
and verified that its SHA-256 is
`5b45bd3b8fffff5a497f23d925f8e918c9734a4d01f272b44e20222df589cc13`,
matching the diagnostic APK built on 2026-11-02. This APK predates the
frame-sequence correction above. Its first eight bounded `GOF2PROBE`
frame readings show program 3, framebuffer 0, viewport 2048×921,
OpenGL error zero, and a black center pixel `(0,0,0,255)` throughout.
The log also records image 58 with texture handle 3, a valid 378×431
region, four vertices and six indices, while image 13 has no valid
texture. There was no fatal signal or resource-permission error in
the captured process log. REA evidence:
`ev_944b2d6af290d4624e59cbb0499b82a41d9206a972199ccd5fe3927cb2701722`
(installed APK) and
`ev_9cdac4334f809ae38aa8e0ccbf060678f5162e9a7ad3d3a6a5e0a1022d515c8e`
(process log). A Play Games sign-in window covered the game; cancelling
it closed the game, so no foreground game screenshot was obtained.
REA force-stopped the test package after each run. The corrected
frame sequence has not yet been tested on a phone. A subsequent test
must observe a foreground frame, capture its screen and process log,
and force-stop GOF2 afterwards. When replacing the test APK, remove
the previous test package first, as observed by the user; an in-place
update was rejected by Play Protect.

Next startup candidates from the same corpus are
`MTitle::OnRender2D` (**83.5%** source-shape),
`PaintCanvas::DrawImage2D(uint,int,int,uchar,uchar)` (**89.3%**),
`Engine::SetTextures(uint,uint)` (**95.3%**), and `MeshDraw`
(**96.2%**). `Engine::SetTexturesExt` scores **57.1%**, but the direct
title-image path uses `SetTextures`; it needs a separately observed
caller before treating it as the current title blocker. None of these
shape scores alone proves semantic correctness.
`TextureShader::UpdateMeshData` is already 100% source-shape and
linked-exact (though not byte-exact); REA shows the same matrix,
uniform, and vertex-attribute calls as current source, so it is not
the first missing-function target for the title image.

A strict ARM64 shared-library link exposed a separate, concrete
inventory of missing reconstruction helpers. Initially 159 symbols
were unresolved: 158 called by `ModStation.cpp`, plus
`paintcanvas_ext_start_fbo`. REA decompiles the original
`PaintCanvas::StartDraw2FBO` as a direct call to
`Engine::ActivateRender2TextureFBO`, which is already implemented.
REA evidence: `ev_0f0a5720ddf9afdf5bb940b7d73401d62cfbe01127690f5ee64b1032b67a6689`.
The wrapper was replaced with that call. ARM64 static build passes;
the strict shared link now reports **158** unresolved helpers, all
from `ModStation.cpp`. They are documented with first call sites in
[ARM64 link gaps](ARM64_LINK_GAPS_2026-11-04.md). A static-library
build alone did not reveal these link gaps. The new APK cannot yet be
built faithfully from current source, so the Poco log describes an
older diagnostic APK only.
Focused ARMv7 verification still gives `StartDraw2FBO` 100% source-shape,
100% strict fuzzy and linked-exact. Windows native and ARM64 static
builds pass. The final full ARM corpus
(`_work/startup_missing_full_20261104.json`) compares 4,323/4,524
originals across 4,769 pairs, with means 96.75% source-shape and
86.74% strict fuzzy, 2,779 linked-exact, 1,108 byte-exact, 201
missing and zero skipped. Against the prior full report, the pair
set and both exact sets have zero additions or losses.

## Module dispatch ABI correction

REA's original `ApplicationManager::OnUpdate` dispatches the current
module through virtual slots `+0x30` (`OnRender3D`), `+0x34`
(`OnRender2D`), and `+0x38` (`OnUpdate`) at original addresses
`0x82a0e`, `0x82a32`, and `0x82a78`. The reconstructed
`IApplicationModule` declared these methods in the reverse frame order:
`OnUpdate`, `OnRender3D`, `OnRender2D`. Its raw `ModuleVTable` view
also labelled the three slots that way. This would call the wrong
module method at each stage of the frame when using the reconstructed
vtable. Both declarations now follow the observed ARM slot order.
ARMv7 compiles 209/209 units and the focused `OnUpdate` comparison
remains 12.3% source-shape and 6.3% strict fuzzy because the dispatch
instructions themselves did not change; the correction is in the
vtable ABI. Windows native and ARM64 static builds pass. The full ARM
corpus still compares 4,323/4,524 originals in 4,769 pairs, with
96.75% mean source-shape, 86.74% strict fuzzy, 2,779 linked-exact,
1,108 byte-exact, 201 missing, and zero skipped. Against the prior
full report, the pair set and exact sets have zero additions or losses.
The report is `_work/appmgr_vtable_full_20261104.json`. This is a
static ABI correction; the corrected APK has not yet been exercised
on a phone.
