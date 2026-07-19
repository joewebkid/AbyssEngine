#!/usr/bin/env python3
"""scandrift.py -- auto-scan ALL classes for clean single-delta layout drift (the easy byte-exact wins).

For every class (mangled prefix) with >=4 exported size-matched methods, builds the our_off->orig_off
consensus and reports classes whose drift collapses to ONE dominant delta -- i.e. a single missing
field (positive delta) or a single extra region (negative delta), fixable by one Ghidra-guided edit.
Pair with `get_struct_layout <Class>` (Ghidra) to place the field, then verify byte_exact must rise.
"""
import sys,subprocess,tempfile,os,re
from collections import Counter,defaultdict
sys.path.insert(0,os.path.join(os.path.dirname(__file__),'verify')); from elf import Elf32
OBJ=subprocess.run(['bash','-lc',"grep -m1 GOF2_NDK_OBJDUMP cmake-build-match/CMakeCache.txt|cut -d= -f2"],capture_output=True,text=True).stdout.strip()
eo=Elf32('../_work/bins/android_2.0.16_libgof2hdaa.so'); eu=Elf32('cmake-build-match/libgof2hdaa.so')
ours=eu.functions(); orig=eo.functions()
prefs=Counter()
for n in ours:
    m=re.match(r'(_ZN(?:\d+[A-Za-z_]\w*?)+)\d+[A-Za-z]\w*E',n)
    if m: prefs[m.group(1)]+=1
def dis(e,fn):
    f=e.functions()[fn]; b=e.bytes_at(f['value'],f['size'])
    t=tempfile.NamedTemporaryFile(suffix='.bin',delete=False);t.write(b);t.close()
    out=subprocess.run([OBJ,'-D','-b','binary','-m','arm','-M','force-thumb',t.name],capture_output=True,text=True).stdout;os.unlink(t.name)
    return [l.split('\t',1)[-1].strip() for l in out.splitlines() if ':\t' in l]
LD=re.compile(r'\b(ldr|str|ldrb|strb|ldrh|strh)(?:\.w)?\b[^[]*\[r\d+,\s*#(\d+)\]')
clean=[]
for pre,cnt in prefs.items():
    if cnt<4: continue
    fns=[n for n in ours if n.startswith(pre) and n in orig][:14]
    votes=defaultdict(Counter); al=0
    for fn in fns:
        o=dis(eo,fn); u=dis(eu,fn)
        if not o or len(o)!=len(u): continue
        al+=1
        for io,iu in zip(o,u):
            mo=LD.search(io.split(';')[0]); mu=LD.search(iu.split(';')[0])
            if mo and mu and mo.group(1)==mu.group(1): votes[int(mu.group(2))][int(mo.group(2))]+=1
    if al<2: continue
    deltas=Counter()
    for uo in votes:
        oo,c=votes[uo].most_common(1)[0]
        if uo!=oo and c>=2: deltas[oo-uo]+=c
    if deltas and len(deltas)==1:
        d,c=deltas.most_common(1)[0]; clean.append((c,pre,d))
clean.sort(reverse=True)
print("CLEAN single-delta classes (votes, delta orig-our, prefix):")
for c,pre,d in clean: print(f"  {c:3}  {d:+#6x}  {pre}")
