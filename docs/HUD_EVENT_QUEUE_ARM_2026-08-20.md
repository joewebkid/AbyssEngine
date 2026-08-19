# Hud event queue ARM pass (2026-08-20)

## Evidence

- Android 2.0.16 `libgof2hdaa.so` ARM body and symbols.
- IDA/Ghidra-backed `Hud.c` bodies for `drawEventQueue`, `addToEventQueue`,
  `updateQueue` and `clearQueue`.
- Focused ARM object comparisons after every accepted source-shape change.

## Recovered behavior and ABI

- `addToEventQueue` now uses the native post-increment slot scan. Index zero
  remains reserved; the first empty slot from index one receives the item and
  raises `eventQueueDirty`.
- `updateQueue` has its confirmed `int` return type. It accumulates the timer,
  enables the fade after 2000 ms, removes and destroys the first queue item
  after 4000 ms, shifts the fixed queue and clears the dirty flag when the next
  slot is empty.
- `drawEventQueue` now follows the native draw order: three fresh
  `Radar::drawTarget` observations, alpha reflection after 255, slide-offset
  calculation before the banner, direct queue slot one access and priority
  colors for kinds 2, 1 and 3.
- Non-native Canvas/Layout/queue guards and the queue-size branch were removed;
  the original function assumes initialized Hud-owned state.
- `clearQueue` was retained unchanged at 100% linked-exact.

## ARM verification

| Function | Before | After | Target/base instructions |
| --- | ---: | ---: | ---: |
| `Hud::addToEventQueue` | 36.8% | 100% linked/byte-exact | 18 / 18 |
| `Hud::updateQueue` | 48.2% | 95.3% | 42 / 44 |
| `Hud::drawEventQueue` | 23.9% | 55.6% | 144 / 133 |
| `Hud::clearQueue` | 100% linked-exact | retained | 47 / 47 |

The explicit-goto color-layout experiment reduced `drawEventQueue` from 55.6%
to 52.0% and was rejected. No volatile registers, artificial scratch locals or
assembly-only padding were kept.

The focused Hud set now contains 48 comparisons at 81.4% average, with 26
linked-exact and 19 byte-exact functions.

## Build status and remaining work

- UCRT64 `libgof2.a` builds successfully.
- The ARM object pass remains 201/204 with the same three unrelated
  `SolarSystem *` versus integer failures.
- The remaining two-instruction `updateQueue` difference is an equivalent
  conditional epilogue selected by Clang; the state lifecycle and return ABI
  are restored.
- `drawEventQueue` remains non-byte-exact. Its remaining differences are VFP
  register allocation, literal-pool placement and the compiler's reordered
  priority-color comparisons.

