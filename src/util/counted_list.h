// Nol Moonen v1.0.0
#ifndef NM_COUNTED_LIST_H
#define NM_COUNTED_LIST_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
    /** Actual item held in the struct. */
    void *m_item;
    /** Id connected to the resource this item is created for. */
    uint32_t m_id;
    /** Last access time, -1 if never accessed. */
    clock_t m_access;
} clist_item_t;

typedef struct {
    /** max number of elements */
    size_t m_num;
    /** size of a single element */
    size_t m_size;
    /** Array of elements. */
    clist_item_t *m_list;

    /** Function that creates a item in {@param t_item}, based on {@param t_id}. */
    int (*m_create_fun)(void *t_item, uint32_t t_id);

    /** Function that deletes the item in {@param t_item}, based on {@param t_id}. */
    int (*m_delete_fun)(void *t_item, uint32_t t_id);
} clist_t;

int create_clist(
        clist_t *t_clist, size_t t_num, size_t t_size,
        int (*t_create_fun)(void *, uint32_t),
        int (*t_delete_fun)(void *, uint32_t)
);

void delete_clist(clist_t *t_clist);

/** Returns a pointer to the requested object indicated by {@param t_id}. */
int get_clist(clist_t *t_clist, void *t_item, uint32_t t_id);

#endif //NM_COUNTED_LIST_H
