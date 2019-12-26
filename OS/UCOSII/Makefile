TARGET = ucosii
CFLAGS += -Os -msave-restore  -fno-unroll-loops -flto

BSP_BASE = ../../bsp


C_SRCS += source/os_core.c  
C_SRCS += source/os_dbg_r.c  
C_SRCS += source/os_flag.c  
C_SRCS += source/os_mbox.c  
C_SRCS += source/os_mem.c  
C_SRCS += source/os_mutex.c  
C_SRCS += source/os_q.c
C_SRCS += source/os_sem.c
C_SRCS += source/os_task.c
C_SRCS += source/os_time.c
C_SRCS += source/os_tmr.c

C_SRCS += arch/os_cpu_c.c

C_SRCS += arch/trap.c

C_SRCS += main.c


INCLUDES += -Isource/
INCLUDES += -Iarch/
INCLUDES += -Icfg/


ASM_SRCS += arch/os_cpu_a.S


include $(BSP_BASE)/core/env/common.mk
