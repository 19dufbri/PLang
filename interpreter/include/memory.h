#ifndef INTER_MEM_H
#define INTER_MEM_H

#include <stdint.h>

#include "mapper.h"

typedef struct inter_core
{
    uint64_t A;
    uint64_t B;
    uint64_t PC;
    uint64_t SP;
    uint8_t FLAG;
    mapper_t *mapper;
} inter_core_t;

enum FLAGS
{
    NEG = 0b10000000,
    ZER = 0b01000000
};

uint8_t get_byte(inter_core_t *core, uint64_t addr);
uint16_t get_short(inter_core_t *core, uint64_t addr);
uint32_t get_int(inter_core_t *core, uint64_t addr);
uint64_t get_long(inter_core_t *core, uint64_t addr);

void put_byte(inter_core_t *core, uint64_t addr, uint8_t data);
void put_short(inter_core_t *core, uint64_t addr, uint16_t data);
void put_int(inter_core_t *core, uint64_t addr, uint32_t data);
void put_long(inter_core_t *core, uint64_t addr, uint64_t data);

uint8_t get_pc_byte(inter_core_t *core);
uint16_t get_pc_short(inter_core_t *core);
uint32_t get_pc_int(inter_core_t *core);
uint64_t get_pc_long(inter_core_t *core);

uint8_t pop_stack_byte(inter_core_t *core);
uint16_t pop_stack_short(inter_core_t *core);
uint32_t pop_stack_int(inter_core_t *core);
uint64_t pop_stack_long(inter_core_t *core);

void push_stack_byte(inter_core_t *core, uint8_t data);
void push_stack_short(inter_core_t *core, uint16_t data);
void push_stack_int(inter_core_t *core, uint32_t data);
void push_stack_long(inter_core_t *core, uint64_t data);

#endif