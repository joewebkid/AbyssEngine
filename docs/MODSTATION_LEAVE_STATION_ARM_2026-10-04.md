# ModStation departure checks and campaign pointer ABI (2026-10-04)

Scope: `ModStation::leaveStation()`, `Status::getCampaignMission()`, and the
pointer-sized local in `Player::damage(int, bool, int)`. This is a source-backed
recovery from Android 2.0.16 `libgof2hdaa.so`, not an in-game runtime test.

## Android evidence and recovered behavior

- `_ZN10ModStation12leaveStationEv` at `0x0d8e58..0x0d9050` was a recursive
  source stub. The original first blocks departure when current cargo load
  exceeds capacity, showing text `0xcc` through the ChoiceWindow at
  `ModStation+0x70` and setting the notice flag at `+0x63`.
- Campaign step 6, or step 7 with zero firepower or unchanged combined/base
  hull points, checks cargo types 0 and 3. It shows text `0x212` when either
  is present, otherwise `0x211`. The original moves the raw `getFirePower()`
  result from `r0` into VFP `s0` and compares it as a float; the recovered
  condition preserves that bit interpretation.
- At step 20 on the campaign target station, departure shows text `0x213`.
  At step 21 on that station, the same notice appears only when none of
  equipment IDs 41, 42 and 43 is present. At step 77, ship index other than
  37 shows text `0x146`.
- Otherwise departure is blocked while byte flags `+0x63`, `+0x69` or
  `+0x6a` are set. The confirmation path shows text `0x18d`, calls
  `ChoiceWindow::left()`, then sets flags `+0xb2` and `+0x63`.
- `_ZN6Status18getCampaignMissionEv` at `0x0a9672..0x0a967c` returns the
  first `Mission *` from the array at `Status+0x198`. The previous C++
  `int` return truncated that address on 64-bit Windows. Its return type is
  now `intptr_t`, preserving the existing ARM32 register ABI and pointer
  width on the native host. `Player::damage` no longer stores the result in
  a 32-bit local. A future typed `Mission *` API would require a broader
  caller-by-caller cleanup and is not claimed here.

## Verification

| Check | Before | After |
| --- | ---: | ---: |
| `leaveStation` source-shape / strict | 0.1% / 0.0% | 91.8% / 71.5% |
| `getCampaignMission` source-shape / strict | 100% / 100% | 100% / 100% |
| Full compared originals | 4,322/4,524 | 4,322/4,524 |
| Full comparison rows | 4,675 | 4,675 |
| Full mean source-shape / strict | 90.09% / 77.24% | 90.11% / 77.25% |
| Linked-exact / byte-exact rows | 2,165 / 977 | 2,165 / 977 |

The UCRT64 `gof2` static library and all 206 ARM translation units build.
The full `--fail-on-wrong-type` report has 202 absent originals, zero
wrong-signature and zero skipped. Exact `(unit, symbol)` set comparison
against the previous full report finds zero linked- or byte-exact losses
or gains. Focused before/after reports are
`_work/blueprint-isunlocked/modstation_leavestation_{before,after}.json`
and `status_campaign_{before,after}.json`; the complete report is
`modstation_leavestation_full.json` in the same ignored directory.

`leaveStation` is not exact, and neither its messages nor the departure
transition were exercised in a running game. The meaning and ownership of
its modal flags, the firepower representation elsewhere in `Ship`, and the
remaining pointer casts around `getCampaignMission()` still need separate
runtime/ABI review. The next bounded priority is a representative native
smoke for the departure decision and campaign pointer path, without treating
instruction similarity as behavioral proof.
