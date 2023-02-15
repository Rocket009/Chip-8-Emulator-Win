#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

//memory size in bytes
#define MEMORYSIZE 4096

//stack size in levels
#define STACKSIZE 16

//start location of program
#define STARTLOCATIONMEM 0x200

//number of 8-bit registers
#define NUMOFREGISTERS 16

//length of the font array
#define FONTLENGTH 80

//memory address where the font is stored
#define FONTSTARTADDRESS 0x50

//height of the prescaleded video
#define VIDEOHEIGHT 32

//width of the prescaled video
#define VIDEOWIDTH 64

//directory of the program to run
#define programloc "C:\\Users\\Hunter Rabun\\Desktop\\octojam2title.ch8"

//clock speed in Hz
#define CLOCKSPEED 300

//if defined will set the y register when doing a bit shift 
//#define SETYBITSHIFT

//if defined will use the 2nd nibble to add to jump
//#define JUMPOFFSETREGISTER

int Read_Program_Binary(char *directory);

int Fetch_Instruction(void);

uint16_t get_nibble(uint8_t index, uint16_t instruction);

void Chip8quit(void);

typedef struct CHIP8_OUT
{
    uint32_t *vram;
    uint8_t *delay_timer;
    uint8_t *sound_timer;

} Chip8_out;

Chip8_out* Chip8init(char* programdir, char* clockspeed);

#endif
