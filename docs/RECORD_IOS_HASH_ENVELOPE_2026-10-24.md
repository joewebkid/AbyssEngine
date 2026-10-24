# iOS full-save hash envelope (2026-10-24)

Two local touchHLE sandboxes produced GOF2 HD iOS 1.1.4
`gofw_save_game_0` files, associated with the preview fixtures in
[the reader gate](RECORD_PREVIEW_IOS_FIXTURES_2026-10-24.md). These are
cross-version evidence, not Android 2.0.16 fixtures; neither original
file nor the IPA is copied into Git.

| Preview location | Full-file SHA-256 | Bytes | Last 32 bytes |
| --- | --- | ---: | --- |
| Eden Prime | `00d1cd7bd9675cf2446697db6b1982ebd7da8095268c5fd84391a05d1eb83f3e` | 13,054 | `a126ae7f0cb3443c7de343536756e123afe49dba0d1d391da089efaed4e81b18` |
| Heinsten | `e575767b381117ab7fdd9efbea9f8359d6db6d937ae34ba1bf8d5e95e465619b` | 15,757 | `6202281630e5659292625cf22d39dadc7204d923b4f3ab8cdc12d2dac6b5d191` |

For **both** files, an independent .NET SHA-256 calculation of all bytes
except the 32-byte tail, followed by the 25-byte `RH_hash_salt` recovered
from `RecordHandler.cpp`, equals that tail exactly. Plain SHA-256 of the
payload does not. The source-extracted host gate in
`tools/run_pending_product_roundtrip_smoke.py --ios-records` repeats this
check at Windows `-O0/-O2`. It then feeds each complete file to the
recovered Android `RecordHandler::checkHash` with the existing synthetic
16-byte Android key; both are rejected and fully consumed. The synthetic
round-trip gate still passes without the optional files.

Android ARM's recovered hash path includes a further 16-byte key after the
salt. Thus these two iOS full-save tails are **not directly usable as
Android keyed-hash fixtures**. No iOS fallback was added to Android
production code. Native `gof2` builds, all **208/208** ARM units compile,
and **93** verifier tests pass (one existing skip). The fresh full ARM
corpus compares **4,323/4,524** original functions over **4,737** pairs,
**94.30% source-shape / 83.70% strict fuzzy**, **2,578 linked-exact** and
**1,082 byte-exact**. Comparison with the prior full report finds zero
lost or gained exact symbol pairs; no new exact claim is made here.

## Payload prefix: a second incompatibility

Both authenticated iOS 1.1.4 files begin with little-endian count **635**,
followed by 635 one-byte visited flags, 32-bit credits and rating (both
zero in these fixtures), and 64-bit playing time. The times are **80,559**
and **850,954**, agreeing with the independently parsed preview files.
The Heinsten file has nonzero visited flags at indices **77** and **135**;
the Eden Prime file has none. The optional host gate now checks these
offsets and values at Windows `-O0/-O2` before checking the hash tail.

The Android 2.0.16 ARM library used here has SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Its `RecordHandler::recordStoreWrite` at `0xce908..0xce91e`
writes count **135** and loops over 135 visited bytes. Android ARM
`GameRecord::GameRecord` at `0x15591e..0x15592c` allocates exactly **135**
bytes for that buffer. The reader at `0xcbda2..0xcbdc8` loops to the
count read from disk, without a visible capacity check before storing
through the buffer pointer. This matches the current recovered source.
Consequently, even bypassing the different hash would not make an iOS
file safe to pass to the Android reader: its 635-byte count exceeds the
Android buffer, and truncation would also discard the observed flag 135.
This is a cross-version boundary, not a license to add an unverified
iOS fallback or change Android's byte-matched format.

For this prefix extension, the source-extracted gate passed with both
authentic files at Windows `-O0/-O2`; native `gof2` remained green (no
rebuild needed). The verifier unit-suite rerun could not complete in this
environment: 27 of 65 discovered cases errored because the legacy ARM
GNU objdump from NDK r18b was unavailable. This does not contradict the
earlier 93-test/full-corpus result, but is not a fresh green ARM claim.
No production C++ or ARM comparison input changed in this extension.

The two files do not establish every iOS version's format. The iOS options
file's 32-byte tail does not match the same salt-only calculation, so its
envelope remains uncharacterized. Payload parsing beyond the confirmed
prefix, the genuine Android key/save, Android filesystem read/fallback,
and live-device loading remain unverified. Next priority: restore Android `OpenRead`/`FileExist`
from ARM evidence and obtain a genuine Android save for end-to-end checks;
do not feed these iOS files to `recordStoreRead`.
