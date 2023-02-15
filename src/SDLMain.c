#include <SDL.h>
#include <SDL_events.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_stdinc.h>
#include <SDL_video.h>
#include <stdio.h>
#include <stdint.h>
#include "../include/Chip-8.h"
#include <time.h>

#define main main
#define SCREEN_SCALE 15


int timer_clock_cycles(int clockspeed1);
void delay_ms(int clockspeed1);

int main(int argc, char** argv)
{
	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;
	Chip8_out *Chip8out= NULL;	
	
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not be intitalized! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}
	else
	{
		window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, VIDEOWIDTH * SCREEN_SCALE, VIDEOHEIGHT * SCREEN_SCALE, SDL_WINDOW_SHOWN);
		SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, VIDEOWIDTH, VIDEOHEIGHT);
		char* dirbuff = (char*)malloc(sizeof(char) * 20);
		int clockspeed = CLOCKSPEED;
		if(argc == 0) 
		{
			dirbuff = NULL;
		}
		else if(argc == 1) 
		{
			dirbuff = argv[0];
		}
		else if(argc == 2)
		{
			dirbuff = argv[0];
			clockspeed = atoi(argv[1]);
		}
		Chip8out = Chip8init(dirbuff, (uint16_t)clockspeed);
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
					free(dirbuff);
					Chip8quit();
					SDL_DestroyRenderer(renderer);
					SDL_DestroyWindow(window);
					SDL_Quit();
					exit(0);
					return 0;
				}
			}
			
			if(timer_counter < timer_clock_cycles(clockspeed))
			{
				if(*(Chip8out->delay_timer) > 0) *(Chip8out->delay_timer) = *(Chip8out->delay_timer) - 1;

				if(*(Chip8out->sound_timer) > 0) *(Chip8out->sound_timer) = *(Chip8out->delay_timer) - 1;

				timer_counter = timer_clock_cycles(clockspeed);
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
			//delay_ms(clockspeed);
		}
	}
	
	return 0;
}


int timer_clock_cycles(int clockspeed1)
{
	return clockspeed1 / 60;
}

void delay_ms(int clockspeed1)
{
	clock_t clock1 = clock();
	float flclock = clockspeed1;
	float ms = (1 / flclock) * 1000;
	for(long long int i = 0; i < (CLOCKS_PER_SEC * ms); i++) {}
}