#include <assert.h>

#include "../util/buddy_allocator_util.h"

void find_block_index_test()
{
    buddy_allocator_init();
    size_t order = 4;
    int num_allocations = 5;
    // allocate a bunch of blocks on this index and assert that the index is
    // as we expect. we should increase in index
    for (int i = 0; i < num_allocations; i++)
    {
        void *addr = alloc_blocks(order);
        size_t index = find_block_index(addr, order);
        assert(index == i);
    }
    buddy_allocator_terminate();
}

void allocation_checks()
{
    buddy_allocator_init();
    // first make sure that the first block is completely unallocated
    assert(!is_allocated(mem_area_start, MAX_ORDER - 1));

    // allocate from the next level, now one of the buddies should be allocated
    size_t order = MAX_ORDER - 2;
    void *block = alloc_blocks(order);
    assert(is_allocated(block, order));
    // now we should allocate the buddy
    void *block_2 = alloc_blocks(order);
    // and now both of the buddies should be allocated
    assert(!is_allocated(block_2, order));

    // now free one of the buddies
    free_blocks(block_2, order);
    assert(is_allocated(block, order));

    // allocate a lower order
    size_t lower_order = MAX_ORDER - 5;
    void *block_3 = alloc_blocks(lower_order);
    assert(is_allocated(block_3, lower_order));

    // allocate from a higher order
    size_t slightly_higher_order = MAX_ORDER - 4;
    void *block_4 = alloc_blocks(slightly_higher_order);
    assert(!is_allocated(block_4, slightly_higher_order));

    // now free the other buddy
    // we really shouldn't free the buddy here because it's still being used by lower order blocks
    free_blocks(block, order);
    assert(is_allocated(block, order));
    buddy_allocator_terminate();
}

void split_block_checks()
{
    buddy_allocator_init();
    // test to make sure that split block results in the correct address difference
    void *block = alloc_blocks(4);
    void *buddy = split_block(block, 3);
    assert((char *)block - (char *)buddy == (1 >> 3) * PAGE_SIZE);
    buddy_allocator_terminate();
}

void run_all_tests()
{
    find_block_index_test();
    allocation_checks();
}
