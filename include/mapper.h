#include <stdint.h>

#include "linked_list.h"

#ifndef MAPPER_H
#define MAPPER_H

typedef struct mapping {
    uint64_t start;
    uint64_t len;
    uint8_t (*read_func)(uint64_t addr);
    void (*write_func)(uint64_t addr, uint8_t value);
} mapping_t;

typedef struct mapper {
    linked_list_t *mapping_list;
} mapper_t;

mapper_t *new_mapper(void);                                        // Initialize a new mapper
void del_mapper(mapper_t *mapper);                                  // Delete a mapper

int add_mapping(mapper_t *mapper, mapping_t *mapping);              // Add mapping to mapper
void del_mapping(mapper_t *mapper, int mapping_handle);             // Remove mapping

uint8_t mapped_read(mapper_t *mapper, uint64_t addr);              // Read value from mapper
void mapped_write(mapper_t *mapper, uint64_t addr, uint8_t value); // Write value to mapper

#endif
