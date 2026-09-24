#!/usr/bin/env python3
"""Disassemble two ARM objects with arm-linux-gnueabihf-objdump and compare them
function-by-function. Used by verify.py.

objdiff cannot read this binary (its ARM disassembler caps at ARMv6K; the game is
ARMv7-A Thumb-2 + VFP/NEON), so we disassemble with GNU objdump — which fully
supports ARMv7 — and do our own normalize + fuzzy match.

For each symbol present in BOTH objects we report:
  * bytes_equal  : raw instruction bytes identical (the gold byte-match signal)
  * match        : fuzzy instruction match 0..100 (difflib ratio over normalized
                   mnemonics+operands; tolerant of post-link absolute branch/pool
                   addresses that have no relocation in the delinked target)
  * source_match : allocation-insensitive source-shape evidence; separately keeps
                   instruction/opcode inventories, control flow, call count and size
"""
import difflib
import os
import re
import subprocess
from collections import Counter

HERE = os.path.dirname(os.path.abspath(__file__))
DEFAULT_OBJDUMP = os.path.join(HERE, "orbobjdump")

if os.name == "nt":
    os.environ["PATH"] = r"C:\msys64\usr\bin;" + os.environ.get("PATH", "")

_COND = r"(?:eq|ne|cs|hs|cc|lo|mi|pl|vs|vc|hi|ls|ge|lt|gt|le|al)"
_BRANCH = re.compile(
    rf"^(?:(?:blx?|bx){_COND}?|b{_COND}?|cbz|cbnz)(\.[wn])?$")
_CALL = re.compile(rf"^blx?{_COND}?$")
_INDIRECT_BRANCH = re.compile(rf"^(?:blx|bx){_COND}?$")
_HEXNUM = re.compile(r"0x[0-9a-fA-F]+")
_ANGLE = re.compile(r"\s*<[^>]*>")          # objdump symbol annotation
_PCREL = re.compile(r"\[pc,\s*#-?\d+\]")    # literal-pool load offset
_PCREL_REFERENCE = re.compile(r"\[pc,\s*#(-?\d+)\]", re.I)
_REG = re.compile(r"^(r\d+|sp|lr|pc|fp|ip|sl|sb)$", re.I)  # ARM register operand
_RELOC = re.compile(r"^\s*([0-9a-f]+):\s+R_ARM_\S+")       # objdump -r inline reloc
_RELOC_WIDTH = 4   # all R_ARM relocs in these objects (THM_CALL/JUMP24/GOT_PREL/REL32) are 4-byte fields
_SHAPE_GPR = re.compile(
    r"(?<![A-Za-z0-9_])(r(?:[0-9]|1[0-2])|fp|ip|sl|sb)(?![A-Za-z0-9_])", re.I)
_SHAPE_VFP = re.compile(
    r"(?<![A-Za-z0-9_])([sdq](?:[0-9]|[12][0-9]|3[01]))(?![A-Za-z0-9_])", re.I)
_ENCODING_WIDTH = re.compile(r"^([a-z][a-z0-9]*)(?:\.[wn])(?=\s|$)", re.I)
_VECTOR_STRUCTURE_WIDTH = re.compile(r"\b(v(?:ld|st)1)\.(?:8|16|32|64)\b", re.I)
_SAVED_REGISTER_LIST = re.compile(
    r"^((?:v?push|v?pop)(?:\.[wn])?)(\s+)(\{[^}]+\})$", re.I)
_STACK_SAVED_REGISTER_LIST = re.compile(
    r"^((?:stmdb|ldmia)(?:\.[wn])?)(\s+sp!,\s*)(\{[^}]+\})$", re.I)
_STACK_SLOT = re.compile(r"\[(?:sp|fp),\s*#-?\d+\]", re.I)
_STACK_ADDRESS = re.compile(
    r"\b((?:add|sub)(?:\.[wn])?)\s+"
    r"(r(?:[0-9]|1[0-2])|fp|ip|sl|sb),\s*(sp|fp),\s*#-?\d+\b",
    re.I)
_STACK_ADJUST = re.compile(
    r"\b((?:add|sub)(?:eq|ne|cs|hs|cc|lo|mi|pl|vs|vc|hi|ls|ge|lt|gt|le|al)?"
    r"(?:\.[wn])?)\s+sp,\s*#-?\d+\b",
    re.I)
_CONTROL_OPCODE = re.compile(
    r"^(?:call|b|blx|bx|cbz|cbnz|beq|bne|bcs|bhs|bcc|blo|bmi|bpl|bvs|bvc|"
    r"bhi|bls|bge|blt|bgt|ble|bal|tbb|tbh|jump-table)$")
_ZERO_COMPARE = re.compile(
    r"^cmp(?:\.[wn])?\s+(?:r(?:[0-9]|1[0-2])|fp|ip|sl|sb),\s*#0$", re.I)
_SINGLE_WORD_MEMORY = re.compile(
    r"^(ldr|str)(?:\.[wn])?\s+"
    r"(r(?:[0-9]|1[0-2])|fp|ip|sl|sb),\s*"
    r"\[(r(?:[0-9]|1[0-2])|fp|ip|sl|sb|sp),\s*#(-?\d+)\]$", re.I)


def _normalize_jump_tables(insns, widths):
    """Replace inline TBB/TBH payloads with one control-flow token.

    The original functions are delinked from a stripped shared object. Their ARM
    ``$d`` mapping symbols are gone, so objdump decodes jump-table bytes as Thumb
    instructions. The locally compiled object still has mapping symbols and prints
    the same payload as ``.byte/.short/.word`` directives. Comparing those two
    renderings as instructions produces a large false mismatch.

    The bounds check immediately preceding a compiler-generated TBB/TBH gives the
    table length. Consume exactly that many payload bytes (including TBB's alignment
    byte) and keep the case count in the replacement token. If the byte boundary
    cannot be proven exactly, leave the stream untouched.
    """
    out_insns, out_widths = [], []
    i = 0
    while i < len(insns):
        token = insns[i]
        out_insns.append(token)
        out_widths.append(widths[i])
        table_kind = token.split(" ", 1)[0]
        if table_kind not in ("tbb", "tbh"):
            i += 1
            continue

        index_match = re.search(r"\[pc,\s*(r\d+)", token, re.I)
        if not index_match:
            i += 1
            continue
        index_reg = index_match.group(1).lower()

        case_count = None
        for prior in reversed(out_insns[:-1][-12:]):
            cmp_match = re.match(r"cmp(?:\.w)?\s+(r\d+),\s*#(\d+)$", prior, re.I)
            if cmp_match and cmp_match.group(1).lower() == index_reg:
                case_count = int(cmp_match.group(2)) + 1
                break
        if case_count is None or case_count <= 0:
            i += 1
            continue

        payload_bytes = case_count * (2 if table_kind == "tbh" else 1)
        if table_kind == "tbb":
            payload_bytes = (payload_bytes + 1) & ~1

        consumed = 0
        j = i + 1
        while j < len(insns) and consumed < payload_bytes:
            consumed += widths[j]
            j += 1
        if consumed != payload_bytes:
            i += 1
            continue

        out_insns.append(f"jump-table {table_kind} {case_count}")
        out_widths.append(payload_bytes)
        i = j
    return out_insns, out_widths


def _normalize_literal_pools(entries):
    """Render proven PC-relative literal words as data on stripped targets.

    Local compiler objects retain ARM ``$d`` mapping symbols, so objdump prints
    literal pools as ``.word`` directives. ``delink.py`` starts from a stripped
    shared object where those local symbols no longer exist; objdump consequently
    decodes the same bytes as Thumb instructions. Every range normalized here is
    proven by an in-function PC-relative ``ldr``/``vldr``/``ldrd`` reference and
    exact instruction boundaries. Ambiguous or partially covered ranges are left
    untouched.
    """
    if not entries:
        return [], []

    function_start = entries[0]["address"]
    function_end = entries[-1]["address"] + entries[-1]["width"]
    referenced_words = set()
    for entry in entries:
        base = entry["mnemonic"].split(".", 1)[0].lower()
        if base not in ("ldr", "ldrd", "vldr"):
            continue
        match = _PCREL_REFERENCE.search(entry["operands"])
        if not match:
            continue
        target = ((entry["address"] + 4) & ~3) + int(match.group(1))
        width = 8 if base == "ldrd" or re.match(r"\s*d\d+\s*,", entry["operands"], re.I) else 4
        if target < function_start or target + width > function_end or target % 4 != 0:
            continue
        for address in range(target, target + width, 4):
            referenced_words.add(address)

    if not referenced_words:
        return ([entry["token"] for entry in entries],
                [entry["width"] for entry in entries])

    ranges = []
    for address in sorted(referenced_words):
        if ranges and address == ranges[-1][1]:
            ranges[-1] = (ranges[-1][0], address + 4)
        else:
            ranges.append((address, address + 4))

    entry_starts = {entry["address"]: index for index, entry in enumerate(entries)}
    proven = {}
    for start, end in ranges:
        index = entry_starts.get(start)
        if index is None:
            continue
        cursor = start
        scan = index
        while scan < len(entries) and cursor < end:
            entry = entries[scan]
            if entry["address"] != cursor:
                break
            cursor += entry["width"]
            scan += 1
        if cursor == end:
            proven[start] = (end, scan)

    out_insns, out_widths = [], []
    index = 0
    while index < len(entries):
        start = entries[index]["address"]
        pool = proven.get(start)
        if pool is None:
            out_insns.append(entries[index]["token"])
            out_widths.append(entries[index]["width"])
            index += 1
            continue
        end, next_index = pool
        for _ in range((end - start) // 4):
            out_insns.append(".word #x")
            out_widths.append(4)
        index = next_index
    return out_insns, out_widths


def _normalize_pool_alignment(insns, widths):
    """Drop code-alignment NOPs immediately before a proven data directive.

    Their presence depends only on the final code size before a four-byte literal
    pool. They remain part of raw/linked byte comparison, but are not behavior and
    must not lower the fuzzy source-shape score.
    """
    out_insns, out_widths = [], []
    for index, token in enumerate(insns):
        if (token == "nop" and index + 1 < len(insns) and
                insns[index + 1].split(" ", 1)[0] in (".word", ".short", ".byte")):
            continue
        out_insns.append(token)
        out_widths.append(widths[index])
    return out_insns, out_widths


def _msys_path(path):
    path = os.path.abspath(path).replace("\\", "/")
    m = re.match(r"^([A-Za-z]):/(.*)$", path)
    if m:
        return f"/{m.group(1).lower()}/{m.group(2)}"
    return path


def tool_argv(tool, args):
    if os.name == "nt" and not tool.lower().endswith(".exe"):
        bash = os.environ.get("GOF2_BASH", r"C:\msys64\usr\bin\bash.exe")
        return [bash, _msys_path(tool), *args]
    return [tool, *args]


def normalize(mnem, ops):
    """Reduce one instruction to a comparable token, neutralizing addresses that
    differ only because the target is post-link (no relocations)."""
    ops = ops.split(";")[0].split("@")[0].strip()      # drop comments
    ops = _ANGLE.sub("", ops)                          # drop <sym+0x..> notes
    ops = re.sub(r"\s+", " ", ops).strip()
    base = mnem.split(".")[0]
    if _BRANCH.match(mnem):
        # A direct bl/blx to a *label* is link-time-interchangeable: our unlinked .o
        # emits `bl`+relocation, the delinked target shows the linker-resolved `blx`
        # interworking veneer. That is not a codegen difference, so collapse both to a
        # single `call` token. A register-indirect blx/bx (blx r3, bx lr) IS a real
        # codegen choice -- keep its register. Other branches: target -> placeholder.
        if _CALL.match(base) and not _REG.match(ops):
            return "call <t>"
        if _INDIRECT_BRANCH.match(base) and _REG.match(ops):
            return f"{base} {ops}"
        return base + " <t>"                           # branch/call target -> placeholder
    # pointer materialization & pool loads carry absolute values post-link
    if base in ("movw", "movt"):
        ops = re.sub(r"#\S+", "#i", ops)
    ops = _PCREL.sub("[pc]", ops)
    ops = _HEXNUM.sub("#x", ops)
    return f"{mnem} {ops}".strip()


def _symbol_sizes(lines):
    """Parse `objdump -t` symbol table -> {name: size_bytes}."""
    sizes, in_tab = {}, False
    for line in lines:
        if line.startswith("SYMBOL TABLE:"):
            in_tab = True
            continue
        if in_tab:
            if not line.strip() or line.startswith("Disassembly"):
                in_tab = False
                continue
            # "0000000a g     F .text\t0000001c _ZN..."
            if "\t" not in line:
                continue
            right = line.split("\t", 1)[1]
            parts = right.split(None, 1)
            if len(parts) == 2:
                try:
                    sizes[parts[1].strip()] = int(parts[0], 16)
                except ValueError:
                    pass
    return sizes


DISASM_TIMEOUT = 90   # seconds; objdump normally finishes in <2s — a longer run is a hang
                      # (a malformed object or a wedged tool call) and must not freeze the run.


def disassemble(obj, objdump=DEFAULT_OBJDUMP):
    """{mangled symbol -> {'insns':[normalized...], 'bytes':'hex'}}.

    Each function is truncated to its real symbol size so trailing alignment
    padding (attributed by objdump to the preceding symbol) never pollutes the
    comparison.

    'reloc' is the set of function-relative byte offsets covered by a relocation
    in this (unlinked) object — the fields whose final value the linker fills in.
    They are wildcarded by linked_equal() so an external call / global load doesn't
    read as a byte mismatch against the post-link target."""
    # stderr is captured (not inherited) so a failing object's objdump message ("No such file",
    # bad format, ...) doesn't interleave into the report table; the failure is reported as a
    # concise per-unit skip instead.
    out = subprocess.check_output(tool_argv(objdump, ["-d", "-t", "-r", obj]), text=True,
                                  timeout=DISASM_TIMEOUT, stderr=subprocess.DEVNULL)
    lines = out.splitlines()
    sizes = _symbol_sizes(lines)
    funcs, cur, start, limit = {}, None, 0, None
    addr2label = {}   # text address -> the one name objdump put on the `<name>:` header
    for line in lines:
        m = re.match(r"^([0-9a-f]+) <(.+)>:$", line)
        if m:
            start = int(m.group(1), 16)
            sz = sizes.get(m.group(2))
            limit = start + sz if sz else None
            cur = {"insns": [], "widths": [], "entries": [], "bytes": "", "reloc": set()}
            funcs[m.group(2)] = cur
            addr2label[start] = m.group(2)
            continue
        if cur is None:
            continue
        # "   a:\t68 0a       \tldr\tr2, [r1, #0]"   (objdump -d, raw bytes shown)
        # A 32-bit literal whose first halfword is invalid has no mnemonic at all:
        # " 386c:\tffff 77cc \t\t\t; <UNDEFINED> instruction: ...". Keep those
        # bytes in the entry stream so the PC-reference pass can prove they are data.
        m = re.match(r"^\s*([0-9a-f]+):\t([0-9a-f ]+)\t(.*)$", line)
        if not m:
            # "\t\t\t22: R_ARM_THM_JUMP24\t_ZN..." -> mask this 4-byte field
            rm = _RELOC.match(line)
            if rm:
                off = int(rm.group(1), 16) - start
                if off >= 0 and (limit is None or start + off < limit):
                    cur["reloc"].update(range(off, off + _RELOC_WIDTH))
            continue
        addr = int(m.group(1), 16)
        if limit is not None and addr >= limit:
            continue
        raw = m.group(2)
        tail = m.group(3).strip()
        if not tail or tail.startswith(";"):
            mnem, ops = ".raw", ""
        else:
            instruction = tail.split(None, 1)
            mnem = instruction[0]
            ops = instruction[1] if len(instruction) > 1 else ""
        width = len(raw.replace(" ", "")) // 2
        if mnem in (".word", ".short", ".byte", ".raw"):
            token = mnem + " " + _HEXNUM.sub("#x", ops).strip()
        else:
            token = normalize(mnem, ops)
        cur["insns"].append(token)
        cur["widths"].append(width)
        cur["entries"].append({
            "address": addr,
            "width": width,
            "mnemonic": mnem,
            "operands": ops.split(";")[0].split("@")[0].strip(),
            "token": token,
        })
        cur["bytes"] += raw.replace(" ", "")

    for func in funcs.values():
        func["insns"], func["widths"] = _normalize_literal_pools(func["entries"])
        func["insns"], func["widths"] = _normalize_pool_alignment(
            func["insns"], func["widths"])
        func["insns"], func["widths"] = _normalize_jump_tables(
            func["insns"], func["widths"])

    # Credit aliased function symbols. objdump's `<name>:` disassembly header carries only ONE name
    # per address, but the compiler aliases C1/C2 (complete/base ctor) and D1/D2 (complete/base dtor)
    # — and, with -Oz, sometimes emits only one of a pair — so a sibling that shares the address is
    # byte-identical but invisible. Parse the `-t` symbol table (in the same objdump output) for all
    # function names at each text address and point every sibling at the labelled entry, so a match
    # under one variant name credits all of them.
    for line in lines:
        m = re.match(r"^([0-9a-f]+)\s.{6,}\bF\s+\.text\s+[0-9a-f]+\s+(\S+)\s*$", line)
        if not m:
            continue
        addr, name = int(m.group(1), 16), m.group(2)
        label = addr2label.get(addr)
        if label and name not in funcs and label in funcs:
            funcs[name] = funcs[label]
    return funcs


def linked_equal(t, b):
    """True if the raw bytes match except inside relocation-covered fields, which
    legitimately differ: our unlinked .o (b) carries placeholder operands where the
    delinked, post-link target (t) has the linker-resolved absolute value. Equivalent
    to byte-identical machine code *after linking* — the real goal — and unlike a raw
    byte compare it doesn't false-negative on every function that calls out or loads a
    global. Relocation sites come from the base side (the post-link target has none)."""
    tb, bb, mask = t["bytes"], b["bytes"], b.get("reloc") or set()
    if len(tb) != len(bb):
        return False
    for i in range(len(bb) // 2):            # byte index; hex chars [2i:2i+2]
        if i not in mask and tb[2 * i:2 * i + 2] != bb[2 * i:2 * i + 2]:
            return False
    return True


def _multiset_ratio(left, right):
    """Dice similarity for duplicate-preserving instruction inventories."""
    if not left and not right:
        return 1.0
    common = sum((Counter(left) & Counter(right)).values())
    return 2.0 * common / (len(left) + len(right))


def _opcode(token):
    return token.split(" ", 1)[0]


def _allocation_shape(token):
    """Instruction shape independent of compiler register/stack-slot allocation.

    Immediate values and object-field offsets stay intact. General-purpose and
    VFP/NEON registers are renamed by first appearance inside each instruction,
    preserving repeated-register aliasing (``add r4,r4,r5`` ->
    ``add g0,g0,g1``). The exact numeric offset of a local stack slot is
    allocator noise and becomes ``#slot``. Mnemonics, branch kinds, argument
    order and memory displacements stay intact.
    """
    token = _ENCODING_WIDTH.sub(r"\1", token)
    token = _STACK_SLOT.sub("[frame, #slot]", token)
    token = _STACK_ADDRESS.sub(r"\1 \2, frame, #slot", token)
    token = _STACK_ADJUST.sub(r"\1 sp, #frame", token)
    token = _VECTOR_STRUCTURE_WIDTH.sub(r"\1.#esize", token)

    def replace_saved_registers(match):
        register_list = match.group(3).lower()
        specials = [register for register in ("lr", "pc")
                    if re.search(rf"\b{register}\b", register_list)]
        suffix = "".join(f", {register}" for register in specials)
        return f"{match.group(1)}{match.group(2)}{{saved{suffix}}}"

    token = _SAVED_REGISTER_LIST.sub(replace_saved_registers, token)
    token = _STACK_SAVED_REGISTER_LIST.sub(replace_saved_registers, token)
    roles = {}
    vfp_roles = {}

    def replace(match):
        register = match.group(1).lower()
        if register not in roles:
            roles[register] = f"g{len(roles)}"
        return roles[register]

    def replace_vfp(match):
        register = match.group(1).lower()
        register_class = register[0]
        if register not in vfp_roles:
            vfp_roles[register] = f"{register_class}v{len(vfp_roles)}"
        return vfp_roles[register]

    token = _SHAPE_GPR.sub(replace, token)
    return _SHAPE_VFP.sub(replace_vfp, token)


def _normalize_zero_compare_branches(insns):
    """Collapse Thumb's long-range zero-test expansion to its CBZ/CBNZ shape."""
    normalized = []
    index = 0
    while index < len(insns):
        if index + 1 < len(insns) and _ZERO_COMPARE.match(insns[index]):
            branch = _opcode(insns[index + 1])
            if branch in ("beq", "bne"):
                normalized.append("cbz <t>" if branch == "beq" else "cbnz <t>")
                index += 2
                continue
        normalized.append(insns[index])
        index += 1
    return normalized


def _normalize_dualword_memory(insns):
    """Equate LDRD/STRD with two adjacent compiler-selected word accesses."""
    normalized = []
    index = 0
    while index < len(insns):
        if index + 1 < len(insns):
            first = _SINGLE_WORD_MEMORY.match(insns[index])
            second = _SINGLE_WORD_MEMORY.match(insns[index + 1])
            if (first and second and first.group(1).lower() == second.group(1).lower() and
                    first.group(3).lower() == second.group(3).lower() and
                    int(second.group(4)) == int(first.group(4)) + 4):
                mnemonic = first.group(1).lower() + "d"
                normalized.append(
                    f"{mnemonic} {first.group(2)}, {second.group(2)}, "
                    f"[{first.group(3)}, #{first.group(4)}]"
                )
                index += 2
                continue
        normalized.append(insns[index])
        index += 1
    return normalized


def source_shape_metrics(target_insns, base_insns):
    """Return compiler-allocation-insensitive source-shape evidence.

    This does not replace strict sequence, linked or byte comparison. It combines
    five equally weighted and independently reported signals: allocation-normalized
    instruction inventory, opcode inventory, control-flow shape, call-count agreement,
    and instruction-count agreement. The control-flow signal accepts either the ordered
    branch sequence or its duplicate-preserving inventory. This keeps source evidence
    stable when the compiler moves cold/basic blocks without changing their branches.
    Register allocation, local stack-slot numbering and non-control instruction
    scheduling do not lower this metric; changed calls, branches, operation counts,
    constants and object offsets do.
    """
    target_insns = _normalize_zero_compare_branches(
        _normalize_dualword_memory(target_insns))
    base_insns = _normalize_zero_compare_branches(
        _normalize_dualword_memory(base_insns))
    target_shapes = [_allocation_shape(token) for token in target_insns]
    base_shapes = [_allocation_shape(token) for token in base_insns]
    target_opcodes = [_opcode(_ENCODING_WIDTH.sub(
        r"\1", _VECTOR_STRUCTURE_WIDTH.sub(r"\1.#esize", token)))
                      for token in target_insns]
    base_opcodes = [_opcode(_ENCODING_WIDTH.sub(
        r"\1", _VECTOR_STRUCTURE_WIDTH.sub(r"\1.#esize", token)))
                    for token in base_insns]
    target_control = [op for op in target_opcodes if _CONTROL_OPCODE.match(op)]
    base_control = [op for op in base_opcodes if _CONTROL_OPCODE.match(op)]

    instruction_inventory = _multiset_ratio(target_shapes, base_shapes)
    opcode_inventory = _multiset_ratio(target_opcodes, base_opcodes)
    ordered_control_flow = difflib.SequenceMatcher(
        None, target_control, base_control, autojunk=False).ratio()
    control_inventory = _multiset_ratio(target_control, base_control)
    control_flow = max(ordered_control_flow, control_inventory)
    target_calls = target_control.count("call")
    base_calls = base_control.count("call")
    if target_calls or base_calls:
        call_count = min(target_calls, base_calls) / max(target_calls, base_calls)
    else:
        call_count = 1.0
    if target_insns or base_insns:
        size = min(len(target_insns), len(base_insns)) / max(len(target_insns), len(base_insns))
    else:
        size = 1.0
    source = (instruction_inventory + opcode_inventory + control_flow + call_count + size) / 5.0
    return {
        "source_match": round(source * 100, 1),
        "instruction_inventory_match": round(instruction_inventory * 100, 1),
        "opcode_inventory_match": round(opcode_inventory * 100, 1),
        "control_flow_match": round(control_flow * 100, 1),
        "ordered_control_flow_match": round(ordered_control_flow * 100, 1),
        "control_inventory_match": round(control_inventory * 100, 1),
        "call_count_match": round(call_count * 100, 1),
        "size_match": round(size * 100, 1),
    }


def compare(target_funcs, base_funcs):
    """List of per-symbol dicts for symbols defined in both, sorted worst-first."""
    rows = []
    for sym, b in base_funcs.items():
        t = target_funcs.get(sym)
        if t is None:
            continue
        # Assembly naturally repeats loads, moves, calls and branch tokens. The
        # SequenceMatcher default marks frequent elements as "autojunk" for streams
        # longer than 200 entries, which discards exactly the structural evidence we
        # need in large functions such as Hud::draw. Disable that text-oriented
        # heuristic; explicit data/pool normalization above handles real noise.
        ratio = difflib.SequenceMatcher(
            None, t["insns"], b["insns"], autojunk=False).ratio()
        row = {
            "symbol": sym,
            "match": round(ratio * 100, 1),
            "bytes_equal": t["bytes"] == b["bytes"],
            "linked_equal": linked_equal(t, b),
            "n_target": len(t["insns"]),
            "n_base": len(b["insns"]),
        }
        row.update(source_shape_metrics(t["insns"], b["insns"]))
        rows.append(row)
    rows.sort(key=lambda r: (r["match"], r["symbol"]))
    return rows


def unified(target_funcs, base_funcs, sym):
    """Human-readable side-by-side diff of one symbol's normalized disassembly."""
    t = target_funcs.get(sym, {}).get("insns", [])
    b = base_funcs.get(sym, {}).get("insns", [])
    diff = difflib.unified_diff(t, b, fromfile="ORIGINAL (.so)", tofile="OURS (build)",
                                lineterm="")
    return "\n".join(diff)
