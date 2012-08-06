#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "vdp.h"
#include "constants.h"

/* VDP */
u8 vdp_addr_high = 0;
u16 reg_vdp_addr = 0;
u16 reg_vdp_code = 0;
u8 reg_vdp_data = 0;
u8 vdp_regs[12];
u8 vcounter=0, hcounter=0;
u16 line=0;
u32 *fb;

static u8 vram[ 0x4000 ];
static u8 cram[ 0x4000 ];

void vdp_write(u8 data) {
	if (vdp_addr_high == 0) {
		reg_vdp_addr = data;
		vdp_addr_high = 1;
	} else {
		reg_vdp_addr |= (data & 0x3f)<<8;
		reg_vdp_code = (data >> 6);
		printf("vdp_addr = 0x%04x action:", reg_vdp_addr);

		switch(reg_vdp_code) {
			case 0:
				printf("vram_read(%04x)", reg_vdp_addr);
				reg_vdp_data = vram[reg_vdp_addr];
				reg_vdp_addr++;
				reg_vdp_addr &= 0x3fff;
			break;
			case 1:
				printf("vram_write(%04x, %02x)", reg_vdp_addr, reg_vdp_data);
				vram[reg_vdp_addr] = reg_vdp_data; 
				reg_vdp_addr++;
				reg_vdp_addr &= 0x3fff;
			break;
			case 2:
				vdp_reg_write((reg_vdp_addr >> 8) & 0xf, reg_vdp_addr & 0xff);
			break;
			case 3:
				printf("cram_write(%04x, %02x)", reg_vdp_addr, reg_vdp_data);
				cram[reg_vdp_addr] = reg_vdp_data;
				reg_vdp_addr++;
				reg_vdp_addr &= 0x3fff;
			break;
		}

		vdp_addr_high = 0;
	}
}

void vdp_reg_write(u8 addr, u8 data) {
	printf("vdp_reg_write(addr:%x, data:%02x)", addr, data);
	if (addr >= 11) { printf(" ILLEGAL!"); exit(-1); }

	// counter reg can only be written outside of
	// of active display period.
	if (addr == 0x0a && line < 225) return;

	vdp_regs[addr] = data;
}

u8 vdp_get_data() {
	return reg_vdp_data;
}

u16 vdp_get_addr() {
	return reg_vdp_addr;
}

u8 vdp_vcounter() { return vcounter; }
u8 vdp_hcounter() { return hcounter; }

u32 sms2rgb(u8 col) {
	u8 r,g,b;

	r = ((col>>0)&3) << 6;
	g = ((col>>2)&3) << 6;
	b = ((col>>4)&3) << 6;

	return (r<<16) | (g<<8) | b;
}

void vdp_draw_line() {
	int i;
	u32 *line_ptr = fb+(line*256);
	// background
	for(i=0;i<256;i++)
		*line_ptr++ = sms2rgb(line ^ i);

	// sprites
}

void vdp_increment_line() {
	line++;
	if (line == SCREEN_PAL_LINES)
		line=0;

	if (vcounter == 0)
		vcounter = vdp_regs[0x0a];
	else
		vcounter--;
}

void vdp_init(u32 *fb_addr) {
	fb = fb_addr;
}
