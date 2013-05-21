#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "builtin.c"

extern char **environ;
extern int optind;

int builtin_export(int argc, char *argv[])
{
    static const char *optstring = "pr";

    int print = 0;
    int addition = 1;

    int opt;
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'p':
            print = 1;
            break;
        case 'r':
            addition = -1;
            break;
        default:               /* ? */
            goto ERROR;
        }
    }

    if (print) {
        int environ_length = 0;
        while (environ[environ_length]) environ_length++;

        int i = addition == 1 ? 0 :  environ_length - 1;
        int last = addition == 1 ? environ_length : -1;
        for (; i != last; i += addition) {
            printf("%s\n", environ[i]);
        }

        goto SUCCESS;
    }

    for (; optind < argc; optind++) {
        char *c;
        if ((c = strchr(argv[optind], '=')) == NULL) {
            if (setenv(argv[optind], "", 0) == -1) {
                perror(argv[0]);
                goto ERROR;
            }
        } else {
            if (c == argv[optind]) {
                fprintf(stderr, "%s: Invalid argument\n", argv[0]);
                goto ERROR;
            }
            char *name = strndup(argv[optind], c - argv[optind]);
            char *value = strdup(*(c + 1) == '\0' ? "" : c + 1);
            if (setenv(name, value, 1) == -1) {
                perror(argv[0]);
                goto ERROR;
            }
            free(name);
            free(value);
        }
    }

SUCCESS:
    optind = 1;
    return 0;

  ERROR:
    optind = 1;
    return -1;
}
