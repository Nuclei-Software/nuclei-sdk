PROGRAM :=baremetal/helloworld

.PHONY: __help
__help:
	@echo "Help about Build/Run/Debug an Nuclei SDK Application"
	@echo "make [PROGRAM=/path/to/app]  help        Show Build System Help Message"
	@echo "make cleanall                            Clean all the applications found in application and test folder"
	@echo "Example:"
	@echo "make PROGRAM=application/baremetal/helloworld all"
	@echo "make -k cleanall"

VALID_PROGRAM=$(wildcard $(PROGRAM))
VALID_PROGRAM_MAKEFILE=$(wildcard $(PROGRAM)/Makefile)

# Valid SDK Rules accepted by build system
VALID_SDK_RULES := all info help bin size dasm upload run_openocd run_gdb clean debug

# Default root directories to search
APP_ROOT_DIRS := application test
# Extra application root directories passed by make
EXTRA_APP_ROOT_DIRS ?=

TOTAL_ROOT_DIRS := $(APP_ROOT_DIRS) $(EXTRA_APP_ROOT_DIRS)

# Default search patterns
SEARCH_PATTERNS := * */* */*/* */*/*/*

PROGS_TO_SEARCH := $(foreach rootdir, $(TOTAL_ROOT_DIRS), $(addprefix $(rootdir)/, $(SEARCH_PATTERNS)))
PROGS_makefile := $(foreach progdir, $(PROGS_TO_SEARCH), $(sort $(dir $(wildcard $(progdir)/makefile))))
PROGS_Makefile := $(foreach progdir, $(PROGS_TO_SEARCH), $(sort $(dir $(wildcard $(progdir)/Makefile))))
PROGS_DIRS := $(sort $(PROGS_makefile) $(PROGS_Makefile))
CLEAN_DIRS_RULES := $(addprefix __CLEAN__, $(PROGS_DIRS))

ifeq ($(VALID_PROGRAM_MAKEFILE), )
APP_PROGRAM=application/$(PROGRAM)
VALID_PROGRAM=$(wildcard $(APP_PROGRAM))
VALID_PROGRAM_MAKEFILE=$(wildcard $(APP_PROGRAM)/Makefile)
ifeq ($(VALID_PROGRAM_MAKEFILE), )
$(error No valid Makefile in $(PROGRAM) directory! please check!)
endif
endif

cleanall: $(CLEAN_DIRS_RULES)

$(CLEAN_DIRS_RULES):
	make -C $(patsubst __CLEAN__%, %, $@) clean

$(VALID_SDK_RULES):
	make -C $(VALID_PROGRAM) $@
