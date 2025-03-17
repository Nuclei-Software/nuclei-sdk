ifeq ($(AUTOVEC),1)
COMMON_FLAGS += -fvectorize -fslp-vectorize
else ifeq ($(AUTOVEC),0)
COMMON_FLAGS += -fno-vectorize -fno-slp-vectorize
else
COMMON_FLAGS +=
endif
