# Station constructor string and field initialization (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The original default `Station` constructor at `0xa6978` default
constructs its `String` field, builds a temporary string from the
literal at `0x1ca472`, assigns it to the field and destroys the
temporary. The literal begins with byte `00`, so the name is empty.
The recovered constructor instead directly constructed the name as
`"Station"`. It now assigns an empty temporary string after the
field's default construction.

The original parameterized constructor at `0xa692c` likewise default
constructs the string field and assigns the by-value name parameter.
The recovered constructor used the copy constructor in its initializer
list. It now follows the observed assignment sequence. Both
constructors write index, system index, technology level, texture
index, visit flag, attacked-friends flag and the three collection
pointers in the observed order.

The original constructor disassembly does not write the `planet` byte
at object offset `+0x14`. The reconstruction continues to initialize
it to zero until its later producer and use are established; this is
an explicit remaining source difference and avoids an uninitialized
read in the native build.

Focused ARMv7 verification builds 209/209 translation units and
compares 48 `Station` symbols. Default constructor variants improve
from 33.4% to 98.4% source-shape and 6.1% to 98.0% strict fuzzy;
parameterized variants improve from 52.8% to 95.6% source-shape and
4.7% to 65.5% strict fuzzy. The unit retains 26 linked-exact and 10
byte-exact comparisons. Focused reports:
`_work/station_ctor_before_focused_20261103.json` and
`_work/station_ctor_assign_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. The Station
loading and display paths have not been exercised on a permitted
device. Next trace the `planet` byte's producer and the remaining
constructor call-shape differences before removing the safe zero.

The full ARM corpus compares 4,323/4,524 original functions in 4,769
pairs, with 201 missing and zero skipped. Mean source-shape improves
from 96.65% to 96.69%, and mean strict fuzzy from 86.60% to 86.67%.
Linked-exact remains 2,778 and byte-exact remains 1,107. No
comparison pair or exact match was lost. Full report:
`_work/station_ctor_assign_full_20261103.json`.
