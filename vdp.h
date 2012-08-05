#ifndef __VDP_H__
#define __VDP_H__

#include "types.h"

void vdp_write(u8 data);
void vdp_reg_write(u8 addr, u8 data);

u8 vdp_get_data();
u16 vdp_get_addr();
void vdp_draw_line();
void vdp_increment_line();

u8 vdp_vcounter();
u8 vdp_hcounter();

#endif
