# ModStation CBS unlock message (2026-11-04)

Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
contains `ModStation::showCBSMessage` at `0xdc0fe`–`0xdc17a`.
REA evidence `ev_bf569faff207e03b527e572028135e15b4b89152058db450e1df3c18f7f40d1b`
shows a German COMPUTERBILD SPIELE unlock prompt, title text 390,
buttons `Code` and `In-App Purchase`, and a modal choice window. It
then sets ModStation bytes `0xdd` and `0x63`.

The previous implementation dereferenced a null text root, passed
uninitialized strings to unresolved helper declarations, and wrote the
choice flag at `0x6b` instead of `0x63`. The source now constructs the
observed strings and calls `ChoiceWindow::set` directly. It sets
`hintFlags.bytes[1]` and `m_nStarMapWindowOpen.bytes[3]` at the original
offsets. The unused helper declarations were removed.

Focused ARMv7 verification compiles **209/209** units. `showCBSMessage`
source-shape rises **88.6% → 98.2%**, strict fuzzy **40.5% → 53.0%**;
it is not exact. Windows native and ARM64 static builds pass. Strict
ARM64 shared linking still passes with zero undefined symbols and
53 JNI exports. The full ARM report
`_work/modstation_cbs_message_full_20261104.json` compares
**4,323/4,524** original functions in **4,770** pairs: mean
source-shape **96.77%**, strict fuzzy **86.76%**, **2,780 linked-exact**
and **1,108 byte-exact**, 201 missing and zero skipped. No pair or
exact-match member was lost.

The signed clone APK is
`D:/gof2-arm64-crypto/gof2-arm64-main-cbs-debug-20261104.apk`
(SHA-256 `af237730114443d560a97e0edc30e2d17d6040d9875d22539ac62e70fc6eb02b`).
Its v1/v2/v3 signatures and embedded ARM64 library hash were checked;
it has not been installed or run. No source caller currently invokes
this function, so the live CBS flow remains unverified. The strict
link uses section garbage collection and can omit unreferenced
functions; zero linker gaps alone did not reveal the old null-root
implementation.
