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

	// Instruction Type
	if (opcode & 0x80 == 0x00)
	{ // Load/Store
		uint64_t val;
		// Addressing Modes
		if (opcode & 0x70 == 0x00)
		{ // Immediate
			addr = core->PC;
			core->PC += 8;
		}
		else if (opcode & 0x70 == 0x10)
		{ // Direct
			addr = get_pc_long(core);
		}
		else if (opcode & 0x70 == 0x20)
		{ // SP Offset
			addr = core->SP +  ((int64_t) get_pc_long(core));
		}
		else if (opcode & 0x70 == 0x30)
		{ // PC Offset
			addr = core->PC + ((int32_t) get_pc_long(core));
		}
		else if (opcode & 0x70 == 0x40)
		{ // A Offset
			addr = core->A + ((int16_t) get_pc_long(core));
		}
		else if (opcode & 0x70 == 0x50)
		{ // B Offset
			addr = core->B + ((int8_t) get_pc_long(core));
		}

		// Load / Store
		if (opcode & 0x01 == 0x00)
		{ // Load
			if (opcode & 0x06 == 0x00)
				val = get_long(core, addr);
			else if (opcode & 0x06 == 0x02)
				val = get_int(core, addr);
			else if (opcode & 0x06 == 0x04)
				val = get_short(core, addr);
			else if (opcode & 0x06 == 0x06)
				val = get_byte(core, addr);

			if (opcode & 0x08 == 0x00)
				core->A = val;
			else if (opcode & 0x08 == 0x08)
				core->B = val;
		}
		else if (opcode & 0x01 == 0x01)
		{ // Store
			if (opcode & 0x08 == 0x00)
				val = core->A;
			else if (opcode & 0x08 == 0x08)
				val = core->B;

			if (opcode & 0x06 == 0x00)
				put_long(core, addr, val);
			else if (opcode & 0x06 == 0x02)
				put_int(core, addr, val);
			else if (opcode & 0x06 == 0x04)
				put_short(core, addr, val);
			else if (opcode & 0x06 == 0x06)
				put_byte(core, addr, val);
		}
	}
	else if (opcode & 0xF0 == 0x80)
	{ // Stack Instructions
		uint64_t val;
		if (opcode & 0x01 == 0x00)
		{ // Pop
			if (opcode & 0x06 == 0x00)
				val = pop_stack_long(core);
			else if (opcode & 0x06 == 0x02)
				val = pop_stack_int(core);
			else if (opcode & 0x06 == 0x04)
				val = pop_stack_short(core);
			else if (opcode & 0x06 == 0x06)
				val = pop_stack_byte(core);
			
			if (opcode & 0x08 == 0x00)
				core->A = val;
			else if (opcode & 0x08 == 0x08)
				core->B = val;
		}
		else if (opcode & 0x01 == 0x011)
		{ // Push
			if (opcode & 0x08 == 0x00)
				val = core->A;
			else if (opcode & 0x08 == 0x08)
				val = core->B;
			
			if (opcode & 0x06 == 0x00)
				push_stack_long(core, val);
			else if (opcode & 0x06 == 0x02)
				push_stack_int(core, val);
			else if (opcode & 0x06 == 0x04)
				push_stack_short(core, val);
			else if (opcode & 0x06 == 0x06)
				push_stack_byte(core, val);
		}
	}
	else
	{ // Other Instructions
		switch (opcode)
		{
		case 0x90: // CAL
			addr = get_pc_long(core);
			push_stack_long(core, core->PC);
			core->PC = addr;
			break;
		case 0x91: // RET
			core->PC = pop_stack_long(core);
			break;
		case 0x92: // JMP
			core->PC = get_pc_long(core);
			break;
		case 0x93: // JEQ
			if (core->FLAG & ZER)
				core->PC = get_pc_long(core);
			else
				get_pc_long(core);
			break;
		case 0x94: // JGT
			if (core->FLAG & NEG)
				get_pc_long(core);
			else
				core->PC = get_pc_long(core);
			break;

		case 0xA0: // ADD
			core->A += core->B;
			break;
		case 0xA1: // SUB
			core->A -= core->B;
			break;
		case 0xA2: // NEA
			core->A = ~core->A;
			break;
		case 0xA3: // NEB
			core->B = ~core->B;
			break;
		case 0xA4: // AND
			core->A &= core->B;
			break;
		case 0xA5: // OR
			core->A |= core->B;
			break;
		case 0xA6: // XOR
			core->A ^= core->B;
			break;
		case 0xA7: // CMP
			if (core->A == core->B)
				core->FLAG |= ZER;
			else
				core->FLAG &= ~ZER;
			if (core->A < core->B)
				core->FLAG |= NEG;
			else
				core->FLAG &= ~NEG;
			break;
		case 0xAA: // SWP
			addr = core->B;
			core->B = core->A;
			core->A = addr;
			break;
		case 0xAB: // SPC
			addr = core->SP;
			core->SP = core->PC;
			core->PC = addr;
			break;
		case 0xAC: // APC
			addr = core->PC;
			core->PC = core->A;
			core->A = addr;
			break;
		case 0xAD: // ASP
			addr = core->SP;
			core->SP = core->A;
			core->A = addr;
			break;
		case 0xAE: // BPC
			addr = core->B;
			core->B = core->PC;
			core->PC = addr;
			break;
		case 0xAF: // BSP
			addr = core->B;
			core->B = core->SP;
			core->SP = addr;
			break;

		case 0xB0: // MTP
			core->A -= (uint64_t)memory;
			break;
		case 0xB1: // PTM
			core->A += (uint64_t)memory;
			break;
		case 0xB2: // MCA
			if (!machine_call(core))
				return false;
			break;

		case 0xFE: // NOP
			break;
		case 0xFF: // HLT
			return false;
			break;
		}
	}
	return true;
}

bool machine_call(inter_core_t *core)
{
	void *map = mmap(NULL, core->B, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0);
	if (map == (void *)-1)
		return false;
	memcpy(map, get_pc_long(core) + memory, core->B);
	core->A = ((uint64_t(*)(uint64_t))map)(core->A);
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
