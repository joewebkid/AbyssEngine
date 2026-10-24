# Android JNI frame and startup routing (2026-10-25)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The installed original APK (SHA-256
`515fc9a651057130c7b2b1c47bb497c61ebaebb0c567d413b5bd5453c13b221c`)
DEX declares `ToJNI.initialize(II)V`, `renderstep(J)V`, and
`resize(II)V`. `GLViewRenderer` passes `System.currentTimeMillis()`
to `renderstep`.

Original ARM JNI exports:

| Export | Address | Native destination |
| --- | --- | --- |
| `initialize` | `0x6b810` | `ndk23_InitWithZip(apk, obb, width, height, 1.0f)`, then `ndk23_setDisplayHeightAndWidth(width, height)` |
| `renderstep` | `0x6b848` | `ndk23_newrender` with the two halves of the Java `long` |
| `resize` | `0x6b874` | `ndk23_resize(width, height)` |

The branch veneers at `0x19aeb8`, `0x19aec8`, and `0x19af38`
resolve respectively to PLT `0x5e9f8`, `0x5ea04`, and `0x5ea58`.
Both original `ndk23_InitWithZip` (`0x93f14..0x93f1c`) and
`ndk23_Init` (`0x93dea..0x93df2`) call
`Engine::InitGL(true, width, height)` after constructing the engine
and setting its version string, before `AEFile::SetAppRootDir` and
`Engine::Initialize`.

The recovered JNI wrappers instead called an undefined
`ndk23_renderstep(int,int)` from all three entry points. The
`renderstep` signature split the Java timestamp into two unrelated
integers, and both initialization helpers omitted `InitGL`. The
wrappers now route to their ARM-backed destinations, `renderstep`
accepts `jlong`, and both initialization helpers call `InitGL` at
the original point. `ndk23_InitWithZip` also accepts the fifth
constant argument; the original callee does not visibly use it.
The two simple frame callbacks and no-op display-size function live
in a separate translation unit so the external call boundaries are
preserved without changing their behavior.

Focused ARM source-shape/strict fuzzy changed from **79.2%/45.7%**
to **79.2%/62.9%** for JNI `initialize`, **90.5%/55.1%** to
**93.3%/63.0%** for `ndk23_InitWithZip`, and **90.8%/55.4%** to
**93.3%/61.5%** for `ndk23_Init`. JNI `renderstep` and `resize`
remain **100%/100% linked-exact**; the display-size no-op remains
**byte-exact**. Their previous linked-exact result did *not* prove
the correct call destination: the linked-exact metric masks branch
relocation targets. This package demonstrates why source and ABI
evidence must accompany exact counts.

Native `gof2`, **209/209** ARM translation units and **93** verifier
tests (one skip) pass. The full corpus compares **4,323/4,524**
originals over **4,737** pairs at **94.56%** mean source-shape and
**84.06%** strict fuzzy, with **2,592 linked-exact** and **1,082
byte-exact**. Comparison by symbol found zero losses and zero gains
in either exact set; the three moved symbols retain their matches.
A recovered Android APK executing this JNI path and its OpenGL
context remains unverified; no full-game runtime claim is made.
