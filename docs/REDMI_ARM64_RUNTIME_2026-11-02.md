# Redmi ARM64 runtime checks (2026-11-02)

Device: Xiaomi `25053RT47C` (`onyx`), Android 16, `arm64-v8a`,
physical display 1280 × 2772. All ADB commands targeted its explicit
wireless device ID; the connected Poco X3 was not changed. This is a
runtime check of previously built APKs, with no source-code edits.

## Installed 2026-10-28 test build

The installed `net.fishlabs.gof2hdallandroid2012` APK has SHA-256
`6f4b4c1e2a8a4d905e4d3ab8b371dc005b1401697aeb8a906d9ee1d89425278e`,
identical to `gof2-arm64-menuglobals-debug-20261028.apk`. Its installed
original OBB has SHA-256
`42cfe13f0f3b9f99f38a1e77c74ac1cf41e542b3becbffea0f5bb7cfc4a6e4cb`,
matching the local reference. The game started in its own foreground
activity and initially stayed alive. Its captured image was nearly
black, with a cropped `BUY` fragment at the top. Its process log
contained 20 Adreno GLSL syntax errors, all at line 1 of placeholder
source, and no startup native crash. A later run crashed at 09:12:03
in `PlayerEgo::setShip(int,int)+88`, fault address `0x20`, called from
`Level::createPlayer` through `ModMainMenu::OnInitialize`. This
reproduces the previously documented null material-block failure;
it is not evidence that the current source still has exactly the
same failure. Evidence on D: `redmi-test-20261102-start.png`,
`redmi-test-20261102-app.log`, and
`redmi-test-20261102-crash.log`.

## 2026-10-30 parallel test build

The separately installed `net.fishlabs.gof2hdallandroiddevx` APK was
`gof2-arm64-vcat-shader-poco-clone-debug-20261030.apk`, SHA-256
`729ea7ebe7bd1a907ce76eafeaf9fb58968d3146185e7cf3c905050e623d668e`.
Play Protect rejected both streamed and pushed ADB installs with
`INSTALL_FAILED_VERIFICATION_FAILURE`; the user manually opened the
APK from Downloads and approved installation. It is a different
package from the older build, not an update to that app.

The clone's downloader could not find an OBB in its package-specific
directory. A device-local copy of the original OBB was byte-identical
but initially owned by `shell` and unreadable by the app: logcat
reported `EACCES`, followed by a null ZIP handle crash in
`_zip_name_locate` from `FileInterfaceAndroid::FileExist`. Changing
the temporary copy's mode to `0644` removed that test setup failure.
The clone then entered its GOF2 activity and remained running after
dismissing the Google Play sign-in screen. Its process log had one
Adreno GLSL syntax error and no new native crash during the observed
run. The captured game image was fully black. One center-screen tap
made no visible change; Android Back closed the app without a new
crash. This does not test gameplay or the `PlayerEgo::setShip` path.
Evidence on D: `redmi-test-20261102-clone-screen.png`,
`redmi-test-20261102-clone-run.log`, and
`redmi-test-20261102-clone-crash.log` (the latter includes the
initial, permission-related crash).

After testing, the temporary clone OBB, uploaded APK, and cloned app
were removed from the Redmi. The original package and its OBB were
left installed and unchanged. The two screenshot results and the
20-versus-1 GLSL error counts compare different historical builds on
the same device; they do not attribute the improvement to one patch.
Neither APK includes the 2026-11-02 ModStation edits. The latest
source corpus remains **4,323/4,524** originals over **4,762**
comparisons, **96.20%** mean source-shape, **85.95%** strict fuzzy,
**2,741 linked-exact** and **1,096 byte-exact**, as reported in
`MODSTATION_KEY_HANGAR_ARM_2026-11-02.md`; no verifier was rerun for
this runtime-only report.

Next: trace the remaining shader placeholder and GL target/frame
path in a rebuilt APK with current source. Then separately trace
ship mesh material creation before treating the old `setShip` crash
as fixed.
