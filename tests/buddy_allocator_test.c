#include <assert.h>

#include "../util/buddy_allocator_util.h"

/**
 * test when the system runs out of memory --> test allocating the largest block and the next block should not exist
 *
 * really want to stress test the allocation and splitting of blocks
 *
 */
void allocation_test()
{
    buddy_allocator_init();
    size_t initial_order = 0;
    alloc_blocks(initial_order);
    for (int i = 0; i < MAX_ORDER - 1; i++)
    {
        // we should've split blocks all the way down
        // which means that we should have one free block in every level
        assert(all_free_areas[i].nr_free == 1);
    }
    alloc_blocks(0);
    // should only allocate from the buddy
    assert(all_free_areas[0].nr_free == 0);
    alloc_blocks(0);
    // now we should've used the buddy from above and split
    // which means that we should have another buddy of the 0 size
    assert(all_free_areas[0].nr_free == 1);
    assert(all_free_areas[1].nr_free == 0);

    // now continue with that pattern
    for (int i = 1; i < MAX_ORDER - 2; i++)
    {
        alloc_blocks(i);
        // should split a block from above which means that
        // there should be a block in the current level
        assert(all_free_areas[i].nr_free == 1);
        assert(all_free_areas[i + 1].nr_free == 0);
    }
    buddy_allocator_terminate();
}

/**
 *
 * coalescing etc
 *
 */
void deallocation_test()
{
    // allocate one block from a lower order
    buddy_allocator_init();
    size_t order = 5;
    void *addr = alloc_blocks(order);

    for (int i = MAX_ORDER - 2; i >= order; i--)
    {
        assert(all_free_areas[i].nr_free == 1);
    }

    // now when we free it, we should be able to see the coalescing happen
    // reflected in the number of free blocks, should end up only with one big block
    free_blocks(addr, order);
    for (int i = MAX_ORDER - 2; i >= order; i--)
    {
        assert(all_free_areas[i].nr_free == 0);
    }
    assert(all_free_areas[MAX_ORDER - 1].nr_free == 1);

    // now we create a scenario in which we need to merge backwards
    size_t new_order = 4;
    void *first_allocation = alloc_blocks(4);
    void *second_allocation = alloc_blocks(4);
    void *third_allocation = alloc_blocks(4);
    assert(all_free_areas[new_order].nr_free == 1);
    assert(all_free_areas[order].nr_free == 0);
    assert(all_free_areas[order + 1].nr_free == 1);

    // go up by one and allocate there as well
    // we had to go up a level to split
    void *another_allocation = alloc_blocks(order);
    assert(all_free_areas[order].nr_free == 1);
    // this is because the order allocation would've split the order + 1 block
    assert(all_free_areas[order + 1].nr_free == 0);

    // and now we free, but nothing can be combined yet
    free_blocks(second_allocation, new_order);
    assert(all_free_areas[new_order].nr_free == 2);
    free_blocks(first_allocation, new_order);

    // we should've coalesced, which means the order above has gotten another block
    assert(all_free_areas[new_order].nr_free == 1);
    assert(all_free_areas[order].nr_free == 2);
    assert(all_free_areas[order + 1].nr_free == 0);

    // now free the third allocation
    free_blocks(third_allocation, new_order);
    assert(all_free_areas[new_order].nr_free == 0);
    assert(all_free_areas[order].nr_free == 1);
    assert(all_free_areas[order + 1].nr_free == 1);

    // do backwards merge again
    free_blocks(another_allocation, order);
    assert(all_free_areas[order].nr_free == 0);
    assert(all_free_areas[order + 1].nr_free == 0);

    // because there is nothing blocking the rest of the coalescing from happening
    // we will keep coalescing up until we've reached the maximum order
    for (int i = order; i < MAX_ORDER - 1; i++)
        assert(all_free_areas[i].nr_free == 0);
    assert(all_free_areas[MAX_ORDER - 1].nr_free == 1);
    buddy_allocator_terminate();
}

void run_all_allocation_tests()
{
    allocation_test();
    deallocation_test();
}