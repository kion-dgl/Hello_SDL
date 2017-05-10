all:
	gcc -c -o lib/dashgl.o lib/dashgl.c -lGL -lGLEW -lpng
	gcc main.c lib/dashgl.o `sdl2-config --cflags --libs` -lGL -lGLEW -lm -lpng
