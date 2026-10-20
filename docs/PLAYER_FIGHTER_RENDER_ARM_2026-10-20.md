# PlayerFighter render recovery (Android ARM, 2026-10-20)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::render` at `0xdfa50–0xdfb82`. The separately recovered IDA
listing in `gof2_recovered_sdk_first_pass/extracted/flight_physics_camera_collision/PlayerFighter.c`
agrees with the ARM branches and offsets. The old C++ body contained null
`render()` receivers, null global-transform placeholders and an incorrect
matrix restore; it was not a usable render path.

The recovered method always renders `wreckGeometry` first when present. An
inactive player draws the body and trail only in state 5. An active player in
states 3/4 renders the explosion, re-reads state, and draws the body without
the trail; state 4 stops drawing the body once `field_0x1f8 >= 300`. Other
active states draw the body and trail. When `subGeometry` exists, the method
copies its 60-byte local transform, temporarily installs the player's matrix,
renders, and restores the saved transform. Without it, state 3 or a normal
state renders `geometry` directly. The original state-4 path dereferences
`subGeometry` without a null check; the recovery keeps that assumption.

`tools/run_player_fighter_render_smoke.py` compiles the production method body
against recording services at `-O0` and `-O2`. It checks active/inactive and
states 1/3/4/5, explosion state mutation, the 299/300 threshold, draw order,
trail suppression and all 15 saved/restored matrix elements. The UCRT64
`gof2` static library builds; 93 verifier tests pass with one existing skip.
Focused NDK r18b source-shape/strict fuzzy rises from **56.1%/33.7%** to
**92.4%/65.8%**. This function remains nonexact; the 44-function
`PlayerFighter` unit retains all its linked- and byte-exact pairs.

The full NDK r18b corpus compares **4,323/4,524** original functions across
**4,735** pairs, with **94.02%/83.36%** mean source-shape/strict fuzzy,
**2,557 linked-exact** and **1,082 byte-exact**. There are zero skipped and
wrong-type symbols. An identity-by-identity comparison with the preceding
corpus found no linked- or byte-exact losses or gains; `render` was the only
changed source-shape result. Full report SHA-256:
`2686392a90670c25fc496e15abe6515e34ffe4462e50a77b56b38ec823c214fe`.

No full game frame was executed. Other `PlayerFighter` paths, especially the
unresolved `PF_update_body` and pirate-death accounting, remain open.
