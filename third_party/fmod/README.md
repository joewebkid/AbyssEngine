# FMOD Ex 4.44.07 — Programmer's API headers

Galaxy on Fire 2 links **FMOD Ex 4.44.07** (Firelight Technologies):
- Android (`libgof2hdaa.so`, APK 2.0.16): dynamic `libfmodex.so` + `libfmodevent.so`
- iOS (1.1.4 HD): the same FMOD, statically linked

Version recovered from the baked-in `FMOD_VERSION` constant `0x00044407` in
`System::getVersion` (print format `FMODEx/%x.%02x.%02x` → `4.44.07`). The libraries were
built with `GCC 4.6 20120106 (prerelease)`.

## What to drop here

Copy the headers from the **FMOD Ex 4.44.07 Programmer's API** (`api/inc/`) into `inc/`. The
platform of the SDK download does not matter — the headers are identical across Android / iOS /
desktop; only the linked binary differs. The files this project needs:

```
inc/fmod.h            inc/fmod.hpp
inc/fmod_codec.h      inc/fmod_dsp.h
inc/fmod_memoryinfo.h inc/fmod_output.h
inc/fmod_errors.h
inc/fmod_event.h      inc/fmod_event.hpp
inc/fmod_event_net.h            (optional — only if net event API is referenced)
```

Use exactly the **4.44.07** revision so `FMOD_VERSION` matches the binary; mismatched headers will
fail the `FMOD_System_init` version check at runtime and can shift struct layouts for the matching
build.

## License / tracking

These are proprietary Firelight headers. They are committed so the engine compiles against the real
FMOD types (replacing the stubs in `src/engine/audio/FModSound.h`). The compiled libraries
(`*.so`, `*.a`, `*.dll`) are **not** committed — see `.gitignore` here.
