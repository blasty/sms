#include <stdio.h>
#include <stdlib.h>
#include <z80.h>

#include "types.h"
#include "vdp.h"
#include "constants.h"

/* VDP */
u8 vdp_addr_high = 0;
u16 reg_vdp_addr = 0;
u16 reg_vdp_code = 0;
u16 map_addr=0, tile_addr=0x3800, pal_addr=0;
u8 reg_vdp_data = 0, reg_vdp_stat = 0;
u8 vdp_regs[12] = { 0x36, 0x80, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0 };
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

		if (reg_vdp_code == 0) {
			reg_vdp_data = vram[reg_vdp_addr];
			reg_vdp_addr++;	
			reg_vdp_addr &= 0x3fff;
		}

		vdp_addr_high = 0;
	}
}

u8 vdp_reg_read(int n) {
	if (n >= 11) { printf(" ILLEGAL!"); exit(-1); }
	return vdp_regs[n];
}



void vdp_reg_write(u8 addr, u8 data) {
	printf("vdp_reg_write(addr:%x, data:%02x)", addr, data);
	if (addr >= 11) { printf(" ILLEGAL!"); exit(-1); }

	switch(addr) {
		case 0x02:
			if (vdp_reg_read(0) & 2) {
				//printf("UH OH MODE FOUR!!! %x\n", vdp_reg_read(0));
			} else {
			}
			map_addr = 0x3800;
		break;

		case 0x04:
			tile_addr = (data << 11) & 0x3800;
		break;

		case 0x05:
			
		break;

		case 0x0a:
			// counter reg can only be written outside of
			// of active display period.
			if (line<225)
				return;	
		break;

		default:
			printf("**** UNHANDLED VDP REG WRITE [%02x]=%02x\n", addr, data);
		break;
	}

	vdp_regs[addr] = data;
}

u8 vdp_get_data() {
	return reg_vdp_data;
}

u8 vdp_get_stat() {
	return reg_vdp_stat;
}

void vdp_set_stat(u8 v) {
	reg_vdp_stat = v;
}

void vdp_set_data(u8 v) {
	reg_vdp_data = v;

	switch(reg_vdp_code) {
		case 0:
			printf("vram_read(%04x)", reg_vdp_addr);
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

unsigned int get_tile(int idx, int y) {
	int a  = tile_addr + (idx*32) + (y*4);

	return (vram[a+3]<<24) | (vram[a+2]<<16) | (vram[a+1]<<8) | vram[a];
}

#define COL(r,g,b) ( ((b>>6)<<4) | ((g>>6)<<2) | (r>>6) )

u8 lolpal[16]={
	COL(0x00,0xff,0x00),
	COL(0xff,0x00,0x00),
	COL(0x00,0xff,0x00),
	COL(0x00,0x00,0xff),
	COL(0xff,0xff,0x00),
	COL(0xff,0x00,0xff),
	COL(0x00,0xff,0xff),
	COL(0xff,0xff,0xff),

	COL(0x00,0x00,0x00),
	COL(0x80,0x00,0x00),
	COL(0x00,0x80,0x00),
	COL(0x00,0x00,0x80),
	COL(0x80,0x80,0x00),
	COL(0x80,0x00,0x80),
	COL(0x00,0x80,0x80),
	COL(0x80,0x80,0x80)
};

void vdp_draw_line() {
	int i, j;
	u32 v;
	u8 *bb = &v;
	u32 *line_ptr = fb+(line*256);

	// background
	for(i=0;i<256/8;i++) {
		v = get_tile(i, line%8);

		for(j=0; j<4; j++) {
			*line_ptr++ = sms2rgb(lolpal[ bb[j]>>4 ]);
			*line_ptr++ = sms2rgb(lolpal[ bb[j]&15 ]);
		}
	}

	//if (line==224) tile_addr+=0x10;

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
