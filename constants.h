#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#define SCREEN_W_GL	SCREEN_W_SMS*2
#define SCREEN_H_GL SCREEN_H_SMS*2

#define SCREEN_W_SMS 256
#define SCREEN_H_SMS 240
#define SCREEN_PAL_LINES 313
#define SCREEN_PAL_BOTTOM_BORDER	24
#define SCREEN_PAL_BOTTOM_BLANKING	3
#define SCREEN_PAL_VERTICAL_BLANKING 3
#define SCREEN_PAL_TOP_BLANKING	13
#define SCREEN_PAL_TOP_BORDER	30

#define MEGAHURTZ	3579545
#define FRAMES_PER_SEC	50
#define TICKS_PER_FRAME	(MEGAHURTZ / FRAMES_PER_SEC)
#define TICKS_PER_LINE (TICKS_PER_FRAME / SCREEN_PAL_LINES)

#endif
