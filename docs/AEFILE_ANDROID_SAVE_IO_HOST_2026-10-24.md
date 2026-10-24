# AEFile Android filesystem write/read gate (2026-10-24)

Android 2.0.16 ARM `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
shows `AEFile::OpenWrite(String&)` passing mode 0 to `Open` at `0x70b3a`.
`FileInterfaceAndroid::OpenWrite` at `0x6e208` uses `"wb"` for `fopen`,
and the FILE branch of `FileInterfaceAndroid::Write` at `0x6e4c4–0x6e4ce`
returns whether `fwrite(buffer, 1, size, file)` wrote the requested size.

The existing original-archive harness now compiles production `AEFile`,
`AENormalFile` and `AEPakFile` together with the source-extracted Android
`OpenWrite`, FILE constructor and read path. Its host adapter maps only
the FILE `Read`/`Write` calls to `fread`/`fwrite`; it does not emulate JNI.
At Windows `-O0` and `-O2`, `AEFile::OpenWrite` creates a five-byte file
under a runner-owned temporary root, `AEFile::Write` writes it, and
`Close` releases the file and restores the instance count. Independent
disk reading and `AEFile::OpenRead` agree on the bytes; a missing parent
directory fails without changing the caller's handle. The original APK
and OBB are opened only for reads, and the temporary save file is removed
by the runner.

Native `gof2` and **93** verifier tests pass (one existing skip). Focused
Android file-interface ARM comparison is **89.58% source-shape / 85.32%
strict fuzzy** over **33** pairs, with **22 linked-exact** and **11
byte-exact**. `OpenWrite` remains **88.9%/74.6%** and `Write`
**33.8%/14.0%**, neither exact; the latter includes the untested JNI
branch. The last full corpus, with no production source change, stays at
**4,323/4,524** originals over **4,737** pairs, **94.31%/83.73%** mean,
**2,578 linked-exact** and **1,082 byte-exact**. This package changes no
production or ARM compilation inputs, so the preceding exact-set audit
remains applicable; a new full corpus was not run.

Next: validate `RecordHandler`'s complete save-file serialization and
hash envelope against independently verified save fixtures, then exercise
the actual Android save directory and JNI path on a device. This gate
does not prove those behaviors.
