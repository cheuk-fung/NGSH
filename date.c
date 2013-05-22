#include <stdio.h>
#include <time.h>
#include <unistd.h>

extern char *optarg;
extern int optind;

int main(int argc, char *argv[])
{
    const char *program = argv[0];

    int format = 0;
    char *formatstring;
    int utc = 0;

    int opt;
    while ((opt = getopt(argc, argv, "f:u")) != -1) {
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
        char datestring[256];
        strftime(datestring, sizeof datestring, formatstring, tm);
        printf("%s\n", datestring);
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
