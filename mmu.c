#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmu.h"
#include "types.h"
#include "io.h"
#include "vdp.h"

/* 64Kb of RAM */
static u8 memory[ 0x10000 ];
static u8 wram[0x2000];

void mmu_init() {
	//memset(memory, 0, 0x10000);
	memset(wram, 0, 0x2000);
}

u8 *mmu_wram_get() {
	return wram;
}

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
		return wram[a-0xc000];
	}

	if (a >= 0xe000) {
		printf("RAM (mirror)\n");
		return wram[a-0xe000];
	}

	printf("WTF\n");
	exit(-1);
}

void mmu_mem_write(int a, int data) {
	a &= 0xffff;
	data &= 0xff;

	printf("mem_write[%04x] = %02x ", a, data);
	if (a<= 0xbfff) {
		printf("ROM. ILLEGAL!\n"); // exit(-1);
	}

	if (a >= 0xc000 && a<= 0xdfff) {
		wram[a-0xc000] = data;
		printf("RAM\n");
		return;
	}

	if (a >= 0xe000) {
		wram[a-0xe000] = data;
		printf("RAM (mirror)\n");
		return;
	}

	printf("UNHANDLED\n");
	//exit(-1);
}

u8 mmu_io_read(u16 address) {
	address &= 0xff;
	printf("IO_READ  %04x ", address);

	switch(address) {
		case IO_JOYPAD1:
		case IO_JOYPAD2:
			printf("joypad%d\n", address-IO_JOYPAD1);
			return 0;
		break;

		case IO_REGION:
			printf("region -- ");
		break;

		case IO_VDP_DATA:
		case IO_VDP_ADDR:
		case IO_VDP_MIRR:
		case IO_COUNT_V:
		case IO_COUNT_H:
			return vdp_io_read(address);
		break;

		case IO_YM_ADDR:
			printf("ym_addr -- ");
		break;

		case IO_YM_DATA:
			printf("ym_data -- ");
		break;

		case IO_YM_CR:
			printf("ym_cr -- ");
		break;

		default:
  		printf("[%02x] UNKNOWN! ", address);
		break;
	}

	printf("OMG\n");
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

		case IO_VDP_ADDR:
		case IO_VDP_MIRR:
		case IO_VDP_DATA:
			vdp_io_write(address, data);
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
			//exit(-1);
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
