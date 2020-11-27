C_SRCDIRS += $(NUCLEI_SDK_RTOS)/src $(NUCLEI_SDK_RTOS)/libcpu/risc-v/nuclei
ifeq ($(RTTHREAD_MSH), 1)
	C_SRCDIRS += $(NUCLEI_SDK_RTOS)/components/finsh
endif

ASM_SRCDIRS += $(NUCLEI_SDK_RTOS)/libcpu/risc-v/nuclei

INCDIRS += $(NUCLEI_SDK_RTOS)/libcpu/risc-v/nuclei \
	$(NUCLEI_SDK_RTOS)/include \
	$(NUCLEI_SDK_RTOS)/include/libc
ifeq ($(RTTHREAD_MSH), 1)
	INCDIRS += $(NUCLEI_SDK_RTOS)/components/finsh
endif
