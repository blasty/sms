#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#define SCREEN_W_GL	256*2
#define SCREEN_H_GL 224*2

#define SCREEN_W_SMS 256
#define SCREEN_H_SMS 224
#define SCREEN_PAL_LINES 313
#define SCREEN_PAL_BOTTOM_BORDER	8
#define SCREEN_PAL_BOTTOM_BLANKING	3
#define SCREEN_PAL_VERTICAL_BLANKING 3
#define SCREEN_PAL_TOP_BLANKING	13
#define SCREEN_PAL_TOP_BORDER	11

#define MEGAHURTZ	3579545
#define FRAMES_PER_SEC	50
#define TICKS_PER_FRAME	(MEGAHURTZ / FRAMES_PER_SEC)
#define TICKS_PER_LINE (TICKS_PER_FRAME / SCREEN_PAL_LINES)

#endif