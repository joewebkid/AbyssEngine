# MissionsWindow font-height call recovery (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The local IDA dump's `MissionsWindow` constructor at `0x14fa10`
calls `PaintCanvas::GetTextHeight(Globals::Canvas, Globals::font)`
and stores half that height minus one. The recovered constructor
previously called an undefined `_mw_GetTextHeight(void*)` wrapper
without the font argument. It now calls the existing typed method
with `Globals::font`, restoring the observed receiver and argument.

Focused ARMv7 verification compares both constructor entry symbols:
source-shape improves from **90.4% to 97.4%**, and strict fuzzy from
**54.5% to 96.7%**. The constructor remains nonexact.
The full ARM corpus compares **4,323/4,524** original functions over
**4,745** pairs: mean source-shape rises **94.78% to 94.79%** and
strict fuzzy **84.28% to 84.30%**. It retains **2,607 linked-exact /
1,083 byte-exact**, zero skips, and no exact-set losses or gains.

Native and AArch64 `gof2` static-library builds pass. The strict
JNI-initialize AArch64 link diagnostic falls from **89 to 88**
unresolved names, and the broader JNI-onload diagnostic from
**305 to 304**, with no new names. Both remove
`_mw_GetTextHeight(void*)`.

The game shared library and APK remain unbuilt. Live font selection
and MissionsWindow layout on Android are unverified.
