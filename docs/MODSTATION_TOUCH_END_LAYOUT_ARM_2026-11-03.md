# ModStation touch-end layout branch

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ModStation::OnTouchEnd(int,int,void*)` starts at `0xd7600`.
The early ARM path compares the incoming touch with the active touch,
clears the drag flag and active-touch slot on a match, and returns
when the radio window is open. Otherwise, at `0xd7650` it loads
`Globals::layout`, checks its choice-window flag, calls
`Layout::OnTouchEnd`, and clears that flag if the call returns
nonzero. The original does not read `Globals::status` before these
early exits.

The reconstruction read an unwritten private layout pointer and
called a surrogate declaration in this branch. It also placed an
unwritten status-pointer read before the touch guard. This branch
now uses `Globals::layout` and its actual `OnTouchEnd` method; the
status read is deferred until after these early returns. The change
is limited to this early dispatch path. The later mission, purchase
and main-window paths still contain provisional pointer aliases and
need separate recovery.

Focused ARM verification compiled **209/209** translation units.
For the whole `OnTouchEnd` symbol, source-shape rises from **65.7%
to 66.2%**, strict fuzzy from **12.4% to 13.2%**, and the generated
instruction count from 928 to 944 against 1,925 original. The
`ModStation` unit retains **9 linked-exact** and **3 byte-exact**
matches without losses. This is a local improvement, not a recovery
of the whole large function.

A host smoke compiles the unchanged production prefix at `-O0` and
`-O2`. It checks mismatched touches, release of the active touch,
the radio early return, and both layout callback outcomes. Windows
native and ARM64 static builds pass. The smoke deliberately ends
before the later mission dispatch, so it does not validate those
branches. No device run was made; Redmi testing remains stopped by
user request.

The full ARM corpus compares **4,323/4,524** original functions in
**4,764** pairs at **96.44% mean source-shape / 86.31% strict fuzzy**,
with **2,757 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. The global averages round to the same hundredth as before;
the pair and both exact-match sets are unchanged, with no losses.

Next: map the later `OnTouchEnd` global aliases against their ARM
references and recover one independently testable dispatch branch.
