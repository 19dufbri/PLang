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

	case 0xFE: // NOP
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
	memcpy(map, get_pc_long(core), core->B);
	core->A = ((uint64_t (*)(uint64_t))map)(core->A);
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
