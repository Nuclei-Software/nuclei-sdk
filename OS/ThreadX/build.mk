C_SRCDIRS += $(NUCLEI_SDK_RTOS)/common/src $(NUCLEI_SDK_RTOS)/ports/nuclei

ASM_SRCDIRS += $(NUCLEI_SDK_RTOS)/ports/nuclei/gcc

INCDIRS += $(NUCLEI_SDK_RTOS)/common/inc $(NUCLEI_SDK_RTOS)/ports/nuclei

ifeq ($(THREADX_MODULE),1)
C_SRCDIRS += $(NUCLEI_SDK_RTOS)/common_modules/module_manager/src $(NUCLEI_SDK_RTOS)/ports/nuclei/module_manager/src
INCDIRS += $(NUCLEI_SDK_RTOS)/common_modules/module_manager/inc $(NUCLEI_SDK_RTOS)/common_modules/inc
endif
