#include <stdio.h>

#include "./buddy_allocator.h"

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
}