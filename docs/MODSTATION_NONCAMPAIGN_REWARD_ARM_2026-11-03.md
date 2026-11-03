# ModStation non-campaign mission reward (Android ARM)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`.
`ModStation::OnTouchEnd(int,int,void*)` begins at `0xd7600`. Its
non-campaign reward continuation is `0xd7b50–0xd7c2a`.

The original increments `Status::missionCount`, calls `Mission::getReward`
and `getBonus`, then calls `Layout::showMissionRewardMessage(sum, false)`.
It calls both mission getters again for the amount credited. The ARM
comparison is unsigned against the literal `1,000,001` at `0xd7d68`.
Amounts below that threshold are credited directly. Larger amounts
use the seven 32-bit words at `.rodata+0x1fe0e0` for campaign steps
55–61: `200000, 6666, 6666, 100000, 6666, 6666, 50000`; other steps
use `6666`. The code then calls `Status::changeCredits` and
`removeMission`, clears the dialogue mission at station offset `0x9c`,
refreshes an existing `SpaceLounge`, clears the dialogue flag at
`0x69`, and checks hints. GOT relocations at `0x21015c` and `0x210224`
identify `Globals::status` and `Globals::layout`.

The recovered source had unresolved `*_ote` calls, a null-initialized
status pointer holder, incorrect reward-message arguments, and no
effective credit amount. The restored branch uses typed objects and
the ARM-backed override table. Unsigned arithmetic preserves the
observed 32-bit comparison for negative or overflowing raw sums.

Windows native and ARM64 static `gof2` builds pass. Focused ARMv7
verification compiles 209/209 units. `OnTouchEnd` source-shape rises
**74.0% → 75.7%** and strict fuzzy changes **13.9% → 13.6%**;
generated instructions **1,145 → 1,188** versus original **1,925**.
The generated ARM call sequence and threshold branch are present.

Full ARM corpus: **4,323/4,524** original functions over **4,767**
pairs, **96.47%** mean source-shape, **86.35%** strict fuzzy,
**2,761 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. No pair or exact-set losses; the corpus means are unchanged
after rounding.
Live reward presentation, credits, mission removal, and subsequent
hint handling remain unverified. Other mission types and campaign
continuations in `OnTouchEnd` still contain unresolved calls. Next
restore the type-11 passenger and cargo completion branch at
`0xd778c–0xd789c`; Redmi testing stays stopped.
