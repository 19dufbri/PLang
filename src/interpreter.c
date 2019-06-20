#include <stdio.h>

typedef struct inter_core {
    uint64_t     A;
    uint64_t     B;
    uint64_t    PC;
    uint64_t    SP;
    uint8_t     FLAG;
    mapper_t    mapper;
} inter_core_t;

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

void run_opcode(inter_core_t *core) {
    uint8_t opcode = get_pc_byte(core);

    switch(opcode) {
        case 0x00: // NOP
            break;
        case 0x01: // LDA
            uint64_t addr = get_pc_long(core);
            core->A = get_long(core, addr);
            break;
        case 0x02: // LDB
            uint64_t addr = get_pc_long(core);
            core->B = get_long(core, addr);
            break;
        case 0x03: // STA
            uint64_t addr = get_pc_long(core);
            put_long(core, addr, core->A);
            break;
        case 0x04: // STB
            uint64_t addr = get_pc_long(core);
            put_long(core, addr, core->B);
            break;
        case 0x05: // ADD
            core->A += core->B;
            break;
        case 0x06: // SUB
            core->A -= core->B;
            break;
        case 0x07: // CAL
            uint64_t addr = get_pc_long(core);
            push_stack_long(core, core->PC);
            core->PC = addr;
            break;
        case 0x08: // RET
            core->PC = pop_stack_long(core);
            break;
        case 0x09: // SWP
            uint64_t tmp = core->A;
            core->A = core->B
            core->B = core->A
            break;
        case 0x0A: // CMP
        case 0x0B: // JMP
        case 0x0C: // JEQ
        case 0x0D: // JGT
        case 0x0E: // PUS
        case 0x0F: // POP
        case 0xF0: // PRT
            char *addr = (char *) get_pc_long(core);
            printf("%s", addr);
            break;
        case 0xFF: // HLT
            exit(core->A);
            break;
    }
}

uint8_t get_byte(inter_core_t *core, uint64_t addr) {
    return mapped_read(core->mapper, addr);
}

uint16_t get_short(inter_core_t *core, uint64_t addr) {
    return get_byte(core, addr) + get_byte(core, addr+1) << 8;
}

uint32_t get_int(inter_core_t *core, uint64_t addr) {
    return get_short(core, addr) + get_short(core, addr+2) << 16;
}

uint64_t get_long(inter_core_t *core, uint64_t addr) {
    return get_int(core, addr) + get_int(core, addr+4) << 32);
}

void put_byte(inter_core_t *core, uint64_t addr, uint8_t data) {
    mapped_write(core->mapper, addr, data);
}

void put_short(inter_core_t *core, uint64_t addr, uint16_t data) {
    put_byte(core, addr, data & 0xFF);
    put_byte(core, addr+1, data >> 8);
}

void put_int(inter_core_t *core, uint64_t addr, uint32_t data) {
    put_short(core, addr, data & 0xFFFF);
    put_short(core, addr+2, data >> 16);
}

void put_long(inter_core_t *core, uint64_t addr, uint64_t data) {
    put_int(core, addr, data & 0xFFFFFFFF);
    put_int(core, addr+4, data >> 32);
}

uint8_t get_pc_byte(inter_core_t *core) {
    return get_byte(core, core->PC++);
}

uint16_t get_pc_short(inter_core_t *core) {
    uint16_t result = get_short(core, core->PC);
    core->PC += 2;
    return result;
}

uint32_t get_pc_int(inter_core_t *core) {
    uint32_t result = get_int(core, core->PC);
    core->PC += 4;
    return result;
}

uint64_t get_pc_long(inter_core_t *core) {
    uint64_t result = get_int(core, core->PC);
    core->PC += 8;
    return result;
}

uint8_t pop_stack_byte(inter_core_t *core) {
    return get_byte(core, core->SP++);
}

uint16_t pop_stack_short(inter_core_t *core) {
    uint16_t result = get_short(core, core->SP);
    core->SP += 2;
    return result;
}

uint32_t pop_stack_int(inter_core_t *core) {
    uint32_t result = get_short(core, core->SP);
    core->SP += 4;
    return result;
}

uint64_t pop_stack_long(inter_core_t *core) {
    uint64_t result = get_short(core, core->SP);
    core->SP += 8;
    return result;
}

void push_stack_byte(inter_core_t *core, uint8_t data) {
    put_byte(core, --core->SP, data);
}

void push_stack_short(inter_core_t *core, uint16_t data) {
    core->SP -= 2;
    put_short(core, core->SP, data);
}

void push_stack_int(inter_core_t *core, uint32_t data) {
    core->SP -= 4;
    put_int(core, core->SP, data);
}

void push_stack_long(inter_core_t *core, uint64_t data) {
    core->SP -= 8;
    put_long(core, core->SP, data);
}
