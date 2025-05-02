#include <assert.h>
#include <stddef.h>

#include "./buddy_allocator_util.h"

/* BLOCK UTILITIES */
void find_num_elements_in_level_and_size(size_t order, size_t *size_of_blocks,
                                         size_t *num_elements_in_level) {
  assert(order < MAX_ORDER);
  size_t current_size = 1 << order;
  size_t num_elements_level = (MAX_SIZE) / current_size;
  size_t curr_size_of_blocks = current_size * PAGE_SIZE;
  if (size_of_blocks)
    *size_of_blocks = curr_size_of_blocks;
  if (num_elements_in_level)
    *num_elements_in_level = num_elements_level;
}

size_t find_block_index(page_t *block, size_t order) {
  assert(block && (void *)block >= mem_area_start);
  assert(order < MAX_ORDER);
  size_t curr_size_of_blocks;
  find_num_elements_in_level_and_size(order, &curr_size_of_blocks, NULL);
  size_t index =
      ((char *)block - (char *)mem_area_start) / (curr_size_of_blocks);
  return index;
}

void *addr_given_index_in_level(size_t index, size_t order) {
  // need to use mem_area_start to find the address
  size_t curr_size_of_blocks;
  find_num_elements_in_level_and_size(order, &curr_size_of_blocks, NULL);
  char *start_addr = (char *)mem_area_start + (index * curr_size_of_blocks);
  return (void *)start_addr;
}

page_t *split_block(page_t *block, size_t order) {
  assert(block && (void *)block >= mem_area_start);
  assert(order < MAX_ORDER);
  size_t num_elements_level;
  size_t curr_size_of_blocks;
  find_num_elements_in_level_and_size(order, &curr_size_of_blocks,
                                      &num_elements_level);

  size_t index = find_block_index(block, order);
  size_t index_buddy = index + 1;
  void *addr_of_buddy =
      (index_buddy * curr_size_of_blocks) + (char *)mem_area_start;

  assert(((char *)addr_of_buddy - (char *)block) == curr_size_of_blocks);
  return addr_of_buddy;
}

/* FREE LIST UTILITIES */
void set_forward_link(page_t *free_block, page_t *link) {
  ((page_t **)free_block->payload)[0] = link;
}

void set_backward_link(page_t *free_block, page_t *link) {
  ((page_t **)free_block->payload)[1] = link;
}

page_t *get_forward_link(page_t *free_block) {
  return ((page_t **)free_block->payload)[0];
}

page_t *get_backward_link(page_t *free_block) {
  return ((page_t **)free_block->payload)[1];
}

page_t *remove_first_block_from_free_list(size_t order) {
  assert(order < MAX_ORDER);
  page_t *block_to_remove = all_free_areas[order].free_list_head;
  page_t *next_block = get_forward_link(block_to_remove);
  page_t *prev_block = get_backward_link(block_to_remove);

  if (next_block == block_to_remove) {
    // there was only one element in the linked list
    all_free_areas[order].free_list_head = NULL;
  } else {
    // set the links of the next block, only need to the set the back link
    all_free_areas[order].free_list_head = next_block;
    set_backward_link(next_block, prev_block);
    set_forward_link(prev_block, next_block);
  }
  assert(all_free_areas[order].nr_free);
  all_free_areas[order].nr_free -= 1;
  mark_allocated_or_not(block_to_remove, order);
  return block_to_remove;
}

void remove_block_from_free_list(page_t *block, size_t order) {
  assert(order < MAX_ORDER);
  page_t *prev_block = get_backward_link(block);
  page_t *next_block = get_forward_link(block);

  if (next_block == block) {
    all_free_areas[order].free_list_head = NULL;
  } else {
    set_backward_link(next_block, prev_block);
    set_forward_link(prev_block, next_block);
  }
  mark_allocated_or_not(block, order);
  assert(all_free_areas[order].nr_free);
  all_free_areas[order].nr_free -= 1;
}

void insert_free_block(page_t *block, size_t order) {
  assert(order < MAX_ORDER);

  page_t *old_free = all_free_areas[order].free_list_head;
  all_free_areas[order].free_list_head = block;
  all_free_areas[order].nr_free += 1;

  if (old_free) {
    void *old_prev = get_backward_link(old_free);
    set_forward_link(block, old_free);
    set_backward_link(old_free, block);
    set_backward_link(block, old_prev);
  } else {
    set_forward_link(block, block);
    set_backward_link(block, block);
  }
  mark_allocated_or_not(block, order);
  assert(all_free_areas[order].nr_free);
}

/* ALLOCATION UTILITIES */
void mark_allocated_or_not(page_t *block, size_t order) {
  assert(block && (void *)block >= mem_area_start);
  assert(order < MAX_ORDER);

  size_t num_elements_level;
  find_num_elements_in_level_and_size(order, NULL, &num_elements_level);
  size_t index = find_block_index(block, order);

  size_t bit = 1 << (index / 2);
  long map = all_free_areas[order].allocation_map;
  map ^= bit;

  all_free_areas[order].allocation_map = map;
}

int is_allocated(page_t *block, size_t order) {
  assert(block && (void *)block >= mem_area_start);
  assert(order < MAX_ORDER);

  size_t num_elements_level;
  find_num_elements_in_level_and_size(order, NULL, &num_elements_level);
  size_t index = find_block_index(block, order);

  size_t bit = 1 << (index / 2);
  long map = all_free_areas[order].allocation_map;

  size_t get_that_bit = (bit & map) >> (index / 2);
  return get_that_bit;
}
