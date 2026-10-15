# PaintCanvas camera creation and matrix ABI (2026-10-15)

## Source-backed findings

REA/IDA read Android 2.0.16 ARM `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`):

- `Camera::Camera(float,float,float,float,float)` at `0x7b724`
  constructs a 60-byte `Matrix` subobject at camera offset `+0x0c`
  (`0x7b730–0x7b738`) before setting perspective.
- `PaintCanvas::CameraGetLocal(uint)` at `0x7b7a4` returns camera
  `+0x0c` for a valid index (`0x7b7bc–0x7b7c4`), otherwise the canvas
  identity matrix at `+0xf8` (`0x7b7ca–0x7b7d2`).
- `CameraSetLocal(uint,const Matrix&)` at `0x7b780` calls
  `Matrix::operator=` on camera `+0x0c` (`0x7b78e–0x7b79a`).
- `CameraCreate(uint&)` at `0x7b6a8` allocates `0x5c` bytes, calls
  `GetWidth` then `GetHeight`, constructs `Camera` with **8000.0f,
  256.0f, 16384.0f, float(width), float(height)** (`0x7b6cc–0x7b6f0`),
  adds it via `ArrayAdd<Camera*>` and writes the new index
  (`0x7b6f4–0x7b704`).

`Camera.h` independently declares the 60-byte `projection` Matrix at
`+0x0c`, and the compiled `CameraGetLocal` and `CameraSetLocal` are both
linked-exact. Thus the earlier concern that the camera pointer necessarily
aliases an unrelated `float[15]` object is resolved for constructed
`Camera` instances: `PCCameraView::localMatrix` is a raw overlay of the
real `Camera::projection` subobject. The previous render audit's ABI caveat
is superseded by this evidence. This does not prove behavior of malformed
or externally supplied camera handles.

## Repair and verification

The old `CameraCreate` called two undefined local helpers and passed only
width and height to the constructor helper. It now calls the real
five-argument `Camera` constructor and the existing explicit
`ArrayAdd<Camera*>` instantiation. The original allocation and
width-before-height order are preserved. Native `gof2` builds, **206/206**
ARM translation units build, and the extracted-production smoke checks two
created cameras, constructor arguments and returned indices:

```powershell
python tools/run_canvas_camera_create_smoke.py --cxx C:/msys64/ucrt64/bin/g++.exe
```

Focused ARM source-shape/strict fuzzy improves **76.8%/69.6% →
84.7%/75.7%**, with 40 original versus 34 reconstructed instructions.
The method is not linked- or byte-exact. The ARM object references real
`Camera` and `ArrayAdd<Camera*>` symbols rather than either old helper.
Full report `_work/camera_create_full_20261015.json` compares
**4,323/4,524** original functions at **93.46%** average source-shape
and **82.36%** strict fuzzy, with **201 missing**, **123 extras** and zero
skipped. Relative to the preceding render report, both exact-symbol sets
are identical: **2,486 linked-exact**, **1,065 byte-exact**, zero losses
and zero gains. The corpus averages are unchanged at two decimals.

REA's IDA adapter does not establish complete function body ranges or
indirect-call coverage. The smoke uses fixture services, not an original
Android/iOS frame. A controlled runtime rendering check remains the next
priority; do not infer it from compile or instruction matching alone.
