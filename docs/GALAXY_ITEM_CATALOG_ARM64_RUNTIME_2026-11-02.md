# Galaxy item catalog and Redmi startup (2026-11-02)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
reads the item catalog through the `Globals::items` slot in
`Galaxy::getAsteroidProbabilities` at Thumb `0x1760ca–0x1760d2`; the
catalog dereference is at `0x176124–0x17612a`. The recovered code had
instead used `Item::g_items`, a separate pointer that is initialized to
null and never assigned by `Globals::init`. The same unsupported pointer
was present in `getPlasmaProbabilities` and five inventory/menu reads.
Those seven reads now use the catalog loaded by `Globals::init`.

The 2026-11-02 Redmi `25053RT47C` test first established that the newer
clone package was absent, so prior visual checks had been showing the
older original-package test APK. The clone APK and its OBB are separate
from that package. The pre-fix clone APK SHA-256 was
`0c963c4dd683969193d3993b59118744b74ce9e73236545162243f6a987291db`.
It launched the native library, displayed a black frame, and then
crashed on GLThread at 15:34:21 with `SIGSEGV`, fault address `0x8`.
The backtrace placed the crash in `Galaxy::getAsteroidProbabilities+208`,
called from `Level::createAsteroids`, `Level::init`,
`CutScene::initialize` and `ModMainMenu::OnInitialize`. ARM64
instruction `0x1751fc` reads offset 8 from the null `Item::g_items`
pointer. The log is outside Git at
`D:/gof2-arm64-crypto/redmi-current-clone-full-20261102.log`.

The replacement clone APK SHA-256 is
`8974748031d8d2a352b85acf842049e69eeb273aaa7ee75d6d42696a9e301362`.
It uses a newly linked ARM64 game library with the same 53 JNI exports
and no unresolved symbols. The signed APK installed successfully on the
same Redmi. After dismissing Google Play sign-in, the game remained in
the foreground and its process stayed alive beyond the prior crash
window; no new fatal signal was reported. Its captured image remains
black (`D:/gof2-arm64-crypto/redmi-catalog-screen3-20261102.png`).
Thus this fixes a confirmed startup crash but does not establish a
playable frame or correct rendering. The original package was not
changed, and no original game assets or APKs were added to Git.

Windows native and ARM64 static builds pass. Focused ARMv7 comparison
of the two Galaxy probability functions is unchanged from the prior
snapshot: **98.2%/98.8%** source-shape and **63.7%/74.7%** strict fuzzy;
both remain nonexact. The full ARMv7 corpus compiles **209/209** units
and compares **4,323/4,524** originals in **4,762** pairs at **96.20%**
mean source-shape and **85.96%** strict fuzzy, with **2,741 linked-exact**
and **1,096 byte-exact** matches, 201 missing and zero skipped. Relative
to the immediately preceding full snapshot, the pair set is identical
and both complete exact sets have zero gains and zero losses. The
focused and full reports are in ignored `_work/galaxy_catalog_*_20261102.json`.

Next: trace whether the opening cutscene completes `Level::init` and
submits a nonblack GL frame on Redmi. A live frame check is needed
before treating asset-table or source-shape gains as visible progress.
