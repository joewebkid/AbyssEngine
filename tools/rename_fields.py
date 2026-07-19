#!/usr/bin/env python3
# rename_fields.py — apply per-class field rename maps (field_0xNN -> semanticName) type-awarely across
# the whole tree: the struct field declaration in the owning header AND every access site, where the
# receiver's class is resolved the same way as the methodize pass (self/this -> file's class; `Class *v`
# local/param decls; lowercased class-named vars). Map file: tools/rename_map.json = {Class:{field_0xNN:name}}.
import glob, re, os, json, sys
ROOT="/Users/fionera/Downloads/GalaxyOnFire2/gof2-decomp"; os.chdir(ROOT)
CLASSES={os.path.basename(f)[:-4] for f in glob.glob("src/*.cpp")} | {os.path.basename(f)[:-2] for f in glob.glob("include/gof2/*.h")}
name2class={c.lower():c for c in CLASSES}
MAP=json.load(open(sys.argv[1] if len(sys.argv)>1 else "tools/rename_map.json"))

# inheritance: `struct D : [public] B` -> parent[D]=B, so a derived class's access to an inherited
# field resolves against the BASE class's rename map (where that field's struct decl was renamed).
parent={}
for h in glob.glob("include/gof2/*.h"):
    for m in re.finditer(r'struct\s+([A-Za-z_]\w*)\s*:\s*(?:public\s+)?([A-Za-z_]\w*)\b', open(h,errors='ignore').read()):
        if m.group(1) in CLASSES and m.group(2) in CLASSES: parent.setdefault(m.group(1), m.group(2))

# drop renames whose new name collides with a method/other identifier in the class's struct (would be
# "redefinition as a different kind of symbol") or is a C++ keyword.
KW={'new','delete','class','struct','int','float','operator','this','default','template','union','if',
    'for','while','return','char','bool','void','short','long','double','unsigned','signed','const'}
for h in glob.glob("include/gof2/*.h"):
    cls=os.path.basename(h)[:-2]
    if cls not in MAP: continue
    t=open(h,errors='ignore').read()
    m=re.search(r'struct\s+'+re.escape(cls)+r'\b[^{;]*\{', t)
    if not m: continue
    d=0;i=m.end()-1
    while i<len(t):
        if t[i]=='{':d+=1
        elif t[i]=='}':
            d-=1
            if d==0:break
        i+=1
    body=t[m.end()-1:i+1]
    methods=set(re.findall(r'\b([A-Za-z_]\w*)\s*\(', body))
    for fld in list(MAP[cls]):
        if MAP[cls][fld] in methods or MAP[cls][fld] in KW: del MAP[cls][fld]

def field_new(cls, fld):
    """walk class -> base -> ... for the rename of `fld`."""
    seen=set()
    while cls and cls not in seen:
        seen.add(cls)
        if fld in MAP.get(cls, {}): return MAP[cls][fld]
        cls=parent.get(cls)
    return None

def rename_struct_decls():
    """In include/gof2/<C>.h, rename `<type> field_0xNN;` inside `struct C { ... }` to the mapped name."""
    n=0
    for cls, fmap in MAP.items():
        hp=f"include/gof2/{cls}.h"
        if not os.path.exists(hp) or not fmap: continue
        t=open(hp).read()
        m=re.search(r'struct\s+'+re.escape(cls)+r'\b[^{;]*\{', t)
        if not m: continue
        d=0; i=m.end()-1
        while i<len(t):
            if t[i]=='{': d+=1
            elif t[i]=='}':
                d-=1
                if d==0: break
            i+=1
        body=t[m.end()-1:i+1]
        def repl(mm):
            return mm.group(1)+fmap.get(mm.group(2), mm.group(2))
        nb=re.sub(r'(\b)(field_0x[0-9a-f]+)\b', lambda mm: fmap.get(mm.group(2), mm.group(2)), body)
        if nb!=body:
            t=t[:m.end()-1]+nb+t[i+1:]; open(hp,'w').write(t); n+=1
    return n

def rename_accesses():
    """Rename `recv->field_0xNN` / `recv.field_0xNN` per the receiver's class map (src + header inline methods)."""
    nfile=0
    files=glob.glob("src/*.cpp")+[h for h in glob.glob("include/gof2/*.h")
                                  if os.path.basename(h) not in {"common.h","math.h","fwd.h"}]
    for f in files:
        owner=os.path.basename(f).rsplit('.',1)[0]
        t=open(f,errors='ignore').read(); o=t
        v2c={'self':owner,'this':owner}
        for m in re.finditer(r'\b([A-Z]\w+)\s*[\*&]\s*([A-Za-z_]\w*)\b', t):
            if m.group(1) in CLASSES: v2c.setdefault(m.group(2), m.group(1))
        def repl(m):
            var, op, fld = m.group(1), m.group(2), m.group(3)
            cls = v2c.get(var) or name2class.get(var.lower())
            nn = field_new(cls, fld) if cls else None
            return f"{var}{op}{nn}" if nn else m.group(0)
        t=re.sub(r'\b([A-Za-z_]\w*)\s*(->|\.)\s*(field_0x[0-9a-f]+)\b', repl, t)
        if t!=o: open(f,'w').write(t); nfile+=1
    return nfile

if __name__=="__main__":
    sd=rename_struct_decls(); ac=rename_accesses()
    print(f"renamed struct decls in {sd} headers; rewrote accesses in {ac} cpp files")
