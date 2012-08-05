CC := gcc
LIBS := -lz80

all:
	$(CC) -framework GLUT -framework OpenGL -o emu mmu.c key.c vdp.c main.c $(LIBS)
