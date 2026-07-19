# void*/union cleanup rules (read before editing)

You are cleaning ONE file (and its paired header only if a void*/union is declared there and owned by
that class) in a C++ decompilation that must keep matching the original libgof2hdaa.so at the SYMBOL
level. Working dir: /Users/fionera/Downloads/GalaxyOnFire2/gof2-decomp

GOAL: remove every `void *`/`void*` and every `union` from the target file(s), keep it compiling, and
do NOT change any exported function signature.

RULES
1. void* → the real pointed-to type. Infer from: cast sites `((Foo*)x)` (then x is Foo*), member and
   usage context, function names, neighboring typed decls, and getters' return types in headers.
   Byte buffers → `unsigned char *`; C strings → `char *`/`const char *`; JNI handles → jni.h types.
   Generic function-pointer slots → a real function-pointer type like `void(*)()` (NOT void*). If the
   type is genuinely unknowable, pick the most specific concrete type that compiles (a forward-declared
   class from context). NEVER leave a bare `void*`.
2. union `{ A x; B y; }` (decompiler dual-name aliases) → KEEP the real semantic member (NOT the
   `field_0xNN` one; if both are semantic keep the more-used), DELETE the `union {...}` wrapper and the
   dropped alias, and replace references to the dropped member WITHIN THIS FILE with the kept name (add
   a cast if the types differ). If the dropped member is referenced in OTHER files (grep src/), do NOT
   remove it — leave the union and report it as a cross-file blocker.
3. SYMBOL-PARITY HARD CONSTRAINT — never change the parameter or return types of: any
   `ClassName::method(...)` definition, or any normally-named free function (one whose `Pv` mangling
   the original exports/imports). Those signatures are baked into mangled names; changing them breaks
   the symbol match. ONLY retype void* that appear in: local variables, casts, struct/class DATA
   MEMBERS, `static`/`static inline` file-local helpers, and forward-declared internal shim functions
   that are file-local. If eliminating a void* would force changing such a signature, LEAVE it and add
   `// lint: void_ptr` ON THE SAME LINE with a one-line reason (same-line so the linter waives it).
4. PRESERVE: every `field_0xNN` member name that is NOT a union alias, every `static_assert` /
   `__builtin_offsetof` / numeric offset, and all behavior. Add `#include`s if needed but avoid include
   cycles (prefer forward declarations in headers). Leave every `extern "C"` untouched (ABI-required).
5. Do NOT run the build. Make minimal, correct edits with the Edit tool. C++14 only.

REPLY (this is data for the orchestrator, not a user message) with: count of void*/union removed,
anything deliberately left with the reason, and any cross-file union members you could not remove.
