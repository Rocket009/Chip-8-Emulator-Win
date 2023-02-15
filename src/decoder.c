#include "../include/Chip-8.h"
#include "../include/intructions.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable:4996)

uint8_t *registers;

uint16_t pc;

size_t program_size;

uint8_t *memory;

uint8_t *extra_memory;

uint16_t *stack;

uint8_t sp;

uint16_t index_reg;

uint32_t *vram;

uint16_t initialpc;

uint8_t delay_timer;

uint8_t sound_timer;

uint8_t keydown_check;

void zerocommands(uint16_t instruction);




const uint8_t font[FONTLENGTH] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


Chip8_out* Chip8init(char *programdir, uint16_t clockspeed)
{
    //Allocate memory
    memory = (uint8_t*) malloc(MEMORYSIZE);
    stack = (uint16_t*) malloc(sizeof(uint16_t) * STACKSIZE);
    registers = (uint8_t*) malloc(sizeof(uint8_t) * NUMOFREGISTERS);
    vram = (uint32_t*) malloc(VIDEOHEIGHT * VIDEOWIDTH * sizeof(uint32_t));
    Chip8_out *out = (Chip8_out*) malloc(sizeof(Chip8_out));
    memset(vram, 0, VIDEOHEIGHT * VIDEOWIDTH * sizeof(uint32_t));
    pc = STARTLOCATIONMEM;
    initialpc = pc;
    delay_timer = 0;
    sound_timer = 0;
    keydown_check = 0;
    //set font to memory
    for(int index = 0; index < FONTLENGTH; index++)
    {
        memory[FONTSTARTADDRESS + index] = font[index];
    }

	char* dirbuff = programloc;
	uint16_t* clockbuff = (uint16_t)CLOCKSPEED;
	if(programdir == NULL)
	{
		dirbuff = programdir;
	}
    Read_Program_Binary(programloc);
    srand(time(NULL));
    out->vram = vram;
    out->delay_timer = &delay_timer;
    out->sound_timer = &sound_timer;

    return out;
}
int Read_Program_Binary(char *directory)
{
    FILE *file_ptr = NULL;
    file_ptr = fopen(directory, "rb");
    if (file_ptr == NULL)
    {
        printf("Error opening files");
        return NULL;
    }

    if(file_ptr == NULL) return 1;

    size_t filesize;

    fseek(file_ptr, 0, SEEK_END);

    filesize = ftell(file_ptr);

    program_size = filesize;

    rewind(file_ptr);

    if(filesize < 1) 
    {
        fclose(file_ptr);
        printf("ERROR File size is less than 1");
        return 1;
    }
    
    uint8_t* buff = (uint8_t*) malloc(filesize);

    if(fread(buff, filesize, 1, file_ptr) == 0) 
    {
        fclose(file_ptr);
        return 1;
    }

    int index2 = STARTLOCATIONMEM;

    //Assign buffer value to memory
    for(int index = 0; index < filesize; index++)
    {
        memory[index2] = buff[index];
        index2++;
    }

    //Check if there is any extra memory after program has been loaded
    if(0xFFF - index2 > 16)
    {
        extra_memory = &memory[index2];
    }

    //else set the extra memory ptr to NULL
    else 
    {
        extra_memory = NULL;
    }

    fclose(file_ptr);
    free(buff);

    return 0; 
}

int Fetch_Instruction(void)
{
    uint16_t instruction;

    if((initialpc + program_size) >= (pc + 1))
    {
        instruction = (memory[pc] << 8) | memory[pc + 1];
        pc += 2;
        printf("%x", instruction);
    }
    else
    {
        return 1;
    }
    

    switch(get_nibble(1, instruction))
    {
        case 0x0: 
        {
            if(instruction == 0x00E0) clear_screen(vram);
            else if(instruction == 0x00EE) return_from_sub(&pc, stack, &sp);
        }
        break;

        case 0x1: jump(get_nibble(234, instruction), &pc);
        break;

        case 0x2: call_sub(get_nibble(234, instruction), &pc, stack, &sp);
        break;

        case 0x3: skip_if_equal(&pc, registers, get_nibble(2, instruction), get_nibble(34, instruction));
        break;

        case 0x4: skip_if_not_equal(&pc, registers, get_nibble(2, instruction), get_nibble(34, instruction));
        break;

        case 0x5: skip_if_reg_equal(&pc, registers, get_nibble(2, instruction), get_nibble(3, instruction));
        break;

        case 0x6: set_register(registers, get_nibble(2, instruction), get_nibble(34, instruction));
        break;

        case 0x7: add_register(registers, get_nibble(2, instruction), get_nibble(34, instruction));
        break;

        case 0x8:
        {
            switch(get_nibble(4, instruction))
            {
                case 0x0: set_reg_to(registers, get_nibble(2, instruction), get_nibble(3, instruction));
                break;

                case 0x1: bin_or(registers, get_nibble(2, instruction), get_nibble(3, instruction));
                break;

                case 0x2: bin_and(registers, get_nibble(2, instruction), get_nibble(3, instruction));
                break;

                case 0x3: bin_xor(registers, get_nibble(2, instruction), get_nibble(3, instruction));
                break;

                case 0x4: bin_add(registers, get_nibble(2, instruction), get_nibble(3, instruction));
                break;

                case 0x5: bin_sub_xy(registers, get_nibble(2, instruction), get_nibble(3, instruction));
                break;

                case 0x6: bin_shift_right(registers, get_nibble(2, instruction), get_nibble(3, instruction));
                break;

                case 0x7: bin_sub_yx(registers, get_nibble(2, instruction), get_nibble(3, instruction));
                break;

                case 0xE: bin_shift_left(registers, get_nibble(2, instruction), get_nibble(3, instruction));
                break;
            }
        }
        break;

        case 0x9: skip_if_reg_not_equal(&pc, registers, get_nibble(2, instruction), get_nibble(3, instruction));
        break;

        case 0xA: set_index_reg(&index_reg, get_nibble(234, instruction));
        break;

        case 0xB: jump_offset(get_nibble(234, instruction), &pc, registers, get_nibble(2, instruction));
        break;

        case 0xC: bin_random(registers, get_nibble(2, instruction), get_nibble(34, instruction));
        break;

        case 0xD: Chip8display(get_nibble(2, instruction), get_nibble(3, instruction), get_nibble(4, instruction), registers, vram, memory, &index_reg);
        break;

        case 0xE:
        {
            switch(get_nibble(34, instruction))
            {
                case 0x9E: skip_if_key(&pc, get_nibble(2, instruction));
                break;

                case 0xA1: skip_if_not_key(&pc, get_nibble(2, instruction));
                break;
            }
        }
        break;

        case 0xF:
        {
            switch(get_nibble(34, instruction))
            {
                case 0x07: get_delay_timer(&delay_timer, registers, get_nibble(2, instruction));
                break;

                case 0x15: set_delay_timer(&delay_timer, registers, get_nibble(2, instruction));
                break;

                case 0x18: set_sound_timer(&sound_timer, registers, get_nibble(2, instruction));
                break;

                case 0x1E: add_to_index(&index_reg, registers, get_nibble(2, instruction));
                break;

                case 0x0A: 
                {
                    keydown_check = 0;
                    get_key(&pc, registers, &keydown_check);
                }
                break;

                case 0x29: font_char(&index_reg, registers, get_nibble(2, instruction));
                break;

                case 0x33: decimal_convert(&index_reg, registers, get_nibble(2, instruction), memory);
                break;

                case 0x55: store_regs(&index_reg, registers, memory, get_nibble(2, instruction));
                break;

                case 0x65: load_regs(&index_reg, registers, memory, get_nibble(2, instruction));
                break;             
            }
        }
        break;

        default: 
        {
            printf("Unkown instruction error: %x", instruction);
            return 1;
        }

    }
    return 0;
}

uint16_t get_nibble(uint8_t index, uint16_t instruction)
{
    switch(index)
    {
        case 1: return ((0xF << 12) & instruction) >> 12;
        case 2: return ((0xF << 8) & instruction) >> 8;
        case 3: return ((0xF << 4) & instruction) >> 4;
        case 4: return 0xF & instruction;
        case 12: return ((0xFF << 8) & instruction) >> 8;
        case 13: return (((0xF << 12) & instruction) | ((0xF << 4) & instruction) << 4) >> 8;
        case 14: return ((0xF << 12) & instruction) | ((0xF & instruction) << 8) >> 8;
        case 23: return ((0xF << 8) & instruction) | (((0xF << 4) & instruction) << 4) >> 4;
        case 34: return 0xFF & instruction;
        case 234: return 0xFFF & instruction;
        default: return 0;
    }
}

void Chip8quit(void)
{
    free((void*)registers);
    free((void*)memory);
    free((void*)stack);
    free((void*)vram);
}
