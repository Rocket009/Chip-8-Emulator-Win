#include <SDL.h>
#include <SDL_events.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_stdinc.h>
#include <SDL_video.h>
#include <stdio.h>
#include <stdint.h>
#include "../Include/Chip-8.h"

#define main main
#define SCREEN_SCALE 10

#define DELAY_MS (1 / CLOCKSPEED) * 1000

#define TIMERCLOCKCYLCLES CLOCKSPEED / 60

int main(int argc, char* args[])
{
	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;
	Chip8_out *Chip8out= NULL;	
	
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not be intitalized! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, VIDEOWIDTH * SCREEN_SCALE, VIDEOHEIGHT * SCREEN_SCALE, SDL_WINDOW_SHOWN);
		SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, VIDEOWIDTH, VIDEOHEIGHT);
		char* dirbuff = "";
		char* numbuff = "";
		if(argc == 0) 
		{
			dirbuff = NULL;
			numbuff = NULL;
		}
		else if(argc == 1) 
		{
			dirbuff = NULL;
			numbuff = NULL;
		}
		else if(argc == 2)
		{
			dirbuff = args[0];
			numbuff = args[1];
		}
		Chip8out = Chip8init(dirbuff, numbuff);
		int quit = 0;
		SDL_Event e;
		int timer_counter = 0;
		while(!quit)
		{
			while(SDL_PollEvent(&e) != 0)
			{
				if(e.type == SDL_QUIT)
				{
					quit = 1;
					Chip8quit();
					SDL_DestroyRenderer(renderer);
					SDL_DestroyWindow(window);
					SDL_Quit();
					return 0;
				}
			}
			
			if(timer_counter < TIMERCLOCKCYLCLES)
			{
				if(*(Chip8out->delay_timer) > 0) *(Chip8out->delay_timer) = *(Chip8out->delay_timer) - 1;

				if(*(Chip8out->sound_timer) > 0) *(Chip8out->sound_timer) = *(Chip8out->delay_timer) - 1;

				timer_counter = TIMERCLOCKCYLCLES;
			}
			else
			{
				timer_counter++;
			}
			if(Fetch_Instruction() != 0) return 1;

			SDL_UpdateTexture(texture, NULL,Chip8out->vram , sizeof(Chip8out->vram[0]) * VIDEOWIDTH);
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			//SDL_Delay(1);
			for(long i = 0; i < 16 * 10 * 10 * 10 * 10; i++)
			{

			}
		}
	}
	
	return 0;
}
