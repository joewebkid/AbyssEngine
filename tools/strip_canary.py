#!/usr/bin/env python3
# strip_canary.py — remove compiler-injected -fstack-protector artifacts the decompiler emitted as fake
# source. Order matters: (1) drop agent #define/#undef workarounds; (2) fix the rare case where a guard
# var was mis-used as a method receiver  ((Class*)(guard))->  ->  this->  (BEFORE we delete the guard
# line, so the real statement survives); (3) strip the check tails (all brace/no-brace forms), keeping the
# real return; (4) delete WHOLE LINES that mention __stack_chk_guard/__stack_chk_fail (setup reads + decls
# — line-based so it can't mangle); (5) delete orphaned intermediate canary-var copy lines.
import glob, re, os
ROOT="/Users/fionera/Downloads/GalaxyOnFire2/gof2-decomp"; os.chdir(ROOT)
P=r'\((?:[^()]|\([^()]*\))*\)'                       # parenthesised group, one nesting level
F=r'\w*stack_chk_fail'                               # __stack_chk_fail OR an agent-renamed variant
TAILS=[
    (re.compile(r'if\s*'+P+r'\s*\{\s*(return\b[^;{}]*;)\s*\}\s*'+F+r'\s*'+P+r'\s*;', re.S), r'\1'),
    (re.compile(r'if\s*'+P+r'\s*(return\b[^;{}]*;)\s*'+F+r'\s*'+P+r'\s*;', re.S), r'\1'),
    (re.compile(r'if\s*'+P+r'\s*\{\s*'+F+r'\s*'+P+r'\s*;\s*\}', re.S), ''),
    (re.compile(r'if\s*'+P+r'\s*'+F+r'\s*'+P+r'\s*;', re.S), ''),
    # standalone fail calls + decls are removed by the line-based pass below.
]
# names used to fix receiver mis-uses (broad — only affects ((Class*)(name)) casts, harmless elsewhere)
RECV_NAMES=r'(?:cookie|guard|canary|guardP|stackGuard|stackGuardPtr|guardCookie)'
# distinctive intermediate names to delete (NOT bare 'guard' — it's a common real var; allow numbered)
INTER_NAMES=r'(?:cookie|canary|saved\d*|savedGuard|stackGuard\w*|stackGuardPtr|stackCanary|guardP\w*|guardPtr|guardCopy|guardVal|guardCookie|guardDelta\w*|stackDifference\w*|delta\d+|current\d+)'

for f in glob.glob("src/*.cpp"):
    t=open(f,errors='ignore').read(); o=t
    t=re.sub(r'^[ \t]*#\s*(?:define|undef)\s+\w*stack_chk_guard\b[^\n]*\n', '', t, flags=re.M)
    # join only canary continuation lines (a `... =` line whose continuation mentions a canary symbol)
    t=re.sub(r'=[ \t]*\n([ \t]*[^\n]*stack_chk_guard[^\n]*)', r'= \1', t)
    # receiver mis-use ((Class*)(guard))-> -> this->  (before deleting the guard line)
    for v in set(re.findall(RECV_NAMES, t)):
        t=re.sub(r'\(\(\s*[A-Za-z_]\w*\s*\*\s*\)\s*\(\s*'+re.escape(v)+r'\s*\)\s*\)', 'this', t)
    for rx,rep in TAILS: t=rx.sub(rep, t)
    # capture the vars seeded from the guard read, then delete the canary-symbol lines
    removed=set(re.findall(r'\b(\w+)\s*=\s*[^\n;]*stack_chk_guard', t))
    t="".join(L for L in t.splitlines(keepends=True) if 'stack_chk_guard' not in L and 'stack_chk_fail' not in L)
    # iterative dead-chain removal: delete `<type> v = <rhs>;` where rhs flows from a removed canary var
    # (skip lines with -> : those are real method calls, e.g. a result computed via a guard-mis-use receiver)
    VD=re.compile(r'^[ \t]*(?:register\s+|volatile\s+|const\s+)*[A-Za-z_][\w:<> ]*[ *&]+(\w+)\s*=\s*([^\n;]*);[ \t]*\n', re.M)
    while True:
        hit=None
        for m in VD.finditer(t):
            rhs=m.group(2)
            if '->' in rhs or re.search(r'[A-Za-z_]\w*\s*\(', rhs): continue   # real method/function call -> keep
            if any(re.search(r'\b'+re.escape(v)+r'\b', rhs) for v in removed):
                hit=m; removed.add(m.group(1)); break
        if not hit: break
        t=t[:hit.start()]+t[hit.end():]
    if t!=o: open(f,'w').write(t)

# headers: drop the declarations too (line-based)
for h in glob.glob("include/gof2/*.h"):
    t=open(h,errors='ignore').read()
    t2="".join(L for L in t.splitlines(keepends=True) if 'stack_chk_guard' not in L and 'stack_chk_fail' not in L)
    if t2!=t: open(h,'w').write(t2)
rem=sum(len(re.findall(r'__stack_chk_(?:guard|fail)', open(f,errors='ignore').read())) for f in glob.glob('src/*.cpp')+glob.glob('include/gof2/*.h'))
print(f"stack-canary strip done; remaining __stack_chk guard/fail refs: {rem}")
