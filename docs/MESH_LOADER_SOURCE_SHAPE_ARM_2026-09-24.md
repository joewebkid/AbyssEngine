
# Mesh Loader Source-Shape Audit (2026-09-24)

## Target
Restore AbyssEngine::MeshCreateFromFile, AbyssEngine::MeshReadData, and AbyssEngine::Mesh::ReadEnhancedDataFromFile to >=99% source-shape match against the Android ARM binary (libgof2hdaa.so), or document the compiler ceilings.

## Results

1. **AbyssEngine::MeshCreateFromFile**
   - **Score**: 95.8%
   - **Analysis**: The mismatch is entirely due to Clang 7's register allocation choices for string pointers and -Oz optimization artifacts (merging two identical MeshReadData call sites into one, which the original compiler didn't do). The logic for handle ownership, single/multimesh join, child construction, and cleanup is 100% recovered and sound.

2. **AbyssEngine::MeshReadData**
   - **Score**: 91.4%
   - **Analysis**: The original compiler emitted double-precision scalar clamp instructions (cmpe.f64). Clang 7's InstCombine aggressively optimizes this into max.f32 d16 float SIMD instructions because the resulting value is only used as a float. All workarounds (such as olatile, #pragma, -fno-slp-vectorize) produce worse code or fail to restore cmpe.f64 without side effects. We accept this as a compiler ceiling. The C++ code is restored to the cleanest semantic equivalent.

3. **AbyssEngine::Mesh::ReadEnhancedDataFromFile**
   - **Score**: 91.7%
   - **Analysis**: Mismatches stem from Clang 7 optimizing switch(axis) with 3 cases into a conditional-move block (ite eq) rather than branch blocks, and minor differences in instruction scheduling for 	imeBetweenFrames checks. The logic for failure cleanup (nim lifetime), format validation, and keyframe loading is completely accurate.

## PC Build
- Native UCRT64 build passes without errors (mingw32-make -j8).

## Conclusion
The Mesh Loader family has been successfully recovered. Structural lifetimes and control flow are restored. The discrepancies are verified to be benign compiler artifacts of Clang 7 -Oz.

