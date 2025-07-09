# Please use it in Nuclei Studio IDE not in command line
# see https://doc.nucleisys.com/nuclei_sdk/design/app.html#demo-profiling
TARGET = demo_profiling

MIDDLEWARE := profiling

NUCLEI_SDK_ROOT = ../../..

SRCDIRS = . src

INCDIRS = . inc

DOWNLOAD ?= sram

COMMON_FLAGS := -O0

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
