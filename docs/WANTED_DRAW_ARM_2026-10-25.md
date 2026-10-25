# WantedWindow draw recovery (2026-10-25)

Primary reference: Android 2.0.16 ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`WantedWindow::draw` begins at Thumb address `0xe1838`. The original
function reads `Globals::layout`, `Canvas`, `font`, `gameText`,
`imageFactory`, and `status`. The recovered body instead dereferenced
four null-initialized pointer slots and used separate canvas/status
aliases. Those reads now use the ARM-observed globals.

Other recovered behavior from the same ARM function:

- The scrollbar draws only when its size or start is at least **1**
  (`0xe18f6..0xe193c`), not when start is zero.
- Inactive entries use color `0x777777ff`; the campaign marker measures
  the name plus one space (`0xe19f8..0xe1b26`).
- The header uses text ID **3219**, left title **3221**, and right title
  **3222** (`0xe1b84..0xe1d28`). The previous right title repeated
  **3221**.
- The detail line uses text ID **3226** plus one space and the
  `detailText` field at ARM offset `+96`. The reward line uses **3225**
  plus one space and `rewardText` at `+120` (`0xe1e40..0xe1ee4`).
  Previous source built English `from:`/`to:` lines from the wrong fields.
- All four box regions and the list rows call the six-argument
  `Layout::drawBox` overload (`0xe19da`, `0xe1c42`, `0xe1cc2`,
  `0xe1d28`, `0xe1da4`), not the seven-argument overload with flag zero.

The source-extracted host gate `tools/run_wanted_draw_smoke.py` passes
at `-O0` and `-O2`: map early return, text IDs and fields, active and
inactive colors, marker width, box overload, button gating, and the
scrollbar threshold. Native `gof2`, the arm64 static archive, and
**209/209** ARMv7 translation units compile. The **95** verifier
tests pass (one skip). Focused `draw` source-shape/strict fuzzy improves from
**1.3%/0.6%** to **85.0%/29.0%**. It is neither linked-exact nor
byte-exact. The full corpus compares **4,323/4,524** original functions
over **4,743** symbol/unit pairs, with **94.71%** mean source-shape and
**84.21%** mean strict fuzzy. The linked-exact and byte-exact sets
remain **2,600** and **1,082**: zero gains and zero losses against the
preceding `selectWanted` report. The full local report is
`_work/wanted_draw_full_20261025.json` (ignored by Git).

The host gate substitutes UI, localization, and world objects. It does
not prove live rendering, all layout coordinates, or the selected index
under real save data. The original ARM32 APK cannot run on the connected
arm64-only handset; a recovered Android app is not yet linkable.

Focused local reports (ignored by Git):
`_work/wanted_draw_before_final_20261025.json` and
`_work/wanted_draw_after_final_20261025.json`.
