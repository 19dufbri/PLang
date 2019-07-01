#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>

#include "mapper.h"
#include "memory.h"

bool run_opcode(inter_core_t *core);
bool machine_call(inter_core_t *core);

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
    free(mapping);
    del_mapper(mapper);
    return code;
}

bool run_opcode(inter_core_t *core)
{
    uint8_t opcode = get_pc_byte(core);
    uint64_t addr;

    switch (opcode)
    {
    case 0x00: // NOP
        break;
    case 0x08: // STA64
        addr = get_pc_long(core);
        put_long(core, addr, core->A);
        break;
    case 0x09: // STA32
        addr = get_pc_long(core);
        put_int(core, addr, core->A & 0xFFFFFFFF);
        break;
    case 0x0A: // STA16
        addr = get_pc_long(core);
        put_short(core, addr, core->A & 0xFFFF);
        break;
    case 0x0B: // STA8
        addr = get_pc_long(core);
        put_byte(core, addr, core->A & 0xFF);
        break;
    case 0x0C: // STB64
        addr = get_pc_long(core);
        put_long(core, addr, core->B);
        break;
    case 0x0D: // STB32
        addr = get_pc_long(core);
        put_int(core, addr, core->B & 0xFFFFFFFF);
        break;
    case 0x0E: // STB16
        addr = get_pc_long(core);
        put_short(core, addr, core->B & 0xFFFF);
        break;
    case 0x0F: // STB8
        addr = get_pc_long(core);
        put_byte(core, addr, core->B & 0xFF);
        break;

    case 0x10: // LAI64
        core->A = get_pc_long(core);
        break;
    case 0x11: // LAI32
        core->A = get_pc_int(core);
        break;
    case 0x12: // LAI16
        core->A = get_pc_short(core);
        break;
    case 0x13: // LAI8
        core->A = get_pc_byte(core);
        break;
    case 0x14: // LBI64
        core->B = get_pc_long(core);
        break;
    case 0x15: // LBI32
        core->B = get_pc_int(core);
        break;
    case 0x16: // LBI16
        core->B = get_pc_short(core);
        break;
    case 0x17: // LBI8
        core->B = get_pc_byte(core);
        break;
    case 0x18: // LAD64
        addr = get_pc_long(core);
        core->A = get_long(core, addr);
        break;
    case 0x19: // LAD32
        addr = get_pc_long(core);
        core->A = get_int(core, addr);
        break;
    case 0x1A: // LAD16
        addr = get_pc_long(core);
        core->A = get_short(core, addr);
        break;
    case 0x1B: // LAD8
        addr = get_pc_long(core);
        core->A = get_byte(core, addr);
        break;
    case 0x1C: // LBD64
        addr = get_pc_long(core);
        core->B = get_long(core, addr);
        break;
    case 0x1D: // LBD32
        addr = get_pc_long(core);
        core->B = get_int(core, addr);
        break;
    case 0x1E: // LBD16
        addr = get_pc_long(core);
        core->B = get_short(core, addr);
        break;
    case 0x1F: // LBD8
        addr = get_pc_long(core);
        core->B = get_byte(core, addr);
        break;
    
    case 0x20: // LAA64
        core->A = get_long(core, core->A);
        break;
    case 0x21: // LAA32
        core->A = get_int(core, core->A);
        break;
    case 0x22: // LAA16
        core->A = get_short(core, core->A);
        break;
    case 0x23: // LAA8
        core->A = get_byte(core, core->A);
        break;
    case 0x24: // LBA64
        core->B = get_long(core, core->A);
        break;
    case 0x25: // LBA32
        core->B = get_int(core, core->A);
        break;
    case 0x26: // LBA16
        core->B = get_short(core, core->A);
        break;
    case 0x27: // LBA8
        core->B = get_byte(core, core->A);
        break;
    case 0x28: // LAB64
        core->A = get_long(core, core->B);
        break;
    case 0x29: // LAB32
        core->A = get_int(core, core->B);
        break;
    case 0x2A: // LAB16
        core->A = get_short(core, core->B);
        break;
    case 0x2B: // LAB8
        core->A = get_byte(core, core->B);
        break;
    case 0x2C: // LBB64
        core->B = get_long(core, core->B);
        break;
    case 0x2D: // LBB32
        core->B = get_int(core, core->B);
        break;
    case 0x2E: // LBB16
        core->B = get_short(core, core->B);
        break;
    case 0x2F: // LBB8
        core->B = get_byte(core, core->B);
        break;

    case 0x30: // PHA64
        push_stack_long(core, core->A);
        break;
    case 0x31: // PHA32
        push_stack_int(core, core->A & 0xFFFFFFFF);
        break;
    case 0x32: // PHA16
        push_stack_short(core, core->A & 0xFFFF);
        break;
    case 0x33: // PHA8
        push_stack_byte(core, core->A & 0xFF);
        break;
    case 0x34: // PHB64
        push_stack_long(core, core->B);
        break;
    case 0x35: // PHB32
        push_stack_int(core, core->B & 0xFFFFFFFF);
        break;
    case 0x36: // PHB16
        push_stack_short(core, core->B & 0xFFFF);
        break;
    case 0x37: // PHB8
        push_stack_byte(core, core->B & 0xFF);
        break;
    case 0x38: // PLA64
        core->A = pop_stack_long(core);
        break;
    case 0x39: // PLA32
        core->A = pop_stack_int(core);
        break;
    case 0x3A: // PLA16
        core->A = pop_stack_short(core);
        break;
    case 0x3B: // PLA8
        core->A = pop_stack_byte(core);
        break;
    case 0x3C: // PLB64
        core->B = pop_stack_long(core);
        break;
    case 0x3D: // PLB32
        core->B = pop_stack_int(core);
        break;
    case 0x3E: // PLB16
        core->B = pop_stack_short(core);
        break;
    case 0x3F: // PLB8
        core->B = pop_stack_byte(core);
        break;

    case 0x40: // CAL
        addr = get_pc_long(core);
        push_stack_long(core, core->PC);
        core->PC = addr;
        break;
    case 0x41: // RET
        core->PC = pop_stack_long(core);
        break;
    case 0x42: // JMP
        core->PC = get_pc_long(core);
        break;
    case 0x43: // JEQ
        if (core->FLAG & ZER)
            core->PC = get_pc_long(core);
        else
            get_pc_long(core);
        break;
    case 0x44: // JGT
        if (core->FLAG & NEG)
            get_pc_long(core);
        else
            core->PC = get_pc_long(core);
        break;

    case 0x50: // ADD
        core->A += core->B;
        break;
    case 0x51: // SUB
        core->A -= core->B;
        break;
    case 0x52: // NEA
        core->A = ~core->A;
        break;
    case 0x53: // NEB
        core->B = ~core->B;
        break;
    case 0x54: // AND
        core->A &= core->B;
        break;
    case 0x55: // OR
        core->A |= core->B;
        break;
    case 0x56: // XOR
        core->A ^= core->B;
        break;
    case 0x57: // CMP
        if (core->A == core->B)
            core->FLAG |= ZER;
        else
            core->FLAG &= ~ZER;
        if (core->A < core->B)
            core->FLAG |= NEG;
        else
            core->FLAG &= ~NEG;
        break;
    case 0x5A: // SWP
        addr = core->B;
        core->B = core->A;
        core->A = addr;
        break;
    case 0x5B: // SPC
        addr = core->SP;
        core->SP = core->PC;
        core->PC = addr;
        break;
    case 0x5C: // APC
        addr = core->PC;
        core->PC = core->A;
        core->A = addr;
        break;
    case 0x5D: // ASP
        addr = core->SP;
        core->SP = core->A;
        core->A = addr;
        break;
    case 0x5E: // BPC
        addr = core->B;
        core->B = core->PC;
        core->PC = addr;
        break;
    case 0x5F: // BSP
        addr = core->B;
        core->B = core->SP;
        core->SP = addr;
        break;
    
    case 0x60: // MTP
        core->A -= (uint64_t) memory;
        break;
    case 0x61: // PTM
        core->A += (uint64_t) memory;
        break;
    case 0x62: // MCA
        if (!machine_call(core)) 
            return false;
        break;

    case 0xFF: // HLT
        return false;
        break;
    }
    return true;
}

bool machine_call(inter_core_t *core) {
    void *map = mmap(NULL, core->B, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0);
    if (map == (void *) -1)
        return false;
    memcpy(map, memory + core->A, core->B);
    core->A = ((uint64_t (*)(uint64_t))map)(pop_stack_long(core));
    munmap(map, core->B);
    return true;
}

uint8_t read_memory(uint64_t addr)
{
    return memory[addr];
}

void write_memory(uint64_t addr, uint8_t value)
{
    memory[addr] = value;
}