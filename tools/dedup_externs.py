#!/usr/bin/env python3
# dedup_externs.py — the decomp scattered identical-purpose extern decls (GL ES functions, the stack
# canary, malloc/free) across many headers AND .cpp with DIFFERENT signatures; cross-class includes then
# collide them. Declare GL + the canary ONCE in common.h (canonical, with GLES2 type aliases) and strip
# every per-file copy. malloc/free come from <cstdlib>. The canary guard is modeled uint32_t (as used).
import glob, re, os, collections
ROOT="/Users/fionera/Downloads/GalaxyOnFire2/gof2-decomp"; os.chdir(ROOT)
FOUND={"common.h","math.h","fwd.h"}
# A function FORWARD-DECLARATION (return type REQUIRED, so call statements like `glUseProgram(p);` are NOT matched).
FDECL=re.compile(r'^[ \t]*(?:extern\s+(?:"C"\s+)?)?(?:[A-Za-z_][\w:<>]*[\s\*&]+)+'
                 r'(gl[A-Z]\w*|__stack_chk_fail|malloc|free)\s*\([^;{]*\)\s*;[ \t]*\n', re.M)
# The stack-canary guard VARIABLE declaration.
GDECL=re.compile(r'^[ \t]*extern\s+(?:"C"\s+)?[\w ]*\*?\s*__stack_chk_guard\s*;[ \t]*\n', re.M)

gl=collections.OrderedDict()
for h in glob.glob("include/gof2/*.h")+glob.glob("src/*.cpp"):
    if os.path.basename(h) in FOUND: continue
    for m in FDECL.finditer(open(h,errors='ignore').read()):
        s=m.group(1)
        if s.startswith('gl') and s not in gl:
            d=m.group(0).strip()
            gl[s]=d if d.startswith('extern') else 'extern "C" '+d
for h in glob.glob("include/gof2/*.h")+glob.glob("src/*.cpp"):
    if os.path.basename(h) in FOUND: continue
    t=open(h,errors='ignore').read(); t2=GDECL.sub('', FDECL.sub('', t))
    if t2!=t: open(h,'w').write(t2)

c=open("include/gof2/common.h").read()
if "decomp artifact externs" not in c:
    block=('\n// ---- GLES2 scalar type aliases (so the centralized gl* decls resolve) ----\n'
           'typedef unsigned int GLenum;  typedef unsigned char GLboolean; typedef unsigned int GLbitfield;\n'
           'typedef signed char GLbyte;   typedef short GLshort;   typedef int GLint;   typedef int GLsizei;\n'
           'typedef unsigned char GLubyte; typedef unsigned short GLushort; typedef unsigned int GLuint;\n'
           'typedef float GLfloat; typedef float GLclampf; typedef int GLfixed; typedef char GLchar;\n'
           'typedef void GLvoid;  typedef ptrdiff_t GLintptr; typedef ptrdiff_t GLsizeiptr;\n'
           '\n// ---- decomp artifact externs (GL ES + stack canary; were duplicated per header/TU) ----\n'
           '// canary guard is read/arithmetic-ed as uint32_t by the decompiled code.\n'
           'extern "C" uint32_t __stack_chk_guard;\n'
           'extern "C" __attribute__((noreturn)) void __stack_chk_fail(...);\n'
           + "\n".join(gl.values()) + "\n")
    c=c.replace("#endif // GOF2_COMMON_H", block+"\n#endif // GOF2_COMMON_H")
    open("include/gof2/common.h","w").write(c)
print(f"centralized {len(gl)} GL functions + stack canary into common.h; stripped per-file copies")
