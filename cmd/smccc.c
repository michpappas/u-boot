/*
 * Copyright 2018
 * Michalis Pappas <mpappas@fastmail.fm>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <asm/psci.h>
#include <common.h>
#include <command.h>
#include <linux/arm-smccc.h>
#include <linux/compiler.h>
#include <linux/psci.h>

/*
 * Identify a TSP service from function ID filtering the last 16 bits from the
 * SMC function ID
 */
#define TSP_BARE_FID(fid)	((fid) & 0xffff)

/*
 * Generate function IDs for TSP services to be used in SMC calls, by
 * appropriately setting bit 31 to differentiate yielding and fast SMC calls
 */
#define TSP_FAST_FID(fid)	((TSP_BARE_FID(fid) | 0x72000000) | (1u << 31))

static int do_call(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    struct arm_smccc_res res;

    unsigned long fid;

    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
    unsigned long a4;
    unsigned long a5;
    unsigned long a6;
    unsigned long a7;

	if (argc < 2)
		return CMD_RET_USAGE;

    fid = simple_strtoul(argv[1], NULL, 16);

    a1 = argc > 2 ? simple_strtoul(argv[2], NULL, 16) : 0;
    a2 = argc > 3 ? simple_strtoul(argv[3], NULL, 16) : 0;
    a3 = argc > 4 ? simple_strtoul(argv[4], NULL, 16) : 0;
    a4 = argc > 5 ? simple_strtoul(argv[5], NULL, 16) : 0;
    a5 = argc > 6 ? simple_strtoul(argv[6], NULL, 16) : 0;
    a6 = argc > 7 ? simple_strtoul(argv[7], NULL, 16) : 0;
    a7 = 0; /* FIXME: Always pass zero or allow to define as an optional parameter? */

    if (!strcmp(argv[0], "smc"))
        arm_smccc_smc(TSP_FAST_FID(fid), a1, a2, a3, a4, a5, a6, a7, &res);
    else
        arm_smccc_hvc(TSP_FAST_FID(fid), a1, a2, a3, a4, a5, a6, a7, &res);

	printf("Res:  %ld %ld %ld %ld\n", res.a0, res.a1, res.a2, res.a3);

	return 0;
}

#ifdef CONFIG_SMC
U_BOOT_CMD(
	smc,	8,		2,	do_call,
	"Call SMC (Fast calls)",
	"fid a1 a2 a3 a4 a5 a6"
);
#endif

#ifdef CONFIG_HVC
U_BOOT_CMD(
	hvc,	8,		2,	do_call,
	"Call HVC (Fast calls)",
	"fid a1 a2 a3 a4 a5 a6"
);
#endif

