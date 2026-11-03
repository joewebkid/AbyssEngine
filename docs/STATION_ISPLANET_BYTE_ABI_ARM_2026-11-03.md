# Station::isPlanet byte return (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The original `Station::isPlanet` at `0xa6c08` loads the byte at
object offset `+0x14` into `r0` and returns it directly:
`ldrb r0, [r0, #20]; bx lr`. The reconstruction declared a `bool`
return, so Clang inserted a comparison and normalized every nonzero
byte to `1`. The return type is now `uint8_t`, preserving the byte
value and the original two-instruction ABI shape. The symbol name
does not encode the return type.

Focused ARMv7 verification builds 209/209 translation units and
compares 48 `Station` symbols. `isPlanet` improves from 70.9% to
100% source-shape, from 57.1% to 100% strict fuzzy, and becomes both
linked-exact and byte-exact. The unit now has 27 linked-exact and 11
byte-exact comparisons; every other function retains its metrics.
Focused reports: `_work/station_ctor_assign_focused_20261103.json`
and `_work/station_isplanet_byte_focused_20261103.json`.

The original constructors do not write this byte. The native source
still initializes it to zero while the producer and runtime use
remain unverified. REA's IDA xref query returned no analyzed direct
references to `isPlanet`; its relationship coverage is unknown, so
this does not prove the function is unused. The method has no source
callers in this reconstruction.

Windows native and ARM64 static `gof2` builds pass. The full ARM
corpus compares 4,323/4,524 original functions in 4,769 pairs, with
201 missing and zero skipped. Mean source-shape rises from 96.69%
to 96.70%; mean strict fuzzy remains 86.67% after rounding.
Linked-exact rises from 2,778 to 2,779 and byte-exact from 1,107 to
1,108, solely through `isPlanet`. No comparison pair or exact match
was lost. Full report: `_work/station_isplanet_byte_full_20261103.json`.

Runtime use of the byte remains unverified. Next inspect the wider
station loading and display path, then validate it on a permitted
device.
