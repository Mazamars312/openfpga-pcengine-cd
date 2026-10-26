// Force-included before cue.cpp when it is built on a PC. The Makefile gives
// cue.h empty apf.h, fileio.h, spi.h, ... and this file declares what cue.cpp
// uses from them. test_cue.cpp implements them against a fake APF.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../drivers/apf/hardware.h"

extern unsigned char fake_scratch[16384];
extern uint32_t fake_dataslot_bram[2];
#undef MAIN_SCRACH_ADDRESS_OFFSET
#define MAIN_SCRACH_ADDRESS_OFFSET ((uintptr_t)fake_scratch)
#undef DATASLOT_BRAM
#define DATASLOT_BRAM(x) (fake_dataslot_bram[(x)])
#define APF_SCRACH_ADDRESS_OFFSET 0x81000000
#define UIO_CD_SET 0x35
#define UIO_CD_DATA 0x36

uint32_t dataslot_read(uint16_t dataslot, uint32_t address, uint32_t offset, uint32_t length);
int GetFileNameDataSlot(dataslot_type *aft);
int OpenFileNameDataSlot(dataslot_type *aft, const char *s);
void clearscrachram(uint32_t offset, uint32_t size);
uint32_t RISCFileSeek(const char *s, uint32_t offset, uint32_t size, uint32_t track, dataslot_type *aft);
uint32_t RISCFileReadAdv(int dataslot, uint8_t cmd0, uint8_t cmd1, uint8_t cmd_hps, uint32_t offset);
void HPS_DisableIO();
uint16_t spi_w(uint16_t word);
uint16_t HPS_spi_uio_cmd_cont(uint16_t cmd);
void osd_display_timing(uint8_t n1, uint8_t n2, uint8_t n3);
int mainprintf(const char *format, ...);

// Use the firmware's own sscanf (lib/sscanf.c, built as fw_sscanf).
extern "C" int fw_sscanf(const char *buf, const char *fmt, ...);
#define sscanf fw_sscanf
