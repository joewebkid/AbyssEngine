# Hangar Layout And Button Slots

Date: 2026-08-16

## Evidence

The Android HD `HangarWindow::initialize`, `render`, `OnTouchBegin`, and
`OnTouchEnd` bodies consistently treat `HangarWindow+0x20` as an array of 24
`TouchButton*` entries. The allocation order and every observed consumer agree
on the following stable slot groups:

| Slots | Role |
| --- | --- |
| `0` | inspect/detail |
| `1` | select ship |
| `2..4` | item select/install actions |
| `5` | sell-list entry |
| `6` | move-to-cargo entry |
| `7` | select blueprint |
| `8..9` | current/station amount |
| `10` | sell ship |
| `11` | credits footer |
| `12..16` | paid-credit products |
| `17` | more/free-credit route |
| `18..22` | social/free-credit actions |
| `23` | blueprint auto-complete |

The source now names these indexes without changing their numeric ABI. Range
endpoints remain exclusive where the recovered loops use `< end`; closing the
paid-credit dialog therefore hides slots `12..17` by passing endpoint `18`.

## Layout Metrics

`Layout+0x238` is a four-word block copied into `HangarWindow+0x100`; its
fourth word is the gap used by list rendering and touch row selection.
Additional copied fields are now typed by their observed consumers:

| Layout field | Hangar consumer |
| --- | --- |
| `+0x248` | fixed width of Sell / Move-to-Cargo row actions |
| `+0x24c` | selected-row action Y offset |
| `+0x250` | row item-icon Y offset |

For the ordinary Android non-iPad/non-N9 branch, `Layout::Layout` writes
`+0x248 = 340`, `+0x24c = -2`, and `+0x250 = 2` on retina displays; low
resolution uses `170`, `0`, and `1`. These are direct constructor writes, not
visual estimates.

`Layout+0x30` is also named as `touchButtonLayoutHeight` at its confirmed
`TouchButton::init` consumer while preserving the offset alias.

## Boundary

The names describe confirmed consumers and Android values. They do not make
the host 64-bit `HangarWindow` byte-identical to the ARM class, and do not yet
cover every iPad/N9 branch or the full `HangarWindow::render` body.

## Validation

The UCRT64 `gof2` target links successfully with this package. The package is
kept separate from the HUD gamma-runtime commit so its evidence boundary stays
clear.
