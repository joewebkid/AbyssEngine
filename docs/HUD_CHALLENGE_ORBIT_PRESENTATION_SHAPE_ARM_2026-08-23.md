# Hud Challenge, Orbit And Event Presentation ARM Pass

Date: 2026-08-23

## Scope

This package audits three adjacent presentation bodies as one source-shape
family:

- `Hud::drawEventString(String, bool)` at Android `0x1636f4`;
- `Hud::drawChallengeModeScore(int)` at Android `0x1637e0`;
- `Hud::drawOrbitInformation()` at Android `0x166018`.

The Android IDA body, the independent Ghidra body and raw ARM disassembly were
checked before retaining source changes. The UCRT64 build is used only as a
native compile gate; percentages below come from the Android ARM verifier.

## Retained Challenge Shape

Ghidra shows that the multiplier branch reuses the original score-row Y local.
After the main seven-digit loop it performs the equivalent of:

`y += frameHeight + rowPad`

The previous source kept a separate `multiplierY`. Both forms currently
optimize to the same ARM object, but the recovered source now records the
native local recurrence and uses the updated Y for the bonus row, multiplier
icon and scaled multiplier digits.

The bonus-centering multiply is unsigned in the target. Android shifts the
`String::size() * digitStep` product with `lsr #1`; the previous explicit
signed cast generated `asr #1`. Keeping the length unsigned through the shift
restores that instruction without changing the rendered coordinate for valid
non-negative digit widths.

Focused similarity improves from `49.0%` to `49.3%`. Target and base remain
`350/340` instructions. The function is source-backed and behavior-complete,
but is not byte-exact: the compiler still assigns the layout, status, width,
Y and loop-index lifetimes to different registers and stack slots.

## Orbit Audit

The independent Ghidra result confirms the existing five-object system-line
graph and its cleanup order:

1. hidden `SolarSystem::getName()` result;
2. `String(name, false)` copy;
3. the space String;
4. the first concatenation;
5. the final concatenation with `GameText[137]`.

No behavior or missing String was found. Android reserves 84 stack bytes and
keeps the orbit text X coordinate at `sp+0x10`, allowing the security level to
stay in `r5`. The current compiler reserves 92 bytes, keeps X in `r8` and
spills the security level. This is a register-allocation/source-history
difference, not evidence for another local object. `drawOrbitInformation`
therefore remains at `56.3%`, `225/219` target/base instructions.

## Event String Boundary

`drawEventString` remains at `90.1%`, `67/64` target/base instructions. Its
only meaningful body difference is the order of the ordinary-line loads:
Android loads `eventLineMargin`, tests `rightAlign`, then loads `eventLineX`;
the current compiler loads `eventLineX` first. The wrapped and ordinary
coordinate formulas, Canvas/font routing and full-width Y argument are already
confirmed. Natural lifetime rewrites did not reproduce the three-instruction
order without damaging the rest of the body, so no change was retained.

## Rejected Experiments

- Converting all three challenge digit loops to explicit `while` bodies
  reduced `drawChallengeModeScore` from `49.0%` to `33.9%`.
- Moving `Globals::w` acquisition after construction of the score String
  reduced the accepted `49.3%` result to `47.2%`.
- Splitting the orbit X expression into width assignment plus layout addition
  was instruction-neutral and was removed.
- Delaying `eventLineX` until after the offset branch, both as a local and as a
  direct field use, reduced `drawEventString` from `90.1%` to `77.5%`.

No volatile/register forcing, fake stack buffers, inline assembly, synthetic
stack-canary scratch or percentage-only behavior changes were retained.

## Validation

- UCRT64 `libgof2.a`: green.
- ARM translation units: `201/204`; the same three unrelated `SolarSystem *`
  versus integer migration failures remain.
- `Hud::drawChallengeModeScore`: `49.0% -> 49.3%`, `350/340` target/base.
- `Hud::drawOrbitInformation`: `56.3%`, `225/219` target/base.
- `Hud::drawEventString`: `90.1%`, `67/64` target/base.
- all 48 Hud functions: `89.1%` average, 26 linked-exact, 19 byte-exact.

## Next Package

The next useful Hud target should return to a behavior-bearing body rather
than force these remaining allocator differences. `Hud::init` at `64.9%` or
`Hud::hudEvent` at `66.1%` can still expose missing resource/dispatch behavior;
the three functions in this report now need whole-frame compiler-shape work,
not isolated stack-slot edits.
