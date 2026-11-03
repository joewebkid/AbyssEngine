# ModStation touch-begin dispatch

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `ModStation::OnTouchBegin(int,int,void*)` at `0xd72ac`
claims the first active touch, records its coordinates, and routes
the event to the first open station window. Its radio-completion
branch reads `Globals::status`, calls `nextCampaignMission(true)`,
sets `Globals::switch_to_target_setting` to zero, and calls
`SetCurrentApplicationModule(5)` on `Globals::appManager`. Its normal
dispatch reads `Globals::layout`, and the main-window vertical bound
uses `Globals::h`.

The reconstruction used private null-initialized pointers for the
layout, screen height, and switch setting. It also passed the first
word of `ApplicationManager` as the next module ID rather than the
original constant five. The method now uses the confirmed globals
and module ID, and reloads the layout slot after window calls as the
original ARM does. The private pointers were removed.

Focused ARM verification compiled **209/209** translation units.
This function rises from **18.0% to 83.2% source-shape** and from
**15.5% to 89.9% strict fuzzy**, with 178 generated instructions
against 169 original. The `ModStation` unit retains **9 linked-exact**
and **3 byte-exact** matches without losses. Most remaining shape
differences are early-dispatch tail calls and store ordering; this
function is not exact.

The production `OnTouchBegin` and `OnTouchMove` bodies pass a host
smoke at `-O0` and `-O2`. The begin cases cover an unfinished radio
message, completed radio transition to module five, first-touch
capture, DLC touch-pointer forwarding, and layout precedence.
Windows native and ARM64 static builds pass. The host fixture does
not exercise the main-window button-row path with 32-bit pointers.
Live touch behavior remains unverified; no device run was made.
Redmi testing remains stopped by user request.

The full ARM corpus compares **4,323/4,524** original functions in
**4,764** pairs at **96.44% mean source-shape / 86.31% strict fuzzy**,
with **2,757 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. Relative to the preceding full report, all pair,
linked-exact and byte-exact sets are unchanged; there are no losses.

The next bounded investigation is `ModStation::OnTouchEnd`, a larger
dispatch path with substantial remaining source-shape differences.
