CFLAGS = -Wall -g

all: buddy_allocator_driver test_driver

clean:
	rm -f buddy_allocator_driver test_driver

check: test_driver
	./test_driver

buddy_allocator_driver: buddy_allocator_driver.c buddy_allocator.c util/buddy_allocator_util.c util/buddy_allocator.h util/buddy_allocator_util.h
	gcc $(CFLAGS) $^ -o $@ 

test_driver: tests/test_driver.c tests/buddy_allocator_util_test.c tests/buddy_allocator_test.c util/buddy_allocator_util.c buddy_allocator.c
	gcc $(CFLAGS) $^ -o $@
