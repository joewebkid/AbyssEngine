# Android JNI touch queue handoff (2026-10-25)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The installed original APK (SHA-256
`515fc9a651057130c7b2b1c47bb497c61ebaebb0c567d413b5bd5453c13b221c`)
declares `ToJNI.handleTouchEvent(IIII)V`. Its `TouchHandler`
multiplies `MotionEvent` coordinates by display scale, converts them
to integers, then calls this method (DEX calls at `0x21fc36`,
`0x21fcac`, and `0x21fd5a`).

The ARM JNI export at `0x6b87c..0x6b884` moves the four Java
integers into native argument registers and branches to
`ndk23_handleTouchScreenEvent`. That function at `0x94014` branches
through veneer `0x19b848` to `AddTouch(int,int,int,int)@plt` at
`0x616a4`, not to `ndk23_handleTouchPadEvent`. `AddTouch` at
`0x950dc` stores all four values in the `Touch` queue; the existing
`ndk23_newrender` consumes `GetTouch` records and calls
`RemoveTouches` after the frame.

Recovered JNI instead declared the coordinates as `jfloat` and
called the direct float touchpad handler. The Java integers were
therefore interpreted as float bit patterns and the screen event
never entered the queue consumed by the frame. The JNI wrapper now
accepts four `jint` arguments, and `ndk23_handleTouchScreenEvent`
forwards them to the existing `AddTouch` implementation. The separate
touchpad handler is unchanged.

Focused ARM source-shape/strict fuzzy for the JNI export rises from
**19.4%/0%** to **100%/100%**, newly linked-exact. The screen
handler remains **100%/100%** and becomes linked-exact; `AddTouch`
was already linked-exact. Native `gof2`, **209/209** ARM translation
units and **93** verifier tests (one skip) pass. The full corpus
compares **4,323/4,524** originals over **4,737** pairs at
**94.58%** mean source-shape and **84.08%** strict fuzzy, with
**2,594 linked-exact** and **1,082 byte-exact**. The two touch
functions enter the linked-exact set; no linked- or byte-exact symbol
is lost.

This validates the source/ABI handoff, not a recovered game receiving
live touch events on Android. The direct touchpad path and its
unverified callers are outside this package. The next runtime gate
is touch input through a recovered APK and GL frame loop.
