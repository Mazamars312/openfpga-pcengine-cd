SRC_DIR      = ./
EXTRA_CFLAGS = -c -I../ -I../api -march=rv32imac_zicsr -mabi=ilp32

# Drivers
EXTRA_CFLAGS += -DCONFIG_IRQCTRL
EXTRA_CFLAGS += -DCONFIG_IRQCTRL_BASE=0x00000020
EXTRA_CFLAGS += -DCONFIG_NO_RPC_SUPPORT
EXTRA_CFLAGS += -DCPU_KHZ=742000

# LIB: printf, etc
SRC_DIR     += ../lib

MEM_SIZE=32768
OPT = 2

TARGET=pce_mpu_bios
ARCH=riscv

MAKE_DIR=../make
include $(MAKE_DIR)/makefile.exe
