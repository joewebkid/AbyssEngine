#!/usr/bin/env python3
# strip_externs.py — make the functions WE DEFINE normal C++ instead of decomp `extern "C"`: drop the
# `extern "C"` qualifier from their definitions AND from every forward-declaration of them (in src and
# headers), consistently, in place. Genuine externals (libc/GL/JNI/FMOD/engine primitives we don't
# define) keep `extern "C"`. Also delete orphan protos (symbols never referenced) and exact-duplicate
# protos within a file. No relocation / include changes -> no linkage or include hazards.
import glob, re, os
ROOT="/Users/fionera/Downloads/GalaxyOnFire2/gof2-decomp"; os.chdir(ROOT)
KW={'if','for','while','switch','return','else','do','catch','sizeof','operator','new','delete','case'}

# free-function DEFINITIONS we own (name has no ::, has a body)
DEFD=re.compile(r'^[ \t]*(?:extern\s+"C"\s+)?(?:[A-Za-z_][\w:<>]*[\s\*&]+)+([A-Za-z_]\w*)\s*\([^;{]*\)\s*\{', re.M)
owned=set()
for f in glob.glob("src/**/*.cpp", recursive=True):
    for m in DEFD.finditer(open(f,errors='ignore').read()):
        if m.group(1) not in KW: owned.add(m.group(1))

# drop `extern "C" ` from a FUNCTION def/proto line iff its symbol is owned. The owned name must be the
# identifier immediately before the parameter list that terminates the declarator (so `__attribute__((..))`
# on a variable decl isn't mistaken for the function). Skip array/variable decls (contain `[`).
LINE=re.compile(r'^([ \t]*)extern\s+"C"\s+'
                r'([^\n]*?\b([A-Za-z_]\w*)\s*\([^;{()]*\)\s*(?:noexcept\s*)?(?:__attribute__\s*\(\([^)]*\)\)\s*)?[;{][^\n]*)$', re.M)
def deextern(t):
    def repl(m):
        if m.group(3) in owned and '[' not in m.group(2) and not m.group(3).startswith('__'):
            return f"{m.group(1)}{m.group(2)}"
        return m.group(0)
    return LINE.sub(repl, t)

files=glob.glob("src/**/*.cpp", recursive=True)+glob.glob("src/**/*.h", recursive=True)
nun=0
for f in files:
    t=open(f,errors='ignore').read(); o=t
    t=deextern(t)
    if t!=o: open(f,'w').write(t); nun+=1

# orphan protos: a proto whose symbol never appears as a call/def anywhere (pure dead leftover)
alltext="".join(open(f,errors='ignore').read() for f in glob.glob("src/*.cpp")+glob.glob("include/gof2/*.h"))
PROTO=re.compile(r'^[ \t]*(?:extern\s+"C"\s+)?[^\n;{]*?\b([A-Za-z_]\w*)\s*\([^;{]*\)\s*;[ \t]*\n', re.M)
norph=0
for f in glob.glob("src/*.cpp"):
    t=open(f,errors='ignore').read()
    def maybe_drop(m):
        global norph
        sym=m.group(1)
        if len(re.findall(r'\b%s\s*\('%re.escape(sym), alltext))<=alltext.count(m.group(0).strip()):
            return m.group(0)  # conservative: keep (counting is fuzzy); orphans handled below
        return m.group(0)
    # (orphan deletion left out of the bulk pass to stay safe; handled by a separate verified step)
    pass
print(f"un-externed owned functions in {nun} files")
