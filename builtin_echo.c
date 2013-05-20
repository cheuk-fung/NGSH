#include <stdio.h>
#include <unistd.h>

#include "builtin.h"

extern int optind;

int builtin_echo(int argc, char *argv[])
{
    static const char *optstring = "neE";

    int newline = 1;
    int escapes = 0;

    char opt;
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'n':
            newline = 0;
            break;
        case 'e':
            escapes = 1;
            break;
        case 'E':
            escapes = 0;
            break;
        case '?':
            return -1;
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
                    case '\\':
                        putchar('\\');
                        break;
                    case 'a':
                        putchar('\a');
                        break;
                    case 'b':
                        putchar('\b');
                        break;
                    case 'c':
                        return 0;
                    case 'e':
                        putchar('\e');
                        break;
                    case 'f':
                        putchar('\f');
                        break;
                    case 'n':
                        putchar('\n');
                        break;
                    case 'r':
                        putchar('\r');
                        break;
                    case 't':
                        putchar('\t');
                        break;
                    case 'v':
                        putchar('\v');
                        break;
                    default:
                        putchar('\\');
                        putchar(*s);
                    }
                    if (*s) {
                        s++;
                    }
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
