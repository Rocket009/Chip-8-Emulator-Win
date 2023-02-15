#include "../include/intructions.h"
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <stdint.h>


void clear_screen(uint32_t *vram)
{
    //Set all bits in vram to 0 to clear the screen
    memset(vram, 0, VIDEOHEIGHT * VIDEOHEIGHT);
}

void jump(uint16_t pos, uint16_t *pc)
{
    //set program counter to pos
    *pc = pos;

}

void set_register(uint8_t *reg_ptr, uint8_t reg, uint8_t val)
{
    reg_ptr[reg] = val;
}

void add_register(uint8_t *reg_ptr, uint8_t reg, uint8_t val)
{
    reg_ptr[reg] += val;
}

void set_index_reg(uint16_t *index_ptr, uint16_t val)
{
    *index_ptr = val;
}

void Chip8display(uint8_t x, uint8_t y, uint8_t h, uint8_t *reg_ptr, uint32_t *vram, uint8_t *memory, uint16_t *index_ptr)
{
    if(!((x <= 0xF) && (y <= 0xF))) return;

	uint8_t xPos = reg_ptr[x] % VIDEOWIDTH;
    uint8_t yPos = reg_ptr[y] % VIDEOHEIGHT;

    reg_ptr[0xF] = 0;

    for (unsigned int row = 0; row < h; ++row)
	{
		uint8_t spriteByte = memory[*index_ptr + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &vram[(yPos + row) * VIDEOWIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					reg_ptr[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}

		}
	}

	for (unsigned int i = 0; i < VIDEOHEIGHT * VIDEOWIDTH; i++)
	{
		if((vram[i] >= 1) && (vram[i] < 0xFFFFFFFF))
		{
			vram[i] = 0xFFFFFFFF;
		}
	}

}

void call_sub(uint16_t pos, uint16_t *pc, uint16_t *stack, uint8_t *sp)
{
	*sp = *sp + 1;
	stack[*sp] = *pc;
	*pc = pos;
}

void return_from_sub(uint16_t *pc, uint16_t *stack, uint8_t *sp)
{
	*pc = stack[*sp];
	stack[*sp] = 0;
	*sp = *sp - 1;
}

void skip_if_equal(uint16_t *pc, uint8_t *registers, uint8_t reg, uint8_t val)
{
	if(registers[reg] == val) *pc = *pc + 2;
}

void skip_if_not_equal(uint16_t *pc, uint8_t *registers, uint8_t reg, uint8_t val)
{
	if(registers[reg] != val) *pc = *pc + 2;
}

void skip_if_reg_equal(uint16_t *pc, uint8_t *registers, uint8_t regx, uint8_t regy)
{
	if(registers[regx] == registers[regy]) *pc = *pc + 2;
}

void skip_if_reg_not_equal(uint16_t *pc, uint8_t *registers, uint8_t regx, uint8_t regy)
{
	if(registers[regx] != registers[regy]) *pc = *pc + 2;
}

void set_reg_to(uint8_t *registers, uint8_t regx, uint8_t regy)
{
	registers[regx] = registers[regy];
}

void bin_or(uint8_t *registers, uint8_t regx, uint8_t regy)
{
	registers[regx] |= registers[regy];
}

void bin_and(uint8_t *registers, uint8_t regx, uint8_t regy)
{
	registers[regx] &= registers[regy];
}

void bin_xor(uint8_t *registers, uint8_t regx, uint8_t regy)
{
	registers[regx] ^= registers[regy];
}

void bin_add(uint8_t *registers, uint8_t regx, uint8_t regy)
{
	if(registers[regx] + registers[regy] > 0xFF)
	{
		registers[0xF] = 1;
		registers[regx] = (registers[regx] + registers[regy]) & 0xFF;
	}
	else
	{
		registers[regx] = registers[regx] + registers[regy];
	}
}

void bin_sub_xy(uint8_t *registers, uint8_t regx, uint8_t regy)
{
	if(registers[regx] > registers[regy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[regx] = registers[regx] - registers[regy];
}

void bin_sub_yx(uint8_t *registers, uint8_t regx, uint8_t regy)
{
	if(registers[regx] < registers[regy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}
	
	registers[regx] = registers[regy] - registers[regx];
}

void bin_shift_left(uint8_t *registers, uint8_t regx, uint8_t regy)
{
	#ifdef SETYBITSHIFT
		registers[regx] = registers[regy];
	#endif

	if(registers[regx] & 0x80) registers[0xF] = 1;
	else registers[0xF] = 0;

	registers[regx] = registers[regx] << 1;
}

void bin_shift_right(uint8_t *registers, uint8_t regx, uint8_t regy)
{
	#ifdef SETYBITSHIFT
		registers[regx] = registers[regy];
	#endif

	if(registers[regx] & 0x1) registers[0xF] = 1;
	else registers[0xF] = 0;

	registers[regx] = registers[regx] >> 1;
}

void jump_offset(uint16_t pos, uint16_t *pc, uint8_t *registers, uint8_t reg)
{
	#ifndef JUMPOFFSETREGISTER
		*pc = pos + registers[0];
	#endif

	#ifdef JUMPOFFSETREGISTER
		*pc = pos + registers[reg];
	#endif
}

void bin_random(uint8_t *registers, uint8_t reg, uint8_t val)
{
	int randomnum = rand() % 0xFF;
	registers[reg] = randomnum & val;
}

void skip_if_key(uint16_t *pc, uint8_t key)
{
	SDL_Event e;
	int skipped = 0;
	while(SDL_PollEvent(&e) && !skipped)
	{
		switch(e.type)
		{
			case SDLK_0: if(key == 0x0) {*pc = *pc + 2; skipped = 1;}
			break;

			case SDLK_1: if(key == 0x1) {*pc = *pc + 2; skipped = 1;}
			break;
			
			case SDLK_2: if(key == 0x2) {*pc = *pc + 2; skipped = 1;}
			break;

			case SDLK_3: if(key == 0x3) {*pc = *pc + 2; skipped = 1;}
			break;
			
			case SDLK_4: if(key == 0x4) {*pc = *pc + 2; skipped = 1;}
			break;
			
			case SDLK_5: if(key == 0x5) {*pc = *pc + 2; skipped = 1;}
			break;

			case SDLK_6: if(key == 0x6) {*pc = *pc + 2; skipped = 1;}
			break;

			case SDLK_7: if(key == 0x7) {*pc = *pc + 2; skipped = 1;}
			break;

			case SDLK_8: if(key == 0x8) {*pc = *pc + 2; skipped = 1;}
			break;

			case SDLK_9: if(key == 0x9) {*pc = *pc + 2; skipped = 1;}
			break;

			case SDLK_a: if(key == 0xA) {*pc = *pc + 2; skipped = 1;}
			break;

			case SDLK_b: if(key == 0xB) {*pc = *pc + 2; skipped = 1;}
			break;

			case SDLK_c: if(key == 0xC) {*pc = *pc + 2; skipped = 1;}
			break;

			case SDLK_d: if(key == 0xD) {*pc = *pc + 2; skipped = 1;}
			break;

			case SDLK_e: if(key == 0xE) {*pc = *pc + 2; skipped = 1;}
			break;

			case SDLK_f: if(key == 0xF) {*pc = *pc + 2; skipped = 1;}
			break;
		}
	}
}

void skip_if_not_key(uint16_t *pc, uint8_t key)
{
	SDL_Event e;
	int skipped = 0;
	while(SDL_PollEvent(&e) && !skipped)
	{
		switch(e.type)
		{
			case SDLK_0: if(key == 0x0) { skipped = 1;}
			break;

			case SDLK_1: if(key == 0x1) { skipped = 1;}
			break;
			
			case SDLK_2: if(key == 0x2) { skipped = 1;}
			break;

			case SDLK_3: if(key == 0x3) { skipped = 1;}
			break;
			
			case SDLK_4: if(key == 0x4) { skipped = 1;}
			break;
			
			case SDLK_5: if(key == 0x5) { skipped = 1;}
			break;

			case SDLK_6: if(key == 0x6) { skipped = 1;}
			break;

			case SDLK_7: if(key == 0x7) { skipped = 1;}
			break;

			case SDLK_8: if(key == 0x8) { skipped = 1;}
			break;

			case SDLK_9: if(key == 0x9) { skipped = 1;}
			break;

			case SDLK_a: if(key == 0xA) { skipped = 1;}
			break;

			case SDLK_b: if(key == 0xB) { skipped = 1;}
			break;

			case SDLK_c: if(key == 0xC) { skipped = 1;}
			break;

			case SDLK_d: if(key == 0xD) { skipped = 1;}
			break;

			case SDLK_e: if(key == 0xE) { skipped = 1;}
			break;

			case SDLK_f: if(key == 0xF) { skipped = 1;}
			break;
		}
	}
	
	if(skipped == 0) *pc = *pc + 2;
}

void get_delay_timer(uint8_t *delay_timer, uint8_t *registers, uint8_t reg)
{
	registers[reg] = *delay_timer;
}

void set_delay_timer(uint8_t *delay_timer, uint8_t *registers, uint8_t reg)
{
	*delay_timer = registers[reg];
}

void set_sound_timer(uint8_t *sound_timer, uint8_t *registers, uint8_t reg)
{
	*sound_timer = registers[reg];
}

void add_to_index(uint16_t *index, uint8_t *registers, uint8_t reg)
{
	if(registers[reg] + *index > 0xFFFF)
	{
		registers[0xF] = 1;
		*index = (registers[reg] + *index) & 0xFFFF;
	}
	else
	{
		*index = registers[reg] + *index;
	}
}

void get_key(uint16_t *pc, uint8_t *registers, uint8_t *keydown_check)
{
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		if(e.type == SDL_KEYDOWN)
		{
			*keydown_check = 1;
			break;
		}
	}

	if(*keydown_check == 0) *pc = *pc - 2;
}

void font_char(uint16_t *index, uint8_t *registers, uint8_t reg)
{
	*index = FONTSTARTADDRESS + (5 * registers[reg]);
}

void decimal_convert(uint16_t *index, uint8_t *registers, uint8_t reg, uint8_t *memory)
{
	uint8_t digit1, digit2, digit3;
	uint8_t dec = registers[reg];
	digit3 = registers[reg] % 10;
	dec /= 10;
	digit2 = dec % 10;
	dec /= 10;
	digit1 = dec % 10;
	memory[*index] = digit1;
	memory[*index + 1] = digit2;
	memory[*index + 2] = digit3;
}

void store_regs(uint16_t *index, uint8_t *registers, uint8_t *memory, uint8_t reg)
{
	for(int i = 0; i <= reg; i++)
	{
		memory[*index + i] = registers[i];
	}
}

void load_regs(uint16_t *index, uint8_t *registers, uint8_t *memory, uint8_t reg)
{
	for(int i = 0; i <= reg; i++)
	{
		registers[i] = memory[*index + i];
	}
}