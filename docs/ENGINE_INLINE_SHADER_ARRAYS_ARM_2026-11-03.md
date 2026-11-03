# Engine shader and triangle-count array ABI

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `Engine` constructor passes `this+0x3d8` to
`Array<int>::Array()` at `0x83f56–0x83f58` and `this+0x510` to
`Array<ShaderBaseStruct*>::Array()` at `0x83f80–0x83f82`.
`ShaderRegister` passes those same embedded addresses to `ArrayAdd`
at `0x84a5e–0x84a70`. The destructor calls `ArrayReleaseClasses` on
`this+0x510` at `0x84190–0x84196`, then destroys both array members
at `0x841d0–0x841da`.

The reconstruction instead stored pointers to separately allocated
arrays at those locations. This changed the 32-bit `Engine` layout
and made shader registration and post-effect dispatch use a different
object representation from the original. The two arrays are now
embedded members at the confirmed offsets, guarded by ARM layout
assertions. The constructor no longer allocates separate arrays;
the destructor uses the original `ArrayReleaseClasses` operation and
lets the member destructors release their storage. `ShaderRegister`
also destroys its temporary name before shader initialization and
uses `operator delete[]` for the allocated character buffer, as the
original does at `0x84a4e–0x84a74`.

Focused ARM verification compiled **209/209** translation units.
`ShaderRegister` improved **97.5% → 99.2% source-shape** and
**86.6% → 96.9% strict fuzzy**. `DoPostEffect` improved
**97.8% → 98.4% source-shape** and **87.9% → 89.7% strict fuzzy**.
`SwapBuffer` and `ReloadShaders` both became byte-exact from
**92.5%/76.2%** and **95.5%/81.8%** source-shape/strict fuzzy.
The `Engine` destructor improved **88.0% → 93.9% source-shape**;
the constructor improved **88.4% → 88.9%** and remains substantially
nonmatching. `ShaderInit` stays **99.7% source-shape**.

Windows native and ARM64 static builds pass. The source-extracted
post-effect routing smoke passes at `-O0` and `-O2`. The full ARM
corpus compared **4,323/4,524** originals in **4,763** pairs at
**96.25%** mean source-shape and **86.04%** strict fuzzy, with
**2,746 linked-exact**, **1,098 byte-exact**, 201 missing and zero
skipped. Against the preceding full report, the sole new pair is
the `ArrayReleaseClasses<ShaderBaseStruct*>` instance. Four
linked-exact symbols were gained: that helper, `Engine::ShaderUpdate`,
`SwapBuffer` and `ReloadShaders`. The latter two are also the only
byte-exact gains. No comparison pair or exact match was lost.

Live shader registration and GLES rendering remain unverified. The
constructor still differs materially in other field initialization
and exact construction order;
next audit those differences and exercise effect rendering on a
permitted host when available. Redmi testing remains stopped by user
request.
