// nolmoonen v1.0.0
#include <stdlib.h>
#include <string.h>

#include "counted_list.h"
#include "log.h"

int create_clist(
        clist_t *t_clist, size_t t_num, size_t t_size,
        int (*t_create_fun)(void *, uint32_t),
        int (*t_delete_fun)(void *, uint32_t)
)
{
    t_clist->m_num = t_num;
    t_clist->m_size = t_size;

    // include space for m_id of every clist_item_t
    if ((t_clist->m_list = malloc(t_num * sizeof(clist_item_t))) == NULL) {
        nm_log(LOG_ERROR, "clist cannot allocate memory\n");
    }

    // todo this memory is not contiguous
    for (uint32_t i = 0; i < t_num; i++) {
        // todo check NULL
        t_clist->m_list[i].m_item = malloc(t_size);
        t_clist->m_list[i].m_access = -1;
    }

    t_clist->m_create_fun = t_create_fun;
    t_clist->m_delete_fun = t_delete_fun;

    return EXIT_SUCCESS;
}

void delete_clist(clist_t *t_clist)
{
    for (uint32_t i = 0; i < t_clist->m_num; i++) {
        if (t_clist->m_list[i].m_access != -1) {
            (*t_clist->m_delete_fun)(t_clist->m_list[i].m_item, t_clist->m_list[i].m_id);
        }
    }

    free(t_clist->m_list);
}

int get_clist(clist_t *t_clist, void *t_item, uint32_t t_id)
{
    /** try to find in buffer */
    for (uint32_t i = 0; i < t_clist->m_num; i++) {
        if (t_clist->m_list[i].m_id == t_id) {
            memcpy(t_item, t_clist->m_list[i].m_item, t_clist->m_size);

            return EXIT_SUCCESS;
        }
    }

    /** not in buffer, find slot for element */
    int32_t index = -1;
    clock_t time = clock(); // unallocated slots have a time of -1
    // there is free space in the list, find an empty spot
    for (uint32_t i = 0; i < t_clist->m_num; i++) {
        if (t_clist->m_list[i].m_access < time) {
            index = i;
            time = t_clist->m_list[i].m_access;
        }
    }

    /** evict item in slot where new element will go, if slot was taken */
    if (t_clist->m_list[index].m_access != -1) {
        if ((*t_clist->m_delete_fun)(t_clist->m_list[index].m_item, t_clist->m_list[index].m_id) == EXIT_FAILURE) {
            nm_log(LOG_ERROR, "clist failed to delete item\n");

            return EXIT_FAILURE;
        }
    }

    /** create item and set in array */
    void *item = malloc(t_clist->m_size);
    if ((*t_clist->m_create_fun)(item, t_id) == EXIT_FAILURE) {
        nm_log(LOG_ERROR, "clist failed to create item\n");
        free(item);

        return EXIT_FAILURE;
    }
    // be safe and assign only when it is known that element is created
    memcpy(t_clist->m_list[index].m_item, item, t_clist->m_size);
    t_clist->m_list[index].m_access = clock();
    t_clist->m_list[index].m_id = t_id;
    free(item);

    /** return a pointer to the actual item */
    memcpy(t_item, t_clist->m_list[index].m_item, t_clist->m_size);

    return EXIT_SUCCESS;
}
