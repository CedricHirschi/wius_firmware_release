/***************************************************************************//**
 * GCC Linker script for Silicon Labs devices
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/


 MEMORY
 {
   rom   (rx)  : ORIGIN = 0x8202000, LENGTH = 0x1fe000

   ram   (rwx) : ORIGIN = 0x400, LENGTH = 0x2f800
 }
 MEMORY
 {
   udma0   (rwx)  : ORIGIN = 0x2fc00, LENGTH = 0x400
   udma1   (rwx)  : ORIGIN = 0x24061c00, LENGTH = 0x400
 }

ENTRY(Reset_Handler)
 
SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
    
		  *(EXCLUDE_FILE(*cmsis_os2.o *port.o *queue.o *sl_rsi_utility.o *tasks.o *clock_update.o *rsi_deepsleep_soc.o *rsi_egpio.o *rsi_ipmu.o *ipmu_apis.o *rsi_pll.o *rsi_power_save.o *rsi_ps_ram_func.o *rsi_system_config.o *rsi_time_period.o *rsi_ulpss_clk.o *system_si91x.o *sl_slist.o *strcmp.o *sl_si91x_power_manager.o *sli_si91x_power_manager.o *sl_si91x_power_manager_handler.o *sl_si91x_power_manager_debug.o *stdio.o *puts.o *wbuf.o *writer.o *memchr.o *memset.o *nano-vfprintf.o *nano-vfprintf_i.o *printf.o *lock.o *fflush.o *rsi_debug.o *rsi_usart.o *syscalls.o *USART.o *libm_a-s_ceil.o *_arm_*.o *libc_a*.o *libm_a*.o *sbrk.o* *sl_si91x_calendar.o *rsi_rtc.o *sl_si91x_ulp_timer.o *rsi_timers.o *memcpy*.o *UDMA.o *rsi_udma.o *rsi_udma_wrapper.o *sl_si91x_led.o ) .text*)
		

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)
		
		/* .dtors */
		*crtbegin.o(.dtors)
		*crtbegin?.o(.dtors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
		*(SORT(.dtors.*))
		*(.dtors)
		
		KEEP(*(.eh_fram e*))			
	} > rom	
	
	.ARM.extab : 
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)			
	} > rom
	
	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)			
	} > rom 
	__exidx_end = .;
	__etext = .;
	
	

	
	/* _sidata is used in code startup code */
	_sidata = __etext;
	
	.data :
	
	{
		__data_start__ = .;
		
		/* _sdata is used in startup code */
		_sdata = __data_start__;
		KEEP(*(.ramVector))
		*(.data*)
		*(.rodata*)
		KEEP(*(.init))
		KEEP(*(.fini))
		*cmsis_os2.o(.text*)
		*port.o(.text*)
		*queue.o(.text*)
		*sl_rsi_utility.o(.text*)
		*tasks.o(.text*)
		*clock_update.o(.text*)
		*rsi_deepsleep_soc.o(.text*)
		*rsi_egpio.o(.text*)
		*rsi_ipmu.o(.text*)
		*ipmu_apis.o(.text*)
		*rsi_pll.o(.text*)
		*rsi_power_save.o(.text*)
		*rsi_ps_ram_func.o(.text*)
		*rsi_system_config.o(.text*)
		*rsi_time_period.o(.text*)
		*rsi_ulpss_clk.o(.text*)
		*system_si91x.o(.text*)
		*sl_slist.o(.text*)
		*strcmp.o(.text*)
		*sl_si91x_power_manager.o(.text*)
		*sli_si91x_power_manager.o(.text*)
		*sl_si91x_power_manager_handler.o(.text*)
		*sl_si91x_power_manager_debug.o(.text*)
		*stdio.o(.text*)
		*puts.o(.text*)
		*wbuf.o(.text*)
		*writer.o(.text*)
		*memchr.o(.text*)
		*memset.o(.text*)
		*nano-vfprintf.o(.text*)
		*nano-vfprintf_i.o(.text*)
		*printf.o(.text*)
		*lock.o(.text*)
		*fflush.o(.text*)
		*rsi_debug.o(.text*)
		*rsi_usart.o(.text*)
		*syscalls.o(.text*)
		*USART.o(.text*)
		*libm_a-s_ceil.o(.text*)
		*_arm_*.o(.text*)
		*libc_a*.o(.text*)
		*libm_a*.o(.text*)
		*sbrk.o*(.text*)
		*sl_si91x_calendar.o(.text*)
		*rsi_rtc.o(.text*)
		*sl_si91x_ulp_timer.o(.text*)
		*rsi_timers.o(.text*)
		*memcpy*.o(.text*)
		*UDMA.o(.text*)
		*rsi_udma.o(.text*)
		*rsi_udma_wrapper.o(.text*)
		*sl_si91x_led.o(.text*)
		
		

		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);
		
		. = ALIGN(4);
		/* init data */
		
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);
		
		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);
		
		KEEP(*(.jcr*))
		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;
		
		/* _edata is used in startup code */
		_edata = __data_end__; 
	} > ram AT> rom	
	
	.bss (NOLOAD) :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .; 
	} > ram 
	
	
	.stack (NOLOAD):
	{
		__StackLimit = .;
		KEEP(*(.stack*))
		. = ALIGN(4);
		__StackTop = .;
		PROVIDE(__stack = __StackTop);			
	} > ram
			
  	.heap (COPY):
  	{
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		_end = __end__;
		KEEP(*(.heap*))	
		. = ORIGIN(ram) + LENGTH(ram);
  		
		__HeapLimit = .;			
  	} > ram
  
	__heap_size = __HeapLimit - __HeapBase;
	.udma_addr0 :
	{
		*(.udma_addr0*)
	} > udma0 AT> rom
	

	.udma_addr1 :
	{
		*(.udma_addr1*)		
	} > udma1 AT> rom	
   
}