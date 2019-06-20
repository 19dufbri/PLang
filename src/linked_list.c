#include <stdlib.h>

#include "linked_list.h"

linked_list_t *new_linked_list(void) {
    linked_list_t *list = (linked_list_t *) malloc(sizeof(linked_list_t));
    
    list->head = NULL;
    list->tail = NULL;
    list->iter_prev = NULL;
    list->iter = NULL;
    list->len = 0;
    
    return list;
}

void del_linked_list(linked_list_t *list) {
    list->iter = list->head;
    while (list->iter != NULL) {
        list->head = list->iter;
        list->iter = list->iter->next;
        free(list->head);
    }
    free(list);
}

void linked_list_add(linked_list_t *list, void *data) {
    struct linked_node *next = malloc(sizeof(struct linked_node));
    
    if (list->tail != NULL)
        list->tail->next = next;
    
    list->tail = next;
    
    next->value = data;
    next->next = NULL;
    
    if (list->head == NULL)
	    list->head = next;
    
    list->len++;
}

size_t linked_list_len(linked_list_t *list) {
    return list->len;
}

void linked_list_iter_rewind(linked_list_t *list) {
    list->iter_prev = NULL;
    list->iter = list->head;
}

void *linked_list_iter_next(linked_list_t *list) {
    if (list->iter != NULL) {
        list->iter_prev = list->iter;
        list->iter = list->iter->next;
        return list->iter_prev->value;
    } else {
        return NULL;
    }
}

void linked_list_remove_iter(linked_list_t *list) {
    if (list->iter_prev != NULL && list->iter != NULL) {
        list->iter_prev->value = list->iter->value;
        list->iter_prev->next = list->iter->next;
        free(list->iter);
        list->iter = list->iter_prev;
        list->iter_prev = NULL;
        list->len--;
    }
}

void linked_list_add_iter(linked_list_t *list, void *data) {
    if (list->iter == NULL) {
        // New or end of list
        linked_list_add(list, data);
    } else if (list->iter_prev == NULL) {
        // Beginning of existing list
        struct linked_node *next = malloc(sizeof(struct linked_node));
        next->next = list->head;
        next->value = data;
        list->head = next;
        list->len++;
    } else {
        // General Case
        struct linked_node *next = malloc(sizeof(struct linked_node));
        next->next = list->iter;
        next->value = data;
        list->iter_prev->next = next;
        list->iter = next;
        list->len++;
    }
}
