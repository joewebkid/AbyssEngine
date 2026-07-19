#!/usr/bin/env python3
# reconcile.py — add cross-class fields the cleanup agents flagged as MISSING into the owning header.
# Agents (which only edit their own batch) report lines like:
#     MISSING FIELD Engine.field_0x40 (void*)
# Pass those lines on stdin (or a file arg); this inserts each missing `<type> field_0xNN;` into the
# owning class's struct in include/gof2/<Class>.h (idempotent — skips fields already present).
import os, re, sys
ROOT="/Users/fionera/Downloads/GalaxyOnFire2/gof2-decomp"; os.chdir(ROOT)
PAT=re.compile(r'MISSING FIELD\s+([A-Za-z_]\w*)\.(field_0x[0-9a-fA-F]+)\s*\(([^)]+)\)')

def add_field(cls, field, typ):
    hp=f"include/gof2/{cls}.h"
    if not os.path.exists(hp): return f"{cls}: NO HEADER"
    t=open(hp).read()
    if re.search(r'\b'+re.escape(field)+r'\b', t): return f"{cls}.{field}: already present"
    m=re.search(r'(struct\s+'+re.escape(cls)+r'\s*(?::[^{]*)?\{)', t)
    if not m: return f"{cls}: no struct to extend"
    ins=m.end()
    t=t[:ins]+f"\n    {typ} {field};   // reconciled cross-class field"+t[ins:]
    open(hp,"w").write(t)
    return f"{cls}.{field} ({typ}): ADDED"

def main():
    data=open(sys.argv[1]).read() if len(sys.argv)>1 else sys.stdin.read()
    seen=set(); out=[]
    for cls,field,typ in PAT.findall(data):
        k=(cls,field)
        if k in seen: continue
        seen.add(k); out.append(add_field(cls,field,typ.strip()))
    print("\n".join(out) or "no MISSING FIELD lines found")

if __name__=="__main__": main()
