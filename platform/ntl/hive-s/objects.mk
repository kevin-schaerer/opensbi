#
# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (c) 2019 Western Digital Corporation or its affiliates.
# Copyright (c) 2025 Kevin Schaerer <kevin.schaerer@nettimelogic.com>
#

# Compiler pre-processor flags
platform-cppflags-y =

# C Compiler and assembler flags.
platform-cflags-y =
platform-asflags-y =

# Linker flags: additional libraries and object files that the platform
# code needs can be added here
platform-ldflags-y =

#
# Platform RISC-V XLEN, ABI, ISA and Code Model configuration.
# These are optional parameters but platforms can optionaly provide it.
# Some of these are guessed based on GCC compiler capabilities
#
PLATFORM_RISCV_XLEN = 32
PLATFORM_RISCV_ABI = ilp32
#PLATFORM_RISCV_ISA = rv32imac_zicsr_zifencei
#PLATFORM_RISCV_CODE_MODEL = medany

ifeq ($(CC_SUPPORT_ZICSR_ZIFENCEI), y)
	PLATFORM_RISCV_ISA = rv$(PLATFORM_RISCV_XLEN)imac_zicsr_zifencei
else
	PLATFORM_RISCV_ISA = rv$(PLATFORM_RISCV_XLEN)imac
endif

# Space separated list of object file names to be compiled for the platform
platform-objs-y += platform.o

platform-runcmd = echo "NTL Hive-S"

FW_TEXT_START=0x40F00000

FW_DYNAMIC=n

FW_JUMP=y
FW_JUMP_ADDR=0x40040000
FW_JUMP_FDT_ADDR=0x40EF0000

FW_PAYLOAD=n
