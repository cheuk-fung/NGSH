#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"

extern char **environ;
extern int optind;

int builtin_export(int argc, char *argv[])
{
    char *PROGNAME = argv[0];

    int print = 0;
    int addition = 1;

    int opt;
    while ((opt = getopt(argc, argv, "pr")) != -1) {
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
    argc -= optind;
    argv += optind;

    if (print) {
        int environ_length = 0;
        while (environ[environ_length]) {
            environ_length++;
        }

        int i = addition == 1 ? 0 : environ_length - 1;
        int last = addition == 1 ? environ_length : -1;
        for (; i != last; i += addition) {
            printf("%s\n", environ[i]);
        }

        goto SUCCESS;
    }

    for (; *argv; argv++) {
        char *c;
        if ((c = strchr(*argv, '=')) == NULL) {
            if (setenv(*argv, "", 0) == -1) {
                perror(PROGNAME);
                goto ERROR;
            }
        } else {
            if (c == *argv) {
                fprintf(stderr, "%s: Invalid argument\n", PROGNAME);
                goto ERROR;
            }
            char *name = strndup(*argv, c - *argv);
            char *value = strdup(*(c + 1) == '\0' ? "" : c + 1);
            if (setenv(name, value, 1) == -1) {
                perror(PROGNAME);
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
