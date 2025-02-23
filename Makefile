CFLAGS = -Wall -g

all: buddy_allocator_driver

clean:
	rm -f buddy_allocator_driver

buddy_allocator_driver: buddy_allocator_driver.c buddy_allocator.c util/buddy_allocator_util.c
	gcc $(CFLAGS) $^ -o $@ 
