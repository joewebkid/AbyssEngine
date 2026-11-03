# Station module fields and box coordinates (Android ARMv7)

Source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The evidence is the original ARM code, with the IDA export used only
to locate the relevant functions.

`ApplicationManager::RegisterApplicationModule` at `0x822d4` calls
`IApplicationModule::SetApplicationManager` before adding the module.
At `0x74154`, the latter stores the manager at module `+0x08`, then
loads the manager's Canvas pointer at `+0x00` and stores it at module
`+0x04`. The Android JNI registration path registers `ModStation` as
module `5`. `ModStation::OnUpdate` reads `+0x08` for elapsed time and
module switching; `OnRender2D` reads `+0x04` for its Canvas calls.
The reconstructed `ModStation` now uses those member pointers, with
compile-time offset checks against `IApplicationModule` on both host
pointer widths and original offsets on ARM32. Its constructor does
not initialize the two fields; registration is their lifecycle owner.

The original `OnRender2D` calls style-2 `Layout::drawBox` with the
four station words at `+0xa0`, `+0xa4`, `+0xa8`, and `+0xac` as X, Y,
width, and height. The reconstructed source previously mislabeled
`+0xa0` as a geometry pointer and converted it back to an integer
when drawing. It is now an integer-sized coordinate field, preserving
the ARM32 offset and the host field width; the other box fields are
named for their observed use.

Windows native and ARM64 static `gof2` builds pass. Focused ARMv7
verification compiles 209/209 translation units. `OnUpdate`
source-shape remains 76.4% and strict fuzzy rises 26.7% to 27.1%;
`OnRender2D` source-shape changes 89.4% to 89.1% and strict fuzzy
rises 10.8% to 20.6%. Focused report:
`_work/modstation_module_abi_focused_20261103.json`.

The full ARM corpus compares 4,323/4,524 original functions in
4,769 pairs, with mean source-shape 96.53% and strict fuzzy 86.41%.
There are 201 missing and zero skipped. Pair-by-pair comparison with
`_work/modstation_main_full_20261103.json` finds no lost pairs,
no lost linked-exact matches (2,763 retained), and no lost byte-exact
matches (1,099 retained). Full report:
`_work/modstation_module_abi_full_20261103.json`.

This establishes the two field injections and the station box
argument types. It does not establish the rest of the module vtable
ABI or live station rendering. The next priority is the remaining
`OnRender2D` String lifetime/field shape and unverified host behavior.
Redmi testing remains stopped.
