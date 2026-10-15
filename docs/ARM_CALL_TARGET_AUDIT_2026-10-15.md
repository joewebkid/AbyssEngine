# Direct ARM call-target audit — 2026-10-15

## Evidence and scope

The original Android 2.0.16 `libgof2hdaa.so` has SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The audited local corpus is the 206 ARM objects at the String numeric-constructor
baseline (4,323/4,524 original functions compared; 93.59% average source-shape,
82.63% strict fuzzy; 2,503 linked-exact and 1,074 byte-exact function-pairs).
No C++ source was changed for this audit.

`tools/verify/call_target_audit.py` parses each local object's direct ARM CALL
relocations and the original ELF's `.text` disassembly. It checks the named
callee at the same function-relative instruction offset only when the function
is already `linked_equal`. PLT and ELF version suffixes are removed for the
comparison. The detailed local report is kept outside Git at
`A:/Galaxy on Fire new version/_work/arm_call_targets_full_20261015.json`.

| Call-target result | Sites |
| --- | ---: |
| Named callee matches | 2,345 |
| Different named callee; review candidate | 80 (59 function-pairs) |
| Unresolved by original disassembly parser | 647 |
| Total direct CALL relocations audited | 3,072 |

The 80 candidates include aliases/thunks as well as materially different
signatures. Examples: `String::operator+=(int const&)` correctly calls
`String::operator+=(long long const&)`; `ImageFactory::createChar` currently
calls `AERandom::nextInt(void*, int)` where the original calls
`AERandom::nextInt(int)`; multiple PaintCanvas methods call
`paintcanvas_ext_*` shims where original calls named engine methods.
Neither equal bytes outside relocations nor a different symbol name alone
establishes semantic equivalence or a behavior bug. These cases need ABI and
implementation review before changing C++.

645 unresolved sites lack a matching original function header, often an alias;
two have a header but no call label at the expected offset. The audit does not
resolve constructor/destructor aliases, veneers, GOT/data relocations, addends,
indirect calls, or runtime behavior. It **does not** change the historical
`linked-exact`/`byte-exact` counters. Those counters are code-comparison
signals, not post-link equality or a completion percentage.

## Reproduction and checks

With existing ARM base/target objects and a lawfully obtained original ELF:

```powershell
python tools/verify/call_target_audit.py --objdump <ARM-objdump.exe> --original <original.so> --output <report-on-A-drive.json>
python -m unittest discover -s tools/verify -p 'test_*.py'
```

The full audit returned 2,503 linked-equal functions and the table above;
the verifier suite passed 40 tests with one pre-existing skip. A negative
regression test deliberately changes a callee while retaining masked-byte
equality and confirms that the new audit flags it. Native and ARM builds were
not rerun because this package changes only verification code and documentation;
the baseline C++ and its saved 206/206 ARM/native green results are unchanged.

Next bounded investigation: inspect the `ImageFactory` random call contract
and the PaintCanvas shim cluster against ARM instructions and REA/IDA, then
run focused/native/full gates for any C++ correction. Keep direct-call audit
results separate from existing exact-pair counts until alias/relocation
resolution is sound.
