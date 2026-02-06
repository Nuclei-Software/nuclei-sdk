# NOTE: Configure whether to initialize BSS section to zero during startup
# Set to 0 to skip BSS initialization (requires BSS section to be pre-zeroed)
# Default behavior is to perform BSS initialization
SIMULATION_BSSZERO ?=

ifeq ($(SIMULATION_BSSZERO),0)
COMMON_FLAGS += -DCFG_SIMULATION_BSSZERO=0
endif
