# String::Set empty-buffer contract — 2026-10-15

## Android evidence

Reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA/IDA `String::Set(unsigned short const *)` at `0x723dc–0x72482`
deletes the previous buffer and scans a non-null input to its first
UTF-16 zero. For a non-null empty input it calls `operator new[](2)`
and stores a zero halfword while retaining `length == 0`. A null input
tail-calls `Set(char const *)` with the byte at `0x1ca472`; the original
ELF maps that address to a zero byte. `Set(char const *)` at
`0x724b4–0x7255c` likewise normalizes null to this empty string and
allocates a two-byte zero buffer for length zero.

The previous C++ returned with `data == nullptr` for both null and
non-null empty inputs. The corrected overloads retain an owned zero
terminator. The UTF-16 nonempty path now scans with a pointer and uses
`memcpy`, matching the observed source-to-buffer transfer. The
single-threaded `Set` input contract still requires readable,
zero-terminated storage; in-place aliasing is not validated here.

## Validation and limits

The production-code ABI smoke covers null and non-null empty inputs
for both overloads, nonempty strings, and the preceding-zero
`SubString` result. Native `gof2`, ABI smoke, all 206 ARM translation
units and 37 verifier tests (one skip) pass. Focused source-shape/
strict fuzzy changes from **50.9%/32.0% to 73.0%/61.3%** for
`Set(unsigned short const *)` and from **52.0%/35.0% to
69.2%/35.4%** for `Set(char const *)`. Neither is exact. Remaining
branch-shape differences must not be removed or introduced solely to
raise a similarity score. The full corpus compares **4,323/4,524**
original functions with **201 missing**, **123 extras**, zero skipped,
and **93.56%/82.59%** average source-shape/strict fuzzy. Both exact
`(unit, symbol)` sets are unchanged versus the prior full report:
**2,499 linked-exact** and **1,074 byte-exact**, zero losses. Local
reports in the parent `_work` directory are
`string_set_empty_focused_v2_20261015.json` and
`string_set_empty_full_20261015.json`.

The original and reconstructed functions have not been executed on
identical ARM32 inputs. After this, the next String priority is an
original-vs-recovery test of ownership and contents across the
constructors and copy/assignment paths.
