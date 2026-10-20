# Pan Level::createSpace return — 2026-10-20

The continuous original-asset Level/GLES gate now runs the production
`Level::createSpace` through its return into `Level::init` for Pan. After
the real `StarSystem` constructor returns, production code copies Pan's
star color from `SolarSystem` into `Level::r/g/b`, `i_r/i_g/i_b`, and the
one-third minimum channels. The gate compares RGB with a separate decoder
of the original `systems.bin`: **21/18/10**. It stops immediately after
`createSpace` returns, before `Level::createPlayer`.

Windows SwiftShader and Android ARMv7/Adreno pass at `-O0/-O2`. The inherited
gate still checks 18 meshes, 22 transforms, 44 collision volumes, all five
orbit planets, real fixed-function lighting and 26/26 closed original-file
reads. No game or engine production source changed in this checkpoint.
The full ARM report for this identical source revision therefore remains
**4,323/4,524** originals, **4,735 pairs**, **93.93%** mean source-shape,
**83.22%** strict fuzzy, **2,551 linked-exact**, **1,079 byte-exact**,
zero skipped units and zero wrong-type signatures. The ignored report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/light-ambient-full.json`
(SHA-256 `53beb4739cda71c4412ce6f9d411a979db413be872dd444527b8e43fea3c832f`).

`Level::init` stage 0 is not complete: the normal mode-3 path next calls
`createPlayer`, which needs a real Ship, equipment and weapon-position
data. Those inputs and the later player/campaign paths remain unverified.
Shader-enabled lighting, fog variants and other star systems also remain.
