# Should alway define variable MIDDLEWARE_$(MID_UPPER) to path to the middleware,
# profiling middleware is mainly used in Nuclei Studio, see README.md in this directory
# In Nuclei Studio, you can set per-file or per-folder compiler options to collect
# gprof or gcov data for specified files
MIDDLEWARE_PROFILING := $(NUCLEI_SDK_MIDDLEWARE)/profiling

C_SRCDIRS += $(MIDDLEWARE_PROFILING)

INCDIRS += $(MIDDLEWARE_PROFILING)
