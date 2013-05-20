#include <stdio.h>
#include <stdlib.h>

#include "builtin.h"
#include "ngsh.h"

int builtin_exit(int argc, char *argv[])
{
    printf("\n");
    printf("The history of the shell is still being written,\n");
    printf("it's way too early for a conclusion.\n");

    int rval = 0;
    if (argc > 1) {
        rval = atoi(argv[1]);
    }

    free_argv(argc, argv);
    free_envp();

    exit(rval);
}
