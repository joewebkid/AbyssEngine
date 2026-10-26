# ARM64 SHA-256 record dependency (2026-10-26)

`RecordHandler` calls OpenSSL 1.0.2's `SHA256_Init`, `SHA256_Update`,
`SHA256_Final`, and `CRYPTO_memcmp` for save/record integrity. The original
ARMv7 crypto archive cannot link into AArch64. The new
`tools/android/build_arm64_crypto.ps1` builds only the needed OpenSSL 1.0.2u
SHA-256 C implementation, `OPENSSL_cleanse`, and the already vendored
`CRYPTO_memcmp` source for Android API 28. No ARMv7 source or archive changes.
The source tarball is checked against SHA-256
`ecd0c6ffb493dd06707d38b14bb4d8c2288bb7033735606569d8f90f89669d16`.
Configure selects `android64-aarch64 no-asm no-shared`; the portable C
SHA-256 block transform is compiled. Generated files stay outside Git.

Reproduce on Windows with MSYS2 Perl and make:

```powershell
./tools/android/build_arm64_crypto.ps1 `
  -NdkRoot 'D:\AndroidSdk\ndk\28.2.13676358' `
  -BuildDir 'D:\gof2-arm64-crypto' `
  -Perl 'C:\msys64\usr\bin\perl.exe' `
  -Make 'C:\msys64\ucrt64\bin\mingw32-make.exe'
```

The AArch64 test executable ran on the connected Surya device. OpenSSL's
`SHA256_CTX` is **112 bytes**, exactly matching the `0x70` allocation in
`RecordHandler`. Two updates (`"a"`, `"bc"`) produced the published SHA-256
digest of `"abc"`; `CRYPTO_memcmp` accepted the expected 32-byte digest
and rejected a one-byte change.
The temporary device test binary was removed after verification.

With the ARM64 libzip archive and Android system libraries already in the
strict `--whole-archive --no-undefined` link diagnostic, this crypto archive
reduces unresolved names **526 → 522**. Exactly the four names above disappear
and there are no new names. The diagnostic still does not produce a game
shared library. Save loading and writing through the recovered `RecordHandler`
have not run on ARM64; the existing opaque SHA header passes an `int` length
where OpenSSL's C API takes `size_t`, so a live record gate remains useful.
The last ARMv7 source-shape and exact-match metrics are unchanged.
