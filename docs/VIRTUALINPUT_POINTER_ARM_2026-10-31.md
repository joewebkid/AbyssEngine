# Virtual input pointer state (2026-10-31)

Primary evidence is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`MouseInput(int, int)` starts at `0x6a848`; `simulateTouch` starts at
`0x6aaf0`. The original exports `stackX` at `0x217980` and `stackY` at
`0x217984`. `MouseInput` checks its pointer-input enable flag and then
checks the first field of the original stick state at `0x2150a0`:
when nonzero it assigns `stackX = dx`; otherwise it adds `dx` to
`stackX` and adds `stackY` to `dy`. It then assigns `stackY = dy`.
The initialized 28-byte ARM data at `0x2150a0` is one integer `1`
followed by six float values `0.5`, confirming the recovered
`SoftwareStick` field offsets and initial values. The previous source
used private pointer fields instead of the exported stack variables
and a second, uninitialized capture flag instead of `useJoystick`.

The original `HideMouse` at `0x6a024` reads the same hidden flag that
`CaptureMouse` at `0x6a030` writes; `MouseInput` and `simulateTouch`
also read it. The previous source kept three independent flag objects,
and the compiler reduced `MouseInput` to a one-instruction no-op.
`SwitchToOtherMouseConifguration` at `0x6a03c` writes the stick's
first field, while the previous source wrote a private unused integer.
The recovered NFC and input functions now share the hidden capture
flag and stick state.

At the start of original `simulateTouch`, the one-time display-edge
setup writes `Globals::left_edge = 10`, right edge = display width
minus 10, `Globals::top_edge = 10`, and bottom edge = display height
minus 10. The previous source wrote 10 to `Globals::resetKeyboard`
instead of `top_edge`. Separately, when `resetKeyboard` is set, the
original resets both eased stick coordinates to 0.5, clears `stackX`
and `stackY`, and clears `resetKeyboard`. The previous source tied
these actions to a private capture flag and disabled pointer input.
The recovered source now follows the observed two branches and uses
`stackX`/`stackY` throughout the remaining pointer calculations and
their 0.96 decay, gated by `useJoystick` as in the ARM body.

Focused `MouseInput` improves **30.8% / 5.6% → 100.0% / 100.0%**
source-shape / strict fuzzy, recovering all 35 original ARM
instructions in the comparison. `simulateTouch` improves **74.5% /
20.1% → 83.3% / 38.2%**. `HideMouse`, `CaptureMouse`, and
`SwitchToOtherMouseConifguration` each reach **100.0% / 100.0%** and
become linked-exact. The focused 20-symbol input unit rises **81.6% /
68.5% → 85.5% / 74.1%**; the 38-symbol NFC unit reaches **40.1% /
22.4%**. Native Windows and ARM64 static builds pass; ARMv7 compiles
209/209 translation units. The recovered pointer path has not been
executed on an Android device; device testing remains deferred.

The full ARM corpus compares **4,323/4,524** originals over **4,749**
pairs. Mean source-shape rises **95.08% → 95.12%** and strict fuzzy
**84.76% → 84.82%**. There are **2,677 linked-exact**, **1,091
byte-exact**, zero skipped and 201 missing. Comparing complete
`(unit, symbol)` exact sets finds three new linked-exact matches and
no linked-exact or byte-exact losses. Reports are stored outside Git
under `D:\gof2-arm64-crypto`.
