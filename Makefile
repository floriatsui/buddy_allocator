CFLAGS = -Wall -g

all: buddy_allocator_driver

clean:
	rm -f buddy_allocator_driver

buddy_allocator_driver: buddy_allocator_driver.c buddy_allocator.c buddy_allocator.h
	gcc $(CFLAGS) $^ -o $@ 
