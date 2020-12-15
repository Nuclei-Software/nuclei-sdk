import os
import sys
import argparse

parser = argparse.ArgumentParser(description="RISC-V Instruction Analyzer via parsing riscv object dump file")

parser.add_argument('-d', '--dump', required=True, help="The objdump file(objdump -d xxx.elf)")

args = parser.parse_args()

dump_file = args.dump

RV32I_INST = ("add", "addi", "and", "andi", "auipc", "beq", "bge", \
    "bgeu", "blt", "bltu", "bne", "fence", "fence.i", "jal", "jalr", \
    "lb", "lbu", "lh", "lhu", "lui", "lw", "or", "ori", "sb", "sh", \
    "sll", "slli", "slt", "slti", "sltiu", "sltu", "sra", "srai", "srl", \
    "srli", "sub", "sw", "xor", "xori"
)
RV32M_INST = ("div", "divu", "mul", "mulh", "mulhsu", "mulhu", "rem", "remu")

RV32A_INST = ("amoadd.w", "amoand.w", "amomax.w", "amomaxu.w", \
    "amomin.w", "amominu.w", "amoor.w", "amoswap.w", "amoxor.w", "lr.w", "sc.w")

RV32C_INST = ("c.addi16sp", "c.ebreak", "c.jalr", "c.jr", "c.nop", "c.add", \
    "c.addi", "c.addi4spn", "c.and", "c.andi", "c.beqz", "c.bnez", "c.fld", \
    "c.fldsp", "c.flw", "c.flwsp", "c.fsd", "c.fsdsp", "c.fsw", "c.fswsp", \
    "c.j", "c.jal", "c.li", "c.lui", "c.lw", "c.lwsp", "c.mv", "c.or", \
    "c.slli", "c.srai", "c.srli", "c.sub", "c.sw", "c.swsp", "c.xor")

RV32F_INST = ("fadd.s", "fclass.s", "fcvt.s.w", "fcvt.s.wu", "fcvt.w.s", \
    "fcvt.wu.s", "fdiv.s", "feq.s", "fle.s", "flt.s", "flw", "fmadd.s", \
    "fmax.s", "fmin.s", "fmsub.s", "fmul.s", "fmv.w.x", "fmv.x.w", "fnmadd.s", \
    "fnmsub.s", "fsgnj.s", "fsgnjn.s", "fsgnjx.s", "fsqrt.s", "fsub.s", "fsw")

RV32D_INST = ("fadd.d", "fclass.d", "fcvt.d.s", "fcvt.d.w", "fcvt.d.wu", \
    "fcvt.s.d", "fcvt.w.d", "fcvt.wu.d", "fdiv.d", "feq.d", "fld", "fle.d", \
    "flt.d", "fmadd.d", "fmax.d", "fmin.d", "fmsub.d", "fmul.d", "fnmadd.d", \
    "fnmsub.d", "fsd", "fsgnj.d", "fsgnjn.d", "fsgnjx.d", "fsqrt.d", "fsub.d")

if os.path.isfile(dump_file) == False:
    print("Object dump file %s not exist, please check!"%(dump_file))
    sys.exit(0)

inst_groups = dict()
INST_EXCLUDED = ("", "unimp", "section", "format", "0x8000", "0x9000")
total_instcnt = 0
inst_matches = {
    "rv32i": {"base": RV32I_INST},
    "rv32m": {"base": RV32M_INST},
    "rv32a": {"base": RV32A_INST},
    "rv32c": {"base": RV32C_INST},
    "rv32f": {"base": RV32F_INST},
    "rv32d": {"base": RV32D_INST}
}

ALLOWED_SECTIONS = (".init", ".text")
SECTION_HEAD = "Disassembly of section"

parse_started = False
parse_enabled = False
with open(dump_file, 'r') as f:
    for line in f.readlines():
        if line.startswith(SECTION_HEAD):
            section = line.strip(SECTION_HEAD).strip()
            # Only parse enabled section
            if section[:-1] in ALLOWED_SECTIONS:
                print("Parse section %s"%(section[:-1]))
                parse_enabled = True
            else:
                print("Ignore section %s"%(section[:-1]))
                parse_enabled = False
        if parse_enabled == False:
            continue

        strs = line.split()
        if len(strs) < 3:
            if len(strs) == 2:
                # Only parse sections after _start label
                if (strs[1].strip() == "<_start>:"):
                    parse_started = True
            continue
        if strs[0].strip().lower()[0] not in \
            ('0', '1', '2', '3', '4', '5', '6', '7', '8', \
            '9', 'a', 'b', 'c', 'd', 'e', 'f') \
                or strs[0].strip().lower()[-1] != ':':
            continue

        if parse_started == False:
            continue

        inst = strs[2].strip()
        if inst in INST_EXCLUDED:
            continue
        total_instcnt += 1
        if inst not in inst_groups:
            inst_groups[inst] = 1
        else:
            inst_groups[inst] += 1

print(">>> Instruction usage for %s"%(dump_file))
for inst in inst_groups:
    print("{}: {}, {:.2f}%".format(inst, inst_groups[inst], \
        100*inst_groups[inst]/float(total_instcnt)))


print(">>> Instruction coverage of riscv spec for %s"%(dump_file))

for key in inst_matches:
    matched = set(inst_groups.keys()).intersection(inst_matches[key]["base"])
    notmatched = set(inst_matches[key]["base"]) - matched
    inst_matches[key]["used"] = matched
    inst_matches[key]["unused"] = notmatched
    inst_matches[key]["percent"] = 100*len(matched) / len(inst_matches[key]["base"])

    if len(matched) > 0:
        print("{} coverage {:.2f}%".format(key, inst_matches[key]["percent"]))
        print("     - used instructions: {}".format(inst_matches[key]["used"]))
        print("     - unused instructions: {}".format(inst_matches[key]["unused"]))
