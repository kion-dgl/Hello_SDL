#include <SDL2/SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 270;

int main(int argc,char* args[]) {

	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;

	if (SDL_Init(SDL_INIT_VIDEO)<0) {
		printf("Could not initialize SDL. SDL_Error: %s\n",SDL_GetError());
		exit(1);
	} 

	window = SDL_CreateWindow(
		"SDL Test",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);

	if (window==NULL) {
		printf("Cannot create window. SDL_Error: %s\n",SDL_GetError());
		exit(1);
	} 
	
	SDL_Rect arect;
	screenSurface=SDL_GetWindowSurface(window);
	SDL_FillRect(screenSurface,NULL,SDL_MapRGB(screenSurface->format,0x00,0x00,0x44));
	
	arect.x=64; 
	arect.y=64; 
	arect.w=SCREEN_WIDTH-128; 
	arect.h=SCREEN_HEIGHT-128;
	
	SDL_FillRect(screenSurface,&arect,SDL_MapRGB(screenSurface->format,0x00,0x77,0x77));
	SDL_UpdateWindowSurface(window);
	SDL_Delay(3000);
	
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
