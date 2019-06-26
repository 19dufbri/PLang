#include <stdint.h>
#include <stdlib.h>

#ifndef LLIST_H
#define LLIST_H

struct linked_node
{
    void *value;
    struct linked_node *next;
};

typedef struct linked_list
{
    struct linked_node *head;
    struct linked_node *tail;
    struct linked_node *iter_prev;
    struct linked_node *iter;
    size_t len;
} linked_list_t;

linked_list_t *new_linked_list(void);                  // Create a new blank list
void del_linked_list(linked_list_t *list);             // Create a new blank list
void linked_list_add(linked_list_t *list, void *data); // Add value to end of list
size_t linked_list_len(linked_list_t *list);           // Get length of linked list

void linked_list_iter_rewind(linked_list_t *list);          // Restart iterator
void *linked_list_iter_next(linked_list_t *list);           // Get value at iterator
void linked_list_remove_iter(linked_list_t *list);          // Remove at iterator
void linked_list_add_iter(linked_list_t *list, void *data); // Add at iterator

#endif