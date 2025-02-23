#include <stddef.h>

#include "./buddy_allocator_util.h"

size_t find_block_index(page_t *block, size_t order)
{
    size_t current_size = 1 << order;
    // TODO: is division the best thing to do here?
    size_t num_elements_level = MAX_SIZE / current_size;
    size_t curr_size_of_blocks = ((MAX_SIZE)*PAGE_SIZE) / num_elements_level;
    size_t index = ((char *)block - (char *)mem_area_start) / (curr_size_of_blocks);
}

page_t *split_block(page_t *block, size_t order)
{
    // TODO: fix this
    size_t current_size = 1 << order;
    size_t num_elements_level = MAX_SIZE / current_size;
    size_t curr_size_of_blocks = ((MAX_SIZE)*PAGE_SIZE) / num_elements_level;

    size_t index = find_block_index(block, order);
    size_t index_buddy = index + 1;
    void *addr_of_buddy = (index_buddy * curr_size_of_blocks) + (char *)mem_area_start;
    return addr_of_buddy;
}

void set_forward_link(page_t *free_block, page_t *link)
{
    ((void **)free_block->payload)[0] = link;
}

void set_backward_link(page_t *free_block, page_t *link)
{
    ((void **)free_block->payload)[1] = link;
}

page_t *get_forward_link(page_t *free_block)
{
    return (page_t *)((void **)free_block->payload)[0];
}

page_t *get_backward_link(page_t *free_block)
{
    return (page_t *)((void **)free_block->payload)[1];
}

page_t *remove_first_block_from_free_list(size_t order)
{
    page_t *block_to_remove = all_free_areas[order].free_list_head;
    page_t *next_block = get_forward_link(block_to_remove);
    page_t *prev_block = get_backward_link(block_to_remove);

    if (next_block == block_to_remove)
    {
        // there was only one element in the linked list
        all_free_areas[order].free_list_head = NULL;
    }
    else
    {
        // set the links of the next block, only need to the set the back link
        all_free_areas[order].free_list_head = next_block;
        set_backward_link(next_block, prev_block);
        set_forward_link(prev_block, next_block);
    }

    all_free_areas[order].nr_free -= 1;
    mark_allocated_or_not(block_to_remove, order);
    return block_to_remove;
}

void remove_block_from_free_list(page_t *block, size_t order)
{
    page_t *prev_block = get_backward_link(block);
    page_t *next_block = get_forward_link(block);

    if (next_block == block)
    {
        all_free_areas[order].free_list_head = NULL;
    }
    else
    {
        set_backward_link(next_block, prev_block);
        set_forward_link(prev_block, next_block);
    }
    mark_allocated_or_not(block, order);
    all_free_areas[order].nr_free -= 1;
}

void insert_free_block(size_t order, page_t *block)
{
    page_t *old_free = all_free_areas[order].free_list_head;
    all_free_areas[order].free_list_head = block;
    all_free_areas[order].nr_free += 1;

    if (old_free)
    {
        void *old_prev = get_backward_link(old_free);
        set_forward_link(block, old_free);
        set_backward_link(old_free, block);
        set_backward_link(block, old_prev);
    }
    else
    {
        set_forward_link(block, block);
        set_backward_link(block, block);
    }
}

// assuming index is already correct
void mark_allocated_or_not(page_t *block, size_t order)
{
    size_t current_size = 1 << order;
    size_t num_elements_level = MAX_SIZE / current_size;
    size_t index = find_block_index(block, order);

    size_t num_bits = num_elements_level / 2;
    size_t bit = 1 << (index / 2);
    long map = all_free_areas[order].allocation_map;
    map ^= bit;

    size_t get_that_bit = (bit & map) >> (index / 2);
    all_free_areas[order].allocation_map = map;
}

int is_allocated(page_t *block, size_t order)
{
    size_t current_size = 1 << order;
    size_t num_elements_level = MAX_SIZE / current_size;
    size_t index = find_block_index(block, order);

    size_t num_bits = num_elements_level / 2;
    size_t bit = 1 << (index / 2);
    long map = all_free_areas[order].allocation_map;
    map ^= bit;

    size_t get_that_bit = (bit & map) >> (index / 2);
    return get_that_bit;
}