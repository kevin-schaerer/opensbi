/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Dolu1990 <charles.papon.90@gmail.com>
 * Copyright (c) 2025 Kevin Schaerer <kevin.schaerer@nettimelogic.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_const.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_hart.h>

#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/fdt/fdt_fixup.h>
#include <sbi_utils/ipi/aclint_mswi.h>
#include <sbi_utils/irqchip/plic.h>
#include <sbi_utils/serial/litex-uart.h>
#include <sbi_utils/timer/aclint_mtimer.h>

#define NTL_HIVE_S_PLIC_ADDR		0xf0c00000
#define NTL_HIVE_S_PLIC_NUM_SOURCES	4
#define NTL_HIVE_S_HART_COUNT		8
#define NTL_HIVE_S_CLINT_ADDR		0xF0010000
#define NTL_HIVE_S_ACLINT_MTIMER_FREQ	100000000
#define NTL_HIVE_S_ACLINT_MSWI_ADDR	(NTL_HIVE_S_CLINT_ADDR + \
					 CLINT_MSWI_OFFSET)
#define NTL_HIVE_S_ACLINT_MTIMER_ADDR	(NTL_HIVE_S_CLINT_ADDR + \
					 CLINT_MTIMER_OFFSET)
#define NTL_HIVE_S_UART_ADDR		0xf0001000
#define SBI_NTL_HIVE_S_DEFAULT_FEATURES SBI_PLATFORM_HAS_MFAULTS_DELEGATION
#define SBI_NTL_HIVE_S_DEFAULT_HART_STACK_SIZE 0x2000

static struct plic_data plic = {
	.addr = NTL_HIVE_S_PLIC_ADDR,
	.num_src = NTL_HIVE_S_PLIC_NUM_SOURCES,
};

static struct aclint_mswi_data mswi = {
	.addr = NTL_HIVE_S_ACLINT_MSWI_ADDR,
	.size = ACLINT_MSWI_SIZE,
	.first_hartid = 0,
	.hart_count = NTL_HIVE_S_HART_COUNT,
};

static struct aclint_mtimer_data mtimer = {
	.mtime_freq = NTL_HIVE_S_ACLINT_MTIMER_FREQ,
	.mtime_addr = NTL_HIVE_S_ACLINT_MTIMER_ADDR +
		      ACLINT_DEFAULT_MTIME_OFFSET,
	.mtime_size = ACLINT_DEFAULT_MTIME_SIZE,
	.mtimecmp_addr = NTL_HIVE_S_ACLINT_MTIMER_ADDR +
			 ACLINT_DEFAULT_MTIMECMP_OFFSET,
	.mtimecmp_size = ACLINT_DEFAULT_MTIMECMP_SIZE,
	.first_hartid = 0,
	.hart_count = NTL_HIVE_S_HART_COUNT,
	.has_64bit_mmio = true,
};

/*
 * NTL Hive-S early initialization.
 */
static int ntl_hive_s_early_init(bool cold_boot)
{
	litex_uart_init(NTL_HIVE_S_UART_ADDR);

	if (!cold_boot)
		return 0;

	return 0;
}

/*
 * NTL Hive-S final initialization.
 */
static int ntl_hive_s_final_init(bool cold_boot)
{
	void *fdt;

	if (!cold_boot)
		return 0;

	fdt = fdt_get_address_rw();
	fdt_fixups(fdt);

	return 0;
}

/*
 * Initialize the platform interrupt controller during cold boot.
 */
static int ntl_hive_s_irqchip_init(void)
{
	return plic_cold_irqchip_init(&plic);
}

/*
 * Initialize IPI during cold boot.
 */
static int ntl_hive_s_ipi_init(void)
{
	return aclint_mswi_cold_init(&mswi);
}

/*
 * Initialize platform timer during cold boot.
 */
static int ntl_hive_s_timer_init(void)
{
	return aclint_mtimer_cold_init(&mtimer, NULL);
}

/*
 * NTL Hive-S descriptor.
 */
const struct sbi_platform_operations ntl_hive_sops = {
	.early_init		= ntl_hive_s_early_init,
	.final_init		= ntl_hive_s_final_init,
	.irqchip_init	= ntl_hive_s_irqchip_init,
	.ipi_init		= ntl_hive_s_ipi_init,
	.timer_init		= ntl_hive_s_timer_init
};

const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x01),
	.name				= "NTL Hive-S",
	.features			= SBI_NTL_HIVE_S_DEFAULT_FEATURES,
	.hart_count			= NTL_HIVE_S_HART_COUNT,
	.hart_stack_size	= SBI_NTL_HIVE_S_DEFAULT_HART_STACK_SIZE,
	.heap_size			= SBI_PLATFORM_DEFAULT_HEAP_SIZE(NTL_HIVE_S_HART_COUNT),
	.platform_ops_addr	= (unsigned long)&ntl_hive_sops
};
