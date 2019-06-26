#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

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

bool run_opcode(inter_core_t *core);

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

uint8_t *memory;

uint8_t read_memory(uint64_t addr);
void write_memory(uint64_t addr, uint8_t value);

int main(int argc, char *argv[])
{
    // Usage: ./bint filename
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    mapper_t *mapper = new_mapper();
    mapping_t *mapping = malloc(sizeof(mapping_t));
    mapping->start = 0x00;
    mapping->len = 0x100;
    mapping->read_func = read_memory;
    mapping->write_func = write_memory;
    int mapping_handle = add_mapping(mapper, mapping);

    memory = calloc(0x100, sizeof(uint8_t));
    FILE *infile = fopen(argv[1], "rb");

    fseek(infile, 0L, SEEK_END);
    uint64_t size = ftell(infile);
    rewind(infile);

    for (uint64_t addr = 0x00; addr < size; addr++)
    {
        memory[addr] = fgetc(infile);
    }

    inter_core_t *core = malloc(sizeof(inter_core_t));
    core->A = 0x00;
    core->B = 0x00;
    core->PC = 0x00;
    core->SP = 0x00;
    core->FLAG = 0x00;
    core->mapper = mapper;

    while (run_opcode(core))
    {
    }

    int code = core->A;

    free(core);
    fclose(infile);
    free(memory);
    del_mapping(mapper, mapping_handle);
    del_mapper(mapper);
    return code;
}

uint8_t read_memory(uint64_t addr)
{
    return memory[addr];
}

void write_memory(uint64_t addr, uint8_t value)
{
    memory[addr] = value;
}

bool run_opcode(inter_core_t *core)
{
    uint8_t opcode = get_pc_byte(core);
    uint64_t addr;

    switch (opcode)
    {
    case 0x00: // NOP
        break;
    case 0x01: // LDA
        addr = get_pc_long(core);
        core->A = get_long(core, addr);
        break;
    case 0x02: // LDB
        addr = get_pc_long(core);
        core->B = get_long(core, addr);
        break;
    case 0x03: // STA
        addr = get_pc_long(core);
        put_long(core, addr, core->A);
        break;
    case 0x04: // STB
        addr = get_pc_long(core);
        put_long(core, addr, core->B);
        break;
    case 0x05: // ADD
        core->A += core->B;
        break;
    case 0x06: // SUB
        core->A -= core->B;
        break;
    case 0x07: // CAL
        addr = get_pc_long(core);
        push_stack_long(core, core->PC);
        core->PC = addr;
        break;
    case 0x08: // RET
        core->PC = pop_stack_long(core);
        break;
    case 0x09: // SWP
        addr = core->A;
        core->A = core->B;
        core->B = addr;
        break;
    case 0x0A: // CMP TODO: Complete Opcode
    case 0x0B: // JMP
        core->PC = get_pc_long(core);
        break;
    case 0x0C: // JEQ
        if (core->FLAG & ZER)
            core->PC = get_pc_long(core);
        else
            get_pc_long(core);
        break;
    case 0x0D: // JGT
        if ((core->FLAG & NEG) == 0x0)
            core->PC = get_pc_long(core);
        else
            get_pc_long(core);
        break;
    case 0x0E: // PHA
        push_stack_long(core, core->A);
        break;
    case 0x0F: // PHB
        push_stack_long(core, core->B);
        break;
    case 0x10: // PLA
        core->A = pop_stack_long(core);
        break;
    case 0x11: // PLB
        core->B = pop_stack_long(core);
        break;
    case 0x12: // SPC
        addr = core->A;
        core->A = core->PC;
        core->PC = addr;
        break;
    case 0x13: // SSP
        addr = core->B;
        core->B = core->PC;
        core->PC = addr;
        break;
    case 0xF0: // PRT
        printf("%s", (char *)(get_pc_long(core) + (uint64_t) memory));
        break;
    case 0xFF: // HLT
        return false;
        break;
    }
    return true;
}

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
