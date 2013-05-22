#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define MAX(x, y) ((x) > (y) ? (x) : (y))

extern char *optarg;
extern int optind;

int display;
int lines;
int columns;

void more(FILE *fin, char *name)
{
    static const char *MORE = "-- MORE --";
    static const char *PROMPT = "[Press space to continue, 'q' to quit.]";

    assert(256 > columns);
    char buf[256];
    printf("----------\n");
    printf("%s\n", name);
    printf("----------\n");
    int lineno = 3;
    while (fgets(buf, columns, fin)) {
        if (lineno++ % lines == 0) {
            printf("%s", display ? PROMPT : MORE);
            fflush(stdout);
            char c;
            do {
                c = getchar();
                if (c == 'q') {
                    printf("\n");
                    exit(EXIT_SUCCESS);
                }
            } while (c != ' ');
            printf("\n");
        }
        printf("%s", buf);
    }
}

int main(int argc, char *argv[])
{
    const char *PROGNAME = argv[0];

    int opt;
    while ((opt = getopt(argc, argv, "dn:")) != -1) {
        switch (opt) {
        case 'd':
            display = 1;
            break;
        case 'n':
            lines = MAX(0, atoi(optarg));
            break;
        default:               /* ? */
            return -1;
        }
    }
    argc -= optind;
    argv += optind;

    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        perror(PROGNAME);
        return -1;
    }

    if (lines == 0) {
        lines = ws.ws_row - 1;
    }
    columns = ws.ws_col;

    struct termios saved_termios, new_termios;
    if (tcgetattr(STDIN_FILENO, &saved_termios) == -1) {
        perror(PROGNAME);
        return -1;
    }
    new_termios = saved_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_termios) == -1) {
        perror(PROGNAME);
        return -1;
    }

    FILE *fin;
    while (*argv) {
        if ((fin = fopen(*argv, "r")) == NULL) {
            perror(PROGNAME);
            continue;
        }

        more(fin, *argv);

        if (fclose(fin) == EOF) {
            perror(PROGNAME);
        }

        if (*++argv) {
            printf("\n");
        }
    }

    if (tcsetattr(STDIN_FILENO, TCSANOW, &saved_termios) == -1) {
        perror(PROGNAME);
        return -1;
    }

    return 0;
}
