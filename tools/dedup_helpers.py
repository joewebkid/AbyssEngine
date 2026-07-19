#!/usr/bin/env python3
# dedup_helpers.py — cleanup agents added identical byte-offset accessor helpers (UC/F/I/P/u32/...) as
# `static inline` in many per-class headers; methodize's cross-class includes now pull several into one
# TU -> "call to X is ambiguous". Move ONE canonical copy of each DUPLICATED helper (>=2 headers) into
# common.h and remove it from the per-class headers. Unique, class-specific helpers are left in place.
import glob, re, os, collections
ROOT="/Users/fionera/Downloads/GalaxyOnFire2/gof2-decomp"; os.chdir(ROOT)
FOUND={"common.h","math.h","fwd.h"}
HELP=re.compile(r'(?P<full>(?:[ \t]*template\s*<[^>]*>\s*\n?)?[ \t]*static\s+inline\s+'
                r'[^\n;{}]*?\b(?P<name>[A-Za-z_]\w*)\s*\([^)]*\)\s*\{[^{}]*\}[ \t]*\n?)', re.S)
def is_helper(b): return '(char *)' in b or '(char*)' in b

cnt=collections.Counter(); defs=collections.defaultdict(list)
for h in glob.glob("include/gof2/*.h"):
    if os.path.basename(h) in FOUND: continue
    for m in HELP.finditer(open(h).read()):
        if is_helper(m.group('full')):
            cnt[m.group('name')]+=1; defs[m.group('name')].append(m.group('full').strip())
DUP={n for n,c in cnt.items() if c>=2}
canon={n: max(defs[n], key=len) for n in DUP}          # prefer the most complete (e.g. template) variant
# only MOVE helpers whose definition compiles standalone (type-independent accessors). Type-dependent
# ones (e.g. returning a class type like Vec3) stay in their headers.
import subprocess, tempfile
def compiles(defn):
    src='#include <cstdint>\n#include <cstddef>\n'+defn+'\nint main(){return 0;}\n'
    p=subprocess.run(['clang++','-std=gnu++14','-fpermissive','-w','-fsyntax-only','-x','c++','-'],
                     input=src, capture_output=True, text=True)
    return p.returncode==0
movable={n for n in DUP if compiles(canon[n])}
print("type-dependent (left in headers):", sorted(DUP-movable))
DUP=movable; canon={n:canon[n] for n in DUP}

for h in glob.glob("include/gof2/*.h"):
    if os.path.basename(h) in FOUND: continue
    t=open(h).read()
    t2=HELP.sub(lambda m: '' if (is_helper(m.group('full')) and m.group('name') in DUP) else m.group('full'), t)
    if t2!=t: open(h,'w').write(t2)

c=open("include/gof2/common.h").read()
if "GOF2 byte-offset accessor helpers" not in c:
    block=("\n// ---- GOF2 byte-offset accessor helpers (single definition; were duplicated per header) ----\n"
           + "\n".join(canon[n] for n in sorted(canon)) + "\n")
    c=c.replace("#endif // GOF2_COMMON_H", block+"\n#endif // GOF2_COMMON_H")
    open("include/gof2/common.h","w").write(c)
print("deduped helpers:", sorted(DUP))
