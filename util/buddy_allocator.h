#include <stddef.h>

void *alloc_block();
void *alloc_blocks(size_t order);
void free_blocks(void *addr, size_t order);
void buddy_allocator_init();

// TODO: why are macros not getting imported hmmm and printed in GDB
// TODO: create a utility for finding the index of a block

// ranges from 0 ... 10
#define MAX_ORDER 3
#define MAX_SIZE 1 << (MAX_ORDER - 1) //
#define PAGE_SIZE 4096

typedef struct free_area_struct
{
    void *free_list_head; // pointer to the first element in the list
    long allocation_map;
    long nr_free;
} free_area_t;

typedef struct page
{
    int payload[PAGE_SIZE];
} page_t;

free_area_t all_free_areas[MAX_ORDER];
void *mem_area_start;