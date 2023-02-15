#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#include <stdint.h>
#define INSTRUCTIONS

#include "../include/Chip-8.h"
#include <string.h>

void clear_screen(uint32_t *vram);

void jump(uint16_t pos, uint16_t *pc);

void set_register(uint8_t *reg_ptr, uint8_t reg, uint8_t val);

void add_register(uint8_t *reg_ptr, uint8_t reg, uint8_t val);

void set_index_reg(uint16_t *index_ptr, uint16_t val);

void Chip8display(uint8_t x, uint8_t y, uint8_t h, uint8_t *reg_ptr, uint32_t *vram, uint8_t *memory, uint16_t *index_ptr);

void call_sub(uint16_t pos, uint16_t *pc, uint16_t *stack, uint8_t *sp);

void return_from_sub(uint16_t *pc, uint16_t *stack, uint8_t *sp);

void skip_if_equal(uint16_t *pc, uint8_t *registers, uint8_t reg, uint8_t val);

void skip_if_not_equal(uint16_t *pc, uint8_t *registers, uint8_t reg, uint8_t val);

void skip_if_reg_equal(uint16_t *pc, uint8_t *registers, uint8_t regx, uint8_t regy);

void skip_if_reg_not_equal(uint16_t *pc, uint8_t *registers, uint8_t regx, uint8_t regy);

void set_reg_to(uint8_t *registers, uint8_t regx, uint8_t regy);

void bin_or(uint8_t *registers, uint8_t regx, uint8_t regy);

void bin_and(uint8_t *registers, uint8_t regx, uint8_t regy);

void bin_xor(uint8_t *registers, uint8_t regx, uint8_t regy);

void bin_add(uint8_t *registers, uint8_t regx, uint8_t regy);

void bin_sub_xy(uint8_t *registers, uint8_t regx, uint8_t regy);

void bin_sub_yx(uint8_t *registers, uint8_t regx, uint8_t regy);

void bin_shift_right(uint8_t *registers, uint8_t regx, uint8_t regy);

void bin_shift_left(uint8_t *registers, uint8_t regx, uint8_t regy);

void jump_offset(uint16_t pos, uint16_t *pc, uint8_t *registers, uint8_t reg);

void bin_random(uint8_t *registers, uint8_t reg, uint8_t val);

void skip_if_key(uint16_t *pc, uint8_t key);

void skip_if_not_key(uint16_t *pc, uint8_t key);

void get_delay_timer(uint8_t *delay_timer, uint8_t *registers, uint8_t reg);

void set_delay_timer(uint8_t *delay_timer, uint8_t *registers, uint8_t reg);

void set_sound_timer(uint8_t *sound_timer, uint8_t *registers, uint8_t reg);

void add_to_index(uint16_t *index, uint8_t *registers, uint8_t reg);

void get_key(uint16_t *pc, uint8_t *registers, uint8_t *keydown_check);

void font_char(uint16_t *index, uint8_t *registers, uint8_t reg);

void decimal_convert(uint16_t *index, uint8_t *registers, uint8_t reg, uint8_t *memory);

void store_regs(uint16_t *index, uint8_t *registers, uint8_t *memory, uint8_t reg);

void load_regs(uint16_t *index, uint8_t *registers, uint8_t *memory, uint8_t reg);

#endif