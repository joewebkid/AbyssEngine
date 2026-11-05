# Main-menu initialization globals (2026-11-04)

The original Android ARMv7 `ModMainMenu::OnInitialize` begins at
`0x175844` in `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
Its Thumb literal/GOT loads identify the global state used by the
final initialization stages:

| Instructions | GOT address | Relocation | Source use |
| --- | ---: | --- | --- |
| `0x175880–0x175886` | `0x21016c` | `Globals::options` | first-run preview flag at offset `0x48` |
| `0x17588c–0x175894` | `0x210420` | `Globals::recordHandler` | read preview and save options |
| `0x1759c6–0x1759ce` | `0x210378` | `Globals::switch_to_target_setting` | conditional music change, then reset to `-1` |
| `0x1759f4–0x175a04` | `0x2101f4` | `Globals::Canvas` | create logo image |
| `0x175a08–0x175a1a` | `0x210170` | `Globals::logoIsShown` | set to `1` |

The source previously used unwritten ARM32 placeholders for the options
and record-handler roots and unwritten placeholders on both architectures
for the music value and logo flag. It now uses the relocated `Globals`
objects directly. The existing `Globals::globals` music call is preserved.
The reconstructed `OptionsRecord::firstRunPreviewChecked` member is at
offset `0x4e` after C++ alignment, while the original uses `0x48`.
This function therefore addresses `Globals::options[0x48]` explicitly;
the wider `OptionsRecord` layout needs separate review.

Focused ARMv7 verification compiled 209/209 translation units and
raised `OnInitialize` source-shape **96.0% → 98.2%** and strict fuzzy
**71.9% → 74.9%**. The generated ARM32 instructions now load and store
at offset `0x48`, matching the original; the intermediate build had
used `0x4e`. The function remains non-exact. Windows native and ARM64
static libraries build, and strict ARM64 shared linking still has zero
unresolved symbols and 53 JNI exports. The current source has not run
on a device; the previously signed APK predates this change.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,770** pairs: mean source-shape **96.78%**, strict fuzzy **86.77%**,
**2,782 linked-exact**, **1,108 byte-exact**, 201 missing, zero skipped,
and zero wrong-type matches. The compared-pair set and both exact sets
have no losses against the preceding `MTitle` report. The aggregate
percentages do not change at two-decimal precision.
