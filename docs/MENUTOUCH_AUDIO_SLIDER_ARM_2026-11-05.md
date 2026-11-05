# Menu touch audio and slider calls (2026-11-05)

The original Android ARMv7 `MenuTouchWindow::OnTouchMove(int, int,
void*)` at `0x12a33c` calls `FModSound::tryToStopMusicForBGMusic`
(PLT `0x656c4`) in its options branches. If that call returns zero,
it obtains slider 1's value through `TouchSlider::getValue`
(PLT `0x62dd8`) and calls `FModSound::setVolume(1, value)`
(PLT `0x64920`). It then calls the same getter and setter for
categories 2 and 3 regardless of the stop result. The call sequence
is visible at `0x12a4b2..0x12a504` and again at
`0x12a950..0x12a994`. The original `OnTouchBegin` also calls
`FModSound::play(int, Vector*, Vector*, float)` directly (PLT
`0x61548`), with null position and velocity in the button-sound path.

The reconstructed ARM32 code had undefined wrappers for sound play,
stop check, slider value and volume setting. Its volume wrapper lacked
the category argument, so the three original calls could not be
reproduced through that signature. The recovered ARM32 path calls the
typed methods with categories 1, 2 and 3, while the existing ARM64
null guards remain for the soundless build. Device behavior remains
unverified.

Focused ARMv7 compilation passes **209/209** translation units.
Focused per-symbol comparison changes `OnTouchMove` source-shape
**91.6% -> 89.2%** and strict fuzzy **32.8% -> 25.4%**;
`OnTouchBegin` changes **94.7% -> 93.8%** and **28.9% -> 15.6%**.
Both remain non-exact. Typed direct calls change instruction scheduling
and register lifetimes in these large methods; the original call
targets and volume-category arguments justify retaining the behavior
change. Windows native and ARM64 static builds pass, as does strict
ARM64 shared linking. The four `_mtw_` references addressed here are
absent from the rebuilt ARMv7 `MenuTouchWindow.o`; three others remain.

Full ARMv7 corpus: **4,323/4,524** original functions compared in
**4,770** pairs; mean source-shape **96.79%**, strict fuzzy **86.79%**,
**2,785 linked-exact**, **1,108 byte-exact**, **201** missing, and zero
skipped or wrong-type functions. Comparison against the preceding full
report found no lost linked-exact or byte-exact members. The two new
internal helper symbols raise the extra-symbol count from 146 to 148;
they are defined in this translation unit and do not create ARM32
undefined references. ARM32 link completeness is still unverified.
