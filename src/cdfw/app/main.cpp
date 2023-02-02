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
 // #include <stdint.h>
 // #include <unistd.h>
 // #include <stdio.h>
 // #include <stdlib.h>

#include "interrupts.h"
#include "hardware.h"
#include "timer.h"
#include "uart.h"
#include "apf.h"
#include "riscprintf.h"
#include "printf.h"
#include "spi.h"
#include "core.h"

void init()
{
  // this makes the core go in to the reset state and halts the bus so we can upload the bios if required
  core_reset(0);
	// This setups the timers and the CPU clock rate on the system.
  DisableInterrupts();
  // Setup the core to know what MHZ the CPU is running at.
	SetTimer();
	SetUART();
	ResetTimer();
  riscusleep(500000);
  mainprintf("\033[0m");
  // printf_register(riscputc);
  // This is where you setup the core startup dataslots that the APT loads up for your core.
  core_restart_first(); // we want to setup the core now :-) but with no startup yet
}

void mainloop()
{
  // We like to see over the UART that hey I did something :-)
	riscusleep(9000);
	mainprintf("RISC MPU Startup core\r\n");
	mainprintf("Created By Mazamars312\r\n");
	mainprintf("Make in 2022\r\n");
	mainprintf("I hate this debugger LOL\r\n");
	riscusleep(2000);
	core_reset(1); // Turns off all the resets via the HPS bus
	// I do enjoy reseting this timer.
  core_update_dataslots();
	// mainprintf("I hate this debugger LOL1\r\n");
	core_input_setup();
  DisableInterrupts();
	// mainprintf("I hate this debugger LOL2\r\n");
	// EnableInterrupts();
	// This loop is what keeps the checking of the core interface
  mainprintf("about to go in baby\r\n");
  while(true){

		if(DATASLOT_UPDATE_REG(0)){
		// 		// This subprogram is for the program to check what dataslots are updated by the APF
			// DisableInterrupts(); // we want to make sure when updating the dataslots - interrupts are stopped.
		// 	EnableInterrupts(); // Re-enable interrupts
      mainprintf("Data Slot Update\r\n");
      core_update_dataslots();
      mainprintf("Data Slot done\r\n");
		}
	  core_poll_io();
    core_reg_update();
    // This checks if we are wanting to do a reboot of the core from the interaction menu. We want to do this last so nothing is held in the buffers
    if (AFP_REGISTOR(0) & 0x1) { // This has 2 bits for reseting the core
  		// DisableInterrupts(); // we make sure all interrupts are disabled
  		full_core_reset();
  		// EnableInterrupts(); // we make sure all interrupts are re-enabled
    }
	};
}

int main()
{

	init();
	riscusleep(20000); // I want the core to wait a bit.
	mainloop();

	return(0);
}

void irqCallback(){
}
