#ifndef __VDP_H__
#define __VDP_H__

#include "types.h"

void vdp_init(u32 *fb_addr);

void vdp_write(u8 data);
void vdp_reg_write(u8 addr, u8 data);
u8 vdp_reg_read(int n);

u8 vdp_get_data();
u8 vdp_get_stat();
void vdp_set_stat(u8 v);
void vdp_draw_line();
void vdp_increment_line();

u8 vdp_vcounter();
u8 vdp_hcounter();
void vdp_set_data(u8 v);

#endif
