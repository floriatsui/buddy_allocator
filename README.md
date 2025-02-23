Goal is to create a dynamic memory allocator using the buddy allocator algorithm a la how Linux manages physical memory. 

I want to try writing this in both C and Rust. 
Should this be in blocks or pages?? 

APIs to support:
- allocating one block
- allocating the number of pages  
- freeing the pages allocated 


Plan:
- create one large block at the start 
- the rest of the blocks will be created as allocations start
- if there is no space (then we should extend the heap by the one big size again)

- how to keep track of the free blocks:
    - use the linked list implementation (that was used in Weenix). The list_head will essentially be a dummy element until something is inserted. This will be a doubly linked list. 
    - i was stoopid and forgot that we still need somewhere to put the forward and backward pointers. so we can just put these in the block themselves i think.
- how to keep track of whether a block is allocated or not? 
    - something about using a bit within the allocation_map to determine this
    - we need to keep track of the start of where memory begins. but that means we also 
    - need to keep track of how many --> but going with this approach means that expanding the buddy allocator may be difficult 

- how to find the buddy of a block. This should be easy given that we are given the order of the block ... 

Questions:
- How to structure the free list? 


References: 