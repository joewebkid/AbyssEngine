# Fighter state-4 ARM map (not implemented, 2026-10-22)

Primary evidence is Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)`. This is a read-only map for the next recovery
package, not a claim that the C++ state-4 branch has been restored. The
verified source-shape and exact metrics remain those in
`PLAYER_FIGHTER_STATE3_ARM_2026-10-22.md`.

The state table reaches `0xde4ac`. The `+0x1f8` timer first gains `dt`. If
it is at least 1, the code clears the byte at `+0x73` and joins at
`0xdeaa4`. Otherwise `0xdea08–0xdeaa2` repeats the drift rotation and
direction/speed translation seen in state 3, then joins at `0xdeaa4`.
That latter route calls `updateReferenceMatrix` both after translation
and again at the join. The join refreshes the geometry reference matrix,
gets the optional target-follow camera from `Level::getPlayer`, calls
`Explosion::update(dt, camera)`, and increments the `+0xd8` death timer.

The crate-motion path requires all of: the `+0x4c` cargo byte, an active
`Player`, `dt >= 1`, and non-null crate geometry at `+0x78`. When bomb force
is positive, `0xdeb00–0xdeb62` multiplies the Vector at Fighter `+0x17c`
by that force, stores the result at `+0x90`, translates both crate and
Fighter geometry, and writes a new force of `force * 0.98f`, floored to
zero below `0.05f`. Whether or not force was positive, it rotates the
crate geometry by the same value on three axes:
`float(int(float(dt >> 1) * 0x1p-16f * 6.2831855f))`. It then copies the
crate matrix to the associated Player transform. The float constants and
conversion are present at `0xdeb64–0xdeba4` and in the literals at
`0xdec2c–0xdec3f`; the odd integer truncation is retained as evidence,
not explained away as intended visual behavior.

If the death timer has not exceeded 60,000, that path returns. Once it
has, and the explosion is no longer playing, `0xdebb4–0xdec14` clears
the `Radar` slot at `+0x1c` if it targets this Fighter, marks an
uncaptured mission crate lost, destroys the crate geometry, zeros the
death timer and crate pointer, then falls through to deactivate the
Fighter and mark `finished`. If the crate-motion gate fails, the branch
at `0xdec40–0xdec7a` instead waits for the explosion to stop, marks
the same mission-crate loss, deactivates only after 60,000 ms, and
sets `finished`.

`PlayerFixedObject::update` contains a similar explosion/force/rotation
sequence and corroborates the `0.98f`, `0.05f`, and rotation constants;
it is not proof of Fighter-specific conditions or ownership. The
original Fighter entry check at `0xdcfd8` may return early for an
already-finished state-4 explosion; a production-method fixture must
exercise both entry and post-update threshold crossings. The next
implementation should cover camera-null and camera-present updates,
zero and positive bomb force, both crate gates, the 60,000-ms boundary,
Radar detachment, and geometry ownership before any full-corpus claim.
