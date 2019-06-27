#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "mapper.h"
#include "linked_list.h"

mapper_t *new_mapper(void)
{
    mapper_t *mapper = malloc(sizeof(mapper_t));
    mapper->mapping_list = new_linked_list();
    return mapper;
}

void del_mapper(mapper_t *mapper)
{
    del_linked_list(mapper->mapping_list);
    free(mapper);
}

int add_mapping(mapper_t *mapper, mapping_t *mapping)
{
    linked_list_add(mapper->mapping_list, mapping);
    return mapping->start;
}

void del_mapping(mapper_t *mapper, int mapping_handle)
{
    linked_list_iter_rewind(mapper->mapping_list);

    mapping_t *ptr;
    while ((ptr = (mapping_t *)linked_list_iter_next(mapper->mapping_list)) != NULL)
    {
        if (ptr->start == mapping_handle)
        {
            linked_list_remove_iter(mapper->mapping_list);
            break;
        }
    }
}

uint8_t mapped_read(mapper_t *mapper, uint64_t addr)
{
    linked_list_iter_rewind(mapper->mapping_list);

    mapping_t *ptr;
    while ((ptr = (mapping_t *)linked_list_iter_next(mapper->mapping_list)) != NULL)
        if (addr >= ptr->start && addr < ptr->start + ptr->len)
            return ptr->read_func(addr);

    return 0x0;
}

void mapped_write(mapper_t *mapper, uint64_t addr, uint8_t value)
{
    linked_list_iter_rewind(mapper->mapping_list);

    mapping_t *ptr;
    while ((ptr = (mapping_t *)linked_list_iter_next(mapper->mapping_list)) != NULL)
        if (addr >= ptr->start && addr < ptr->start + ptr->len)
            ptr->write_func(addr, value);
}
