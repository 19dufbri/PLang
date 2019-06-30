#include <stdint.h>
#include "memory.h"
#include "mapper.h"

// Redundant, removes stupid linter error
typedef struct inter_core
{
    uint64_t A;
    uint64_t B;
    uint64_t PC;
    uint64_t SP;
    uint8_t FLAG;
    mapper_t *mapper;
} inter_core_t;

uint8_t get_byte(inter_core_t *core, uint64_t addr)
{
    return mapped_read(core->mapper, addr);
}

uint16_t get_short(inter_core_t *core, uint64_t addr)
{
    return get_byte(core, addr) + (((uint16_t)get_byte(core, addr + 1)) << 8);
}

uint32_t get_int(inter_core_t *core, uint64_t addr)
{
    return get_short(core, addr) + (((uint32_t)get_short(core, addr + 2)) << 16);
}

uint64_t get_long(inter_core_t *core, uint64_t addr)
{
    return get_int(core, addr) + (((uint64_t)get_int(core, addr + 4)) << 32);
}

void put_byte(inter_core_t *core, uint64_t addr, uint8_t data)
{
    mapped_write(core->mapper, addr, data);
}

void put_short(inter_core_t *core, uint64_t addr, uint16_t data)
{
    put_byte(core, addr, data & 0xFF);
    put_byte(core, addr + 1, data >> 8);
}

void put_int(inter_core_t *core, uint64_t addr, uint32_t data)
{
    put_short(core, addr, data & 0xFFFF);
    put_short(core, addr + 2, data >> 16);
}

void put_long(inter_core_t *core, uint64_t addr, uint64_t data)
{
    put_int(core, addr, data & 0xFFFFFFFF);
    put_int(core, addr + 4, data >> 32);
}

uint8_t get_pc_byte(inter_core_t *core)
{
    return get_byte(core, core->PC++);
}

uint16_t get_pc_short(inter_core_t *core)
{
    uint16_t result = get_short(core, core->PC);
    core->PC += 2;
    return result;
}

uint32_t get_pc_int(inter_core_t *core)
{
    uint32_t result = get_int(core, core->PC);
    core->PC += 4;
    return result;
}

uint64_t get_pc_long(inter_core_t *core)
{
    uint64_t result = get_int(core, core->PC);
    core->PC += 8;
    return result;
}

uint8_t pop_stack_byte(inter_core_t *core)
{
    return get_byte(core, core->SP++);
}

uint16_t pop_stack_short(inter_core_t *core)
{
    uint16_t result = get_short(core, core->SP);
    core->SP += 2;
    return result;
}

uint32_t pop_stack_int(inter_core_t *core)
{
    uint32_t result = get_short(core, core->SP);
    core->SP += 4;
    return result;
}

uint64_t pop_stack_long(inter_core_t *core)
{
    uint64_t result = get_short(core, core->SP);
    core->SP += 8;
    return result;
}

void push_stack_byte(inter_core_t *core, uint8_t data)
{
    put_byte(core, --core->SP, data);
}

void push_stack_short(inter_core_t *core, uint16_t data)
{
    core->SP -= 2;
    put_short(core, core->SP, data);
}

void push_stack_int(inter_core_t *core, uint32_t data)
{
    core->SP -= 4;
    put_int(core, core->SP, data);
}

void push_stack_long(inter_core_t *core, uint64_t data)
{
    core->SP -= 8;
    put_long(core, core->SP, data);
}