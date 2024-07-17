# TODO: You can replace this file using the generated one by nuclei_gen tool
CORE ?= n300fd
ARCH_EXT ?=

# TODO: For CPU without unaligned load/store feature
# eg. If you are using Nuclei 200 Series CPU, and your cpu are configured without
# unaligned load/store access support, you should uncomment below lines to make sure
# not generate unaligned memory access
# see -mstrict-align in https://gcc.gnu.org/onlinedocs/gcc/RISC-V-Options.html
#COMMON_FLAGS += -mstrict-align
