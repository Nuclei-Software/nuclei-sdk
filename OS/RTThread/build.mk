C_SRCDIRS += $(NUCLEI_SDK_RTOS)/src $(NUCLEI_SDK_RTOS)/libcpu/risc-v/nuclei
ifeq ($(RTTHREAD_MSH), 1)
	C_SRCDIRS += $(NUCLEI_SDK_RTOS)/components/finsh
endif

ASM_SRCDIRS += $(NUCLEI_SDK_RTOS)/libcpu/risc-v/nuclei/gcc

INCDIRS += $(NUCLEI_SDK_RTOS)/include
ifeq ($(RTTHREAD_MSH), 1)
	INCDIRS += $(NUCLEI_SDK_RTOS)/components/finsh
endif
