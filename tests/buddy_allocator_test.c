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
    // TODO: let's write some tests!
}

/**
 * really want to test coalescing
 *
 * Making sure that we only coalesce up until the point that a buddy is allocated
 *
 * Stress
 *
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

    buddy_allocator_terminate();
}