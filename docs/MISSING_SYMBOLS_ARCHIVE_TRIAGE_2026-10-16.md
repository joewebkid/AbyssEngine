# ARM verifier missing-symbol triage (2026-10-16)

Reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
At commit `09d2cf64`, the full per-function report
`A:/Galaxy on Fire new version/_work/objectgun_render_semantics_full_20261016.json`
compares **4,323/4,524** original functions and writes **201** names
to `cmake-build-match/verify/missing.txt`; it reports zero
wrong-signature matches and zero skipped comparisons.

The verifier compares recovered `base/**/*.o` objects. It does not
include the ARM static archives used by the separate whole-library
relink gate. A case-sensitive classification of all 201 missing
names, followed by exact-name lookup in `arm-linux-androideabi-nm
-g --defined-only -P` output, found:

| Missing category | Names | ARM archive defining every name |
| --- | ---: | --- |
| ZIP | 74 | `third_party/libzip/libzip_gof2.a` |
| zlib | 47 | `third_party/zlib/libz_gof2.a` |
| OpenSSL/crypto | 23 | `third_party/openssl/libcrypto_gof2.a` |
| C++ ABI/runtime | 57 | NDK r18b `libc++abi.a` (`armeabi-v7a`) |
| Unclassified | **0** | — |

Archive SHA-256 values for this lookup, in table order:
`528d0faaa49e5a15a02ee18537ff916157c5e245354995743aafb9dcea6e786a`,
`52a764ae7f51c48d138d8fb321ed96e4498210d0140af95cf2fe5be268f9900b`,
`c14733e4a2eaf9b82362caf22fa16ff27cc13a397ff9e9f0ff9896d94c5c6686`,
`432d9cf278d5346f93c688bf231c2a15a687c214e5e323febbf092912b198ffc`.

This is **symbol availability**, not evidence that those archives
link successfully into the current whole `.so`, match original
machine code, or behave identically. The 201 names remain missing
from the *per-function* verifier and its coverage number is not
changed. No first-party game/engine name appears in that missing
list; this does not mean all first-party bodies are semantically
correct. The archives are already wired into the separate relink
path in `tools/verify/relink.py` and `third_party/match_libs.cmake`.

Next priority: first-party behavior and ABI discrepancies and the
124 extra local symbols, not adding stubs for these 201 library
names. Whole-library relink/symbol parity and live ARM32 behavior
remain separate unverified gates.
