all:
	gcc -c -o lib/dashgl.o lib/dashgl.c -lGL -lGLEW -lpng
	gcc -I/usr/include/SDL2 -D_REENTRANT main.c lib/dashgl.o -L/usr/lib/x86_64-linux-gnu -lSDL2 -lGL -lGLEW -lm -lpng
