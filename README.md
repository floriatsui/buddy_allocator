# Buddy Allocator
Goal is to create a really simple memory allocator using the buddy allocator algorithm a la how Linux manages physical memory. The motivation behind this is I remember seeing a buddy allocator in practice and 

I want to try writing this in both C and Rust, starting with C... 

## APIs supported:
- allocating the number of pages  
- freeing the pages allocated 

## General Plan:
- create one large block at the start 
- the rest of the blocks will be created as allocations start
- if there is no space (then we should extend the heap by the one big size again) --> this is currently not supported.

- how to keep track of the free blocks: using a circular and doubly linked list 
    - store the pointers within the free blocks (they aren't being used anyways). First 8 bytes correspond to the forward pointer, the second 8 bytes correspond to the back pointer.
    - there will be a field in the struct that points to the start of the free list at a given a level.
- how to keep track of whether a block is allocated or not? 
    - using a bit within the allocation_map to determine this for both of the buddies (similar to the way it's handled in Linux)
    - we need to keep track of the start of where memory begins. 
    - but going with this approach means that expanding the buddy allocator may be difficult 
- how to find the buddy of a block. This should be easy given that we are given the order of the block ... We will need to find the index of the block within a given level and then find the adjacent buddy index.  

## Next Steps
- Adding tests both for the utility functions as well as stress testing each of the APIs in conjunction with each other
- Adding a driver program that can offer a command loop as well as a visualizer tool to see what memory looks like 
- Running this with address sanitizers

## References: 
- https://students.mimuw.edu.pl/ZSO/Wyklady/06_memory2/BuddySlabAllocator.pdf
- https://github.com/red-rocket-computing/buddy-alloc/blob/master/doc/bitsquid-buddy-allocator-design.md
- https://www.kernel.org/doc/gorman/html/understand/understand009.html