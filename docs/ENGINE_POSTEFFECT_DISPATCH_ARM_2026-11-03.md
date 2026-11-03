# Post-effect shader dispatch and arguments

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Engine::DoPostEffect` begins at `0x8675c`; `Engine::SetPostEffect`
begins at `0x865fc`.

The original `DoPostEffect` has three shader-index GOT relocations:
`0x20ffe8` targets `GlowPPShader::ShaderIndex`, `0x20ffec` targets
`BloomShader::ShaderIndex`, and `0x20fff0` targets
`BlurShader::ShaderIndex`. Its effect bits 2, 1 and 4 use these in
that order. The reconstruction instead used three private integers
with no writers, so all three selections defaulted to shader index
zero. The recovered function now reads the class shader indices.

For bit 4, original ARM dispatches through virtual-table slots 24
and 32 at `0x86856–0x86866` and `0x8683c–0x86850`. These are the
`RenderEffect` overloads with a float strength and a three-float
`Vector` center. It loads those arguments from `Engine+0x3c8` and
`Engine+0x3cc`; the old source called the overloads without them.
The recovered calls now pass copies of those fields. Rebuilt ARM
instructions contain the same virtual-table slots and GOT references.

The original initializes the source for a skipped first effect from
`postEffectFBO` (`r9 = r5` at `0x867cc`). The reconstruction used
`refractFBO` there. The original also clears each completed bit from
the local remaining-flags value before deciding whether the next
effect is terminal, and it swaps source/target roles after a composite
pass. The recovered flow now does all three; this matters when only
the second or third effect is active and when effects are combined.

The original `SetPostEffect` compares the effect code with immediate
values `0x01400002`, `0x01400001` and `0x01400000`. The first two
were represented by zero-initialized private globals with no writers,
misclassifying effect requests. The code now uses the confirmed
numeric values. `DoPostEffect` also passes `0x01400002` when it
clears the one-shot flag. The original flag loads and stores are
byte-wide; the reconstruction now uses a byte-wide field.

Focused ARM verification compiled **209/209** translation units.
`DoPostEffect` improved **83.9% → 97.8% source-shape** and
**45.3% → 87.9% strict fuzzy**. `SetPostEffect` improved
**80.1% → 87.5% source-shape**, while strict fuzzy changed
**55.2% → 48.5%**. Neither function is exact. Native Windows and
ARM64 static builds pass. A source-extracted host smoke runs the actual
`DoPostEffect` body at `-O0` and `-O2`, covering each individual bit,
all combinations, target replacement through a reference, strength
and center arguments, the one-shot reset and the shaders-disabled
path. Both configurations pass. The full ARM corpus compares
**4,323/4,524** originals in **4,762** pairs at **96.24%** mean
source-shape and **86.00%** strict fuzzy, with **2,742 linked-exact**,
**1,096 byte-exact**, 201 missing and zero skipped. Relative to the
prior post-effect FBO report, the comparison-pair set and both exact
sets have zero gains and zero losses.

The effect rendering and shader registration order have not been
tested live. Redmi testing remains stopped by user request. Next,
audit registration/initialization of the three effect shaders and
inspect the remaining nonmatching control flow before claiming full
post-processing equivalence.
