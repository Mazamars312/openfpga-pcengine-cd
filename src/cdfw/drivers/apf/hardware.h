/*
 * Copyright 2022 Murray Aickin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef HARDWARE_H
#define HARDWARE_H

#include <inttypes.h>
#include <stdio.h>



#define RESTARTBASE               0xFFFFFFA4
#define AFP_REGISTOR_BASE         0xffffff00
#define IO_CORE_REGISTOR_BASE     0xffffff50
#define CONTROLLER_KEY_BASE       0xffffff20
#define CONTROLLER_JOY_BASE       0xffffff30
#define CONTROLLER_TRIG_BASE      0xffffff40
#define MISTERGPOHARDWAREBASE     0xffffffd0
#define MISTERGPIHARDWAREBASE     0xffffffd4
#define DATASLOT_BRAM_BASE        0xffff0000

#define RAMBUFFER_BASE            0x00008000

// Global Shit!!!!
// We need to make sure that the AFP stuff is here as Im sick of not having it everywhere and then needing it, then having it and not needing it..... Yea AVP Quote here without the Condom.

typedef struct dataslot_type
{
    uint16_t aft_dataslot;
    char aft_filename[255];
    char aft_filepath[255];
    uint32_t aft_op_flags;
    uint32_t aft_op_filesize;
    uint32_t aft_filesize;
} dataslot_type;

extern dataslot_type aft;

extern bool file_error_enabled;
extern int file_error_code;
extern int apf_data_already_streamed;

/*********************************
 * 
 * Error codes for APF
 * 
 *********************************/

#define error_info_dataslot_Faild 1;
#define error_info_dataslot_File_Not_Found 2;
#define error_info_dataslot_Load_Faild 3;
#define error_info_dataslot_save_Faild 4;
#define error_info_Core_Restart_code 5;
#define error_info_Core_data_slot_change 6;

#define MAIN_ADDRESS_OFFSET          0x00000000 // We want to make sure we advise the APF where the BRAM is of the CPU
#define MAIN_SCRACH_ADDRESS_OFFSET   0x01000000 // We want to make sure we advise the APF where the BRAM is of the CPU
#define MAIN_PSRAM_ADDRESS_OFFSET    0x02000000 // We want to make sure we advise the APF where the BRAM is of the CPU

#define SYS_CLOCK  7425 // This is the CPU clock speed in a int size 74.2mhz
// #define UART_RATE  4608 // This is the UART Rate shifted right by 2
#define UART_RATE  1152 // This is the UART Rate shifted right by 2 this is for MACS

#define RESTARTBASE 0xFFFFFFA4

#define RESET_CORE(x) *(volatile unsigned int *)(RESTARTBASE+x)

#define AFP_REGISTOR(x) *(volatile unsigned int *)(AFP_REGISTOR_BASE+(x<<2))
#define DATASLOT_BRAM_SAVE(x) *(volatile unsigned int *)(DATASLOT_BRAM_BASE+(5<<2))

#define CORE_OUTPUT_REGISTOR() *(volatile unsigned int *)(IO_CORE_REGISTOR_BASE+(4))
#define CORE_INPUT_REGISTOR()  *(volatile unsigned int *)(IO_CORE_REGISTOR_BASE+(0))

#define CONTROLLER_KEY_REG(x)  *(volatile unsigned int *)(CONTROLLER_KEY_BASE +((x-1)<<2))
#define CONTROLLER_JOY_REG(x)  *(volatile unsigned int *)(CONTROLLER_JOY_BASE +((x-1)<<2))
#define CONTROLLER_TRIG_REG(x) *(volatile unsigned int *)(CONTROLLER_TRIG_BASE+((x-1)<<2))

#define RAMBUFFER(x) *(volatile unsigned int *)(RAMBUFFER_BASE + (x<<2))

#define SCRACH_RAMBUFFER_BYTE(x) 	*(volatile uint8_t *)		(MAIN_SCRACH_ADDRESS_OFFSET + (x))
#define SCRACH_RAMBUFFER_WORD(x) 	*(volatile uint16_t int *)	(MAIN_SCRACH_ADDRESS_OFFSET + (x))
#define SCRACH_RAMBUFFER(x) 		*(volatile unsigned int *)	(MAIN_SCRACH_ADDRESS_OFFSET + (x))


 #define FETCH_BYTE(p)	(*(unsigned char*)(p))
 #define FETCH_WORD(p)	(*(unsigned short*)(p))
 #define FETCH_LONG(p)	(*(unsigned long*)(p))

#define DATASLOT_BRAM(x) *(volatile unsigned int *)(DATASLOT_BRAM_BASE+(x<<2))

#endif // SPI_H
/***********************************************************
	Memory map for the 832 CPU

	32'h0000_0000 - 32'h0000_3FFF - Program Ram for the CPU - The program is in here and starts at address 0000_0000.
									This is also mapped at address 32'8000_0000 on the APF Bus so data can be up/downloaded
									from the APF PIC32. Mostly used for the target dataslot bridge address location.

	32'hFFFF_0000 - 32'hFFFF_0FFF - Data slot ram from APF Core (R/W)

	// Target Interface to APF
	32'hFFFF_FF80 - target_dataslot_id (R/W) 15 bit - For which asset to read
	32'hFFFF_FF84 - target_dataslot_bridgeaddr (R/W) 32 bit - Where in ram to place this this data (Buffer and program) (The MPU Ram starts at 32'h8000_0000 to 32'h8000_3FFF )
	32'hFFFF_FF88 - target_dataslot_length (R/W) 32 bit - How much buffer to pull
	32'hFFFF_FF8c - target_dataslot_slotoffset (R/W) 32 bit
	32'hFFFF_FF90 - target_dataslot Controls (R) Respoce from the APF Core once the reg is written to
					{Bit 4 - target_dataslot_ack,
					 Bit 3 - target_dataslot_done,
					 Bit [2:0] target_dataslot_err[2:0]}
	32'hFFFF_FF90 - target_dataslot Controls (W)
					{Bit 1 - target_dataslot_write - Send a Write signal to the APF ( the target id, Bridgeaddr, length and offset need to be written to first)
					 Bit 0 - target_dataslot_read - Send a read signal to the APF ( the target id, Bridgeaddr, length and offset need to be written to first)

	32'hFFFF_FFA4 - Bit 0 Reset_out - Used for reseting the core if required (R/W)

	// Interupts and dataslot updates

	32'hFFFF_FFB0 - Interrupt core - Bit 0 = Dataslot updated by APF (R) Read clears the interupt to the CPU

	32'hFFFF_FFB4 - dataslot_update_id ID = Dataslot updated by APF (R) 15 bit
	32'hFFFF_FFB8 - dataslot_update_size ID = Dataslot updated by APF (R) 32 bit

	// UART core
	32'hFFFF_FFC0 - UART access - ser_rxrecv,ser_txready,ser_rxdata
		bit [7:0] data received,
		Bit8 - Transmit ready,
		Bit9 - Data received (cleared once read) (R)
	32'hFFFF_FFC0 - UART access
		ser_txdata [7:0] data to send (W)

	32'hFFFF_FFC8 - Timer - millisecond_counter (R) 32 bit

	The SPI/Mister EXT interface
	32'hFFFF_FFD0 - This is setup for the SPI interface (R)
					Bit [15:0] 	- IO_DIN Data from core
					Bit [16]	- IO_WIDE 1 = 16bit, 0 = 8 bit
					Bit [17]	- io_ack - Data has been ack from core (Both read and writes)

	32'hFFFF_FFD0 - This is setup for the SPI interface (W)
					Bit [15:0] 	- IO_DIN Data from core
					Bit [17]	- io_clk - This is send as a clock from the CPU when data is send or received - Also part of the strobe signal
					Bit [18]	- io_ss0
					Bit [19]	- io_ss1
					Bit [20]	- io_ss2

	assign IO_FPGA     = ~io_ss1 & io_ss0; - This is for the CPU to send commands to the FPGA
	assign IO_UIO      = ~io_ss1 & io_ss2; - This is used for a Broardcast on the bus (Floopy drives use this access)



***********************************************************/
