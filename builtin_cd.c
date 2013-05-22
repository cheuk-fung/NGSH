#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"

extern int optind;

int builtin_cd(int argc, char *argv[])
{
    char *PROGNAME = argv[0];

    char *curpath = NULL;

    int print = 0;
    int physically = 0;

    int opt;
    while ((opt = getopt(argc, argv, "pLP")) != -1) {
        switch (opt) {
        case 'p':
            print = 1;
            break;
        case 'L':
            physically = 0;
            break;
        case 'P':
            physically = 1;
            break;
        default:               /* ? */
            goto ERROR;
        }
    }
    argc -= optind;
    argv += optind;

    if (argc == 0) {
        if ((curpath = getenv("HOME")) == NULL) {
            perror(PROGNAME);
            goto ERROR;
        }
    } else if (strcmp(*argv, "~") == 0) {
        if ((curpath = getenv("HOME")) == NULL) {
            perror(PROGNAME);
            goto ERROR;
        }
    } else if (strcmp(*argv, "-") == 0) {
        if ((curpath = getenv("OLDPWD")) == NULL) {
            perror(PROGNAME);
            goto ERROR;
        }
        print = 1;
    } else {
        curpath = *argv;
    }
    curpath = strdup(curpath);

    if (curpath[0] != '/') {
        char *pwd = getenv("PWD");
        char *tmp = strdup(curpath);
        curpath = realloc(curpath,
                          sizeof(char) * (strlen(pwd) + strlen(tmp) + 2));
        sprintf(curpath, "%s/%s", pwd, tmp);
        free(tmp);
    }

    if (physically) {
        char *tmp = curpath;
        if ((curpath = realpath(tmp, NULL)) == NULL) {
            free(tmp);
            perror(PROGNAME);
            goto ERROR;
        }
        free(tmp);
    }

    if (chdir(curpath) == -1) {
        perror(PROGNAME);
        goto ERROR;
    }
    if (print) {
        printf("%s\n", curpath);
    }

    if (setenv("OLDPWD", getenv("PWD"), 1) == -1) {
        perror(PROGNAME);
        goto ERROR;
    }
    if (setenv("PWD", curpath, 1) == -1) {
        perror(PROGNAME);
        goto ERROR;
    }

    free(curpath);
    optind = 1;
    return 0;

  ERROR:
    free(curpath);
    optind = 1;
    return -1;
}
