#include <stdio.h>

#include "./util/buddy_allocator.h"

// TODO: write a REPL program that takes in commands and allocations
//       one of those commands should be a visualization tool
int main()
{
    buddy_allocator_init();
    int *addr = (int *)alloc_block(0);
    addr[0] = 1;
    addr[1] = 2;
    addr[2] = 4;
    for (int i = 0; i < 3; i++)
        printf("Number stored in allocated block: %d\n", addr[i]);

    int *addr_2 = (int *)alloc_block(0);
    free_blocks(addr_2, 0);
    free_blocks(addr, 0);
}