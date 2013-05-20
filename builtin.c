#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ngsh.h"
#include "builtin.h"

extern int optind;

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
    static const char *optstring = "neE";

    int newline = 1;
    int escapes = 0;

    char opt;
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            case 'n': newline = 0; break;
            case 'e': escapes = 1; break;
            case 'E': escapes = 0; break;
            case '?': return -1;
        }
    }

    argv += optind;
    optind = 1;
    while (*argv) {
        if (escapes) {
            char *s = *argv;
            while (*s) {
                if (*s == '\\') {
                    switch (*++s) {
                        case '\\': putchar('\\'); break;
                        case 'a': putchar('\a'); break;
                        case 'b': putchar('\b'); break;
                        case 'c': return 0;
                        case 'e': putchar('\e'); break;
                        case 'f': putchar('\f'); break;
                        case 'n': putchar('\n'); break;
                        case 'r': putchar('\r'); break;
                        case 't': putchar('\t'); break;
                        case 'v': putchar('\v'); break;
                        default:
                                  putchar('\\');
                                  putchar(*s);
                    }
                    if (*s) s++;
                } else {
                    putchar(*s++);
                }
            }
        } else {
            printf("%s", *argv);
        }
        if (*++argv) {
            putchar(' ');
        }
    }
    if (newline) {
        printf("\n");
    }

    return 0;
}

int builtin_exit(int argc, char *argv[])
{
    printf("\n");
    printf("The history of the shell is still being written,\n");
    printf("it's way too early for a conclusion.\n");

    int r = 0;
    if (argc > 1) {
        r = atoi(argv[1]);
    }

    free_argv(argc, argv);
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
