#!/usr/bin/env python3
# finish_methods.py — the agents converted many Class_method() free functions to real methods at the
# DEFINITION site (Class::method) but left some CALLERS still calling the free `Class_method(...)` with
# `extern "C"` protos (compiles, but would fail to LINK — the free symbol doesn't exist). Finish the job:
# rewrite those calls to `((Class*)recv)->method(args)`, delete the dead protos, and add the includes.
import glob, re, os, collections
ROOT="/Users/fionera/Downloads/GalaxyOnFire2/gof2-decomp"; os.chdir(ROOT)
CLASSES={os.path.basename(f)[:-4] for f in glob.glob("src/*.cpp")} | {os.path.basename(f)[:-2] for f in glob.glob("include/gof2/*.h")}

# registry of REAL methods (line-anchored Class::method(params) { definitions)
DEF=re.compile(r'^[ \t]*(?:[A-Za-z_][\w:<>]*[\s\*&]+)+([A-Za-z_]\w*)::([A-Za-z_]\w*)\s*\((?P<params>[^;{]*)\)\s*\{', re.M)
REG={}
for f in glob.glob("src/*.cpp"):
    for m in DEF.finditer(open(f,errors='ignore').read()):
        if m.group(1) in CLASSES:
            REG.setdefault(f"{m.group(1)}_{m.group(2)}", (m.group(1), m.group(2), m.group('params').strip()))

def split_top(s):
    out=[]; d=0; cur=""
    for ch in s:
        if ch in '([{': d+=1
        elif ch in ')]}': d-=1
        if ch==',' and d==0: out.append(cur); cur=""
        else: cur+=ch
    if cur.strip() or out: out.append(cur)
    return out

CALL=re.compile(r'(?P<pre>.?)\b(?P<sym>[A-Za-z_]\w*?_[A-Za-z_]\w*)\s*\(')
# a forward-declaration line for SYM: optional `extern "C"`, a return type, SYM(params); + optional comment.
# Matches both `extern "C" R Class_method(...);` and the plain `R Class_method(...);` left by the un-extern pass.
PROTO=re.compile(r'^[ \t]*(?:extern\s+"C"\s+)?(?:[A-Za-z_][\w:<>]*[\s\*&]+)+([A-Za-z_]\w*_[A-Za-z_]\w*)\s*\([^;{]*\)\s*;[^\n]*\n', re.M)

ncall=nproto=0
for f in glob.glob("src/*.cpp"):
    t=open(f,errors='ignore').read(); o=t; called=set()
    def dp0(m):
        global nproto
        if m.group(1) in REG: nproto+=1; return ''
        return m.group(0)
    t=PROTO.sub(dp0, t)                                   # remove dead protos FIRST (before call rewrite)
    for _ in range(8):                                    # multi-pass for nested calls
        out=[]; i=0; n=0
        for m in CALL.finditer(t):
            if m.start()<i: continue
            sym=m.group('sym')
            if sym not in REG or m.group('pre')=='&': continue
            ls=t.rfind('\n', 0, m.start())+1                 # don't rewrite a declaration line (vs a call)
            pre_line=t[ls:m.start()]
            if t[ls:ls+1]=='#' or re.match(r'^[ \t]*(extern\s+"C"\s+)?([A-Za-z_][\w:<>]*[\s\*&]+)+$', pre_line):
                continue
            j=m.end(); d=1
            while j<len(t) and d:
                if t[j]=='(': d+=1
                elif t[j]==')': d-=1
                j+=1
            args=[a.strip() for a in split_top(t[m.end():j-1])]
            if not args or not args[0]: continue
            cls,method,params=REG[sym]
            nparams=len([p for p in split_top(params) if p.strip()]) if params.strip() else 0
            rest=", ".join(args[1:1+nparams])
            pl=1 if m.group('pre') else 0
            out.append(t[i:m.start()+pl]); out.append(f'(({cls} *)({args[0]}))->{method}({rest})')
            called.add(cls); i=j; n+=1
        out.append(t[i:]); t="".join(out); ncall+=n
        if n==0: break
    own=os.path.basename(f)[:-4]
    need=[c for c in sorted(called) if c!=own and f'"gof2/{c}.h"' not in t]
    if need:
        blk="".join(f'#include "gof2/{c}.h"\n' for c in need)
        mm=re.search(r'^#include "gof2/[^"]+\.h"\n', t, re.M)
        t=(t[:mm.end()]+blk+t[mm.end():]) if mm else blk+t
    if t!=o: open(f,'w').write(t)
print(f"methods in registry: {len(REG)}; free calls rewritten: {ncall}; dead protos removed: {nproto}")
