ifneq ($(OS),)
ifneq ($(OS),Windows_NT)
MAKEFILE_PREREQS += $(NUCLEI_SDK_BUILD)/Makefile.os
MAKEFILE_PREREQS += $(NUCLEI_SDK_BUILD)/Makefile.os.$(OS)
include $(NUCLEI_SDK_BUILD)/Makefile.os.$(OS)
endif
endif
