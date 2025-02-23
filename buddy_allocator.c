#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "./buddy_allocator_util.h"

void buddy_allocator_init()
{
    // initially, we should allocate one large chunk of memory of the max order
    // and add that to the free list
    mem_area_start = malloc(PAGE_SIZE * (1 << MAX_ORDER));
    (all_free_areas[MAX_ORDER - 1]).free_list_head = mem_area_start;
    set_forward_link(mem_area_start, mem_area_start);
    set_backward_link(mem_area_start, mem_area_start);
    all_free_areas[MAX_ORDER - 1].nr_free = 1;
}

/**
 * order: means that we will allocate 2^order number of pages of PAGE_SIZE
 *        in other words, 2^order * PAGE_SIZE
 *
 * algorithm:
 * if the free list contains elements of that order, we should just use that block
 * and updaate the allocation map and remove from the free list. yay for doubly
 * linked lists.
 *
 * if the free list is empty, that means we need to increase in order until
 * we've found a free list of an order that is not empty. And we will remove that
 * block from the free list and split it into 2 incrementally until we've reached the
 * order. This means that we've created two blocks of the size order.
 *
 * One will be allocated, and the other will be added to the free list.
 * The allocation map should be updated accordingly.
 */
void *
alloc_block(size_t order)
{
    size_t current_order = order;
    void *allocated_block = NULL;
    // first find order that doesn't have an empty free list
    while (current_order < MAX_ORDER && !all_free_areas[current_order].nr_free)
    {
        current_order += 1;
    }

    if (current_order == MAX_ORDER)
    {
        // cannot return any more memory, at least in this current implementation
        return NULL;
    }
    else if (current_order == order)
    {
        // allocate from current list and take from the free_list_head
        // remove from free list
        allocated_block = remove_first_block_from_free_list(current_order);
    }
    while (current_order != order)
    {
        // now we split the block
        // now we need to do the splitting logic
        // remove the first block in the free list I think
        page_t *block_to_remove = !allocated_block ? remove_first_block_from_free_list(current_order) : allocated_block;
        current_order -= 1;

        page_t *buddy_block = split_block(block_to_remove, current_order);

        // put the buddy in the free list now (insert at the head)
        insert_free_block(current_order, buddy_block);
        allocated_block = block_to_remove;
    }
    mark_allocated_or_not(allocated_block, order);
    return allocated_block;
}

/**
 * addr: the address of the area that needs to be freed.
 * size_t: the order of the original allocation. Sorry, I'm sure
 *         there's a way to do this without needing to pass in the order.
 *         But, as of right now, I want to this more simply. We can talk talk later.
 *
 * algorithm:
 * We need to find its buddy and determine if the buddy has been allocated.
 * If the buddy is free, we can coalesce and merge into one big buddy. Will
 * need to make sure that the buddy has been removed from the free list during this process.
 * We can then incrementally check with increasing order, if the buddy is free
 * and continue until a buddy has ben allocated. T
 */
void free_blocks(void *addr, size_t order)
{
    while (1)
    {
        size_t index = find_block_index(addr, order);
        // mark this block as free in the allocation map

        // check if buddy is free
        size_t buddy_index = index % 2 == 0 ? index + 1 : index - 1;
        // remove buddy from free list
        page_t *buddy_addr = (char *)addr + ((1 << order) * PAGE_SIZE);
        if (!is_allocated(buddy_addr, order))
        {
            remove_block_from_free_list(buddy_addr, order);
            // increase the order
            order += 1;
        }
        else
        {
            break;
        }
    }

    // coalesce as much as possible and add the final result to the resulting order
    // might be order - 1
    insert_free_block(order, addr);
}