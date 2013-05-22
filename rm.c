#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

extern int optind;

int main(int argc, char *argv[])
{
    const char *PROGNAME = argv[0];

    int force = 0;
    int interact = 0;
    int dir = 0;
    int verbose = 0;

    int opt;
    while ((opt = getopt(argc, argv, "fidv")) != -1) {
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
    argc -= optind;
    argv += optind;

    for (; *argv; argv++) {
        struct stat statbuf;
        if (stat(*argv, &statbuf) == -1) {
            if (!force) {
                perror(PROGNAME);
            }
            continue;
        }
        if (S_ISDIR(statbuf.st_mode) && !dir) {
            if (!force) {
                fprintf(stderr, "%s: cannot remove '%s': Is a directory\n",
                        PROGNAME, *argv);
            }
            continue;
        }
        if (interact) {
            printf("%s: remove '%s'? (y/N) ", PROGNAME, *argv);
            char c;
            scanf(" %c", &c);
            if (c != 'y') {
                continue;
            }
        }
        if (remove(*argv) == -1) {
            if (!force) {
                perror(PROGNAME);
            }
        }
        if (verbose) {
            printf("%s: removed '%s'\n", PROGNAME, *argv);
        }
    }

    return 0;
}
