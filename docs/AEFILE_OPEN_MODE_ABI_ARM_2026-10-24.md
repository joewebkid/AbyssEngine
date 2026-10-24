# AEFile open-mode ABI (2026-10-24)

Android 2.0.16 ARM wrappers pass numeric modes directly to
`AEFile::Open`: `OpenWrite(String&)` passes **0** at `0x70b3a`,
`OpenRead(String&)` passes **1** at `0x70532`, and
`OpenAppend(String&)` passes **2** at `0x70b8e`. The recovered C++ header
had read/write reversed. Its in-tree wrappers and branches compensated
for one another, but the public enum's numeric ABI contradicted the
original. No other direct `AEFile::Open` caller is present in `src/`.

`FileOpenType` now uses `OPEN_WRITE=0`, `OPEN_READ=1`, `OPEN_APPEND=2`.
The three String overloads each reach **100% source-shape and 100%
strict fuzzy** in focused ARM verification; read and write were previously
**93.3%/66.7%**, while append was already **100%/100%**. The larger
`AEFile::Open` body remains nonexact at **82.3% source-shape / 27.4%
strict fuzzy**. This is an ABI correction, not a claim that all of
`AEFile`'s fallback and handle-lifetime behavior is recovered.

Native `gof2` builds, all **208/208** ARM translation units compile,
**93** verifier tests pass (one existing skip), and the existing host
file-stack test using original assets passes **179/179** file lifetimes.
The full ARM corpus compares **4,323/4,524** original functions over
**4,737** pairs, with mean **94.31% source-shape / 83.73% strict fuzzy**,
**2,578 linked-exact** and **1,082 byte-exact**. The linked- and byte-exact
sets are unchanged from the previous full report, with zero skipped functions.
The next priority is source-backed restoration and end-to-end testing of
`AEFile::Open` / `findPakFile` with the identified original APK/OBB and
save-file paths. Live Android remains unverified.
