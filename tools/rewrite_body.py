#!/usr/bin/env python3
# rewrite_body.py <Class> [...] — mechanical bulk pass for the cleanup: rewrite byte-offset access
# in merged/src/<Class>.cpp to NAMED field access (field_0xNN) against the canonical gof2 headers,
# fix the header include, and pull in cross-class headers. Writes src/<Class>.cpp and prints
# what it could NOT convert (ambiguous locals) so the agent can finish those by hand + Ghidra.
#
# Conversions:
#   F<T>(p, 0xNN) / G<T>(p, 0xNN)        -> p->field_0xNN
#   *(T *)((char *)p + 0xNN)             -> p->field_0xNN     (offset 0 -> p->field_0x0)
#   #include "<Class>.h"                 -> #include "gof2/<Class>.h" (+ cross-class headers)
# Variable->class: self/this -> owning class; `Class *var` / `Class &var` decls in the TU; else
# lowercased-name match (engine->Engine). Unknown bases are LEFT AS-IS (printed for the agent).
import os, re, json, sys
ROOT="/Users/fionera/Downloads/GalaxyOnFire2/gof2-decomp"; os.chdir(ROOT)
CM=json.load(open("tools/classmap.json")); CLASSES=set(CM)
name2class={c.lower():c for c in CLASSES}
os.makedirs("src",exist_ok=True)

def fname(off):
    o=int(off,16) if str(off).startswith('0x') else int(off)
    return "field_"+hex(o)

HLP=re.compile(r'\b[FG]\s*<[^>]+>\s*\(\s*([A-Za-z_]\w*)\s*,\s*(0x[0-9a-fA-F]+|\d+)\s*\)')
RAW=re.compile(r'\*\(\s*[A-Za-z_][\w\s:]*?\**\s*\)\s*\(\s*\(\s*char\s*\*\s*\)\s*'
               r'([A-Za-z_]\w*)\s*(?:\+\s*(0x[0-9a-fA-F]+|\d+))?\s*\)')

def rewrite(cls):
    src=f"src/{cls}.cpp"   # operate IN PLACE on the canonical tree (git tracks the evolution)
    if not os.path.exists(src): return f"{cls}: NO SRC"
    txt=open(src,errors='ignore').read()
    v2c={'self':cls,'this':cls}
    for m in re.finditer(r'\b([A-Z]\w+)\s*[\*&]\s*([A-Za-z_]\w*)\b', txt):
        if m.group(1) in CLASSES: v2c.setdefault(m.group(2), m.group(1))
    refs=set(); left=[0]
    def cls_of(v): return v2c.get(v) or name2class.get(v.lower())
    def hlp(m):
        v,off=m.group(1),m.group(2); c=cls_of(v)
        if not c: left[0]+=1; return m.group(0)
        refs.add(c); return f"{v}->{fname(off)}"
    def raw(m):
        v,off=m.group(1),(m.group(2) or '0'); c=cls_of(v)
        if not c: left[0]+=1; return m.group(0)
        refs.add(c); return f"{v}->{fname(off)}"
    txt=HLP.sub(hlp,txt); txt=RAW.sub(raw,txt)
    inc=f'#include "gof2/{cls}.h"\n'+"".join(f'#include "gof2/{r}.h"\n' for r in sorted(refs) if r!=cls)
    if re.search(r'^#include "[^"]+\.h"\n', txt, re.M):
        txt=re.sub(r'^#include "[^"]+\.h"\n', inc, txt, count=1, flags=re.M)
    else:
        txt=inc+txt
    open(f"src/{cls}.cpp","w").write(txt)
    rem_F=len(re.findall(r'\b[FG]\s*<',txt)); rem_cast=len(re.findall(r'\(char ?\*\)',txt))
    return f"{cls}: -> src/{cls}.cpp | refs={sorted(refs)} | UNCONVERTED: {left[0]} (F/G left={rem_F}, casts left={rem_cast})"

if __name__=="__main__":
    for c in sys.argv[1:]: print(rewrite(c))
