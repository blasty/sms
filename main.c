#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <GLUT/glut.h>
#include <z80.h>
#include "types.h"
#include "io.h"
#include "constants.h"
#include "regs_short.h"
#include "mmu.h"
#include "vdp.h"
#include "key.h"

/* GL */
GLsizei sw=SCREEN_W_GL,sh=SCREEN_H_GL;
u32 screen[SCREEN_W_SMS*SCREEN_H_SMS];
u32 fps=0, lastFps=0;
u32 waitTime = 1000/60;
u32 frameStartTime = 0;
signed int delayTime;

static Z80Context context;

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
        screen[i] = 0x0000ff00 | (i^0xff) & 0xff;
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
	int y;

	glClear( GL_COLOR_BUFFER_BIT );

	// active display period
	for(y = 0; y < SCREEN_H_SMS; y++) {
		Z80ExecuteTStates(&context, TICKS_PER_LINE);
		vdp_draw_line();
		vdp_increment_line();
	}

	for(y = 0; y < SCREEN_PAL_BOTTOM_BORDER; y++) {
		Z80ExecuteTStates(&context, TICKS_PER_LINE);
		vdp_increment_line();
	}

	for(y = 0; y < SCREEN_PAL_BOTTOM_BLANKING; y++) {
		Z80ExecuteTStates(&context, TICKS_PER_LINE);
		vdp_increment_line();
	}

	for(y = 0; y < SCREEN_PAL_TOP_BLANKING; y++) {
		Z80ExecuteTStates(&context, TICKS_PER_LINE);
		vdp_increment_line();
	}

	for(y = 0; y < SCREEN_PAL_TOP_BORDER; y++) {
		Z80ExecuteTStates(&context, TICKS_PER_LINE);
		vdp_increment_line();
	}

	//Z80ExecuteTStates();

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


void init_emulator() {
  context.memRead  = mmu_mem_read;
  context.memWrite = mmu_mem_write;
  context.ioRead   = mmu_io_read;
  context.ioWrite  = mmu_io_write;
}

static void reset() {
  Z80RESET(&context);
}

static void dump_z80_state( void ) {
  printf(
		"%04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x\n",
	  AF1, BC1, DE1, HL1, AF2, BC2, DE2, HL2, IX, IY, SP, PC
	);

	printf(
		"%02x %02x %d %d %d %d %d\n", I, R,
	  IFF1, IFF2, IM, context.halted, context.tstates
	);
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
