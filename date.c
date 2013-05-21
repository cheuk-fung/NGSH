#include <stdio.h>
#include <time.h>
#include <unistd.h>

extern char *optarg;
extern int optind;

int main(int argc, char *argv[])
{
    const char *program = argv[0];

    const char *optstring = "f:u";

    int format = 0;
    char *formatstring;
    int utc = 0;

    int opt;
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'f':
            format = 1;
            formatstring = optarg;
            break;
        case 'u':
            utc = 1;
            break;
        default:               /* ? */
            return -1;
        }
    }

    time_t now = time(NULL);
    struct tm *tm;
    if (utc) {
        tm = gmtime(&now);
    } else {
        tm = localtime(&now);
    }
    if (tm == NULL) {
        perror(program);
        return -1;
    }

    if (format) {
        char buf[1024];
        if (strftime(buf, sizeof buf, formatstring, tm) == 0) {
            perror(program);
            return -1;
        }
        printf("%s\n", buf);
    } else {
        char *asc = asctime(tm);
        if (asc == NULL) {
            perror(program);
            return -1;
        }
        printf("%s", asc);
    }

    return 0;
}
