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
 * @file     gcc_demosoc_ilm.ld
 * @brief    GNU Linker Script for Nuclei N/NX based device in ilm Download Mode
 * @version  V1.0.0
 * @date     17. Dec 2019
 ******************************************************************************/
OUTPUT_ARCH( "riscv" )

ENTRY( _start )

__STACK_SIZE = 2K;
__HEAP_SIZE  = 2K;

MEMORY
{
  ilm (rxa!w) : ORIGIN = 0x80000000, LENGTH = 64K
  ram (wxa!r) : ORIGIN = 0x90000000, LENGTH = 64K
}

SECTIONS
{
  .init           :
  {
    *(.vtable)
    KEEP (*(SORT_NONE(.init)))
  } >ilm AT>ilm

  .ilalign         :
  {
    . = ALIGN(4);
    PROVIDE( _ilm_lma = . );
  } >ilm AT>ilm

  .ialign         :
  {
    . = ALIGN(4);
    PROVIDE( _ilm = . );
  } >ilm AT>ilm

  .text           :
  {
    *(.text.unlikely .text.unlikely.*)
    *(.text.startup .text.startup.*)
    *(.text .text.*)
    *(.gnu.linkonce.t.*)
  } >ilm AT>ilm

  .fini           :
  {
    KEEP (*(SORT_NONE(.fini)))
  } >ilm AT>ilm

  .preinit_array  :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >ilm AT>ilm

  .init_array     :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >ilm AT>ilm

  .fini_array     :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >ilm AT>ilm

  .ctors          :
  {
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    /* We don't want to include the .ctor section from
       the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  } >ilm AT>ilm

  .dtors          :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  } >ilm AT>ilm

  PROVIDE (__etext = .);
  PROVIDE (_etext = .);
  PROVIDE (etext = .);
  PROVIDE( _eilm = . );

  /* NOTICE:
   * The following data section VMA and LMA are the same,
   * so there is no need to copy when startup.
   * rodata is also placed in ram now.
   */
  .lalign         :
  {
    . = ALIGN(4);
    PROVIDE( _data_lma = . );
  } >ram AT>ram

  .dalign         :
  {
    . = ALIGN(4);
    PROVIDE( _data = . );
  } >ram AT>ram

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
  } >ram AT>ram

  .data          : ALIGN_WITH_INPUT
  {
    KEEP(*(.data.ctest*))
    *(.data .data.*)
    *(.gnu.linkonce.d.*)
    . = ALIGN(8);
    PROVIDE( __global_pointer$ = . + 0x800 );
    *(.sdata .sdata.* .sdata*)
    *(.gnu.linkonce.s.*)
    . = ALIGN(8);
    *(.srodata.cst16)
    *(.srodata.cst8)
    *(.srodata.cst4)
    *(.srodata.cst2)
    *(.srodata .srodata.*)
  } >ram AT>ram

  .tdata           : ALIGN(8)
  {
    PROVIDE( __tls_base = . );
    *(.tdata .tdata.* .gnu.linkonce.td.*)
  } >ram AT>ram

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
