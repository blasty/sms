CC := gcc
LIBS := -lglut -lGLU
#OSX := -framework GLUT -framework OpenGL

all:
	$(CC) $(OSX) -DLSB_FIRST -Wall -o emu z80.c mmu.c key.c vdp.c main.c $(LIBS)
