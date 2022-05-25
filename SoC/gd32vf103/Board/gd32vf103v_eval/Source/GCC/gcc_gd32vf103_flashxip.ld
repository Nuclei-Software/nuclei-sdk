/*
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 * @file     gcc_gd32vf103_flashxip.ld
 * @brief    GNU Linker Script for gd32vf103 based device
 * @version  V2.0.0
 * @date     10. March 2022
 ******************************************************************************/

/*********** Use Configuration Wizard in Context Menu *************************/

OUTPUT_ARCH( "riscv" )
/********************* Flash Configuration ************************************
 * <h> Flash Configuration
 * <o0> Flash Base Address <0x0-0xFFFFFFFF:8>
 * <o1> Flash Size (in Bytes) <0x0-0xFFFFFFFF:8>
 * </h>
 */
__ROM_BASE = 0x08000000;
__ROM_SIZE = 0x00020000;

/*--------------------- Embedded RAM Configuration ---------------------------
 * <h> RAM Configuration
 * <o0> RAM Base Address    <0x0-0xFFFFFFFF:8>
 * <o1> RAM Size (in Bytes) <0x0-0xFFFFFFFF:8>
 * </h>
*/
__RAM_BASE = 0x20000000;
__RAM_SIZE = 0x00008000;

/**************************** end of configuration section ********************/

/* Define base address and length of flash and ram */
MEMORY
{
  flash (rxa!w) : ORIGIN = __ROM_BASE, LENGTH = __ROM_SIZE
  ram (wxa!r) : ORIGIN = __RAM_BASE, LENGTH = __RAM_SIZE
}

/* Define entry label of program */
ENTRY(_start)
SECTIONS
{
  /* To provide symbol __STACK_SIZE, __HEAP_SIZE */
  PROVIDE(__STACK_SIZE = 2K);
  PROVIDE(__HEAP_SIZE = 2K);

  .init           :
  {
    /* vector table locate at flash */
    *(.vtable)
    KEEP (*(SORT_NONE(.init)))
  } >flash AT>flash

  .ilalign         :
  {
    . = ALIGN(4);
    /* Create a section label as _ilm_lma which located at flash */
    PROVIDE( _ilm_lma = . );
  } >flash AT>flash

  .ialign         :
  {
    . = ALIGN(4);
    /* Create a section label as _ilm which located at flash */
    PROVIDE( _ilm = . );
  } >flash AT>flash

  /* Code section located at flash */
  .text           :
  {
    *(.text.unlikely .text.unlikely.*)
    *(.text.startup .text.startup.*)
    *(.text .text.*)
    *(.gnu.linkonce.t.*)
  } >flash AT>flash

  .rodata : ALIGN(4)
  {
    . = ALIGN(4);
    *(.rdata)
    *(.rodata .rodata.*)
    /* section information for initial. */
    . = ALIGN(4);
    __rt_init_start = .;
    KEEP(*(SORT(.rti_fn*)))
    __rt_init_end = .;
    /* section information for finsh shell */
    . = ALIGN(4);
    __fsymtab_start = .;
    KEEP(*(FSymTab))
    __fsymtab_end = .;
    . = ALIGN(4);
    __vsymtab_start = .;
    KEEP(*(VSymTab))
    __vsymtab_end = .;
    *(.gnu.linkonce.r.*)
    . = ALIGN(8);
    *(.srodata.cst16)
    *(.srodata.cst8)
    *(.srodata.cst4)
    *(.srodata.cst2)
    *(.srodata .srodata.*)
  } >flash AT>flash

  .fini           :
  {
    KEEP (*(SORT_NONE(.fini)))
  } >flash AT>flash

  .preinit_array  :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >flash AT>flash

  .init_array     :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >flash AT>flash

  .fini_array     :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >flash AT>flash

  .ctors          :
  {
    /* gcc uses crtbegin.o to find the start of
     * the constructors, so we make sure it is
     * first.  Because this is a wildcard, it
     * doesn't matter if the user does not
     * actually link against crtbegin.o; the
     * linker won't look for a file to match a
     * wildcard.  The wildcard also means that it
     * doesn't matter which directory crtbegin.o
     * is in.
     */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    /* We don't want to include the .ctor section from
     * the crtend.o file until after the sorted ctors.
     * The .ctor section from the crtend file contains the
     * end of ctors marker and it must be last
     */
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  } >flash AT>flash

  .dtors          :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  } >flash AT>flash

  PROVIDE (__etext = .);
  PROVIDE (_etext = .);
  PROVIDE (etext = .);
  PROVIDE( _eilm = . );

  .lalign         :
  {
    . = ALIGN(4);
    PROVIDE( _data_lma = . );
  } >flash AT>flash

  .dalign         :
  {
    . = ALIGN(4);
    PROVIDE( _data = . );
  } >ram AT>flash

  /* Define data section virtual address is ram and physical address is flash */
  .data          :
  {
    KEEP(*(.data.ctest*))
    *(.data .data.*)
    *(.gnu.linkonce.d.*)
    . = ALIGN(8);
    PROVIDE( __global_pointer$ = . + 0x800 );
    *(.sdata .sdata.* .sdata*)
    *(.gnu.linkonce.s.*)
  } >ram AT>flash

  .tdata           : ALIGN(8)
  {
    PROVIDE( __tls_base = . );
    *(.tdata .tdata.* .gnu.linkonce.td.*)
  } >ram AT>flash

  PROVIDE( _edata = . );
  PROVIDE( edata = . );

  PROVIDE( _fbss = . );
  PROVIDE( __bss_start = . );

  .tbss (NOLOAD)   : ALIGN(8)
  {
    *(.tbss .tbss.* .gnu.linkonce.tb.*)
    *(.tcommon)
    PROVIDE( __tls_end = . );
  } >ram AT>ram

  .tbss_space (NOLOAD) : ALIGN(8)
  {
    . = . + SIZEOF(.tbss);
  } >ram AT>ram

  .bss (NOLOAD)   : ALIGN(8)
  {
    *(.sbss*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(4);
  } >ram AT>ram

  PROVIDE( _end = . );
  PROVIDE( end = . );

  /* Nuclei C Runtime Library requirements:
   * 1. heap need to be align at 16 bytes
   * 2. __heap_start and __heap_end symbol need to be defined
   * 3. reserved at least __HEAP_SIZE space for heap
   */
  .heap (NOLOAD)   : ALIGN(16)
  {
    . = ALIGN(16);
    PROVIDE( __heap_start = . );
    . += __HEAP_SIZE;
    . = ALIGN(16);
    PROVIDE( __heap_limit = . );
  } >ram AT>ram

  .stack ORIGIN(ram) + LENGTH(ram) - __STACK_SIZE (NOLOAD) :
  {
    . = ALIGN(16);
    PROVIDE( _heap_end = . );
    PROVIDE( __heap_end = . );
    PROVIDE( __StackLimit = . );
    PROVIDE( __StackBottom = . );
    . += __STACK_SIZE;
    . = ALIGN(16);
    PROVIDE( __StackTop = . );
    PROVIDE( _sp = . );
  } >ram AT>ram
}
