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



//  #include <stdint.h>
//  #include <unistd.h>
//  #include <stdio.h>
//  #include <stdlib.h>
// #include <inttypes.h>


#include "core.h"
#include "riscprintf.h"
#include "hardware.h"
#include "apf.h"
#include "cue.h"
#include "timer.h"
#include "spi.h"
#define DATASLOT_FDD_BASE 100 // From 100-128 data slots I have advised that are used for BIN and CUE files



// Sends a reset via the HPS Bus
void core_reset(int reset){
		RESET_CORE(0) = reset;
		mainprintf("reset %d\r\n",reset);
};

void full_core_reset(){
	mainprintf("starting the reset\r\n");
	core_reset(0); // this sends the reset command to core - This one uses the AFP registor for this as the Amiga has 3 types of reset.
	riscusleep(200);
	AFP_REGISTOR(0) = 0;	// This makes the reset registor on the APF bus go back to zero for the user to see - need to do a read-modify-write back here
	riscusleep(20000);
	core_update_dataslots();
	core_reset(1);	// activate the core
	riscusleep(500);
	mainprintf("Completed the reset\r\n");
}
// Update dataslots

void core_update_dataslots(){
  	int tmp = DATASLOT_UPDATE_REG(1);
	pcecd_set_image(DATASLOT_BRAM(8), DATASLOT_BRAM(9));
	HPS_spi_uio_cmd8_cont(UIO_SET_SDSTAT, 1);
	HPS_DisableIO();
	mainprintf("UPLOADS\r\n");
};

void core_poll_io(){
	pcecd_poll();
      // Here is where you do your polling on the core - eg Floppy and CDROM data

};

bool old_region = 0;
bool old_arcade = 0;
void core_reg_update(){
	// This can be used for polling the APF regs from the interaction menu to change core settings
	// Region setup
	if (AFP_REGISTOR(1) & 0x1) cue.SetRegion(1);
	else cue.SetRegion(0);
	// if (old_region != AFP_REGISTOR(1) & 0x1) full_core_reset();
	// Arcade Setup

	if (AFP_REGISTOR(1) & 0x2) CORE_OUTPUT_REGISTOR() = 0x2;
	else CORE_OUTPUT_REGISTOR() = 0x0;
	// if (old_arcade != AFP_REGISTOR(1) & 0x2) full_core_reset();

};



void core_restart_first(){
	// what to do to start up the core if required
	
};

void core_restart_running_core() {
	// this can be used for restarting the core

};

void core_input_setup() {
	// this is used for controller setup

};

void core_input_update() {
	// this is called via the interrupts.c for controller updates to the core

};

void core_interupt_update(){

};
