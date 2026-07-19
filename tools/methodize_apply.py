#!/usr/bin/env python3
# methodize_apply.py [--only Class1,Class2] — convert `RetType Class_method(Class *self, ...)` free
# functions into C++ instance methods on the struct. Global, consistent pass:
#   * definition:  [extern "C"] R Class_method(Class *self, rest){body}  ->  R Class::method(rest){ Class *self=this; body }
#   * prototype:   [extern "C"] R Class_method(...);                      ->  (removed)
#   * call site:   Class_method(arg0, rest)  ->  ((Class*)(arg0))->method(rest)   [receiver cast: no-op
#                  when already Class*, the needed fix when callers used void* (decomp opaque pattern)]
#   * header:      add `R method(rest);` inside struct Class
#   * includes:    add #include "gof2/Other.h" to files that now call Other's methods
# Left for the follow-up compile-fix pass (reported, not mangled): &Class_method (fn-pointer), and
# 0-arg calls where the decompiler dropped `self` (no receiver to form).
import glob, re, os, json, sys, collections
ROOT="/Users/fionera/Downloads/GalaxyOnFire2/gof2-decomp"; os.chdir(ROOT)
REG=json.load(open("tools/method_registry.json"))
only=None
if "--only" in sys.argv: only=set(sys.argv[sys.argv.index("--only")+1].split(","))
SYMS={s:v for s,v in REG.items() if (only is None or v["class"] in only)}
BY_CLASS=collections.defaultdict(list)
for s,v in SYMS.items(): BY_CLASS[v["class"]].append(v)

# Definition/prototype signature, ANCHORED to line start (so `return Class_method(...);` etc. are NOT
# matched). Excludes statement keywords. term '{' = definition, ';' = prototype.
SIG=re.compile(r'^[ \t]*(?P<ext>extern\s+"C"\s+)?'
               r'(?!(?:return|case|goto|delete|new|else|do)\b)'
               r'(?P<head>[A-Za-z_][\w:\s]*?[\w>]\s*[\*&]*\s*)'
               r'(?P<sym>[A-Za-z_]\w*?_[A-Za-z_]\w*)\s*'
               r'\((?P<params>[^;{]*?)\)\s*(?P<term>[{;])', re.M)

def convert_defs_decls(txt, report):
    out=[]; i=0
    for m in SIG.finditer(txt):
        sym=m.group('sym')
        if sym not in SYMS: continue
        v=SYMS[sym]
        out.append(txt[i:m.start()])
        if m.group('term')=='{':
            params=", ".join(v["params_after_self"])
            out.append(f'{v["ret"]} {v["class"]}::{v["method"]}({params}) {{\n    {v["class"]} *{v["self_name"]} = this;')
            report["defs"]+=1; i=m.end()
        else:                                    # prototype -> drop
            report["decls"]+=1; i=m.end()
            while i<len(txt) and txt[i] in ' \t': i+=1
            if i<len(txt) and txt[i]=='\n': i+=1
    out.append(txt[i:])
    return "".join(out)

def split_top(s):
    out=[]; depth=0; cur=""
    for ch in s:
        if ch in '([{': depth+=1
        elif ch in ')]}': depth-=1
        if ch==',' and depth==0: out.append(cur); cur=""
        else: cur+=ch
    if cur.strip() or out: out.append(cur)
    return out

CALL=re.compile(r'(?P<pre>.?)\b(?P<sym>[A-Za-z_]\w*?_[A-Za-z_]\w*)\s*\(')
def rewrite_calls(txt, report, called, owner=None):
    out=[]; i=0
    for m in CALL.finditer(txt):
        if m.start() < i: continue               # inside a region already rewritten this pass
        sym=m.group('sym')
        if sym not in SYMS: continue
        if m.group('pre')=='&':                  # address-of (fn pointer) -> leave for manual fix
            report["addrof"]+=1; continue
        j=m.end(); depth=1                        # find matching ')'
        while j<len(txt) and depth:
            c=txt[j]
            if c=='(': depth+=1
            elif c==')': depth-=1
            j+=1
        inner=txt[m.end():j-1]
        v=SYMS[sym]
        args=[a.strip() for a in split_top(inner)]
        if not args or not args[0]:               # 0-arg call (self lost)
            # in-class 0-arg, 0-param self-call -> this->method(); else leave for agent
            if owner and v["class"]==owner and not v["params_after_self"]:
                pl=1 if m.group('pre') else 0
                out.append(txt[i:m.start()+pl]); out.append(f'this->{v["method"]}()'); i=j
                report["calls"]+=1; continue
            out.append(txt[i:j]); i=j; report["noself"]+=1; continue
        arg0=args[0]
        # trust the definition's arity: keep exactly len(params_after_self) args after self, drop
        # extras the (body-truthful) signature doesn't use.
        n=len(v["params_after_self"])
        rest=", ".join(args[1:1+n])
        pre_len = 1 if m.group('pre') else 0
        out.append(txt[i:m.start()+pre_len])
        out.append(f'(({v["class"]} *)({arg0}))->{v["method"]}({rest})')
        called.add(v["class"]); report["calls"]+=1; i=j
    out.append(txt[i:])
    return "".join(out)

def add_includes(txt, called, own):
    need=[c for c in sorted(called) if c!=own and f'"gof2/{c}.h"' not in txt]
    if not need: return txt
    blk="".join(f'#include "gof2/{c}.h"\n' for c in need)
    m=re.search(r'^#include "gof2/[^"]+\.h"\n', txt, re.M)
    return (txt[:m.end()]+blk+txt[m.end():]) if m else blk+txt

def update_header(cls):
    hp=f"include/gof2/{cls}.h"
    if not os.path.exists(hp): return 0
    t=open(hp).read()
    # match the struct DEFINITION, not a forward decl: [^{;]* won't cross a ';' (`struct X;`)
    m=re.search(r'struct\s+'+re.escape(cls)+r'\b[^{;]*\{', t)
    if not m: return 0
    depth=0; i=m.end()-1
    while i<len(t):
        if t[i]=='{': depth+=1
        elif t[i]=='}':
            depth-=1
            if depth==0: break
        i+=1
    body=t[m.end()-1:i+1]                         # only the struct's { ... } region
    decls=[]
    for v in sorted(BY_CLASS[cls], key=lambda x:x["method"]):
        if re.search(r'\b%s\s*\('%re.escape(v["method"]), body): continue   # already a member decl
        decls.append(f'    {v["ret"]} {v["method"]}({", ".join(v["params_after_self"])});')
    if not decls: return 0
    ins="\n    // ---- methods (converted from free functions) ----\n"+"\n".join(decls)+"\n"
    open(hp,"w").write(t[:i]+ins+t[i:])
    return len(decls)

def main():
    rep=collections.Counter()
    for f in glob.glob("src/*.cpp"):
        own=os.path.basename(f)[:-4]
        txt=open(f,errors='ignore').read()
        txt=convert_defs_decls(txt, rep)
        called=set()
        for _ in range(8):
            r2=collections.Counter()
            txt=rewrite_calls(txt, r2, called, own)
            rep.update(r2)
            if r2['calls']==0: break
        txt=add_includes(txt, called, own)
        open(f,"w").write(txt)
    hdr=sum(update_header(c) for c in BY_CLASS)
    print(f"defs->methods: {rep['defs']}  protos removed: {rep['decls']}  calls rewritten: {rep['calls']}  "
          f"header decls: {hdr}  | left for fixup: &addr-of={rep['addrof']}  0-arg(no self)={rep['noself']}")

if __name__=="__main__": main()
