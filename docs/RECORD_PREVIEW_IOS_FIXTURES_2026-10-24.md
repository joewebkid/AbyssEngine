# iOS preview fixtures through the recovered reader (2026-10-24)

Two local touchHLE sandboxes contain `gofw_save_game_preview_0` files.
Their logs identify the loaded GOF2 HD iOS **1.1.4** IPA, bundle ID
`www.fishlabs.net.gof2hd`, and an ARMv7 slice. Neither save nor IPA is
copied into Git. These are cross-version behavior evidence, not proof that
the Android **2.0.16** save format is identical.

An independent little-endian/UTF-16 byte parse consumed each file exactly:

| Preview SHA-256 | Bytes | Time | Credits | Station | System | Campaign | Level | Rank | Ship |
| --- | ---: | ---: | ---: | --- | --- | ---: | ---: | ---: | ---: |
| `eb2fe9011302cbe78a0a278cfb4141b6792b314c47dbfc3afb155727d772c415` | 68 | 80,559 | 0 | Eden Prime | Astere | 1 | 0 | 0 | 5 |
| `141c8001d1af25128a31ab75fcbde1500dc391e51ab12f9ee0e7267f361a1c93` | 60 | 850,954 | 0 | Heinsten | Mido | 1 | 0 | 0 | 0 |

The optional `--ios-previews` input to
`tools/run_record_preview_roundtrip_smoke.py` then loads the two files in
that order. Its host `AEFile` boundary feeds the unchanged production
`RecordHandler::recordStoreReadPreview` body and real `GameRecord`
constructor. Windows `-O0/-O2` both check all eight recovered fields,
UTF-16 code units and complete file consumption; the synthetic round trip
also still passes without the optional files. No production C++ changed.

Native `gof2` builds (no-op); ARM compiles **208/208** units; **93**
verifier tests pass with one existing skip. Focused preview-reader ARM is
**84.9% source-shape / 61.0% strict fuzzy**, nonexact. Because the
production source is unchanged, the last full ARM corpus stays at
**4,323/4,524** originals over **4,737** pairs, mean **94.30% / 83.70%**
source-shape/strict fuzzy, **2,578 linked-exact** and **1,082 byte-exact**.
No new exact-match claim is made from a host fixture test.

The file boundary is synthetic, not Android `OpenRead`; the full
`gofw_save_game_0` files (**13,054** and **15,757** bytes) remain unparsed
by this gate. Their appended hash envelopes were subsequently checked in
[the full-save gate](RECORD_IOS_HASH_ENVELOPE_2026-10-24.md), along with
their count/visited/credits/rating/time prefixes; the rest of their
payloads remains unparsed. Live Android, failure handling, all save
versions and the complete load lifecycle remain unverified. Next priority: restore
ARM-backed Android `OpenRead` / `FileExist` state and literals before
end-to-end claims.
