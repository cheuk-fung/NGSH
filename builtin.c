#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ngsh.h"
#include "builtin.h"

const struct builtin_entry builtin_table[] = {
    {"cd", builtin_cd},
    {"echo", builtin_echo},
    {"exit", builtin_exit},
    {"export", builtin_export},
    {"history", builtin_history},
    {NULL, NULL}
};

int builtin_cd(int argc, char *argv[])
{
    return 0;
}

int builtin_echo(int argc, char *argv[])
{
    return 0;
}

int builtin_exit(int argc, char *argv[])
{
    printf("\n");
    printf("The history of the shell is still being written,\n");
    printf("it's way too early for a conclusion.\n");

    int r = EXIT_SUCCESS;
    if (argc > 1) {
        r = atoi(argv[1]);
    }

    cleanup(argc, argv);
    free_envp();

    exit(r);
}

int builtin_export(int argc, char *argv[])
{
    return 0;
}

int builtin_history(int argc, char *argv[])
{
    return 0;
}

builtin_handle get_builtin(char *cmd)
{
    int i;
    for (i = 0; builtin_table[i].cmd; i++) {
        if (strcmp(cmd, builtin_table[i].cmd) == 0) {
            return builtin_table[i].handle;
        }
    }
    return NULL;
}
