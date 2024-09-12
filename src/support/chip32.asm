architecture chip32.vm
output "chip32.bin", create

// we will put data into here that we're working on.  It's the last 1K of the 8K chip32 memory
constant rambuf = 0x1b00

constant rom_dataslot = 1
constant save_dataslot = 2
constant mpu_dataslot = 3
constant json_dataslot = 99
constant json_dataslot_start = 100

// Host init command
constant host_init = 0x4002

// Error vector (0x0)
jp error_handler

// Init vector (0x2)
// Choose core
cmp r15,#0001
jp c, main //
cmp r0,#0000
jp z, reload_cd_assets // 

main:
ld r0,#0
core r0

ld r1,#mpu_dataslot // Load MPU core file
loadf r1 // Load slot

ld r1,#rom_dataslot // populate data slot
ld r2,#rambuf // get ram buf position
getext r1,r2
ld r1,#ext_sgx
test r2,r1
jp z,set_sgx // Set sgx

dont_set_sgx:
ld r3,#0
jp start_load

set_sgx:
ld r3,#1

start_load:
ld r1,#8
pmpw r1,r3 // Write is_sgx = 1

ld r1,#0 // Set address for write
ld r2,#1 // Downloading start
pmpw r1,r2 // Write ioctl_download = 1

ld r1,#rom_dataslot
ld r14,#load_err_msg
loadf r1 // Load ROM
jp nz,print_error_and_exit

ld r1,#0 // Set address for write
ld r2,#0 // Downloading end
pmpw r1,r2 // Write ioctl_download = 0

ld r1,#4 // Set address for write
ld r2,#1 // Downloading start
pmpw r1,r2 // Write save_download = 1

ld r1,#save_dataslot
loadf r1 // Load save

ld r1,#4 // Set address for write
ld r2,#0 // Downloading end
pmpw r1,r2 // Write save_download = 0

// Start core
ld r0,#host_init
host r0,r0
ld r15,#1 // This sets the CPU to have a flag that this core has been started up and this is a running core
exit 0

reload_cd_assets:
ld r4,#0	// write dataslot 0
ld r5,#0xf8002000
pmpw r5,r4

ld r4,#0	// write dataslot 0 data size
add r5,#4
pmpw r5,r4

ld r4,#1	// write dataslot 1
add r5,#4
pmpw r5,r4

ld r6,#1
open r6,r4	// write dataslot 1 data size
close
add r5,#4
pmpw r5,r4

ld r4,#2	// write dataslot 2	
add r5,#4
pmpw r5,r4

ld r4,#0	// write dataslot 2 data size	
add r5,#4
pmpw r5,r4

ld r4,#3	// write dataslot 3	
add r5,#4
pmpw r5,r4

ld r6,#3
open r6,r4	// write dataslot 3 data size	
close
add r5,#4
pmpw r5,r4


ld r4,#json_dataslot_start // We load the starting dataslot pointer

reload_cd_assets_loop:
queryslot r4 // check it
jp z,reload_cd_assets_process // jump if nothing there
jp reload_cd_assets_increase

// What do we do here now 

reload_cd_assets_process:


add r5,#4	// we do the add to the address for the data slots again
pmpw r5,r4	// write the dataslot number to that address

open r4,r6	// read the local data slot for the size	
close		// we have to close the file
add r5,#4	// add 4 to the address
pmpw r5,r6  // write it 


// We increase the dataslot 
reload_cd_assets_increase: // we increase the data slot
add r4,#1	// increase the dataslot ref number
cmp r4,#128	// 129 is the highest number of slots
jp c,reload_cd_assets_loop // we loop back YAY

// update the slots 

ld r4,#0xf8000020 // we are doing a hack with the host stuff. where you can write to the dataslot update regs in the core
pmpw r4,r4		 // this writes to the address f8000020 which is the dataslot ID

ld r4,#json_dataslot_start // We load the starting dataslot pointer
open r4,r6	// we want the size of this 

ld r4,#0xf8000024	// The address for the dataslot update 
pmpw r4,r6			// write it 

ld r4,#0xf8000000
ld r6,#0x434D008A
pmpw r4,r6			// this send the host command to the core 

reload_cd_assets_done: 

exit 0

// Error handling
error_handler:
ld r14,#test_err_msg

print_error_and_exit:
printf r14
exit 1

ext_sgx:
db "SGX",0

test_err_msg:
db "Error",0

load_err_msg:
db "Could not load ROM",0