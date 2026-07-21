#!/usr/bin/env python3
"""Extract the original Android ARM evidence for resource id 6801.

This intentionally reads the shipped libgof2hdaa.so rather than the recovered
BuildResourceList.cpp, whose body is too large for the checked-in decompiler
exports. It reports the instruction windows that construct ID 6801 and the
nearby ResourceMesh constructor calls.
"""

from __future__ import annotations

import argparse
import struct
from pathlib import Path

from capstone import CS_ARCH_ARM, CS_MODE_THUMB, Cs
from capstone.arm import ARM_OP_IMM


RESOURCE_ID = 6801
FUNCTION_START = 0x000E8F54
FUNCTION_END = 0x00112C88
RESOURCE_MESH_CTOR = 0x00112CF0
RESOURCE_MESH_THUNK = 0x00065E80
FX_MATERIAL_ID = 24201
TARGET_PATH = b"data/assets/main/3d/meshes/fx/projectile_025_anim_add.aem"


def load_segments(image: bytes) -> list[tuple[int, int, int]]:
    if image[:4] != b"\x7fELF" or image[4] != 1 or image[5] != 1:
        raise ValueError("expected a little-endian ELF32 image")

    phoff = struct.unpack_from("<I", image, 28)[0]
    phentsize = struct.unpack_from("<H", image, 42)[0]
    phnum = struct.unpack_from("<H", image, 44)[0]
    segments: list[tuple[int, int, int]] = []
    for index in range(phnum):
        offset = phoff + index * phentsize
        p_type, p_offset, p_vaddr, _, p_filesz = struct.unpack_from("<IIIII", image, offset)
        if p_type == 1:
            segments.append((p_vaddr, p_vaddr + p_filesz, p_offset))
    return segments


def va_to_file_offset(segments: list[tuple[int, int, int]], address: int) -> int:
    for start, end, file_offset in segments:
        if start <= address < end:
            return file_offset + address - start
    raise ValueError(f"address 0x{address:08x} is outside ELF load segments")


def file_offset_to_va(segments: list[tuple[int, int, int]], offset: int) -> int:
    for start, end, file_offset in segments:
        segment_size = end - start
        if file_offset <= offset < file_offset + segment_size:
            return start + offset - file_offset
    raise ValueError(f"file offset 0x{offset:x} is outside ELF load segments")


def instruction_target(instruction) -> int | None:
    if instruction.id == 0:
        return None
    if not instruction.operands:
        return None
    operand = instruction.operands[0]
    return operand.imm if operand.type == ARM_OP_IMM else None


def render_window(instructions, center: int, labels: dict[int, str]) -> list[str]:
    first = max(0, center - 18)
    last = min(len(instructions), center + 19)
    lines: list[str] = []
    for index in range(first, last):
        instruction = instructions[index]
        marker = ">>>" if index == center else "   "
        target = instruction_target(instruction)
        suffix = f" ; {labels[target]}" if target in labels else ""
        lines.append(
            f"{marker} 0x{instruction.address:08x}: {instruction.bytes.hex():<10} "
            f"{instruction.mnemonic:<7} {instruction.op_str}{suffix}"
        )
    return lines


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("binary", type=Path, help="original libgof2hdaa.so")
    args = parser.parse_args()

    image = args.binary.read_bytes()
    segments = load_segments(image)
    string_offset = image.find(TARGET_PATH)
    if string_offset < 0:
        raise ValueError("projectile_025 path is absent from the supplied binary")
    string_address = file_offset_to_va(segments, string_offset)

    start_offset = va_to_file_offset(segments, FUNCTION_START)
    end_offset = va_to_file_offset(segments, FUNCTION_END)
    disassembler = Cs(CS_ARCH_ARM, CS_MODE_THUMB)
    disassembler.detail = True
    # BuildResourceList contains literal pools between code fragments. Without
    # this Capstone stops at the first pool and silently misses later entries.
    disassembler.skipdata = True
    instructions = list(disassembler.disasm(image[start_offset:end_offset], FUNCTION_START))
    labels = {
        RESOURCE_MESH_CTOR: "ResourceMesh::ResourceMesh(char const*, uint16_t, bool)",
        RESOURCE_MESH_THUNK: "ResourceMesh::ResourceMesh(char const*, uint16_t, bool) thunk",
        string_address: "projectile_025_anim_add.aem string",
    }

    matches = []
    for index, instruction in enumerate(instructions):
        if instruction.id == 0:
            continue
        immediates = [operand.imm for operand in instruction.operands if operand.type == ARM_OP_IMM]
        if RESOURCE_ID in immediates or string_address in immediates:
            matches.append(index)

    print(f"binary={args.binary}")
    print(f"BuildResourceList=0x{FUNCTION_START:08x}..0x{FUNCTION_END:08x}")
    print(f"resource_id={RESOURCE_ID} (0x{RESOURCE_ID:04x})")
    print(f"projectile_025_string=0x{string_address:08x}")
    print(f"decoded_thumb_instructions={len(instructions)}")
    print()

    if not matches:
        print("No direct immediate reference found. ID may be loaded through a literal pool.")
    for number, index in enumerate(matches, start=1):
        instruction = instructions[index]
        print(f"=== direct match {number}: 0x{instruction.address:08x} ===")
        print("\n".join(render_window(instructions, index, labels)))
        print()

    mesh_call_count = sum(
        1
        for instruction in instructions
        if instruction.id != 0
        if instruction_target(instruction) in (RESOURCE_MESH_CTOR, RESOURCE_MESH_THUNK)
    )
    linked_mesh_calls = [
        index
        for match in matches
        # The resource record is written after its payload constructor returns,
        # so the associated ResourceMesh call immediately precedes the ID store.
        for index in range(max(0, match - 24), match)
        if instructions[index].id != 0
        if instruction_target(instructions[index]) in (RESOURCE_MESH_CTOR, RESOURCE_MESH_THUNK)
    ]
    print(f"ResourceMesh constructor/thunk calls in BuildResourceList: {mesh_call_count}")
    for number, index in enumerate(linked_mesh_calls, start=1):
        instruction = instructions[index]
        print(f"=== ResourceMesh call linked to 6801 ({number}): 0x{instruction.address:08x} ===")
        print("\n".join(render_window(instructions, index, labels)))
        print()

    linked_material_mentions = [
        instructions[index].address
        for match in matches
        for index in range(max(0, match - 24), match)
        if instructions[index].id != 0
        if any(
            operand.type == ARM_OP_IMM and operand.imm == FX_MATERIAL_ID
            for operand in instructions[index].operands
        )
    ]
    print(
        "Material 24201 immediate linked to 6801: "
        + ", ".join(f"0x{address:08x}" for address in linked_material_mentions)
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
