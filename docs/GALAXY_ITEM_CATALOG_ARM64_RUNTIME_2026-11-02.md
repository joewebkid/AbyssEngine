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
same Redmi. After dismissing Google Play sign-in, it passed the former
Galaxy crash site, but its image remained black
(`D:/gof2-arm64-crypto/redmi-catalog-screen3-20261102.png`). At
15:48:04 it then crashed at `Globals::addSoundResourceToList+12`, reached
from `Level::createGun` during `Level::createScene`. The fault address
was zero. This is a later initialization stage, not a playable frame.
The original package was not changed, and no original game assets or
APKs were added to Git.

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

The second crash exposed a similar recovered-global mismatch:
`Globals::gGlobals` is a separate null pointer in this project, while
the original Android binary exports and initializes `Globals::globals`.
For 64-bit hosts, `Globals::init` now mirrors its initialized instance
into `gGlobals`. The ARMv7 source form is unchanged by this guarded
port bridge. The subsequent Redmi run passed the former sound-resource
crash site.

Further Redmi instrumentation showed that the native frame callback ran
and `MTitle::OnRender2D` drew repeatedly, but `GetElapsedTimeMillis()`
returned exactly 1 ms per frame while wall time advanced about 36 ms.
`ApplicationManager::OnUpdate` did not advance its frame timestamps in
state 5. Updating them before rendering restored the title transition:
the trace reached `ModMainMenu::OnInitialize` after about eight seconds.
The title image itself was still black, so this is a state progression
fix, not a rendering success.

The first menu initialization then exposed a null recovered pointer to
the difficulty record in `Level::assignGuns` (fault address `0x2c`).
The settings value is already present at `Globals::options + 0x2c`, as
also read by `Status`. `Level` now reads that field. Its thirteen
32-bit-size `Gun` allocations now use `sizeof(Gun)` so the object fits
on ARM64. The next Redmi run completed `CutScene::initialize` and
logged `ModMainMenu CutScene initialize done`. The following menu step
used another null recovered alias for the options record (fault `0x4e`);
the 64-bit path now uses `Globals::options` and the initialized
`Globals::recordHandler`.

The next instrumented Redmi run advanced through that options step
and stopped in `TouchButton::init`: the recovered menu helper passed the
button's X coordinate (`-6`) as its kind, indexing before the frame-ID
table. Correcting the constructor argument order moved execution to
`SpriteSystemCreate`, which still allocated the ARMv7 structure size
`0x14` on ARM64. `sizeof(SpriteSystem)` moved execution into the menu's
text window. There `Globals::getLineArray` attempted to free the data
field of a `String` before constructing it; real constructors and
destructors now manage those strings. The following Redmi test stayed
running for at least twelve seconds, through the menu transition, but
its screenshot remained fully black. This establishes startup progress,
not a visible or playable menu frame.

A temporary final-frame OpenGL clear produced a solid magenta screen on
the same Redmi. The Android GL surface and frame presentation therefore
work; the black output is inside the game's image/texture/shader draw
path. The magenta probe and all `GOF2TRACE` statements were removed
from source after testing. The test APK is outside Git, and the
original installed game package was not modified. Next: trace a title
image from `Image2DCreate` through texture loading, draw submission and
shader state. Later menu states still have unverified recovered globals.

The final normal clone APK remained running on Redmi after the title-to-menu
transition, but its screenshot was entirely black. The logo resource
`7000` maps to texture `10002`; one registered variant,
`gof2_logos_ipad2.aei`, is present in the original OBB with a valid
`AEimage` header. The current
five-argument `DrawImage2D` interprets a 32-bit field as a pointer, which
requires a focused ARM64 check before changing its representation. Whether
`Image2DCreate` reaches this draw path on the device is not yet established.

The final full ARMv7 check after the runtime fixes compiled **209/209**
units and compared **4,323/4,524** original functions in **4,762** pairs:
**96.20%** source-shape, **85.96%** strict fuzzy, **2,741 linked-exact**,
**1,096 byte-exact**, 201 missing, zero skipped. Relative to
`ship013-036-dummy-full-20261102.json`, the pair set and both exact sets
have zero gains and zero losses. The report is in ignored
`_work/redmi_runtime_full_20261102.json`.
