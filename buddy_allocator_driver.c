#include <stdio.h>

#include "./util/buddy_allocator.h"

// TODO: write a REPL program that takes in commands and allocations
//       one of those commands should be a visualization tool

#define BUF_SIZE 512
#define NUM_COMMANDS 4 // keeping this a constant for now

typedef struct command
{
    const char *command_name;
    void (*handler_func)(const char *)
} command_t;

command_t commands[] = {{"help", help_cmd},
                        {"alloc", alloc_cmd},
                        {"free", free_cmd},
                        {"print", print_cmd}};

// statically intialize commands
// the commands to be supported
// help (h) or (help) --> print out all available commands
// alloc (a) or (alloc) with num representing size --> allocate block
// free (f) or (free) with index and num --> free block of a given size
// print (p) or (print)

void help_cmd(const char *args)
{
    // should also explain that to exit the REPL should use EOF
    // if we want to be super fancey we can block the signals so they can't stop it any other way
}

void alloc_cmd(const char *args)
{
		int size_allocation;
    int ret = sscanf(args, "alloc %d", &size_allocation);
		if (ret != 1) {
			printf(stderr, "Invalid number of arguments. Usage: alloc [\%d]");
			return;
		} else {
			alloc_blocks(size_allocation);
		}
}

void free_cmd(const char *args)
{
}

void print_cmd(const char *args)
{
}

int repl()
{
    char buf[BUF_SIZE];
    char cmd[BUF_SIZE];

    buddy_allocator_init();

    while (1)
    {
        memset(buf, '\0', BUF_SIZE);
        memset(cmd, '\0', BUF_SIZE);

        fprintf(stdout, ">");
        char *bytes = fgets(buf, BUF_SIZE, stdin);
        if (!bytes)
        {
            buddy_allocator_terminate();
            return 0;
        }

        int ret = sscanf(bytes, "%s", cmd);
        if (ret != 1)
        {
            fprintf(stderr,
                    "syntax error (first argument must be a command)\n");
            continue;
        }

        for (int i = 0; i < sizeof(commands) / sizeof(command_t); i++)
        {
            if (!strcmp(commands[i].command_name, cmd))
            {
                commands[i].handler_func(bytes);
            }
        }
        fflush(stdout);
    }
}

// sample program
int main()
{
    buddy_allocator_init();
    int *addr = (int *)alloc_blocks(0);
    addr[0] = 1;
    addr[1] = 2;
    addr[2] = 4;
    for (int i = 0; i < 3; i++)
        printf("Number stored in allocated block: %d\n", addr[i]);

    int *addr_2 = (int *)alloc_blocks(0);
    free_blocks(addr_2, 0);
    free_blocks(addr, 0);
}
