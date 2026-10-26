# PaintCanvas destructor recovery (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`;
`PaintCanvas::~PaintCanvas` at `0x77a70` in the original ARMv7 library.

The destructor now calls the recovered `ReleaseAllResources`, invokes the
typed `ResourceTexture`, `ResourceMesh`, and `ResourceTransform` destructors,
releases the two canvas meshes through `MeshRelease`, and destroys cube
texture records through `AELoadedTexture`. The original `ArrayRelease` calls
free the glow arrays' backing storage only. The previous recovery also
interpreted unsigned glow colors as pointers and tried to free them, which
would corrupt the heap. The raw glow, transform, and mesh arrays now use
their named pointer fields, avoiding the 32-bit header reinterpretation that
read incorrect pointers on AArch64. Typed `Array` members use their normal
C++ destructors.

The production destructor body passes a 64-bit host smoke test at `-O0` and
`-O2`, including five resource types, glow colors, cube texture strings, and
the mesh lifetime. Native `gof2` and AArch64 static archives build. Focused
ARMv7 verification compares 198 `PaintCanvas` symbols with **94 linked-exact**
and **23 byte-exact**; neither exact set loses a symbol. Both destructor
aliases rise from **79.2% to 88.0% source-shape** and **30.2% to 68.1%
strict fuzzy**. A strict JNI-initialize AArch64 link still has **130**
unresolved names. The broader `JNI_OnLoad` link probe falls from **354 to
347** unresolved names, removing the seven destructor placeholders with no
new names. These probes are diagnostics; neither produces a game `.so`.

The full ARMv7 corpus compares **4,323/4,524** original functions over
**4,745** pairs, with zero skips. Its averages rise from **94.77% to
94.78% source-shape** and **84.27% to 84.29% strict fuzzy**. The
**2,607 linked-exact** and **1,083 byte-exact** sets have no symbol-level
losses or gains versus the preceding full report.

The original code and the host smoke test establish the destructor's local
ownership behavior. Live GLES release and full Android gameplay are still
unverified. Other methods that reinterpret the five raw glow fields as
`Array<T>` may also need AArch64-safe field access before gameplay can run.
