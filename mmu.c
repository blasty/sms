#include <stdio.h>
#include <stdlib.h>
#include "mmu.h"
#include "types.h"
#include "io.h"
#include "vdp.h"

/* 64Kb of RAM */
static u8 memory[ 0x10000 ];

u8 *mmu_mem_get() {
	return memory;
}

u8 mmu_mem_read(u16 a) {
	printf("mem_read[%04x] ", a);

	if (a <= 0xbfff) {
		printf("ROM\n");
		return memory[a];
	}

	if (a >= 0xc000 && a<= 0xdfff) {
		printf("RAM\n");
		return memory[a-0xc000];
	}

	if (a >= 0xe000) {
		printf("RAM (mirror)\n");
		return memory[a-0xe000];
	}

	printf("WTF\n");
	exit(-1);
}

void mmu_mem_write(u16 a, u8 data) {
	printf("mem_write[%04x] = %02x ", a, data);
	if (a<= 0xbfff) {
		printf("ROM. ILLEGAL!\n"); exit(-1);
	}

	if (a >= 0xc000 && a<= 0xcfff) {
		memory[a] = data;
		printf("RAM\n");
		return;
	}

	if (a >= 0xe000) {
		memory[a-0x2000] = data;
		printf("RAM (mirror)\n");
		return;
	}

	printf("UNHANDLED\n");
}

u8 mmu_io_read(u16 address) {
	address &= 0xff;
	printf("IO_READ  %04x ", address);

	switch(address) {
		case IO_JOYPAD1:
		case IO_JOYPAD2:
			printf("joypad%d\n", address-IO_JOYPAD1);
		break;

		case IO_REGION:
			printf("region\n");
		break;

		case IO_COUNT_V:
			printf("vcount\n");
			return vdp_vcounter(); 
		break;

		case IO_PSG_OUT:
			printf("hcounter\n");
			return vdp_hcounter();
		break;

		case IO_VDP_DATA:
			printf("vdp_data\n");
			return vdp_get_data();
		break;

		case IO_VDP_ADDR:
		case IO_VDP_MIRR:
			printf("vdp_addr/vdp_stat\n");
			return vdp_get_stat();
		break;

		case IO_YM_ADDR:
			printf("ym_addr\n");
		break;

		case IO_YM_DATA:
			printf("ym_data\n");
		break;

		case IO_YM_CR:
			printf("ym_cr\n");
		break;

		default:
  		printf("UNKNOWN!\n");
		break;
	}

	return rand() & 0xff;
}

void mmu_io_write(u16 address, u8 data) {
	address &= 0xff;

	printf("IO_write[%02x=%02x] ", address, data);

	switch(address) {
		case IO_JOYPAD1:
		case IO_JOYPAD2:
			printf("joypad%d", address-IO_JOYPAD1);
		break;

		case IO_REGION:
			printf("region");
		break;

		case IO_PSG_OUT:
			printf("psg_out");
		break;

		case IO_VDP_DATA:
			printf("vdp_data");
			vdp_set_data(data);
		break;

		case IO_VDP_ADDR:
		case IO_VDP_MIRR:
			vdp_write(data);
		break;

		case IO_YM_ADDR:
			printf("ym_addr");
		break;

		case IO_YM_DATA:
			printf("ym_data");
		break;

		case IO_YM_CR:
			printf("ym_cr");
		break;

		default:
  		printf("UNKNOWN!");
		break;
	}

	printf("\n");
}

/*
u8 mmu_int_read(Z80EX_CONTEXT *cpu, void *user_data) {
	printf("!!!!!! INT READ1\n");
	return 0xFF;
}
*/
