#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <GL/glu.h>
	#include <GL/glut.h>
#endif

//#include <z80ex/z80ex.h>
//#include <z80ex/z80ex_common.h>

#include "z80.h"

#include "types.h"
#include "io.h"
#include "constants.h"
#include "regs_short.h"
#include "mmu.h"
#include "vdp.h"
#include "key.h"

u8 dummy_write[0x1000];

/* GL */
GLsizei sw=SCREEN_W_GL,sh=SCREEN_H_GL;
u32 screen[SCREEN_W_SMS*SCREEN_H_SMS];
u32 fps=0, lastFps=0;
u32 waitTime = 1000/60;
u32 frameStartTime = 0;
signed int delayTime;

static Z80EX_CONTEXT *cpu;

void updateFPS() {
	if( ( glutGet( GLUT_ELAPSED_TIME )-lastFps ) > 1000 ) {
		static char buffer[20] = {0};
		sprintf( buffer, "%d FPS", fps );

		glutSetWindowTitle( buffer );
		fps = 0;
		lastFps = glutGet( GLUT_ELAPSED_TIME );
	}

	fps++;
}

void initTexture() {
	int i;

	for( i=0; i<( SCREEN_H_SMS*SCREEN_W_SMS ); i++ ) {
		screen[i] = 0x0000ff00 | ((i^0xff) & 0xff);
	}

	// Create a texture
	glTexImage2D( GL_TEXTURE_2D, 0, 3, SCREEN_W_SMS, SCREEN_H_SMS, 0, GL_BGRA, GL_UNSIGNED_BYTE, ( GLvoid* )screen );

	// Set up the texture
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	// Enable textures
	glEnable( GL_TEXTURE_2D );
}

void updateTexture() {
	// Update Texture
	glTexSubImage2D( GL_TEXTURE_2D, 0 ,0, 0, SCREEN_W_SMS, SCREEN_H_SMS, GL_BGRA, GL_UNSIGNED_BYTE, ( GLvoid* )screen );

	glBegin( GL_QUADS );

	glTexCoord2d( 0.0, 0.0 );
	glVertex2d( 0.0, 0.0 );

	glTexCoord2d( 1.0, 0.0 );
	glVertex2d( sw, 0.0 );

	glTexCoord2d( 1.0, 1.0 );
	glVertex2d( sw, sh );

	glTexCoord2d( 0.0, 1.0 );
	glVertex2d( 0.0, sh );
	glEnd();
}

void display() {
	int y, total;

	glClear( GL_COLOR_BUFFER_BIT );

	// active display period
	for(y = 0; y < SCREEN_H_SMS; y++) {
		z80_execute(TICKS_PER_LINE);
		vdp_draw_line();
		vdp_increment_line();
	}

	for(y = 0; y < SCREEN_PAL_BOTTOM_BORDER; y++) {
		z80_execute(TICKS_PER_LINE);
		vdp_increment_line();
	}

	if (vdp_reg_read(1) & 0x20) {
		printf("!!!!! TRIGGERING VBL.. 0x%x -> ", z80_get_elapsed_cycles());
		vdp_set_stat(vdp_get_stat() | 0x80);
		z80_set_irq_line(0, ASSERT_LINE);
		printf("0x%x\n", z80_get_elapsed_cycles());
	}
	//printf("!!!!! TOOK %d cycles! +++ %d IFF1:%02x\n", z80ex_int(cpu), z80ex_nmi_possible(cpu), z80ex_get_reg(cpu, regIFF1));

	for(y = 0; y < SCREEN_PAL_BOTTOM_BLANKING; y++) {
		z80_execute(TICKS_PER_LINE);
		vdp_increment_line();
	}

	for(y = 0; y < SCREEN_PAL_VERTICAL_BLANKING; y++) {
		z80_execute(TICKS_PER_LINE);
		vdp_increment_line();
	}

	for(y = 0; y < SCREEN_PAL_TOP_BLANKING; y++) {
		z80_execute(TICKS_PER_LINE);
		vdp_increment_line();
	}

	for(y = 0; y < SCREEN_PAL_TOP_BORDER; y++) {
		z80_execute(TICKS_PER_LINE);
		vdp_increment_line();
	}

	updateTexture();
	updateFPS();
	glutSwapBuffers();

	delayTime = waitTime - (glutGet( GLUT_ELAPSED_TIME ) - frameStartTime);
	if(delayTime > 0) {
		usleep((u32)delayTime*1000);
	}
	frameStartTime = glutGet( GLUT_ELAPSED_TIME );
}

void reshape_window( GLsizei w, GLsizei h ) {
	glClearColor( 0.0f, 0.0f, 0.5f, 0.0f );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0, w, h, 0 );
	glMatrixMode( GL_MODELVIEW );
	glViewport( 0, 0, w, h );

	// Resize quad
	sw = w;
	sh = h;
}


int sms_irq_callback() {
	printf("OMFG IRQ CB!!\n");
	return 0xFF;
}

void init_emulator() {
	u8 *m;
	int i;

	/*
	cpu = z80ex_create(
		(z80ex_mread_cb)mmu_mem_read,  NULL,
		(z80ex_mwrite_cb)mmu_mem_write, NULL,
		(z80ex_pread_cb)mmu_io_read,   NULL,
		(z80ex_pwrite_cb)mmu_io_write,  NULL,
		mmu_int_read,  NULL
	);

	z80ex_set_reti_callback(cpu, reti_cb, 0);
	z80ex_reset(cpu);
	*/

	z80_init(0, 0, 0, sms_irq_callback);
	cpu_readport16 = mmu_io_read;
	cpu_writeport16 = mmu_io_write;
	cpu_writemem16 = mmu_mem_write;

	m = mmu_mem_get();

	// internal ROM (0000-BFFF)
	for(i = 0; i < 0x30; i++) {
		cpu_readmap[i] = m + ((i&0x1f) << 10);
		cpu_writemap[i] = dummy_write;
	}

	// internal RAM (C000-FFFF)
	for(i = 0x30; i < 0x40; i++) {
		cpu_readmap[i] = m + (i << 10);
		cpu_writemap[i] = m + (i << 10);
	}

	//data_bus_pullup = 0;
	//data_bus_pulldown = 0;
	
	z80_reset();
	z80_reset_cycle_count();
	z80_set_irq_line(0, 0);
}

int main( int argc, char **argv ) {
	FILE *f;
	u8 *mem;
	int f_sz;

	mem = mmu_mem_get();

	f = fopen(argv[1], "r");
	fseek(f,0,SEEK_END);
	f_sz=ftell(f);
	fseek(f,0,SEEK_SET);
	memset(mem, 0, 0x10000);
	fread(mem, (f_sz < 1024*32) ? f_sz : 1024*32, 1, f);
	fclose(f);

	init_emulator();
	vdp_init(screen);

	// Setup OpenGL
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );

	glutInitWindowSize( SCREEN_W_GL, SCREEN_H_GL );
	glutInitWindowPosition( 320, 288 );
	glutCreateWindow("SMS");

	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape_window);
	glutKeyboardFunc(key_pressed);
	glutKeyboardUpFunc(key_released);
	glutSpecialFunc(key_special_pressed);
	glutSpecialUpFunc(key_special_released);
	initTexture();
	glutMainLoop();

	return 0;
}
