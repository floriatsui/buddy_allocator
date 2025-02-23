#include "./buddy_allocator.h"

void set_forward_link(page_t *free_block, page_t *link);
void set_backward_link(page_t *free_block, page_t *link);
page_t *get_forward_link(page_t *free_block);
page_t *get_backward_link(page_t *free_block);

void mark_allocated_or_not(page_t *block, size_t order);
int is_allocated(page_t *block, size_t order);

page_t *remove_first_block_from_free_list(size_t order);
void remove_block_from_free_list(page_t *block, size_t order);
void insert_free_block(size_t order, page_t *block);
page_t *split_block(page_t *block, size_t order);
size_t find_block_index(page_t *block, size_t order);