# Agent mod-price table (2026-10-11)

Android 2.0.16 ARM `Agent::getModPricePercentage` at `0x177738`
indexes the four-word table at ELF virtual address `0x1fb8e8` when
`sellModIndex` is 0..3, otherwise returning 40. The table's raw
little-endian words are **20, 30, 40, 20**. The recovered source had
four zeros, causing zero-price mod offers for those indices in the
`Globals::getAgentMissionText` offer-8 path. The four values are now
restored; the index check and fallback 40 are unchanged.

An assert-based read of the original ELF confirmed all four words and
the fallback in source. The generated ARM `Agent.o` contains the same
16-byte table in `.rodata.cst16`. Native `gof2`, 206/206 ARM translation
units, and 36 verifier tests pass (one skipped). The focused function
remains 100% source-shape, 100% strict fuzzy and linked-exact, but not
byte-exact. That pre-existing linked-exact result did **not** prove the
referenced data table: the verifier compares instructions, not rodata.

The full corpus compares 4,323/4,524 original functions at 92.05%
source-shape and 80.12% strict fuzzy, with 2,325 linked-exact and 997
byte-exact. Against `integrated_collision_parts_full_20261011.json`,
no exact pair was lost; there are zero skipped rows and zero wrong-type
symbols. The report is retained at
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_agent_mod_price_full_20261011.json`.

Unverified: a running-game offer for each mod index, and whether every
agent record uses the table indices as inferred from the constructor.
Next priority: audit other economy rodata tables that instruction-only
matching cannot validate.
