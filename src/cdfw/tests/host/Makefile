# Host tests for the CUE loader in src/cdfw/drivers/cue/cue.cpp.
#
#   make test     build natively with AddressSanitizer + UBSan and run
#   make test32   build 32-bit like the MPU and run (needs g++-multilib)

FW  := $(CURDIR)/../..
LIB := $(FW)/lib/sscanf.c $(FW)/lib/vscanf.c $(FW)/lib/stropen.c
SRC := test_cue.cpp host_stubs.h $(FW)/drivers/cue/cue.cpp
SAN := -fsanitize=address,undefined -fno-sanitize-recover=all

# $(call build,<dir>,<flags>)
define build
	mkdir -p $1/stubs
	cd $1/stubs && touch apf.h fileio.h spi.h timer.h riscprintf.h osd_menu.h
	cd $1 && gcc $2 -w -c -I$(FW)/lib -Dsscanf=fw_sscanf $(LIB)
	g++ $2 -std=gnu++17 -g -O1 -I$1/stubs -include host_stubs.h -o $1/test_cue test_cue.cpp $1/*.o
endef

test: build/native/test_cue
	./$<

test32: build/m32/test_cue
	./$<

build/native/test_cue: $(SRC)
	$(call build,build/native,$(SAN))

build/m32/test_cue: $(SRC)
	$(call build,build/m32,-m32)

clean:
	rm -rf build

.PHONY: test test32 clean
