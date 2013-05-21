#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

extern int optind;

int main(int argc, char *argv[])
{
    const char *program = argv[0];

    const char *optstring = "fidv";

    int force = 0;
    int interact = 0;
    int dir = 0;
    int verbose = 0;

    int opt;
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'f':
            force = 1;
            break;
        case 'i':
            interact = 1;
            break;
        case 'd':
            dir = 1;
            break;
        case 'v':
            verbose = 1;
            break;
        default:               /* ? */
            return -1;
        }
    }

    for (argv += optind; *argv; argv++) {
        struct stat s;
        if (stat(*argv, &s) == -1) {
            if (!force) {
                perror(program);
            }
            continue;
        }
        if (S_ISDIR(s.st_mode) && !dir) {
            if (!force) {
                fprintf(stderr, "%s: cannot remove '%s': Is a directory\n",
                        program, *argv);
            }
            continue;
        }
        if (interact) {
            printf("%s: remove '%s'? (y/N) ", program, *argv);
            char c;
            scanf(" %c", &c);
            if (c != 'y') {
                continue;
            }
        }
        if (remove(*argv) == -1) {
            if (!force) {
                perror(program);
            }
        }
        if (verbose) {
            printf("%s: removed '%s'\n", program, *argv);
        }
    }

    return 0;
}
