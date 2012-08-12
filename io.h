#ifndef __IO_H__
#define __IO_H__

#define IO_JOYPAD1	0xdc
#define IO_JOYPAD2	0xdd
#define IO_JOYPAD1_MIRR	0xc0
#define IO_JOYPAD2_MIRR	0xc1


#define IO_MEMORY		0x3e
#define IO_REGION		0x3f

#define IO_COUNT_V	0x7e
#define IO_COUNT_H	0x7f
#define IO_PSG_OUT	0x7f

#define IO_VDP_DATA	0xbe
#define IO_VDP_MIRR 0xbd
#define IO_VDP_ADDR	0xbf
#define IO_VDP_STAT	0xbf
#define IO_VDP_CR 	0xbf

#define IO_YM_ADDR	0xf0
#define IO_YM_DATA	0xf1
#define IO_YM_CR		0xf2

#endif
