#!/usr/bin/env python3
"""structdiff.py <mangled-prefix> -- consensus struct layout drift detector.

Disassembles every size-matched function whose mangled name starts with <prefix> (a class's own
methods) in ours + the original, aligns 1:1, and votes our_off->orig_off for load/store immediates.
A consistent non-identity delta = struct layout drift vs the original. Pair with Ghidra
get_struct_layout to reconstruct, then verify byte_exact must rise. e.g.:
  tools/structdiff.py _ZN11AbyssEngine18ApplicationManager
"""
import sys,subprocess,tempfile,os,re
from collections import Counter,defaultdict
sys.path.insert(0,'tools/verify'); from elf import Elf32
OBJ=subprocess.run(['bash','-lc',"grep -m1 GOF2_NDK_OBJDUMP cmake-build-match/CMakeCache.txt|cut -d= -f2"],capture_output=True,text=True).stdout.strip()
eo=Elf32('../_work/bins/android_2.0.16_libgof2hdaa.so'); eu=Elf32('cmake-build-match/libgof2hdaa.so')
ours=eu.functions(); orig=eo.functions()
pre=sys.argv[1]  # mangled prefix
pc=[n for n in ours if n.startswith(pre) and n in orig]
def dis(e,fn):
    f=e.functions()[fn]; b=e.bytes_at(f['value'],f['size'])
    t=tempfile.NamedTemporaryFile(suffix='.bin',delete=False);t.write(b);t.close()
    out=subprocess.run([OBJ,'-D','-b','binary','-m','arm','-M','force-thumb',t.name],capture_output=True,text=True).stdout;os.unlink(t.name)
    return [l.split('\t',1)[-1].strip() for l in out.splitlines() if ':\t' in l]
LD=re.compile(r'\b(ldr|str|ldrb|strb|ldrh|strh)(?:\.w)?\b[^[]*\[r\d+,\s*#(\d+)\]')
votes=defaultdict(Counter); aligned=0
for fn in pc:
    o=dis(eo,fn); u=dis(eu,fn)
    if len(o)!=len(u): continue
    aligned+=1
    for io,iu in zip(o,u):
        mo=LD.search(io.split(';')[0]); mu=LD.search(iu.split(';')[0])
        if mo and mu and mo.group(1)==mu.group(1): votes[int(mu.group(2))][int(mo.group(2))]+=1
print(f"{pre}: aligned {aligned}/{len(pc)}; drifted our->orig:")
dd=0
for uo in sorted(votes):
    oo,c=votes[uo].most_common(1)[0]; tot=sum(votes[uo].values())
    if uo!=oo and c>=tot/2: print(f"  our 0x{uo:<4x} -> orig 0x{oo:<4x} ({c}/{tot})"); dd+=1
if not dd: print("  (no consistent drift)")
