#!/bin/sh
# Builds the MPU firmware: app/build.riscv.pce_mpu_bios/pce_mpu_bios.bin
#
#   ./build.sh            build
#   ./build.sh clean      remove build output
#
# The makefiles call riscv32-unknown-elf-gcc. When only a riscv64-unknown-elf
# toolchain is installed (Ubuntu 24.04: gcc-riscv64-unknown-elf 13.2.0 and
# libnewlib-dev), it is wrapped to target rv32imac/ilp32 instead. That setup
# rebuilds the 0.2.3 release pce_mpu_bios.bin byte for byte.
set -e
cd "$(dirname "$0")/app"

if ! command -v riscv32-unknown-elf-gcc >/dev/null 2>&1; then
	if ! command -v riscv64-unknown-elf-gcc >/dev/null 2>&1; then
		echo "build.sh: need riscv32-unknown-elf-gcc or riscv64-unknown-elf-gcc on PATH" >&2
		exit 1
	fi
	shim=$(mktemp -d)
	trap 'rm -rf "$shim"' EXIT
	printf '#!/bin/sh\nexec riscv64-unknown-elf-gcc -march=rv32imac_zicsr -mabi=ilp32 "$@"\n' > "$shim/riscv32-unknown-elf-gcc"
	for tool in objcopy objdump; do
		printf '#!/bin/sh\nexec riscv64-unknown-elf-%s "$@"\n' "$tool" > "$shim/riscv32-unknown-elf-$tool"
	done
	chmod +x "$shim"/*
	PATH="$shim:$PATH"
	# Debian and Ubuntu package the newlib headers separately
	if [ -d /usr/include/newlib ]; then
		export C_INCLUDE_PATH=/usr/include/newlib CPLUS_INCLUDE_PATH=/usr/include/newlib
	fi
fi

make "$@"
