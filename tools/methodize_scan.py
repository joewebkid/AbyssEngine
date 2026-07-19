#!/usr/bin/env python3
# methodize_scan.py — find free functions `RetType Class_method(Class *self, ...)` that should become
# C++ instance methods, build the conversion registry, and report scope. Read-only (no edits).
import glob, re, os, json, collections
ROOT="/Users/fionera/Downloads/GalaxyOnFire2/gof2-decomp"; os.chdir(ROOT)
CLASSES={os.path.basename(f)[:-4] for f in glob.glob("src/**/*.cpp", recursive=True)}
CLASSES|={os.path.basename(f)[:-2] for f in glob.glob("src/**/*.h", recursive=True)}

# A definition signature (single line up to the param close-paren), optionally `extern "C"`.
# Groups: ret (return type incl. trailing * / spaces), sym (Class_method), params, term ({ or ; possibly on next line)
SIG=re.compile(r'^[ \t]*(?:extern\s+"C"\s+)?'
               r'(?P<ret>[A-Za-z_][\w:\s]*?[\w>])\s*(?P<stars>[\*&]*)\s*'
               r'(?P<sym>[A-Za-z_]\w*?_[A-Za-z_]\w*)\s*'
               r'\((?P<params>[^;{]*?)\)\s*(?P<term>[{;])', re.M)

def split_top(params):
    out=[]; depth=0; cur=""
    for ch in params:
        if ch in "([<": depth+=1
        elif ch in ")]>": depth-=1
        if ch=="," and depth==0: out.append(cur); cur=""
        else: cur+=ch
    if cur.strip(): out.append(cur)
    return [p.strip() for p in out]

registry={}             # sym -> {class, method, ret, params_after_self, self_name, file}
defs_seen=collections.Counter()
edge_first_not_class=0; edge_static=0
multiline_flag=0

for f in glob.glob("src/**/*.cpp", recursive=True):
    txt=open(f,errors='ignore').read()
    for m in SIG.finditer(txt):
        sym=m.group('sym'); ret=(m.group('ret')+' '+m.group('stars')).strip()
        if m.group('term')!='{':   # only DEFINITIONS create methods (decls handled in transform)
            continue
        # split Class_method on the FIRST underscore-delimited class that is a known class (longest prefix)
        cls=None
        parts=sym.split('_')
        for i in range(len(parts)-1,0,-1):
            cand='_'.join(parts[:i])
            if cand in CLASSES: cls=cand; method='_'.join(parts[i:]); break
        if not cls: continue
        params=split_top(m.group('params'))
        defs_seen[sym]+=1
        # instance method requires first param to be `Class * name`
        if params and re.match(r'(const\s+)?%s\s*\*\s*\w+$'%re.escape(cls), params[0]):
            self_name=params[0].split('*')[-1].strip()
            registry[sym]={"class":cls,"method":method,"ret":ret,
                           "params_after_self":params[1:],"self_name":self_name,"file":os.path.basename(f)}
        else:
            edge_first_not_class+=1

# call-site counts: occurrences of `sym(` minus its definition(s)
calltxt="".join(open(f,errors='ignore').read() for f in glob.glob("src/**/*.cpp", recursive=True))
callcounts={}
for sym in registry:
    callcounts[sym]=len(re.findall(r'\b%s\s*\('%re.escape(sym), calltxt))
addr_of=sum(len(re.findall(r'&\s*%s\b'%re.escape(sym), calltxt)) for sym in registry)

byclass=collections.Counter(v["class"] for v in registry.values())
json.dump(registry, open("tools/method_registry.json","w"), indent=0)
print(f"instance-method-convertible functions: {len(registry)}  across {len(byclass)} classes")
print(f"total call sites (incl defs):           {sum(callcounts.values())}")
print(f"address-of (&Class_method) usages:      {addr_of}  (function-pointer edge cases)")
print(f"defs whose 1st param is NOT the class:  {edge_first_not_class}  (static-like / skipped)")
print("top classes by method count:", dict(byclass.most_common(10)))
print("sample registry entries:")
for s in list(registry)[:5]:
    r=registry[s]; print(f"  {s}  ->  {r['ret']} {r['class']}::{r['method']}({', '.join(r['params_after_self'])})")
