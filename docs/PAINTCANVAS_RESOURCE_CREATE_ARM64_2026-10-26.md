# PaintCanvas mesh and sprite creation (2026-10-26)

The original Android ARMv7 `PaintCanvas` constructor at `0x77674`
calls `AbyssEngine::MeshCreate` for the line and quad meshes, then
writes line indices `0x20000`, `1`, `0x20003`. IDA shows the last
literal directly; the ELF bytes at virtual/file offset `0x77878`
are `03 00 02 00`. Ghidra displays the same literal pool at
`DAT_00087878` after its `+0x10000` image rebase. The previous
recovered constructor treated that Ghidra address as a runtime
pointer, which would be invalid on AArch64.

The constructor and two `PaintCanvas::MeshCreate` overloads now call
the existing typed engine `MeshCreate`, and the two
`PaintCanvas::SpriteSystemCreate` overloads call the existing typed
`SpriteSystemCreate`. The constructor reads indices through `Mesh`;
its `lineMesh` and `quad2dMesh` fields are typed `Mesh*`. The sprite
system's `+0x10` member is `Mesh*`, so the material overload writes
`ss->mesh->material` without truncating an AArch64 pointer through
the old `Node::field_0x30` integer view. ARMv7 struct layout asserts
and the original call path support these mappings.

The focused ARMv7 `PaintCanvas` corpus compares **198** symbols.
Both previously linked-exact resource-creation methods remain exact;
the focused set stays at **93 linked-exact / 23 byte-exact** with no
losses. Mean source-shape changes **96.01% → 95.98%** and strict fuzzy
**82.91% → 82.87%**. The material-bearing `MeshCreate` drops
**97.9% → 93.0% source-shape** as its synthetic external call and
untyped mesh pointer are replaced by the real call and `Mesh*`.
This is an accepted nonexact change for source-backed behavior and
AArch64 pointer correctness, not a claim of compiler equivalence.

The native and AArch64 static libraries build. A 64-bit host smoke
test extracts the production methods and passes at `-O0` and `-O2`:
resource creation, empty-slot reuse, append, failure, and preservation
of the full material pointer. Strict JNI-reachable AArch64 unresolved
names fall **143 → 139**, removing four creation placeholders with
none added.

Live GL-backed resource creation, the constructor's full runtime
effects, destruction, and device rendering remain unverified. The
shared game library and APK still do not link; resource release,
remaining render calls, and compatible audio are next priorities.
