# Hud image metrics and mission overlay ARM pass (2026-08-22)

## Scope and evidence

This package audits the kill-counter, passenger, production, ordinary cargo and
mission-timer part of `Hud::draw(long long, long long, PlayerEgo *, bool,
unsigned int, unsigned int)`. The main evidence is the Android 2.0.16 body in
`gof2_recovered_sdk_first_pass/extracted/ui_input_hud/Hud.c`, including its
eight 12-byte String slots at stack offsets `0x7c..0xd0`, plus the target ARM
object used by `tools/verify/verify.py`.

The existing C++ String expressions already produce the native constructor,
operator-plus and destructor order:

- kill counter: integer, `" : "`, intermediate, integer, final;
- passenger count: integer, `" / "`, intermediate, integer, final;
- ordinary cargo: integer, `" / "`, intermediate, integer, intermediate,
  `"t"`, final;
- production: the native `operator+(const int &, const String &)` first step,
  followed by the amount-plus-free-space String;
- the passenger final String remains alive through the status panel, while the
  branch-local cargo String is destroyed before the status String is built.

No artificial named temporaries were added: they did not represent a missing
source behavior and would disturb the shared exception cleanup graph.

## Recovered contract

`PaintCanvas::GetImage2DWidth` and `GetImage2DHeight` returned `unsigned short`
in the reconstructed header. Android callers, local variable recovery and the
target `Hud::draw` instructions instead use a signed `int` result. The clearest
runtime evidence is the volatile-cargo width conversion:

- target: `vcvt.f32.s32`;
- old build: `vcvt.f32.u32`.

Both declarations and definitions now return `int`. This does not change the
ARM calling convention or either mangled symbol. Their bodies still read the
16-bit Image2D fields and both functions remain linked- and byte-exact.

The volatile-force branch now uses the target ordered-greater shape. The target
branches over the second `getVolatileForce()` call only when the first result is
strictly greater than `1.0f`; an unordered value follows the assignment path.
The C++ condition therefore uses `!(value > 1.0f)` rather than `value <= 1.0f`.
This restores the target `bgt` instruction instead of `bhi`.

## Rejected source-shape experiment

Moving the image ID and Canvas acquisition to mimic the decompiler statement
order reduced `Hud::draw` from `50.0%` to `41.3%`. The change was removed. It
altered whole-function register allocation even though its runtime behavior was
equivalent; the decompiler's statement order is not sufficient evidence for
that lifetime.

## Verification

- UCRT64: `libgof2.a` links successfully.
- ARM object build: `201/204`; the same three unrelated `SolarSystem *` versus
  integer translation units remain.
- `Hud::draw`: `49.5% -> 50.0%`, `3223/3047` target/base instructions.
- all 48 `Hud` functions: `88.6% -> 89.0%` average, 26 linked-exact and 19
  byte-exact.
- `PaintCanvas::GetImage2DWidth`: `100%`, linked- and byte-exact, `9/9`.
- `PaintCanvas::GetImage2DHeight`: `100%`, linked- and byte-exact, `9/9`.

The 224-byte `Hud::draw` stack reservation and `d8-d11` VFP save set remain
unchanged. This is an ARM source-shape improvement, not a whole-function
byte-match claim.
