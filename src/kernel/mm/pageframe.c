/*
 * pageframe.c
 *
 *  Created on: 29/10/2021
 *      Author: Paulo Almeida
 */

#include "kernel/mm/pageframe.h"
#include "kernel/mm/init.h"
#include "kernel/compiler/bug.h"
#include "kernel/mm/addressconv.h"

static pageframe_database pfdb;

/* calc amount of pageframes that that pfdb must store */
uint64_t pageframe_calc_space_needed(uint64_t pagetable_bytes) {
    return (pagetable_bytes / PAGE_SIZE) * sizeof(struct pageframe_t);
}

void pageframe_init(mem_map_region_t k_pages_struct_rg, mem_map_region_t k_pfdb_struct_rg) {
    /* init page frame database */
    pfdb.free = NULL;
    pfdb.used = NULL;

    /* populate page frame database */
    size_t pfn = k_pfdb_struct_rg.length / sizeof(struct pageframe_t);
    uint64_t pagetable_addr = k_pages_struct_rg.base_addr;
    uint64_t pageframe_addr = k_pfdb_struct_rg.base_addr;

    for (size_t i = 0; i < pfn; i++) {
        /*
         * It's very important that we transform pageframe_addr to virtual address,
         * otherwise, we won't be able to access this linked list after finalising
         * the shift to higher-half memory.
         */
        struct pageframe_t *pf = (struct pageframe_t*) va(pageframe_addr);
        pf->phy_addr = pagetable_addr;

        if (pfdb.free == NULL) {
            /* first entry */
            pf->next = NULL;

        } else {
            /* add to the beginning of the linked list */
            pf->next = pfdb.free;
        }

        /* add to the list of free pageframes */
        pfdb.free = pf;

        /* adjust pointers */
        pagetable_addr += PAGE_SIZE;
        pageframe_addr += sizeof(struct pageframe_t);
    }
}

uint64_t pageframe_alloc(void) {
    /* check if we haven't run out of page frames to allocate */
    BUG_ON(pfdb.free == NULL);

    /* alloc page frame */
    struct pageframe_t *page_frame = pfdb.free;
    uint64_t ret = page_frame->phy_addr;

    /* adjust references to the next free page frame */
    pfdb.free = pfdb.free->next;
    page_frame->next = pfdb.used;
    pfdb.used = page_frame;

    return ret;
}

static struct pageframe_t* find_predecessor(struct pageframe_t *head, uint64_t val) {
    struct pageframe_t *tmp = head;

    if (tmp->phy_addr == val)
        return NULL;

    while (tmp != NULL) {
        if (tmp->next != NULL && tmp->next->phy_addr == val)
            return tmp;
        tmp = tmp->next;
    }

    /* means the item wasn't found */
    BUG_ON(tmp == NULL);
    return NULL;
}

static struct pageframe_t* delete_node(struct pageframe_t **head, uint64_t val) {
    struct pageframe_t *pred = find_predecessor(*head, val);
    struct pageframe_t *item = NULL;

    if (pred == NULL) {
        if ((*head)->phy_addr == val) {
            // the item is at the begining of the list
            item = *head;
            *head = (*head)->next;
        }
    } else {
        item = pred->next;
        pred->next = item->next;
    }

    /* means the item wasn't found */
    BUG_ON(item == NULL);
    return item;
}

void pageframe_free(uint64_t phy_addr) {
    /* sanity check */
    BUG_ON(pfdb.used == NULL);

    /* alloc page frame */
    struct pageframe_t *pf = delete_node(&pfdb.used, phy_addr);

    /* something went terribly wrong for this to be true, ay? */
    BUG_ON(pf == NULL);

    /* prepend item to list */
    pf->next = pfdb.free;
    pfdb.free = pf;
}
