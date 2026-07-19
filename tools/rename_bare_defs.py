#!/usr/bin/env python3
# rename_bare_defs.py — some instance functions were recovered as BARE free functions `method(Class*self,
# ...)` (no Class_ prefix), so the methodize pass (which keyed on Class_method) missed them while their
# call sites use `Class_method(...)` with an extern "C" proto. Rename those bare defs to real methods
# `Class::method(...)` (keeping `self` as a `this` alias). The subsequent methodize re-run then converts
# the call sites + drops the protos.
import glob, re, os, collections
ROOT="/Users/fionera/Downloads/GalaxyOnFire2/gof2-decomp"; os.chdir(ROOT)
CLASSES={os.path.basename(f)[:-4] for f in glob.glob("src/*.cpp")}|{os.path.basename(f)[:-2] for f in glob.glob("include/gof2/*.h")}

# targets: (Class, method) that have an extern "C" Class_method(Class*,...) proto for one of our classes
targets=set()
for f in glob.glob("src/*.cpp"):
    for m in re.finditer(r'extern\s+"C"\s+[^;{]*?\b([A-Z][A-Za-z0-9]*)_([a-z]\w*)\s*\(\s*(?:const\s+)?([A-Za-z_]\w*)\s*\*', open(f,errors='ignore').read()):
        if m.group(1) in CLASSES: targets.add((m.group(1), m.group(2)))

n=0
for f in glob.glob("src/*.cpp"):
    t=open(f,errors='ignore').read(); o=t
    for cls,meth in targets:
        # a bare definition `<ret> method(Class *self [, params]) {`  -> `<ret> Class::method(params) { Class *self=this;`
        # return type may be a template-with-pointers e.g. `Array<Item*>*` or `Ship *`
        pat=re.compile(r'(?m)^([ \t]*)((?:[A-Za-z_][\w:]*(?:<[^;{]*?>)?[\s*&]*)+?[\s*&])'+re.escape(meth)+
                       r'\s*\(\s*(?:const\s+)?'+re.escape(cls)+r'\s*\*\s*(\w+)\s*((?:,[^;{()]*(?:\([^()]*\)[^;{()]*)*)?)\)\s*\{')
        def repl(m, cls=cls, meth=meth):
            indent, ret, selfname, rest = m.group(1), m.group(2), m.group(3), m.group(4).lstrip(',').strip()
            return f'{indent}{ret}{cls}::{meth}({rest}) {{\n{indent}    {cls} *{selfname} = this;'
        t2=pat.sub(repl, t)
        if t2!=t: t=t2; n+=1
    if t!=o: open(f,'w').write(t)
print(f"renamed {n} bare free-function definitions to Class::method")
