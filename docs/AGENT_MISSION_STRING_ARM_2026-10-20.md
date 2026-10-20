# Agent mission-string assignment (Android ARM, 2026-10-20)

The original Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
contains `Agent::setMissionString(AbyssEngine::String)` at `0x177684`.
Its body constructs a temporary `String` using the `(const String&, bool)`
constructor with `false`, assigns that temporary to the member at `+0x6c`,
then destroys the temporary. The old recovered body assigned the incoming
value directly and emitted only two ARM instructions. The source now expresses
the observed intermediate copy as `missionString = String(src, false)`.

The focused NDK r18b ARM comparison changed from 1.2% source-shape / 0%
strict fuzzy, nonexact, to **100% source-shape / 100% strict fuzzy and
linked-exact** (not byte-exact). For a non-null source, both forms deep-copy
the string. There is also a conditional null-source distinction: a default
`String` has `data == nullptr`; if such a parameter reaches the setter,
direct `operator=` leaves the previous member value untouched, whereas the
observed temporary constructor calls `Set(nullptr)` and makes an empty string
before assignment. Ordinary lvalue calls may materialize a non-null copy
before entering the setter, so this is not a proven gameplay path.
`String::operator=` and the copy constructor remain separate shared code and
were not changed. This rare path and exception behavior have not been
exercised through a full game runtime.

Validation: the changed ARM translation unit compiles with NDK r18b; the
native UCRT64 `gof2` static library builds in a temporary C: build directory;
the verifier's 93 tests pass with one existing skip. A: had insufficient
space for the native archive's temporary copy; this was a build-location
constraint, not a compiler error.

The final full NDK r18b corpus report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/agent-mission-full-current.json`
(SHA-256 `d24e490b539fc5adadfbae91df3542efb4fccae9a994ba012b6639075dfec8e7`).
It compares **4,323/4,524** original functions across **4,735** pairs:
**93.99%** mean source-shape, **83.31%** mean strict fuzzy, **2,555
linked-exact** and **1,082 byte-exact**. There are zero skipped and zero
wrong-type symbols. Comparing exact-pair identities against the previous
`gas-r18b-full.json` report gives zero losses and exactly one linked-exact
gain, this setter. Source-shape changes for no other function.

An initial whole-corpus run used several stale local ARM objects, including
`PlayerGasCloud`, and temporarily reported exact losses. Recompiling the ten
out-of-date game/engine objects restored the prior pairs; the final corpus
above uses these current objects. The `missing=201` names are bundled
zlib/zip, OpenSSL and C++ runtime/ABI symbols, not game-class functions.
